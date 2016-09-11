#pragma once
#include "memutils.h"
#include "logicsafety.h"

//allocates chunks >= 12bytes
class BigChunkAllocator
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

    typedef AllocHeader* AH_PTR;
    typedef FreeBlock*   FB_PTR;
    static const size_t AH_SZ = sizeof(AllocHeader);
    static const size_t FB_SZ = sizeof(FreeBlock);

    BigChunkAllocator();
    BigChunkAllocator(void* start, size_t size);
    ~BigChunkAllocator();


    void* Allocate(size_t size);
    void Deallocate(void* p);

    void* GetStartPointer() { return m_Start; }
private:
    void* m_Start;
    size_t m_Size;

    FreeBlock* m_FreeBlock;
};
