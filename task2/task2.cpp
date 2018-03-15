// task2.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include "Test.h"

#include "StandardHeap.h"
#include "HeapManager.h"

int main()
{
	CTestHeap tester;
	tester.TestAllocFree<CHeapManager, CTestInt>("Ints, custom heap", 100000);
	tester.TestAllocFree<CStandardHeap, CTestInt>( "Ints, standard heap", 100000);
	tester.TestAllocFree<CHeapManager, CTestBigBlock>( "Big blocks, custom heap", 1000 );
	tester.TestAllocFree<CStandardHeap, CTestBigBlock>( "Big blocks, standard heap", 1000 );
	tester.TestAllocFreeInRow<CHeapManager, CTestInt>( "Ints row, custom heap", 100000 );
	tester.TestAllocFreeInRow<CStandardHeap, CTestInt>( "Ints row, standard heap", 100000 );
	tester.TestRandomFixedBlocks<CHeapManager>( "Random, fixed blocks, custom heap" , 10000);
	tester.TestRandomFixedBlocks<CStandardHeap>( "Random, fixed blocks, standard heap", 10000 );
	tester.TestRandom<CHeapManager>( "Random test, custom heap", 10000 );
	tester.TestRandom<CStandardHeap>( "Random test, standard heap", 10000 );
	tester.TestCorrectness( "Correctness test, two numbers below should be equal", 1000 );
	std::cin.get();
    return 0;
}

