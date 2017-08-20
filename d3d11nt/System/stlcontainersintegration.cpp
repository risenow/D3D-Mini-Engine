#include "stdafx.h"
#include "System/MemoryManager.h"
#include "System/stlcontainersintegration.h"


// get rid of MemoryManager dependency
BigChunkAllocator* AllocatorWrapperUtils::GetBigChunkAllocatorFromMemoryManager()
{
	return MemoryManager::GetBigChunkAllocator();
}
SmallChunkAllocator* AllocatorWrapperUtils::GetSmallChunkAllocatorFromMemoryManager()
{
	return MemoryManager::GetSmallChunkAllocator();
}

// get rid of BigChunkAllocator dependency
void* AllocatorWrapperUtils::AllocateBy(BigChunkAllocator* allocator, size_t size, unsigned long al, unsigned long hint)
{
	return allocator->Allocate(size, al, hint);
}
void AllocatorWrapperUtils::DeallocateBy(BigChunkAllocator* allocator, void* p)
{
	allocator->Deallocate(p);
}

// get rid of SmallChunkAllocator dependency
void* AllocatorWrapperUtils::AllocateBy(SmallChunkAllocator* allocator, size_t size, unsigned long al, unsigned long hint)
{
	return allocator->Allocate(size, al, hint);
}
void AllocatorWrapperUtils::DeallocateBy(SmallChunkAllocator* allocator, void* p)
{
	allocator->Deallocate(p);
}