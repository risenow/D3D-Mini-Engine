#include "stdafx.h"
#include <algorithm>
#include "System/mtutils.h"
#include "System/NewBigChunkAllocator.h"
#include "System/ProfileUnit.h"

thread_local BigChunkAllocator* BigChunkSelfAllocatedPagesVectorContainer::m_ThisAllocator = nullptr;

BigChunkAllocator* GetThisAllocator()
{
	return BigChunkAllocator::m_ThisAllocator;
}

void BigChunkSelfAllocatedPagesVectorContainer::InitializePagesVector()
{
	m_ThisAllocator = (BigChunkAllocator*)this;
	m_Pages = (BigChunkPagesVector*)m_ThisAllocator->Allocate(sizeof(BigChunkPagesVector), __alignof(BigChunkPagesVector), BigChunksAllocatorHint_AllocateFromServiceSpace);
	new (m_Pages) BigChunkPagesVector();
}

BigChunkPagesVector& BigChunkSelfAllocatedPagesVectorContainer::GetPagesVector()
{
	m_ThisAllocator = (BigChunkAllocator*)this;
	return *m_Pages;
}

BigChunksPage::BigChunksPage() : m_Begin(nullptr), m_LargestFreeBlock(nullptr), m_Size(0), m_FreeSize(0)
{}
BigChunksPage::BigChunksPage(size_t pageSize)
{
	m_Begin = malloc(pageSize);
	m_End = (uptr)m_Begin + pageSize;

	m_Size = pageSize;

	m_HeadFreeBlock = (FB_PTR)m_Begin;
	m_HeadFreeBlock->m_Start = nullptr;
	m_HeadFreeBlock->m_Size = 0;
	m_HeadFreeBlock->m_Next = nullptr;
#ifdef FREEBLOCK_LIST_IS_DOUBLE_LINKED
	m_HeadFreeBlock->m_Prev = nullptr;
#endif

	FreeBlock* firstFreeBlock = m_HeadFreeBlock + 1;
	firstFreeBlock->m_Start = (uptr)firstFreeBlock + FB_SZ;
	firstFreeBlock->m_Size = m_Size - 2*FB_SZ;
	firstFreeBlock->m_Next = nullptr;

	m_LargestFreeBlock = firstFreeBlock;

	FreeBlock* dummy;
	InsertFreeBlock(firstFreeBlock, dummy);

#ifdef FREEBLOCK_LIST_IS_DOUBLE_LINKED
	firstFreeBlock->m_Prev = m_HeadFreeBlock;
#endif

	m_HeadFreeBlock->m_Next = firstFreeBlock;

	m_FreeSize = firstFreeBlock->m_Size;
}
bool BigChunksPage::CanAllocate(size_t size)
{
	return m_LargestFreeBlock->m_Size >= size;
	return true;
}
bool BigChunksPage::Owns(void* p)
{
	return (p >= m_Begin && p < m_End);
}

void* BigChunksPage::Allocate(size_t size) //we don't use al in this allocator since this allocator's algorithm produces only aligned by 4 addresses
{
	size_t overallSize = size + AH_SZ;

	//maintaining important invariant for this type of allocator
	//popAssert(overallSize >= FB_SZ);
	if (overallSize < FB_SZ)
		overallSize = FB_SZ;

	FB_PTR  prevFreeBlock = m_HeadFreeBlock;
	FB_PTR  currentFreeBlock = m_HeadFreeBlock->m_Next;
	while (currentFreeBlock)
	{
		if (currentFreeBlock->m_Size >= overallSize)
		{
			void* result = currentFreeBlock;
			FB_PTR movedFreeBlock = (FB_PTR)((uptr)result + overallSize);

			unsigned int offset = 0;
			movedFreeBlock = (FB_PTR)align((void*)movedFreeBlock, __alignof(FreeBlock), offset);

			if ((currentFreeBlock->m_Size - overallSize) >= offset)
			{
				(*movedFreeBlock) = (*currentFreeBlock);
				movedFreeBlock->m_Start = (uptr)movedFreeBlock + FB_SZ;
				movedFreeBlock->m_Size -= (overallSize + offset);

				prevFreeBlock->m_Next = movedFreeBlock;
#ifdef FREEBLOCK_LIST_IS_DOUBLE_LINKED
				if (movedFreeBlock->m_Next)
					movedFreeBlock->m_Next->m_Prev = movedFreeBlock;
#endif

				AH_PTR header = (AH_PTR)result;
				header->m_Size = overallSize - AH_SZ + offset; //size

				result = (uptr)result + AH_SZ;

#ifdef FREEBLOCK_LIST_IS_SORTED
				FixSortInvariantDueToFreeBlockSizeDecrease(movedFreeBlock, prevFreeBlock);
#endif

				//reinit largest FB since it now relocated/moved
				if (currentFreeBlock == m_LargestFreeBlock)
				{
					m_LargestFreeBlock = movedFreeBlock;
				}

#ifdef KEEP_TRACK_OF_LARGEST_FREEBLOCK
				m_LargestFreeBlock = SearchLargestFreeBlock();
#endif

				//m_AllocatedMemorySize += overallSize + offset;
				//DEBUG_ONLY(WriteAllocationDebugInfo(size, overallSize + offset, false));

				return result;
			}

			RemoveFreeBlock(currentFreeBlock, prevFreeBlock);

			AH_PTR header = (AH_PTR)result;
			header->m_Size = currentFreeBlock->m_Size + FB_SZ - AH_SZ;

			result = (uptr)result + AH_SZ;

			//m_AllocatedMemorySize += currentFreeBlock->m_Size + FB_SZ;
			//DEBUG_ONLY(WriteAllocationDebugInfo(size, currentFreeBlock->m_Size + FB_SZ, true));

			return result;
		}

		if ((currentFreeBlock->m_Size + FB_SZ) >= overallSize)
		{
			AH_PTR header = (AH_PTR)currentFreeBlock;
			header->m_Size = currentFreeBlock->m_Size + FB_SZ - AH_SZ; //give it all the memory that this block has

			RemoveFreeBlock(currentFreeBlock, prevFreeBlock);


			//m_AllocatedMemorySize += currentFreeBlock->m_Size + FB_SZ;
			//DEBUG_ONLY(WriteAllocationDebugInfo(size, currentFreeBlock->m_Size + FB_SZ, false));

			return (uptr)header + AH_SZ;
		}
		prevFreeBlock = currentFreeBlock;
		currentFreeBlock = currentFreeBlock->m_Next;
	}

	popAssert(false);
	return nullptr;
}

void BigChunksPage::Deallocate(void* p)
{
	AH_PTR header = (AH_PTR)((uptr)p - AH_SZ);
	size_t size = header->m_Size + AH_SZ;
	FB_PTR newFreeBlock = (FB_PTR)header;
	newFreeBlock->m_Size = size - FB_SZ;
	newFreeBlock->m_Start = (uptr)newFreeBlock + FB_SZ;
	newFreeBlock->m_Next = nullptr;

#ifdef FREEBLOCK_LIST_IS_DOUBLE_LINKED
	newFreeBlock->m_Prev = nullptr;
#endif
	//m_AllocatedMemorySize -= size;
	//DEBUG_ONLY(WriteDeallocationDebugInfo(size));

	//defragmentation
	FB_PTR beforeChangedFreeBlock = nullptr;
	FB_PTR prevFreeBlock = m_HeadFreeBlock;
	FB_PTR currentFreeBlock = m_HeadFreeBlock->m_Next;
	bool inList = false;
	while (currentFreeBlock)
	{
		if (currentFreeBlock->end() == (void*)newFreeBlock)
		{
			currentFreeBlock->m_Size += newFreeBlock->m_Size + FB_SZ;

			if (inList)
			{
				popAssert(beforeChangedFreeBlock);
				RemoveFreeBlock(newFreeBlock, beforeChangedFreeBlock);

				//not tested
				return;
			}

			beforeChangedFreeBlock = prevFreeBlock;
			newFreeBlock = currentFreeBlock;
			inList = true;
		}
		if (newFreeBlock->end() == currentFreeBlock)
		{
			newFreeBlock->m_Size += currentFreeBlock->m_Size + FB_SZ;

			RemoveFreeBlock(currentFreeBlock, prevFreeBlock);

			if (!inList)
			{
				InsertFreeBlock(newFreeBlock, beforeChangedFreeBlock);
			}
			else
			{
				beforeChangedFreeBlock = prevFreeBlock;
				return;
			}
			//not tested

			
			inList = true;
		}

		prevFreeBlock = currentFreeBlock;
		currentFreeBlock = currentFreeBlock->m_Next;
	}

	if (!inList)
	{
		popAssert(!newFreeBlock->m_Next);
#ifdef FREEBLOCK_LIST_IS_DOUBLE_LINKED
		popAssert(!newFreeBlock->m_Prev);
#endif
		FreeBlock* dummy;
		InsertFreeBlock(newFreeBlock, dummy);
	}
	
	if (newFreeBlock->m_Size > m_LargestFreeBlock->m_Size)
	{
		m_LargestFreeBlock = newFreeBlock;
	}
}


BigChunkAllocator::BigChunkAllocator()// : m_Start(nullptr), m_Size(0)
{}
BigChunkAllocator::BigChunkAllocator(size_t pageSize, count_t pagesInitialCount, ExtendMode extendMode, bool multithreadEnabled) : m_ExtendMode(extendMode), m_MutithreadEnabled(multithreadEnabled)
{
	m_ServiceSpacePage = BigChunksPage(512 BYTES);

	InitializePagesVector();

	BigChunkPagesVector& pagesVector = GetPagesVector();
	pagesVector.reserve(pagesInitialCount);
	for (unsigned long i = 0; i < pagesInitialCount; i++)
	{
		pagesVector.push_back(BigChunksPage(pageSize));
	}
}

BigChunkAllocator::~BigChunkAllocator()
{
}

void* BigChunkAllocator::Allocate(size_t size, unsigned long al, unsigned long hint) //we don't use al in this allocator since this allocator's algorithm produces only aligned by 4 addresses
{
	DynamicConditionalLockGuard autoLock(m_Lock, m_MutithreadEnabled);

	if (hint & BigChunksAllocatorHint_AllocateFromServiceSpace)
	{
		void* r = m_ServiceSpacePage.Allocate(size);
		popAssert(r);
		return r;
	}

	BigChunkPagesVector& pagesVector = GetPagesVector();
	for (BigChunksPage& page : pagesVector)
	{
		if (page.CanAllocate(size))
			return page.Allocate(size);
	}

	popAssert(false);
	
	if (m_ExtendMode == ExtendMode_Disabled)
	{
		popAssert(false);
		return nullptr;
	}

	if (m_ExtendMode == ExtendMode_AllowThroughRegularPages && size > m_RegularPageSize)
	{
		popAssert(false);
		return nullptr;
	}

	pagesVector.push_back(BigChunksPage(std::max(size, m_RegularPageSize)));
	void* r = pagesVector[pagesVector.size() - 1].Allocate(size);
	popAssert(r);
	return r;
}

void BigChunkAllocator::Deallocate(void* p)
{
	DynamicConditionalLockGuard autoLock(m_Lock, m_MutithreadEnabled);

	BigChunkPagesVector& pagesVector = GetPagesVector();
	for (BigChunksPage& page : pagesVector)
	{
		if (page.Owns(p))
		{
			page.Deallocate(p);
			return;
		}
	}

	popAssert(false);
}

#ifdef _DEBUG
void BigChunkAllocator::WriteAllocationDebugInfo(size_t requestedAllocationSize, size_t realAllocatedSize, bool blockConsumed)
{
	std::string blockConsumeInfoStr = ", block consumed: ";
	if (blockConsumed)
		blockConsumeInfoStr += "yes";
	else
		blockConsumeInfoStr += "no";

	LOG_CONS(std::string("Big Chunk Allocator allocation, requested size: " + std::to_string(requestedAllocationSize) +
		" real allocated size: " + std::to_string(realAllocatedSize) +
		", free memory: " + BytesNumberToFormattedString(m_Size - m_AllocatedMemorySize) + blockConsumeInfoStr));
}

void BigChunkAllocator::WriteDeallocationDebugInfo(size_t deallocatedSize)
{
	LOG_CONS(std::string("Big Chunk Allocator deallocation, size: " + std::to_string(deallocatedSize) +
		", free memory: " + BytesNumberToFormattedString(m_Size - m_AllocatedMemorySize)));
}
#endif // _DEBUG

