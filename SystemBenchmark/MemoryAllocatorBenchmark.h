#pragma once
#include <string>
#include "Benchmark.h"
#include "System/memutils.h"
#include "System/PoolAllocator.h"
#include "System/NewBigChunkAllocator.h"
#include "System/SmallChunkAllocator.h"

static const unsigned int PreAllocatedSize = 360 MB;
static const unsigned int NumAllocatedChunks = 450000;
static const unsigned int AllocatedChunkSize = 400;

class CustomMemoryAllocationBenchmark : public Benchmark
{
public:
    CustomMemoryAllocationBenchmark(const std::string& name) : Benchmark(name) {}
    ~CustomMemoryAllocationBenchmark() {}

    virtual void Run()
    {
		BigChunkAllocator bigChunkAllocator(PreAllocatedSize, 1, BigChunkAllocator::ExtendMode_Disabled, false);

        void** allocatedChunks = new void*[NumAllocatedChunks];

		BigChunksPage& allocatorPage = (bigChunkAllocator.GetPagesVector())[0];

		BeginMeasure();
        for (int i = 0; i < NumAllocatedChunks; i++)
        {
			popProfile(BigChunkAllocatorAllocate);
            /*allocatedChunks[i] = *///bigChunkAllocator.Allocate(AllocatedChunkSize, 1);
			allocatorPage.Allocate(AllocatedChunkSize);
			//popAssert(allocatedChunks[i]);
        }
        EndMeasure();

        delete[] allocatedChunks;
    }
private:
};

class NormalMemoryAllocationBenchmark : public Benchmark
{
public:
    NormalMemoryAllocationBenchmark(const std::string& name) : Benchmark(name) {}
    ~NormalMemoryAllocationBenchmark() {}

    virtual void Run()
    {
        void** allocatedChunks = new void*[NumAllocatedChunks];
        
		BeginMeasure();
        for (int i = 0; i < NumAllocatedChunks; i++)
        {
            allocatedChunks[i] = malloc(AllocatedChunkSize);
			popAssert(allocatedChunks[i]);
        }
        EndMeasure();

        for (int i = 0; i < NumAllocatedChunks; i++)
        {
            free(allocatedChunks[i]);
        }
    }
private:
};

class CustomMemoryDeallocationBenchmark : public Benchmark
{
public:
    CustomMemoryDeallocationBenchmark(const std::string& name) : Benchmark(name) {}
    ~CustomMemoryDeallocationBenchmark() {}

    virtual void Run()
    {
        void* preAllocatedMemoryP = malloc(PreAllocatedSize);
        BigChunkAllocator bigChunkAllocator(PreAllocatedSize, 1, BigChunkAllocator::ExtendMode_Disabled, false);

		BigChunksPage& allocatorPage = (bigChunkAllocator.GetPagesVector())[0];

        void** allocatedChunks = new void*[NumAllocatedChunks];

        for (int i = 0; i < NumAllocatedChunks; i++)
        {
            allocatedChunks[i] = bigChunkAllocator.Allocate(AllocatedChunkSize, 1);
        }

		BeginMeasure();
        for (int i = 0; i < NumAllocatedChunks; i++)
        {
			//popProfile(BigChunkAllocatorDeallocate);
			allocatorPage.Deallocate(allocatedChunks[i]);
            //bigChunkAllocator.Deallocate(allocatedChunks[i]);
        }
        EndMeasure();

        free(preAllocatedMemoryP);
    }
private:
};

class NormalMemoryDeallocationBenchmark : public Benchmark
{
public:
    NormalMemoryDeallocationBenchmark(const std::string& name) : Benchmark(name) {}
    ~NormalMemoryDeallocationBenchmark() {}

    virtual void Run()
    {
        void** allocatedChunks = new void*[NumAllocatedChunks];

        for (int i = 0; i < NumAllocatedChunks; i++)
        {
            allocatedChunks[i] = malloc(AllocatedChunkSize);
        }

		BeginMeasure();
        for (int i = 0; i < NumAllocatedChunks; i++)
        {
            free(allocatedChunks[i]);
        }
        EndMeasure();
    }
private:
};

class PooledMemoryAllocationBenchmark : public Benchmark
{
public:
	PooledMemoryAllocationBenchmark(const std::string& name) : Benchmark(name) {}

	virtual void Run()
	{
		PoolAllocator poolAllocator(AllocatedChunkSize, NumAllocatedChunks, 1, false);
		void** allocatedChunks = new void*[NumAllocatedChunks];
		
		BeginMeasure();
		for (unsigned long i = 0; i < NumAllocatedChunks/2; i++)
		{
			allocatedChunks[i] = poolAllocator.Allocate();
			popAssert(allocatedChunks[i]);
		}
		EndMeasure();
	}
private:
};

class PooledMemoryDeallocationBenchmark : public Benchmark
{
public:
	PooledMemoryDeallocationBenchmark(const std::string& name) : Benchmark(name) {}

	virtual void Run()
	{
		PoolAllocator poolAllocator(AllocatedChunkSize, NumAllocatedChunks, 1, false);
		void** allocatedChunks = new void*[NumAllocatedChunks];

		for (unsigned long i = 0; i < NumAllocatedChunks; i++)
		{
			allocatedChunks[i] = poolAllocator.Allocate();
		}

		BeginMeasure();
		for (unsigned long i = 0; i < NumAllocatedChunks; i++)
		{
			poolAllocator.Deallocate(allocatedChunks[i]);
		}
		EndMeasure();
	}

private:
};