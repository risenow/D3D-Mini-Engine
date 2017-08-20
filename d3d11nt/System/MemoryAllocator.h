#pragma once

class MemoryAllocator
{
public:
    MemoryAllocator() : m_AllocatedMemorySize(0) {}

    virtual void* Allocate(size_t size = 1, unsigned long al = 1, unsigned long hint = 0) = 0;
    virtual void Deallocate(void* p) = 0;

    size_t GetAllocatedMemorySize() { return m_AllocatedMemorySize; }
protected:
    size_t m_AllocatedMemorySize;
};