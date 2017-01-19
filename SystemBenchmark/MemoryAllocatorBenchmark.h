#pragma once
#include <string>
#include "Benchmark.h"
#include "System/memutils.h"
#include "System/BigChunkAllocator.h"
#include "System/SmallChunkAllocator.h"

static const unsigned int PreAllocatedSize = 256 MB;
static const unsigned int NumAllocatedChunks = 450000;
static const unsigned int AllocatedChunkSize = 6;

class CustomMemoryAllocationBenchmark : public Benchmark
{
public:
    CustomMemoryAllocationBenchmark(const std::string& name) : Benchmark(name) {}
    ~CustomMemoryAllocationBenchmark() {}

    virtual void Run()
    {
        void* preAllocatedMemoryP = malloc(PreAllocatedSize);
        SmallChunkAllocator bigChunkAllocator(preAllocatedMemoryP, PreAllocatedSize);

        void** allocatedChunks = new void*[NumAllocatedChunks];

        StartMeasure();
        for (int i = 0; i < NumAllocatedChunks; i++)
        {
            allocatedChunks[i] = bigChunkAllocator.Allocate(AllocatedChunkSize, 1);
        }
        EndMeasure();

        delete[] allocatedChunks;
        free(preAllocatedMemoryP);
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
        
        StartMeasure();
        for (int i = 0; i < NumAllocatedChunks; i++)
        {
            allocatedChunks[i] = malloc(AllocatedChunkSize);
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
        SmallChunkAllocator bigChunkAllocator(preAllocatedMemoryP, PreAllocatedSize);

        void** allocatedChunks = new void*[NumAllocatedChunks];

        for (int i = 0; i < NumAllocatedChunks; i++)
        {
            allocatedChunks[i] = bigChunkAllocator.Allocate(AllocatedChunkSize, 1);
        }

        StartMeasure();
        for (int i = 0; i < NumAllocatedChunks; i++)
        {
            bigChunkAllocator.Deallocate(allocatedChunks[i]);
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

        StartMeasure();
        for (int i = 0; i < NumAllocatedChunks; i++)
        {
            free(allocatedChunks[i]);
        }
        EndMeasure();
    }
private:
};