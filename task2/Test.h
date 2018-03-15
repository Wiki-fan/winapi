#pragma once
#include "HeapManager.h"
#include <chrono>
#include <iostream>
#include <string>

IHeapManager* heap;

class AllocateInCustomHeap {
public:
	static void* operator new(std::size_t sz)
	{
		return heap->Alloc( sz );
	}

	static void operator delete(void* ptr)
	{
		return heap->Free( ptr );
	}
};

class CTestInt : public AllocateInCustomHeap {
private:
	int a = 0;
};

class CTestBigBlock : public AllocateInCustomHeap {
private:
	int a[ 1024 * 4];
};

class CTestMediumBlock : public AllocateInCustomHeap {
private:
	int a[1024];
};

class CTestHeap {
	const int minSize = 4 * 1024 * 1024;
	const int maxSize = 1 * 1024 * 1024 * 1024;
public:
	template<typename HeapManager, typename TestClass>
	void TestAllocFree( std::string msg, const int N )
	{
		std::cout << msg << std::endl;
		HeapManager _heap;
		heap = &_heap;
		_heap.Create(minSize, maxSize );
		std::vector<TestClass*> objects;
		auto start = std::chrono::system_clock::now();
		for( int i = 0; i < N; ++i ) {
			auto obj = new TestClass;
			objects.push_back( obj );
		}
		auto end = std::chrono::system_clock::now();

		int elapsed_seconds = std::chrono::duration_cast<std::chrono::milliseconds>
			(end - start).count();
		std::cout << elapsed_seconds << std::endl;

		start = std::chrono::system_clock::now();
		for( int i = 0; i < N; ++i ) {
			auto obj = objects.back();
			objects.pop_back();
			delete obj;
		}
		end = std::chrono::system_clock::now();

		elapsed_seconds = std::chrono::duration_cast<std::chrono::milliseconds>
			(end - start).count();
		std::cout << elapsed_seconds << std::endl;

		_heap.Destroy();
	}

	template<typename HeapManager, typename TestClass>
	void TestAllocFreeInRow( std::string msg, const int N )
	{
		std::cout << msg << std::endl;
		HeapManager _heap;
		heap = &_heap;
		_heap.Create( minSize, maxSize );
		std::vector<TestClass*> objects;
		auto start = std::chrono::system_clock::now();
		for( int i = 0; i < N; ++i ) {
			auto obj = new TestClass;
			delete obj;
		}
		auto end = std::chrono::system_clock::now();

		int elapsed_seconds = std::chrono::duration_cast<std::chrono::milliseconds>
			(end - start).count();
		std::cout << elapsed_seconds << std::endl;

		_heap.Destroy();
	}

	template<typename HeapManager>
	void TestRandomFixedBlocks( std::string msg, const int N )
	{
		std::cout << msg << std::endl;
		HeapManager _heap;
		heap = &_heap;
		_heap.Create( minSize, maxSize );
		std::vector<AllocateInCustomHeap*> objects;
		auto start = std::chrono::system_clock::now();
		for( int i = 0; i < N; ++i ) {
			AllocateInCustomHeap* obj;
			int rnd = rand();
			if( rnd % 15 <9 ) {
				obj = new CTestInt;
			} else if( rnd % 5 < 13 ) {
				obj = new CTestMediumBlock;
			} else {
				obj = new CTestBigBlock;
			}
			objects.push_back( obj );
			if( (i + 5) % 10 == 0 ) {
				int index = rand() % objects.size();
				if( objects[index] != 0 ) {
					delete objects[index];
					objects[index] = 0;
				}
			}
		}
		auto end = std::chrono::system_clock::now();

		int elapsed_seconds = std::chrono::duration_cast<std::chrono::milliseconds>
			(end - start).count();
		std::cout << elapsed_seconds << std::endl;

		start = std::chrono::system_clock::now();
		for( int i = 0; i < objects.size(); ++i ) {
			auto obj = objects[i];
			if( obj != 0 )
				delete obj;
		}
		end = std::chrono::system_clock::now();

		elapsed_seconds = std::chrono::duration_cast<std::chrono::milliseconds>
			(end - start).count();
		std::cout << elapsed_seconds << std::endl;

		_heap.Destroy();
	}

	template<typename HeapManager>
	void TestRandom( std::string msg, const int N )
	{
		std::cout << msg << std::endl;
		HeapManager _heap;
		heap = &_heap;
		_heap.Create( minSize, maxSize );
		std::vector<void*> objects;
		auto start = std::chrono::system_clock::now();
		for( int i = 0; i < N; ++i ) {
			int size = rand()+1; 
			//std::cout << size <<std::endl;
			void* obj = _heap.Alloc(size);
			objects.push_back( obj );
			if( (i + 5) % 10 == 0 ) {
				int index = rand() % objects.size();
				if( objects[index] != 0 ) {
					_heap.Free(objects[index]);
					objects[index] = 0;
				}
				
			}
		}
		auto end = std::chrono::system_clock::now();

		int elapsed_seconds = std::chrono::duration_cast<std::chrono::milliseconds>
			(end - start).count();
		std::cout << elapsed_seconds << std::endl;

		start = std::chrono::system_clock::now();
		for( int i = 0; i < objects.size(); ++i ) {
			auto obj = objects[i];
			if( obj != 0 ) {
				_heap.Free( obj );
			}
		}
		end = std::chrono::system_clock::now();

		elapsed_seconds = std::chrono::duration_cast<std::chrono::milliseconds>
			(end - start).count();
		std::cout << elapsed_seconds << std::endl;

		_heap.Destroy();
	}

	int roundUpTo( int n, int factor )
	{
		return ((n + factor - 1) / factor) * factor;
	}

	void TestCorrectness( std::string msg, const int N )
	{
		std::cout << msg << std::endl;
		CHeapManager _heap;
		heap = &_heap;
		_heap.Create( 4 * 1024 * 1024, 1 * 1024 * 1024 * 1024 );
		std::vector<void*> objects;
		int correctAnswer = 0;
		for( int i = 0; i < N; ++i ) {
			int size = rand() + 1;
			correctAnswer += roundUpTo( size, 4096 ) / 4096;
			void* obj = _heap.Alloc( size );
			objects.push_back( obj );
		}
		std::cout << correctAnswer <<std::endl;
		_heap.Destroy();
	}
};