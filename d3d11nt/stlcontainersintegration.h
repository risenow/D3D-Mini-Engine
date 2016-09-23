#pragma once
#include "SmallChunkAllocator.h"
#include "BigChunkAllocator.h"
#include "MemoryManager.h"
#include <vector>
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

//vector
template<class T>
#ifdef USE_CUSTOM_ALLOCATORS
using STLVector = std::vector<T, BigChunkAllocatorSTLWrapper<T>>;
#else
using STLVector = std::vector<T>;
#endif

//map
template<class K, class T, class C = std::less<K>>
#ifdef USE_CUSTOM_ALLOCATORS
using STLMap = std::map<K, T, C, BigChunkAllocatorSTLWrapper<std::pair<K, T>>>;
#else
using STLMap = std::map<K, T, C, std::allocator<std::pair<K, T>>>;
#endif