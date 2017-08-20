#pragma once
#pragma once
#include <mutex>
#include "System/typesalias.h"
#include "System/memutils.h"
#include "System/logicsafety.h"
#include "System/builddefines.h"
#include "System/stlcontainersintegration.h"
#include "System/MemoryAllocator.h"

//This allocator is very cache unfriendly
//TODO:
//disconnect free space and free block structures space, allocate free block structures as pools with effective relatively to cache size
//@risenow

#define ALLOCATOR_IS_GROWABLE 1

#define FREEBLOCK_LIST_SIZE_DECREASE_SORT_ORDER 1 //provides faster allocation
#define FREEBLOCK_LIST_SIZE_INCREASE_SORT_ORDER 2 //provides lower fragmentation

//#define FREEBLOCK_LIST_IS_SORTED FREEBLOCK_LIST_SIZE_DECREASE_SORT_ORDER

#if (FREEBLOCK_LIST_IS_SORTED == FREEBLOCK_LIST_SIZE_INCREASE_SORT_ORDER)
#define FREEBLOCK_LIST_IS_DOUBLE_LINKED
#endif

#ifdef FREEBLOCK_LIST_IS_SORTED
#define FREEBLOCK_LIST_BINARY_TREE_OPTIMIZATION //not implemented
#endif

#if ((!FREEBLOCK_LIST_IS_SORTED) && (ALLOCATOR_IS_GROWABLE))
//#define KEEP_TRACK_OF_LARGEST_FREEBLOCK
#endif

class BigChunksPage
{
public:
	struct AllocHeader
	{
		size_t m_Size;
	};
	struct FreeBlock
	{
		FreeBlock() : m_Start(nullptr), m_Size(0), m_Next(nullptr)
#ifdef FREEBLOCK_LIST_IS_DOUBLE_LINKED
			, m_Prev(nullptr)
#endif
		{}

		void* m_Start;
		size_t m_Size;

		FreeBlock* m_Next;

#ifdef FREEBLOCK_LIST_IS_DOUBLE_LINKED
		FreeBlock* m_Prev;
#endif

		void* end() { return (uptr)m_Start + m_Size; }
	};

	typedef AllocHeader* AH_PTR; // AH = Allocate Header
	typedef FreeBlock*  FB_PTR; // FB = Free Block
	static const size_t AH_SZ = sizeof(AllocHeader);
	static const size_t FB_SZ = sizeof(FreeBlock);

	BigChunksPage();
	BigChunksPage(size_t pageSize);

	bool CanAllocate(size_t size);
	bool Owns(void* p);

	void* Allocate(size_t size);
	void Deallocate(void* p);
private:
	void* m_Begin;
	void* m_End;
	size_t m_Size;
	size_t m_FreeSize;

	FreeBlock* m_LargestFreeBlock;

	FreeBlock* m_HeadFreeBlock;

#ifdef FREEBLOCK_LIST_IS_SORTED
	inline void FixSortInvariantDueToFreeBlockSizeDecrease(FreeBlock* decreasedSizeFreeBlock, FreeBlock* beforeDecreasedSizeFreeBlock)
	{
#if (FREEBLOCK_LIST_IS_SORTED == FREEBLOCK_LIST_SIZE_DECREASE_SORT_ORDER)
		FreeBlock* currentFreeBlock = decreasedSizeFreeBlock;
		while (currentFreeBlock->m_Next && (currentFreeBlock->m_Size < currentFreeBlock->m_Next->m_Size))
			currentFreeBlock = currentFreeBlock->m_Next;

		if (currentFreeBlock != decreasedSizeFreeBlock)
		{
			RemoveFreeBlock(decreasedSizeFreeBlock, beforeDecreasedSizeFreeBlock);
			InsertFreeBlockAfter(decreasedSizeFreeBlock, currentFreeBlock);
		}
#elif (FREEBLOCK_LIST_IS_SORTED == FREEBLOCK_LIST_SIZE_INCREASE_SORT_ORDER)
#ifndef FREEBLOCK_LIST_IS_DOUBLE_LINKED
		static_assert(false, "FREEBLOCK LIST MUST BE DOUBLE LINKED TO MAINTAIN INCREASE SORT ORDER INVARIANT");
#else
		FreeBlock* currentFreeBlock = decreasedSizeFreeBlock;
		while ((currentFreeBlock->m_Prev != m_HeadFreeBlock) && (currentFreeBlock->m_Size < currentFreeBlock->m_Prev->m_Size))
			currentFreeBlock = currentFreeBlock->m_Prev;

		if (currentFreeBlock != decreasedSizeFreeBlock)
		{
			RemoveFreeBlock(decreasedSizeFreeBlock, beforeDecreasedSizeFreeBlock);
			InsertFreeBlockAfter(decreasedSizeFreeBlock, currentFreeBlock->m_Prev);
		}
#endif
#endif
	}
#endif

#ifdef KEEP_TRACK_OF_LARGEST_FREEBLOCK
	inline FreeBlock* SearchLargestFreeBlock()
	{
		FreeBlock* currentFreeBlock = m_HeadFreeBlock->m_Next;
		FreeBlock* prevFreeBlock = m_HeadFreeBlock;

		FreeBlock* largestFreeBlock = nullptr;

		while (currentFreeBlock)
		{
			if (currentFreeBlock->m_Size >= prevFreeBlock->m_Size)
				largestFreeBlock = currentFreeBlock;
			currentFreeBlock = currentFreeBlock->m_Next;
		}

		return largestFreeBlock;
	}
#endif // KEEP_TRACK_OF_LARGEST_FREEBLOCK

	inline void RemoveFreeBlock(FreeBlock* freeBlock, FreeBlock* prevFreeBlock)
	{

		prevFreeBlock->m_Next = freeBlock->m_Next;
#ifdef FREEBLOCK_LIST_IS_DOUBLE_LINKED
		if (freeBlock->m_Next)
			freeBlock->m_Next->m_Prev = prevFreeBlock;
#endif // FREEBLOCK_LIST_IS_DOUBLE_LINKED

#ifdef KEEP_TRACK_OF_LARGEST_FREEBLOCK
		if (freeBlock == m_LargestFreeBlock)
			m_LargestFreeBlock = SearchLargestFreeBlock();
#endif // KEEP_TRACK_OF_LARGEST_FREEBLOCK
	}


	inline void InsertFreeBlockAfter(FreeBlock* freeBlock, FreeBlock* baseFreeBlock)
	{
		freeBlock->m_Next = baseFreeBlock->m_Next;
#ifdef FREEBLOCK_LIST_IS_DOUBLE_LINKED
		if (freeBlock->m_Next)
			freeBlock->m_Next->m_Prev = freeBlock;
		freeBlock->m_Prev = baseFreeBlock;
#endif // FREEBLOCK_LIST_IS_DOUBLE_LINKED
		baseFreeBlock->m_Next = freeBlock;
	}

	inline void InsertFreeBlock(FreeBlock* freeBlock, FreeBlock*& foundBaseFreeBlock) //base = prev //newBaseFreeBlock is needed since we are still maintaining not double linked free block list
	{
		FreeBlock* currentFreeBlock = m_HeadFreeBlock->m_Next;
		FreeBlock* prevFreeBlock = m_HeadFreeBlock;

#ifdef KEEP_TRACK_OF_LARGEST_FREEBLOCK
		if (freeBlock->m_Size > m_LargestFreeBlock->m_Size)
		{
			currentFreeBlock = nullptr;
			prevFreeBlock = m_LargestFreeBlock;

			m_LargestFreeBlock = freeBlock;
		}
#endif // KEEP_TRACK_OF_LARGEST_FREEBLOCK

#ifdef FREEBLOCK_LIST_IS_SORTED

#if (FREEBLOCK_LIST_IS_SORTED == FREEBLOCK_LIST_SIZE_INCREASE_SORT_ORDER)
#define FREEBLOCK_LIST_SORT_COMPARE_CHAR <=
#elif (FREEBLOCK_LIST_IS_SORTED == FREEBLOCK_LIST_SIZE_DECREASE_SORT_ORDER) // (FREEBLOCK_LIST_IS_SORTED == FREEBLOCK_LIST_SIZE_INCREASE_SORT_ORDER)
#define FREEBLOCK_LIST_SORT_COMPARE_CHAR >=
#endif // (FREEBLOCK_LIST_IS_SORTED == FREEBLOCK_LIST_SIZE_DECREASE_SORT_ORDER)

		while (currentFreeBlock)
		{
			if (freeBlock->m_Size FREEBLOCK_LIST_SORT_COMPARE_CHAR currentFreeBlock->m_Size)
				break;

			prevFreeBlock = currentFreeBlock;
			currentFreeBlock = currentFreeBlock->m_Next;
		}
#endif // FREEBLOCK_LIST_IS_SORTED
		
#if (FREEBLOCK_LIST_IS_SORTED == FREEBLOCK_LIST_SIZE_INCREASE_SORT_ORDER)
		if (!currentFreeBlock)
			m_LargestFreeBlock = freeBlock;
#endif // (FREEBLOCK_LIST_IS_SORTED == FREEBLOCK_LIST_SIZE_INCREASE_SORT_ORDER)

		InsertFreeBlockAfter(freeBlock, prevFreeBlock);

		foundBaseFreeBlock = prevFreeBlock;
#if (FREEBLOCK_LIST_IS_SORTED == FREEBLOCK_LIST_SIZE_DECREASE_SORT_ORDER)
		m_LargestFreeBlock = m_HeadFreeBlock->m_Next;
#endif // (FREEBLOCK_LIST_IS_SORTED == FREEBLOCK_LIST_SIZE_DECREASE_SORT_ORDER)
	}
};

class BigChunkAllocator;

BigChunkAllocator* GetThisAllocator();

enum BigChunksAllocatorHint
{
	BigChunksAllocatorHint_AllocateFromServiceSpace = 1u << 0
};

typedef std::vector<BigChunksPage, BigChunkAllocatorSTLWrapper<BigChunksPage, BigChunksAllocatorHint_AllocateFromServiceSpace, GetThisAllocator>> BigChunkPagesVector;

//needed to incapsulate allocate hack to prevent wrong self allocated pages vector usage
//every manipulation of pages vector must be done only through refering to it by GetPagesVector function
class BigChunkSelfAllocatedPagesVectorContainer
{
	friend BigChunkAllocator* GetThisAllocator();
public:
	void InitializePagesVector();

	BigChunkPagesVector& GetPagesVector();

	BigChunkPagesVector* m_Pages;// do not forget to move it back to private
private:
	thread_local static BigChunkAllocator* m_ThisAllocator;

	
};

//allocates chunks >= 12bytes
class BigChunkAllocator : public MemoryAllocator, public BigChunkSelfAllocatedPagesVectorContainer
{
public:
	//grow ability 
	enum ExtendMode
	{
		ExtendMode_Disabled,
		ExtendMode_AllowThroughRegularPages,
		ExtendMode_AllowThroughIrregularPages
	};

	BigChunkAllocator();
	BigChunkAllocator(size_t pageSize, count_t pagesInitialCount, ExtendMode extendMode, bool multithreadEnabled);
	~BigChunkAllocator();

	virtual void* Allocate(size_t size = 1, unsigned long al = 1, unsigned long hint = 0);
	virtual void Deallocate(void* p);

	//void* GetStartPointer() { return m_Start; }
private:
#ifdef _DEBUG
	void WriteAllocationDebugInfo(size_t requestedAllocationSize, size_t realAllocatedSize, bool blockConsumed);
	void WriteDeallocationDebugInfo(size_t deallocatedSize);
#endif

	BigChunksPage m_ServiceSpacePage;

	ExtendMode m_ExtendMode;

	bool m_MutithreadEnabled;
	std::mutex m_Lock;

	//void* m_Start;
	size_t m_Size;

	size_t m_RegularPageSize;

	//std::vector<BigChunksPage, BigChunkAllocatorSTLWrapper<BigChunksPage, Hint_AllocateFromServiceSpace, GetThisAllocator>> m_Pages;
};

