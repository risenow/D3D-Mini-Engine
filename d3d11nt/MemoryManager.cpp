#include "stdafx.h"
#include "MemoryManager.h"

BigChunkAllocator* MemoryManager::m_BigChunkAllocator = nullptr;
SmallChunkAllocator* MemoryManager::m_SmallChunkAllocator = nullptr;

void MemoryManager::Initialize(size_t size)
{
    void* start = malloc(size);
    size_t sizeForBigChunksAllocator = (size_t)((3.0 / 4.0)*size);
    
    m_BigChunkAllocator = new BigChunkAllocator(start, sizeForBigChunksAllocator);
    m_SmallChunkAllocator = new SmallChunkAllocator((uptr)start + sizeForBigChunksAllocator, size - sizeForBigChunksAllocator);
}

BigChunkAllocator* MemoryManager::GetBigChunkAllocator()
{
    if (!m_BigChunkAllocator)
    {
        Initialize(MEMORY_BUDJET);
    }

    return m_BigChunkAllocator;
}

SmallChunkAllocator* MemoryManager::GetSmallChunkAllocator()
{
    if (!m_SmallChunkAllocator)
    {
        Initialize(MEMORY_BUDJET);
    }

    return m_SmallChunkAllocator;
}

size_t MemoryManager::GetAllocatedMemorySize()
{
    size_t size = 0;
    if (m_BigChunkAllocator)
        size += m_BigChunkAllocator->GetAllocatedMemorySize();
    if (m_SmallChunkAllocator)
        size += m_SmallChunkAllocator->GetAllocatedMemorySize();

    return size;
}