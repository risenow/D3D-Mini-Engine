#include "stdafx.h"
#include "MemoryManager.h"

BigChunkAllocator MemoryManager::m_BigChunkAllocator = BigChunkAllocator();
SmallChunkAllocator MemoryManager::m_SmallChunkAllocator = SmallChunkAllocator();

void MemoryManager::Initialize(size_t size)
{
    void* start = malloc(size);
    size_t sizeForBigChunksAllocator = (3.0 / 4.0)*size;
    
    m_BigChunkAllocator = BigChunkAllocator(start, sizeForBigChunksAllocator);
    m_SmallChunkAllocator = SmallChunkAllocator((uptr)start + sizeForBigChunksAllocator, size - sizeForBigChunksAllocator);
}

BigChunkAllocator* MemoryManager::GetBigChunkAllocator()
{
    if (m_BigChunkAllocator.GetStartPointer() == 0)
    {
        Initialize(MEMORY_BUDJET);
    }
    return &m_BigChunkAllocator;
}

SmallChunkAllocator* MemoryManager::GetSmallChunkAllocator()
{
    if (m_SmallChunkAllocator.GetStartPointer() == 0)
    {
        Initialize(MEMORY_BUDJET);
    }
    return &m_SmallChunkAllocator;
}