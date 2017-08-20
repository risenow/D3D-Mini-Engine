// SystemBenchmark.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "MemoryAllocatorBenchmark.h"
#include "OutBenchmark.h"
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
	PooledMemoryAllocationBenchmark pooledMemoryAllocationBenchmark("Pooled memory allocation");
	PooledMemoryDeallocationBenchmark pooledMemoryDeallocationBenchmark("Pooled memory deallocation");
	CoutBySingleCharBenchmark coutBySingleCharBenchmark("cout by single char");
	CoutByWholeStringBenchmark coutByWholeStringBenchmark("cout by whole string");
	PrintBySingleCharBenchmark printBySingleCharBenchmark("print by single char");
	PrintByWholeStringBenchmark printByWholeStringBenchmark("print by whole string");
	ConcatSingleCharsAndPrintBenchmark concatSingleCharsAndPrintBenchmark("concat and print");

    customMemoryAllocationBenchmark.Run();
    customMemoryDeallocationBenchmark.Run();
    normalMemoryAllocationBenchmark.Run();
    normalMemoryDeallocationBenchmark.Run();
	pooledMemoryAllocationBenchmark.Run();
	pooledMemoryDeallocationBenchmark.Run();

	//std::ios::sync_with_stdio(false);
	coutBySingleCharBenchmark.Run();
	coutByWholeStringBenchmark.Run();
	printBySingleCharBenchmark.Run();
	printByWholeStringBenchmark.Run();
	concatSingleCharsAndPrintBenchmark.Run();

    customMemoryAllocationBenchmark.WriteResultToConsole();
    customMemoryDeallocationBenchmark.WriteResultToConsole();
    normalMemoryAllocationBenchmark.WriteResultToConsole();
    normalMemoryDeallocationBenchmark.WriteResultToConsole();
	pooledMemoryAllocationBenchmark.WriteResultToConsole();
	pooledMemoryDeallocationBenchmark.WriteResultToConsole();
	coutBySingleCharBenchmark.WriteResultToConsole();
	coutByWholeStringBenchmark.WriteResultToConsole();
	printBySingleCharBenchmark.WriteResultToConsole();
	printByWholeStringBenchmark.WriteResultToConsole();
	concatSingleCharsAndPrintBenchmark.WriteResultToConsole();

    WaitForInput();

	ProfileStats stats = ProfileUnitsManager::GetInstance().GatherStatsForProfileWithName("BigChunkAllocatorAllocate");

    return 0;
}

