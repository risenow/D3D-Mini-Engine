#pragma once

#include "System/memutils.h"
#include "System/logicsafety.h"
#include "System/MemoryAllocator.h"
#include "System/ProfileUnit.h"
#include <mutex>

//allocates chunks < 12bytes
class SmallChunkAllocator : public MemoryAllocator //we can only handle 1gb by one instance of this allocator
{
public:
    struct FreeBlock
    {
        static const unsigned int AL_MASK = 0b11 << 30; //two bits only for alignment
        static const unsigned int SZ_MASK = AL_MASK ^ std::numeric_limits<unsigned int>::max();

        FreeBlock() : m_Start(nullptr), m_Data(0), m_Next(nullptr)
        {}

        void* m_Start;
        size_t m_Data; //alignment encoded with size(reason why we can handle only 1gb)
        FreeBlock* m_Next;

        void SetAl(unsigned char al)
        {
            popAssert(al <= 3);

            m_Data &= SZ_MASK;
            m_Data |= (al << 30);
        }
        unsigned char GetAl()
        {
            return (unsigned char)(m_Data & AL_MASK);
        }

        void SetSize(size_t size)
        {
            popAssert(size <= SZ_MASK);

            m_Data &= AL_MASK;
            m_Data |= size;
        }
        size_t GetSize()
        {
            return m_Data & SZ_MASK;
        }

        void* end()
        {
            return (uptr)m_Start + GetSize();
        }
    };
    struct AllocHeader
    {
        static const int AL_MASK = 0b11110000;
        static const int SZ_MASK = 0b00001111;

        unsigned char GetSize()
        {
            return m_Data & SZ_MASK;
        }
        unsigned char GetAl()
        {
            return (m_Data & AL_MASK) >> 4;
        }

        void SetSize(unsigned char size)
        {
            popAssert(size <= SZ_MASK);
            m_Data &= AL_MASK;
            m_Data |= size;
        }
        void SetAl(unsigned char al)
        {
            popAssert(al <= 3);
            m_Data &= SZ_MASK;
            m_Data |= (al << 4);
        }

        unsigned char m_Data;
    };
    static const size_t FB_SZ = sizeof(FreeBlock);
    static const size_t AH_SZ = sizeof(AllocHeader);

    typedef FreeBlock*   FB_PTR;
    typedef AllocHeader* AH_PTR;

    SmallChunkAllocator();
    SmallChunkAllocator(void* start, size_t size);

    virtual void* Allocate(size_t size, unsigned int al);
    virtual void Deallocate(void* p);

    void* GetStartPointer() { return m_Start; }
private:

    FB_PTR AllocateFreeBlock(bool& isAllreadyInList)
    {
        popProfile(SmallChunkAllocatorAllocateFreeBlock);

        isAllreadyInList = false;
        FB_PTR currentFreeBlock = m_FreeBlock;

        while (currentFreeBlock)
        {
            unsigned int offset = 0;
            FB_PTR alingedBlock = (FB_PTR)align(currentFreeBlock->m_Start, __alignof(FreeBlock), offset);
            size_t overallSize = FB_SZ + offset;

            if (currentFreeBlock->GetSize() >= overallSize)
            {
                currentFreeBlock->m_Start = (uptr)currentFreeBlock->m_Start + overallSize;
                currentFreeBlock->SetSize(currentFreeBlock->GetSize() - overallSize);

                alingedBlock->SetAl(offset);

                m_AllocatedMemorySize += overallSize;

                return (FB_PTR)alingedBlock;
            }

            if (currentFreeBlock->GetSize() == 0)
            {
                isAllreadyInList = true;
                return currentFreeBlock;
            }

            currentFreeBlock = currentFreeBlock->m_Next;
        }

        popAssert(false);
        return nullptr;
    }

    FreeBlock* m_FreeBlock;
    void* m_Start;
    size_t m_Size;

    std::mutex m_Lock;
};