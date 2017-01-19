#include "stdafx.h"
#include "System/SmallChunkAllocator.h"

SmallChunkAllocator::SmallChunkAllocator() : m_Start(nullptr), m_Size(0)
{}
SmallChunkAllocator::SmallChunkAllocator(void* start, size_t size)
{
    m_Start = start;
    m_Size = size;

    m_FreeBlock = (FB_PTR)m_Start;
    m_FreeBlock->m_Start = (uptr)m_Start + FB_SZ;
    m_FreeBlock->SetAl(0);
    m_FreeBlock->SetSize(size - FB_SZ);
    m_FreeBlock->m_Next = nullptr;
}

void* SmallChunkAllocator::Allocate(size_t size, unsigned int al)
{
    popProfile(SmallChunkAllocatorAllocate);

    std::lock_guard<std::mutex> autoLock(m_Lock);

    popAssert(size < FB_SZ);

    size_t overallSize = size + AH_SZ;

    FB_PTR prevFreeBlock = nullptr;
    FB_PTR currentFreeBlock = m_FreeBlock;

    while (currentFreeBlock)
    {
        unsigned int offset;
        void* data = align((uptr)currentFreeBlock->m_Start + AH_SZ, al, offset);

        if (currentFreeBlock->GetSize() >= overallSize + offset)
        {
            AH_PTR header = (AH_PTR)((uptr)data - AH_SZ);
            header->SetSize(size);
            header->SetAl(offset);

            overallSize += offset;
            currentFreeBlock->m_Start = (uptr)currentFreeBlock->m_Start + overallSize;
            currentFreeBlock->SetSize(currentFreeBlock->GetSize() - overallSize);

            m_AllocatedMemorySize += overallSize;

            return data;
        }


        void* currentFreeBlockBlockStart = (uptr)currentFreeBlock->m_Start - currentFreeBlock->GetAl();
        data = align((uptr)currentFreeBlockBlockStart + AH_SZ, al, offset);
        //data = align((uptr)currentFreeBlock + AH_SZ, al, offset);
        if (currentFreeBlock->GetSize() == 0 && (FB_SZ + currentFreeBlock->GetAl()) >= overallSize + offset) //FIXME: we don't use the fact that the full size that free block has is it's size + al 
        {
            AH_PTR header = (AH_PTR)((uptr)data - AH_SZ);
            //header->SetSize(size); //memory loose
            header->SetSize(FB_SZ - offset - AH_SZ);
            header->SetAl(offset);

            if (prevFreeBlock)
            {
                prevFreeBlock->m_Next = currentFreeBlock->m_Next;
            }
            else
            {
                m_FreeBlock = m_FreeBlock->m_Next;
            }

            m_AllocatedMemorySize += (FB_SZ + currentFreeBlock->GetAl());

            return data;
        }

        prevFreeBlock = currentFreeBlock;
        currentFreeBlock = currentFreeBlock->m_Next;
    }

    popAssert(false);
    return nullptr;
}

void SmallChunkAllocator::Deallocate(void* p)
{
    popProfile(SmallChunkAllocatorDeallocate);

    std::lock_guard<std::mutex> autoLock(m_Lock);

    FB_PTR tmpFreeBlock = (FB_PTR)_alloca(FB_SZ);
    AH_PTR header = (AH_PTR)((uptr)p - AH_SZ);
    unsigned int alignnment = header->GetAl();
    tmpFreeBlock->m_Start = (uptr)header - alignnment;
    tmpFreeBlock->SetSize(header->GetSize() + alignnment + AH_SZ);

    m_AllocatedMemorySize -= tmpFreeBlock->GetSize();

    FB_PTR inListPrevFreeBlock = nullptr;
    FB_PTR prevFreeBlock = nullptr;
    FB_PTR currentFreeBlock = m_FreeBlock;
    bool inList = false;
    while (currentFreeBlock)
    {
        if (tmpFreeBlock->m_Start == currentFreeBlock->end())
        {
            currentFreeBlock->SetSize(currentFreeBlock->GetSize() + tmpFreeBlock->GetSize());

            if (inList) //if tmpFreeBlock points on block that is in list
            {
                if (inListPrevFreeBlock)
                    inListPrevFreeBlock->m_Next = tmpFreeBlock->m_Next; //delete tmp freeblock
                else
                    m_FreeBlock = tmpFreeBlock->m_Next;
            }

            tmpFreeBlock = currentFreeBlock;
            inListPrevFreeBlock = prevFreeBlock;
            inList = true;
        }

        if (tmpFreeBlock->end() == currentFreeBlock->m_Start)
        {
            currentFreeBlock->m_Start = tmpFreeBlock->m_Start;
            currentFreeBlock->SetSize(currentFreeBlock->GetSize() + tmpFreeBlock->GetSize());

            if (inList)
            {
                if (inListPrevFreeBlock)
                    inListPrevFreeBlock->m_Next = tmpFreeBlock->m_Next;
                else
                    m_FreeBlock = tmpFreeBlock->m_Next;
            }

            tmpFreeBlock = currentFreeBlock;
            inListPrevFreeBlock = prevFreeBlock;
            inList = true;
        }

        prevFreeBlock = currentFreeBlock;
        currentFreeBlock = currentFreeBlock->m_Next;
    }

    if (!inList)
    {
        bool isAllreadyInList;
        FB_PTR newFreeBlock = AllocateFreeBlock(isAllreadyInList);

        (*newFreeBlock) = (*tmpFreeBlock);
        if (!isAllreadyInList)
        {
            newFreeBlock->m_Next = m_FreeBlock;
            m_FreeBlock = newFreeBlock;
        }
    }
}