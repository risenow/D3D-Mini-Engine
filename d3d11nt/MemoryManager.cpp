#include "stdafx.h"
#include "MemoryManager.h"

BigChunkAllocator MemoryManager::BigChunksAllocator = BigChunkAllocator();
SmallChunkAllocator MemoryManager::SmallChunksAllocator = SmallChunkAllocator();

void MemoryManager::Initialize(size_t size)
{
    void* start = malloc(size);
    size_t sizeForBigChunksAllocator = (3.0 / 4.0)*size;
    
    BigChunksAllocator = BigChunkAllocator(start, sizeForBigChunksAllocator);
    SmallChunksAllocator = SmallChunkAllocator((uptr)start + sizeForBigChunksAllocator, size - sizeForBigChunksAllocator);
}
