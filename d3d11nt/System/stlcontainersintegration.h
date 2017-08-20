#pragma once
//#include "System/SmallChunkAllocator.h"
//#include "System/NewBigChunkAllocator.h"
//#include "System/MemoryManager.h"
#include <vector>
#include <deque>
#include <stack>
#include <map>
#include "System/logicsafety.h"

class BigChunkAllocator;
class SmallChunkAllocator;

namespace AllocatorWrapperUtils
{
	BigChunkAllocator* GetBigChunkAllocatorFromMemoryManager();
	SmallChunkAllocator* GetSmallChunkAllocatorFromMemoryManager();

	// get rid of BigChunkAllocator dependency
	void* AllocateBy(BigChunkAllocator* allocator, size_t size, unsigned long al, unsigned long hint);
	void DeallocateBy(BigChunkAllocator* allocator, void* p);

	// get rid of SmallChunkAllocator dependency
	void* AllocateBy(SmallChunkAllocator* allocator, size_t size, unsigned long al, unsigned long hint);
	void DeallocateBy(SmallChunkAllocator* allocator, void* p);
}

template<class T, unsigned long H, BigChunkAllocator* (*GetAllocF)()>
class BigChunkAllocatorSTLWrapper : public std::allocator<T>
{
public:
    typedef size_t size_type;
    typedef T* pointer;
    typedef const T* const_pointer;

    template<typename T1>
    struct rebind
    {
        typedef BigChunkAllocatorSTLWrapper<T1, H, GetAllocF> other;
    };

    BigChunkAllocatorSTLWrapper() : m_RawAllocator(GetAllocF()), m_Hint(H)
    {}

    BigChunkAllocatorSTLWrapper(BigChunkAllocator* all) : m_RawAllocator(all)
    {}

    pointer allocate(size_type n, const void *hint = 0)
    {
        popAssert(m_RawAllocator);
		return (T*)AllocatorWrapperUtils::AllocateBy(m_RawAllocator, n*sizeof(T), __alignof(T), H);
    }

    void deallocate(pointer p, size_type n)
    {
        popAssert(m_RawAllocator);
		AllocatorWrapperUtils::DeallocateBy(m_RawAllocator, p);
    }


    BigChunkAllocatorSTLWrapper(const BigChunkAllocatorSTLWrapper<T, H, GetAllocF> &a) : m_RawAllocator(a.GetRawAllocator())  { }
    template <class U>
    BigChunkAllocatorSTLWrapper(const BigChunkAllocatorSTLWrapper<U, H, GetAllocF> &a) : m_RawAllocator(a.GetRawAllocator()) { }
    ~BigChunkAllocatorSTLWrapper() { }

	BigChunkAllocator* GetRawAllocator() const { return m_RawAllocator; }

private:
	unsigned long m_Hint;
    BigChunkAllocator* m_RawAllocator;
};

template<class T>
class SmallChunkAllocatorSTLWrapper : public std::allocator<T>
{
public:
    typedef size_t size_type;
    typedef T* pointer;
    typedef const T* const_pointer;

    template<typename T1>
    struct rebind
    {
        typedef SmallChunkAllocatorSTLWrapper<T1> other;
    };

    SmallChunkAllocatorSTLWrapper() : m_RawAllocator(AllocatorWrapperUtils::GetSmallChunkAllocatorFromMemoryManager())
    {}

    SmallChunkAllocatorSTLWrapper(BigChunkAllocator* all) : m_RawAllocator(all)
    {}

    pointer allocate(size_type n, const void *hint = 0)
    {
        size_t allocationSize = sizeof(T) * n;
        popAssert(allocationSize < 12);
        popAssert(m_RawAllocator);
		return (T*)AllocatorWrapperUtils::AllocateBy(m_RawAllocator, allocationSize, __alignof(T), 0);
    }

    void deallocate(pointer p, size_type n)
    {
        popAssert(m_RawAllocator);
		AllocatorWrapperUtils::DeallocateBy(m_RawAllocator, p);
    }


    SmallChunkAllocatorSTLWrapper(const SmallChunkAllocatorSTLWrapper<T> &a) : m_RawAllocator(a.GetRawAllocator()) { }
    template <class U>
    SmallChunkAllocatorSTLWrapper(const SmallChunkAllocatorSTLWrapper<U> &a) : m_RawAllocator(a.GetRawAllocator()) { }
    ~SmallChunkAllocatorSTLWrapper() { }

    SmallChunkAllocator* GetRawAllocator() const { return m_RawAllocator; }
private:
    SmallChunkAllocator* m_RawAllocator;
};

template<class T>
using DefaultBigChunkAllocatorSTLWrapper = BigChunkAllocatorSTLWrapper<T, 0, AllocatorWrapperUtils::GetBigChunkAllocatorFromMemoryManager>;

//deque
template<class T, class A =
#ifdef USE_CUSTOM_ALLOCATORS
	DefaultBigChunkAllocatorSTLWrapper<T>
#else
    std::allocator<T>
#endif
>
using STLDeque = std::deque<T, A>;

//stack
template<class T, class C = STLDeque<T>>
using STLStack = std::stack<T, C>;

//vector
template<class T, class A =
#ifdef USE_CUSTOM_ALLOCATORS
	DefaultBigChunkAllocatorSTLWrapper<T>
#else
    std::allocator<T>
#endif
>
using STLVector = std::vector<T, A>;

//map
template<class K, class T, class C = std::less<K>, class A =
#ifdef USE_CUSTOM_ALLOCATORS
	DefaultBigChunkAllocatorSTLWrapper<std::pair<K, T>>
#else
    std::allocator<std::pair<K, T>>
#endif
>
using STLMap = std::map<K, T, C, A>;

//string
template<class T, class Tr = std::char_traits<T>, class A =
#ifdef USE_CUSTOM_ALLOCATORS
	DefaultBigChunkAllocatorSTLWrapper<T>
#else
    std::allocator<T>
#endif
>
using STLBasicString = std::basic_string<T, Tr, A>;

typedef STLBasicString<char>    STLString;
typedef STLBasicString<wchar_t> STLWString;