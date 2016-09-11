#pragma once
#include "memutils.h"
#include "BigChunkAllocator.h"
#include "SmallChunkAllocator.h"

const size_t MEMORY_BUDJET = 64 MB;

class MemoryManager
{
public:
    MemoryManager()
    {
        Initialize(MEMORY_BUDJET);
    }

    static void Initialize(size_t size);

    static SmallChunkAllocator* GetSmallChunkAllocator();
    static BigChunkAllocator* GetBigChunkAllocator();
private:
    static SmallChunkAllocator m_SmallChunkAllocator;
    static BigChunkAllocator   m_BigChunkAllocator;
};