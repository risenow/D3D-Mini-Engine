#pragma once
#include "SmallChunkAllocator.h"
#include "BigChunkAllocator.h"
#include "MemoryManager.h"
#include <vector>

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
        typedef AllocatorSTLWrapper<T1> other;
    };

    AllocatorSTLWrapper() : m_RawAllocator(&MemoryManager::BigChunksAllocator)
    {}

    AllocatorSTLWrapper(BigChunkAllocator* all) : m_RawAllocator(all)
    {}

    pointer allocate(size_type n, const void *hint = 0)
    {
        //assert(sizeof(T) >= 12);
        assert(m_RawAllocator);
        return (T*)m_RawAllocator->Allocate(n*sizeof(T));
    }

    void deallocate(pointer p, size_type n)
    {
        assert(m_RawAllocator);
        m_RawAllocator->Deallocate(p);
    }


    AllocatorSTLWrapper(const AllocatorSTLWrapper &a) : m_RawAllocator(a.m_RawAllocator) { }
    template <class U>
    AllocatorSTLWrapper(const AllocatorSTLWrapper<U> &a) : m_RawAllocator(a.m_RawAllocator) { }
    ~AllocatorSTLWrapper() { }
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
        typedef AllocatorSTLWrapper<T1> other;
    };

    AllocatorSTLWrapper() : m_RawAllocator(nullptr)
    {}

    AllocatorSTLWrapper(BigChunkAllocator* all) : m_RawAllocator(all)
    {}

    pointer allocate(size_type n, const void *hint = 0)
    {
        size_t allocationSize = sizeof(T) * n;
        assert(allocationSize < 12);
        assert(m_RawAllocator);
        return (T*)m_RawAllocator->Allocate(allocationSize, __alignof(T));
    }

    void deallocate(pointer p, size_type n)
    {
        assert(m_RawAllocator);
        m_RawAllocator->Deallocate(p);
    }


    AllocatorSTLWrapper(const AllocatorSTLWrapper &a) : m_RawAllocator(a.m_RawAllocator) { }
    template <class U>
    AllocatorSTLWrapper(const AllocatorSTLWrapper<U> &a) : m_RawAllocator(a.m_RawAllocator) { }
    ~AllocatorSTLWrapper() { }
private:
    SmallChunkAllocator* m_RawAllocator;
};

template<class T>
typedef std::vector<T, BigChunkAllocatorSTLWrapper<T>> STLVector;