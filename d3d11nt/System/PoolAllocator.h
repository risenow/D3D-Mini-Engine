#pragma once

#include "System/MemoryAllocator.h"
#include "System/stlcontainersintegration.h"
#include "System/typesalias.h"
#include <memory>

class MemoryPoolPage
{
public:
	MemoryPoolPage(size_t objSize, count_t count);
	~MemoryPoolPage();

	void Initialize(); // not unified solution // mb it's more appr solution to create instances by new op
	void Finalize();

	bool Owns(void* p);
	bool TryPush(void* p);
	void* TryPop();

	count_t GetObjsRemainCount();

private:
	void InitializeObjPtrsStack();

	size_t m_ObjSize;
	count_t m_ObjsMaxCount;
	size_t m_OverallSize;
	void* m_Page;

	STLStack<void*> m_ObjPtrs;
};

class PoolAllocator : public MemoryAllocator
{
public:
	PoolAllocator(size_t objSize, count_t objsPerPageCount, count_t initialPagesCount, bool extendable);
	~PoolAllocator();

	void* Allocate(size_t size = 1, unsigned long al = 1, unsigned long hint = 0);
	void Deallocate(void* p);
private:
	void AddPage();

	size_t m_ObjSize;
	count_t m_ObjsPerPageCount;
	count_t m_InitialPagesCount;
	bool m_Extendable;

	STLVector<MemoryPoolPage> m_Pages;
};