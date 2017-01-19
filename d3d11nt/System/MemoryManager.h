#pragma once
#include "System/memutils.h"
#include "System/BigChunkAllocator.h"
#include "System/SmallChunkAllocator.h"
#include <mutex>

class MemoryManager
{
public:
    static void Initialize(size_t size);

    static SmallChunkAllocator* GetSmallChunkAllocator();
    static BigChunkAllocator* GetBigChunkAllocator();

    static size_t GetAllocatedMemorySize();
private:
    static SmallChunkAllocator* m_SmallChunkAllocator;
    static BigChunkAllocator*   m_BigChunkAllocator;
};

#define popNew(type) new (MemoryManager::GetBigChunkAllocator()->Allocate(sizeof(type), __alignof(type))) type

#define popDelete(p) (MemoryManager::GetBigChunkAllocator()->Deallocate(p))