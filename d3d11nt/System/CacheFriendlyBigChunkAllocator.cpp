#include "stdafx.h"
#include "System/BigChunkAllocator.h"
#include "System/ProfileUnit.h"

BigChunkAllocator::BigChunkAllocator() : m_Start(nullptr), m_Size(0)
{}
BigChunkAllocator::BigChunkAllocator(void* start, size_t size)
{
    m_Start = start;
    m_Size = size;

    m_FreeBlock = (FB_PTR)m_Start;
    m_FreeBlock->m_Start = (uptr)m_Start + FB_SZ;
    m_FreeBlock->m_Size = size - FB_SZ;
    m_FreeBlock->m_Next = nullptr;

    m_AllocatedMemorySize += FB_SZ;
}

BigChunkAllocator::~BigChunkAllocator()
{
}

void* BigChunkAllocator::Allocate(size_t size, unsigned long al) //we don't use al in this allocator since this allocator's algorithm produces only aligned by 4 addresses
{
    //popProfile(BigChunkAllocatorAllocate);

    //std::lock_guard<std::mutex> autoLock(m_Lock);

    size_t overallSize = size + AH_SZ;

    //maintaining important invariant for this type of allocator
    //popAssert(overallSize >= FB_SZ);
    if (overallSize < FB_SZ)
        overallSize = FB_SZ;  

    FB_PTR prevFreeBlock = nullptr;
    FB_PTR currentFreeBlock = m_FreeBlock;
    while (currentFreeBlock)
    {
        if (currentFreeBlock->m_Size >= overallSize)
        {
            void* result = currentFreeBlock;
            FB_PTR movedFreeBlock = (FB_PTR)((uptr)result + overallSize);

            unsigned int offset = 0;
            movedFreeBlock = (FB_PTR)align((void*)movedFreeBlock, __alignof(FreeBlock), offset);

            if ((currentFreeBlock->m_Size - overallSize) >= offset)
            {
                (*movedFreeBlock) = (*currentFreeBlock);
                movedFreeBlock->m_Start = (uptr)movedFreeBlock + FB_SZ;
                movedFreeBlock->m_Size -= (overallSize + offset);

                if (prevFreeBlock)
                {
                    prevFreeBlock->m_Next = movedFreeBlock;
                }
                else
                {
                    m_FreeBlock = movedFreeBlock;
                }

                AH_PTR header = (AH_PTR)result;
                header->m_Size = overallSize - AH_SZ + offset; //size

                result = (uptr)result + AH_SZ;

                m_AllocatedMemorySize += overallSize + offset;
                DEBUG_ONLY(WriteAllocationDebugInfo(size, overallSize + offset, false));

                return result;
            }

            if (prevFreeBlock)
            {
                prevFreeBlock->m_Next = currentFreeBlock->m_Next;
            }
            else
            {
                m_FreeBlock = currentFreeBlock->m_Next;
            }

            AH_PTR header = (AH_PTR)result;
            header->m_Size = currentFreeBlock->m_Size + FB_SZ - AH_SZ;

            result = (uptr)result + AH_SZ;

            m_AllocatedMemorySize += currentFreeBlock->m_Size + FB_SZ;
            DEBUG_ONLY(WriteAllocationDebugInfo(size, currentFreeBlock->m_Size + FB_SZ, true));

            return result;
        }

        if ((currentFreeBlock->m_Size + FB_SZ) >= overallSize)
        {
            AH_PTR header = (AH_PTR)currentFreeBlock;
            header->m_Size = currentFreeBlock->m_Size + FB_SZ - AH_SZ; //give it all the memory that this block has
            if (prevFreeBlock)
                prevFreeBlock->m_Next = currentFreeBlock->m_Next; //delete this block from list
            else
                m_FreeBlock = currentFreeBlock->m_Next;

            m_AllocatedMemorySize += currentFreeBlock->m_Size + FB_SZ;
            DEBUG_ONLY(WriteAllocationDebugInfo(size, currentFreeBlock->m_Size + FB_SZ, false));

            return (uptr)header + AH_SZ;
        }
        prevFreeBlock = currentFreeBlock;
        currentFreeBlock = currentFreeBlock->m_Next;
    }

    popAssert(false);
    return nullptr;
}

void BigChunkAllocator::Deallocate(void* p)
{
    //popProfile(BigChunkAllocatorDeallocate);

    //std::lock_guard<std::mutex> autoLock(m_Lock);

    AH_PTR header = (AH_PTR)((uptr)p - AH_SZ);
    size_t size = header->m_Size + AH_SZ;
    FB_PTR newFreeBlock = (FB_PTR)header;
    newFreeBlock->m_Size = size - FB_SZ;
    newFreeBlock->m_Start = (uptr)newFreeBlock + FB_SZ;
    newFreeBlock->m_Next = nullptr;

    m_AllocatedMemorySize -= size;
    DEBUG_ONLY(WriteDeallocationDebugInfo(size));

    //defragmentation
    FB_PTR beforeChangedFreeBlock = nullptr;
    FB_PTR prevFreeBlock = nullptr;
    FB_PTR currentFreeBlock = m_FreeBlock;
    bool inList = false;
    while (currentFreeBlock)
    {
        if (currentFreeBlock->end() == (void*)newFreeBlock)
        {
            currentFreeBlock->m_Size += newFreeBlock->m_Size + FB_SZ;

            if (inList)
            {
                if (beforeChangedFreeBlock)
                    beforeChangedFreeBlock->m_Next = newFreeBlock->m_Next;
                else
                    m_FreeBlock = m_FreeBlock->m_Next;
            }

            beforeChangedFreeBlock = prevFreeBlock;
            newFreeBlock = currentFreeBlock;
            inList = true;
        }
        if (newFreeBlock->end() == currentFreeBlock)
        {
            newFreeBlock->m_Size += currentFreeBlock->m_Size + FB_SZ;

            if (prevFreeBlock)
                prevFreeBlock->m_Next = currentFreeBlock->m_Next; //delete currentFreeBlock from list
            else
                m_FreeBlock = currentFreeBlock->m_Next;

            if (!inList)
            {
                newFreeBlock->m_Next = m_FreeBlock;
                m_FreeBlock = newFreeBlock;
            }

            beforeChangedFreeBlock = prevFreeBlock;
            inList = true;
        }

        prevFreeBlock = currentFreeBlock;
        currentFreeBlock = currentFreeBlock->m_Next;
    }
	
    if (!inList)
    {
        popAssert(!newFreeBlock->m_Next);
        newFreeBlock->m_Next = m_FreeBlock;
        m_FreeBlock = newFreeBlock;
    }
}

#ifdef _DEBUG
void BigChunkAllocator::WriteAllocationDebugInfo(size_t requestedAllocationSize, size_t realAllocatedSize, bool blockConsumed)
{
    std::string blockConsumeInfoStr = ", block consumed: ";
    if (blockConsumed)
        blockConsumeInfoStr += "yes";
    else
        blockConsumeInfoStr += "no";

    LOG_CONS(std::string("Big Chunk Allocator allocation, requested size: " + std::to_string(requestedAllocationSize) + 
             " real allocated size: " + std::to_string(realAllocatedSize) +
             ", free memory: " + BytesNumberToFormattedString(m_Size - m_AllocatedMemorySize) + blockConsumeInfoStr));
}

void BigChunkAllocator::WriteDeallocationDebugInfo(size_t deallocatedSize)
{
    LOG_CONS("Big Chunk Allocator deallocation, size: " + std::to_string(deallocatedSize) +
             ", free memory: " + BytesNumberToFormattedString(m_Size - m_AllocatedMemorySize));
}
#endif // _DEBUG