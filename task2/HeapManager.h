#pragma once
#include <Windows.h>
#include <vector>
#include <map>
#include <set>
#include "IHeapManager.h"

// Для маленьких блоков храним тот размер, который они занимают (учитывать 4 байта на размер)

class CHeapManager : public IHeapManager {
public:
	void Create( int minSize, int maxSize );
	void* Alloc( int size );
	void Free( void* mem );
	void Destroy(); // Печатает количество занятых блоков (0, если таковых нет, и деаллокация успешна).
protected:
	LPVOID heap;
	std::vector<LPVOID> commitedHeaps;
	LPVOID heapBorder;
	// Количество занятых блоков, лежащих на этой странице.
	std::vector<int> pages;

	int minSize;

	// Размеры для больших блоков (как занятых, так и свободных).
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

	// Возвращает степень двойки
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

	// Память выделяется сплошным куском и с выравниванием по 4 КБ, поэтому можно пользоваться такой формулой.
	int getPage( void* address )
	{
		return ((char*)address - (char*)heap) / pageSize;
	}

	// fromRawAddr определяет, надо ли делать сдвиг. Переддавать true, если там 
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
			//bigBlocksSize.erase( address ); это же неверно
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
