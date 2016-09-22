#pragma once
#include "memutils.h"
#include "logicsafety.h"
#include "builddefines.h"
#include "MemoryAllocator.h"

//allocates chunks >= 12bytes
class BigChunkAllocator : public MemoryAllocator
{
public:
    struct AllocHeader
    {
        size_t m_Size;
    };
    struct FreeBlock
    {
        void* m_Start;
        size_t m_Size;

        FreeBlock* m_Next;

        void* end() { return (uptr)m_Start + m_Size; }
    };

    typedef AllocHeader* AH_PTR; // AH = Allocate Header
    typedef FreeBlock*   FB_PTR; // FB = Free Block
    static const size_t AH_SZ = sizeof(AllocHeader);
    static const size_t FB_SZ = sizeof(FreeBlock);

    BigChunkAllocator();
    BigChunkAllocator(void* start, size_t size);
    ~BigChunkAllocator();

    virtual void* Allocate(size_t size, unsigned int al);
    virtual void Deallocate(void* p);

    void* GetStartPointer() { return m_Start; }
private:
    void* m_Start;
    size_t m_Size;

    FreeBlock* m_FreeBlock;
};
