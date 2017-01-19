// SystemBenchmark.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "MemoryAllocatorBenchmark.h"
#include "System/ProfileUnitsManager.h"

#include <thread>
#include <chrono>

void WaitForInput()
{
    int i;
    std::cin >> i;
}

int main()
{
    popGetLogger().SetEnabled(false); //for perfomance purposes

    CustomMemoryAllocationBenchmark customMemoryAllocationBenchmark("Custom memory allocation");
    CustomMemoryDeallocationBenchmark customMemoryDeallocationBenchmark("Custom memory deallocation");
    NormalMemoryAllocationBenchmark normalMemoryAllocationBenchmark("Normal memory allocation");
    NormalMemoryDeallocationBenchmark normalMemoryDeallocationBenchmark("Normal memory deallocation");

    customMemoryAllocationBenchmark.Run();
    customMemoryDeallocationBenchmark.Run();
    normalMemoryAllocationBenchmark.Run();
    normalMemoryDeallocationBenchmark.Run();

    customMemoryAllocationBenchmark.WriteResultToConsole();
    customMemoryDeallocationBenchmark.WriteResultToConsole();
    normalMemoryAllocationBenchmark.WriteResultToConsole();
    normalMemoryDeallocationBenchmark.WriteResultToConsole();

    WaitForInput();

    return 0;
}

