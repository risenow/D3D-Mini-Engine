#pragma once
#include "memutils.h"
#include "BigChunkAllocator.h"
#include "SmallChunkAllocator.h"

class MemoryManager
{
public:
    static void Initialize(size_t size);

    static SmallChunkAllocator SmallChunksAllocator;
    static BigChunkAllocator BigChunksAllocator;
};