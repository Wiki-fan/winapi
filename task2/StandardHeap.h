#pragma once
#include <Windows.h>
#include "IHeapManager.h"

class CStandardHeap :public IHeapManager {
public:
	void Create( int minSize, int maxSize )
	{
		heap = HeapCreate( 0, minSize, maxSize );
		if( heap == 0 ) {
			std::cout << GetLastError();
			throw HeapException( "Standard heap failed to create" );
		}
	}
	void* Alloc( int size )
	{
		return HeapAlloc( heap, 0, size );
	}
	void Free( void* mem )
	{
		if( !HeapFree( heap, 0, mem ) ) {
			throw HeapException( "Standard heap failed to free mem" );
		}
	}
	void Destroy()
	{
		if( !HeapDestroy( heap ) ) {
			throw HeapException( "Standard heap failed to destroy" );
		}
	}
private:
	HANDLE heap;
};