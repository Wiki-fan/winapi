#pragma once
#include <Windows.h>
#include <vector>
#include <map>
#include <set>
#include "IHeapManager.h"

// ��� ��������� ������ ������ ��� ������, ������� ��� �������� (��������� 4 ����� �� ������)

class CHeapManager : public IHeapManager {
public:
	void Create( int minSize, int maxSize );
	void* Alloc( int size );
	void Free( void* mem );
	void Destroy(); // �������� ���������� ������� ������ (0, ���� ������� ���, � ����������� �������).
protected:
	LPVOID heap;
	std::vector<LPVOID> commitedHeaps;
	LPVOID heapBorder;
	// ���������� ������� ������, ������� �� ���� ��������.
	std::vector<int> pages;

	int minSize;

	// ������� ��� ������� ������ (��� �������, ��� � ���������).
	std::map<LPVOID, int> bigBlocksSize;
	std::vector<std::set<LPVOID>> freeBlocks;

	int getFreeBlocksIndex( int size )
	{
		if( size+sizeof(int) < 4 * 1024 ) {
			return 0;
		} else if( size <= 128 * 1024 ) {
			return 1;
		} else {
			return 2;
		}
	}

	bool isBigBlock( int size )
	{
		return (size + sizeof( int ) > pageSize);
	}

	int pageSize;

	int roundUpTo( int n, int factor )
	{
		return ((n +factor-1) / factor) * factor;
	}

	// ���������� ������� ������
	int roundUpToDeg2( int n )
	{
		int deg = 0;
		int d = 1;
		while( d < n ) {
			++deg;
			d *= 2;
		}
		return deg;
	}

	// ������ ���������� �������� ������ � � ������������� �� 4 ��, ������� ����� ������������ ����� ��������.
	int getPage( void* address )
	{
		return ((char*)address - (char*)heap) / pageSize;
	}

	// fromRawAddr ����������, ���� �� ������ �����. ����������� true, ���� ��� 
	std::pair<void*, int> getBlockInfo( void* mem, bool fromRawAddr )
	{
		void* block_beginning;
		int size;
		auto res = bigBlocksSize.find( mem );
		if( res == bigBlocksSize.end() ) {
			// small block, size stored nearby
			block_beginning = reinterpret_cast<char*>(mem) - (fromRawAddr?sizeof( int ):0);
			size = *reinterpret_cast<int*>(block_beginning);
		} else {
			size = res->second;
			block_beginning = mem;
		}
		return std::make_pair( block_beginning, size );
	}

	/*void freeBlock( void* address, int size )
	{
		freeBlocks[getFreeBlocksIndex( size )].erase( address );
		if( size > pageSize ) {
			//bigBlocksSize.erase( address ); ��� �� �������
			bigBlocksSize.insert_or_assign( address, size );
		}
		for( int pos_in_block = 0; pos_in_block < size; pos_in_block += pageSize ) {
			pages[getPage( (char*)address + pos_in_block )] -= 1;
		}
	}*/

	void updatePages( void* address, int size, int val );

	void addPagesForSize( int size );

	void setBlockSize(void* newAddress, int newSize)
	{
		if( newSize == 0 ) {
			return;
		}
		if( isBigBlock( newSize ) ) {
			bigBlocksSize.insert_or_assign( newAddress, newSize );
		} else {
			*(int*)((char*)newAddress) = newSize;
		}
	}
};
