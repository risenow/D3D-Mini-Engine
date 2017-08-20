#pragma once
#include <mutex>
#include "System/memutils.h"
#include "System/logicsafety.h"
#include "System/builddefines.h"
#include "System/MemoryAllocator.h"

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

    virtual void* Allocate(size_t size = 1, unsigned long al = 1);
    virtual void Deallocate(void* p);

    void* GetStartPointer() { return m_Start; }
private:
#ifdef _DEBUG
    void WriteAllocationDebugInfo(size_t requestedAllocationSize, size_t realAllocatedSize, bool blockConsumed);
    void WriteDeallocationDebugInfo(size_t deallocatedSize);
#endif

    void* m_Start;
    size_t m_Size;

    FreeBlock* m_FreeBlock;

    std::mutex m_Lock;
};
