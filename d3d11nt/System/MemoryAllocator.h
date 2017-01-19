#pragma once

class MemoryAllocator
{
public:
    MemoryAllocator() : m_AllocatedMemorySize(0) {}

    virtual void* Allocate(size_t size, unsigned int al) = 0;
    virtual void Deallocate(void* p) = 0;

    size_t GetAllocatedMemorySize() { return m_AllocatedMemorySize; }
protected:
    size_t m_AllocatedMemorySize;
};