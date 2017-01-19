#pragma once
#include "System/SmallChunkAllocator.h"
#include "System/BigChunkAllocator.h"
#include "System/MemoryManager.h"
#include <vector>
#include <deque>
#include <stack>
#include <map>

template<class T>
class BigChunkAllocatorSTLWrapper : public std::allocator<T>
{
public:
    typedef size_t size_type;
    typedef T* pointer;
    typedef const T* const_pointer;

    template<typename T1>
    struct rebind
    {
        typedef BigChunkAllocatorSTLWrapper<T1> other;
    };

    BigChunkAllocatorSTLWrapper() : m_RawAllocator(MemoryManager::GetBigChunkAllocator())
    {}

    BigChunkAllocatorSTLWrapper(BigChunkAllocator* all) : m_RawAllocator(all)
    {}

    pointer allocate(size_type n, const void *hint = 0)
    {
        //assert(sizeof(T) >= 12);
        popAssert(m_RawAllocator);
        return (T*)m_RawAllocator->Allocate(n*sizeof(T), __alignof(T));
    }

    void deallocate(pointer p, size_type n)
    {
        popAssert(m_RawAllocator);
        m_RawAllocator->Deallocate(p);
    }


    BigChunkAllocatorSTLWrapper(const BigChunkAllocatorSTLWrapper<T> &a) : m_RawAllocator(a.GetRawAllocator()) { }
    template <class U>
    BigChunkAllocatorSTLWrapper(const BigChunkAllocatorSTLWrapper<U> &a) : m_RawAllocator(a.GetRawAllocator()) { }
    ~BigChunkAllocatorSTLWrapper() { }

    BigChunkAllocator* GetRawAllocator() const { return m_RawAllocator; }
private:
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

    SmallChunkAllocatorSTLWrapper() : m_RawAllocator(MemoryManager::GetSmallChunkAllocator())
    {}

    SmallChunkAllocatorSTLWrapper(BigChunkAllocator* all) : m_RawAllocator(all)
    {}

    pointer allocate(size_type n, const void *hint = 0)
    {
        size_t allocationSize = sizeof(T) * n;
        popAssert(allocationSize < 12);
        popAssert(m_RawAllocator);
        return (T*)m_RawAllocator->Allocate(allocationSize, __alignof(T));
    }

    void deallocate(pointer p, size_type n)
    {
        popAssert(m_RawAllocator);
        m_RawAllocator->Deallocate(p);
    }


    SmallChunkAllocatorSTLWrapper(const SmallChunkAllocatorSTLWrapper<T> &a) : m_RawAllocator(a.GetRawAllocator()) { }
    template <class U>
    SmallChunkAllocatorSTLWrapper(const SmallChunkAllocatorSTLWrapper<U> &a) : m_RawAllocator(a.GetRawAllocator()) { }
    ~SmallChunkAllocatorSTLWrapper() { }

    SmallChunkAllocator* GetRawAllocator() const { return m_RawAllocator; }
private:
    SmallChunkAllocator* m_RawAllocator;
};

//deque
template<class T, class A =
#ifdef USE_CUSTOM_ALLOCATORS
    BigChunkAllocatorSTLWrapper<T>
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
    BigChunkAllocatorSTLWrapper<T>
#else
    std::allocator<T>
#endif
>
using STLVector = std::vector<T, A>;

//map
template<class K, class T, class C = std::less<K>, class A =
#ifdef USE_CUSTOM_ALLOCATORS
    BigChunkAllocatorSTLWrapper<std::pair<K, T>>
#else
    std::allocator<std::pair<K, T>>
#endif
>
using STLMap = std::map<K, T, C, A>;

//string
template<class T, class Tr = std::char_traits<T>, class A =
#ifdef USE_CUSTOM_ALLOCATORS
    BigChunkAllocatorSTLWrapper<T>
#else
    std::allocator<T>
#endif
>
using STLBasicString = std::basic_string<T, Tr, A>;

typedef STLBasicString<char>    STLString;
typedef STLBasicString<wchar_t> STLWString;