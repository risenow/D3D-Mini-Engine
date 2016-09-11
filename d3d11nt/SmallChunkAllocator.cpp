#include "stdafx.h"
#include "SmallChunkAllocator.h"

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
    popAssert(size < FB_SZ);

    size_t overallSize = size + AH_SZ;

    FB_PTR prevFreeBlock = nullptr;
    FB_PTR currentFreeBlock = m_FreeBlock;

    while (currentFreeBlock)
    {
        unsigned int offset;
        void* data = align((uptr)currentFreeBlock->m_Start + sizeof(unsigned char), al, offset);

        if (currentFreeBlock->GetSize() >= overallSize + offset)
        {
            AH_PTR header = (AH_PTR)((uptr)data - sizeof(unsigned char));
            header->SetSize(size);
            header->SetAl(offset);

            overallSize += offset;
            currentFreeBlock->m_Start = (uptr)currentFreeBlock->m_Start + overallSize;
            currentFreeBlock->SetSize(currentFreeBlock->GetSize() - overallSize);

            return data;
        }

        data = align((uptr)currentFreeBlock + 1, al, offset);
        if (currentFreeBlock->GetSize() == 0 && FB_SZ >= overallSize + offset)
        {
            AH_PTR header = (AH_PTR)((uptr)data - 1);
            header->SetSize(size);
            header->SetAl(offset);

            if (prevFreeBlock)
                prevFreeBlock->m_Next = currentFreeBlock->m_Next;
            else
                m_FreeBlock = m_FreeBlock->m_Next;

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
    FB_PTR tmpFreeBlock = (FB_PTR)_alloca(FB_SZ);
    AH_PTR header = (AH_PTR)((uptr)p - AH_SZ);
    unsigned int alignnment = header->GetAl();
    tmpFreeBlock->m_Start = (uptr)header - alignnment;
    tmpFreeBlock->SetSize(header->GetSize() + alignnment);

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
        FB_PTR newFreeBlock = allocateFreeBlock(isAllreadyInList);

        (*newFreeBlock) = (*tmpFreeBlock);
        if (!isAllreadyInList)
        {
            newFreeBlock->m_Next = m_FreeBlock;
            m_FreeBlock = newFreeBlock;
        }
    }
}