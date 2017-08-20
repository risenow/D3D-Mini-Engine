#include "stdafx.h"
#include "System/PoolAllocator.h"
#include "System/logicsafety.h"
#include <cmath>

MemoryPoolPage::MemoryPoolPage(size_t objSize, count_t count) : m_ObjSize(objSize), m_ObjsMaxCount(count), m_OverallSize(objSize * count), m_Page(nullptr)
{
}
MemoryPoolPage::~MemoryPoolPage()
{
}

void MemoryPoolPage::Initialize()
{
	popAssert(!m_Page);
	m_Page = malloc(m_OverallSize); // mb it's worth to use our custom allocators

	InitializeObjPtrsStack();
}

void MemoryPoolPage::Finalize()
{
	popAssert(m_Page);
	free(m_Page);
	m_Page = nullptr;
}

bool MemoryPoolPage::Owns(void* p)
{
	if (p >= m_Page && p < ((uint8_t*)m_Page + m_OverallSize))
		return true;
	return false;
}

bool MemoryPoolPage::TryPush(void* p)
{
	if (!Owns(p))
		return false;
	m_ObjPtrs.push(p);
	return true;
}

void* MemoryPoolPage::TryPop()
{
	if (!m_ObjPtrs.size())
		return nullptr;
	void* p = m_ObjPtrs.top();
	m_ObjPtrs.pop();
	return p;
}

count_t MemoryPoolPage::GetObjsRemainCount()
{
	return m_ObjPtrs.size();
}

void MemoryPoolPage::InitializeObjPtrsStack()
{
	for (unsigned int i = 0; i < m_ObjsMaxCount; i++)
	{
		m_ObjPtrs.push((uint8_t*)m_Page + i*m_ObjSize);
	}
}




PoolAllocator::PoolAllocator(size_t objSize, count_t objsPerPageCount, count_t initialPagesCount, bool extendable) : m_ObjSize(objSize), m_ObjsPerPageCount(objsPerPageCount), m_InitialPagesCount(initialPagesCount), m_Extendable(extendable)
{
	for (unsigned long i = 0; i < m_InitialPagesCount; i++)
	{
		AddPage();
	}
}
PoolAllocator::~PoolAllocator()
{
	for (MemoryPoolPage& page : m_Pages)
	{
		page.Finalize();
	}
}


void* PoolAllocator::Allocate(size_t size, unsigned long al, unsigned long hint)
{
	bool allocated = false;
	void* result = nullptr;
	unsigned long i = 0;
	while (!result && i < m_Pages.size())
	{
		result = m_Pages[i].TryPop();
		i++;
	}

	popAssert((!result && m_Extendable) || result);

	if (!result)
	{
		AddPage();
		result = m_Pages[m_Pages.size() - 1].TryPop();
		popAssert(result);
	}

	return result;
}

void PoolAllocator::Deallocate(void* p)
{
	bool returnedToPool = false;
	unsigned long i = 0;
	while (!returnedToPool && i < m_Pages.size())
	{
		returnedToPool = m_Pages[i].TryPush(p);
		i++;
	}
	popAssert(returnedToPool);
}

void PoolAllocator::AddPage()
{
	MemoryPoolPage page(m_ObjSize, m_ObjsPerPageCount);
	page.Initialize();
	m_Pages.push_back(page);
}