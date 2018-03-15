#include "HeapManager.h"
#include <exception>
#include <string>
#include <iterator>
#include <set>
#include <cassert>
#include <iostream>
#include <algorithm>

void CHeapManager::Create( int _minSize, int maxSize )
{
	SYSTEM_INFO info;
	GetSystemInfo( &info );
	pageSize = info.dwPageSize;

	// reserve
	maxSize = roundUpTo( maxSize, pageSize );
	heap = VirtualAlloc( 0, maxSize, MEM_RESERVE, PAGE_READWRITE );
	if( heap == 0 ) {
		throw HeapException( "Not enough virtual memory for maxSize" );
	}

	// commit
	_minSize = roundUpTo( _minSize, pageSize );
	auto addr = VirtualAlloc( heap, _minSize, MEM_COMMIT, PAGE_READWRITE );
	if( addr == 0 ) {
		throw HeapException( "Not enough physical memory for minSize" );
	}
	commitedHeaps.push_back( addr );
	minSize = _minSize;
	heapBorder = reinterpret_cast<void*>((char*)addr + _minSize);

	pages.resize( maxSize / pageSize );
	freeBlocks.resize( 3 );

	// add first block
	freeBlocks[getFreeBlocksIndex( _minSize )].insert( addr );
	bigBlocksSize.insert_or_assign( addr, _minSize );
}

void * CHeapManager::Alloc( int _size )
{
	assert( _size > 0 );
	int neededSize = roundUpTo( _size, 4 );
	if( isBigBlock( neededSize ) ) {
		// big block, not storing address in block
	} else {
		// small block, storing address in block
		neededSize += sizeof( void* );
	}

	int index = getFreeBlocksIndex( neededSize );

	while( true ) {
		for( int i = index; i < 3; ++i ) {
			for( auto iter = freeBlocks[i].begin(); iter != freeBlocks[i].end(); ++iter ) {
				void* address;
				int size;
				std::tie( address, size ) = getBlockInfo( *iter, false );
				if( size >= neededSize ) {
					freeBlocks[getFreeBlocksIndex( size )].erase( address );
					if( isBigBlock( size ) ) {
						bigBlocksSize.erase( address );
					}

					void* addr1 = address;
					int size1 = neededSize;
					void* addr2 = reinterpret_cast<void*>((char*)(address)+neededSize);
					int size2 = size - neededSize;

					updatePages( addr1, size1, 1 );
					if( size2 > 0 ) {
						freeBlocks[getFreeBlocksIndex( size2 )].insert(addr2 );//hint std::next(iter), 
						if( isBigBlock( size2 ) ) {
							bigBlocksSize.insert_or_assign( addr2, size2 );
						}
					}

					if( isBigBlock(size1) ) {
						bigBlocksSize.insert_or_assign( addr1, size1 );
						return addr1;
					} else {
						*reinterpret_cast<int*>(addr1) = size1;
						return reinterpret_cast<void*>((char*)(addr1)+sizeof( int ));
					}
				}
			}
		}
		addPagesForSize( _size );
	}

}

// ≈сли блок <4k, у него первые 4 бита адрес, далее - данные, т. е. данные всегда = 4 (mod 8). 
// ≈сли блок большой, у него такого нет, и сделаем его = 0 (mod 8).
void CHeapManager::Free( void * mem )
{
	void* address;
	int size;
	std::tie( address, size ) = getBlockInfo( mem, true );
	//updatePages( address, size, -1 );
	void* oldAddress = address;
	int oldSize = size;

	for( int index = 0; index < 3; ++index ) {
		auto insert = freeBlocks[index].insert( address );
		assert( insert.second );
		auto iter = insert.first;

		void* newAddress = address;
		int newSize = size;

		auto prev = std::prev( iter );
		if( prev != freeBlocks[index].end() ) {
			void* prevAddress;
			int prevSize;
			std::tie( prevAddress, prevSize ) = getBlockInfo( *prev, false );

			if( (char*)prevAddress + prevSize == (char*)address ) {
				newAddress = prevAddress;
				newSize += prevSize;
				freeBlocks[getFreeBlocksIndex( prevSize )].erase( prev );
				if( isBigBlock(prevSize)) {
					bigBlocksSize.erase( prevAddress );
				}
			}
		}

		auto next = std::next( iter );
		if( next != freeBlocks[index].end() ) {
			void* nextAddress;
			int nextSize;
			std::tie( nextAddress, nextSize ) = getBlockInfo( *next, false );

			if( (char*)address + size == (char*)nextAddress ) {
				newSize += nextSize;
				freeBlocks[getFreeBlocksIndex( nextSize )].erase( nextAddress );
				if( isBigBlock(nextSize )) {
					bigBlocksSize.erase( nextAddress );
				}
			}
		}

		//ѕросто удал€ем, т. к. могли не по тому индексу вставить.
		freeBlocks[index].erase( address );

		// ≈сли смогли увеличить, обновл€ем size и address.
		if( newSize > size ) {
			if( isBigBlock(size )) {
				bigBlocksSize.erase( address );
			}

			setBlockSize( newAddress, newSize );
			address = newAddress;
			size = newSize;

		}
	}
	freeBlocks[getFreeBlocksIndex( size )].insert( address );
	updatePages( oldAddress, oldSize, -1 );
}

void CHeapManager::Destroy()
{
	int nonFreeBlocks = 0;
	for( int i = 0; i < pages.size(); ++i ) {
		if( pages[i] != 0 ) {
			nonFreeBlocks += pages[i];
			//std::cout << i << " not free\n";
		}
	}
	if( nonFreeBlocks != 0 ) {
		std::cout << "Non-free blocks: " << nonFreeBlocks << std::endl;
	} else {

		if( VirtualFree( heap, 0, MEM_RELEASE ) == NULL ) {
			throw HeapException( "Error when deallocating heap" );
		}

		/*for( int i = 0; i < commitedHeaps; ++i ) {
		if( VirtualFree( addresses[i], 0, MEM_RELEASE ) == NULL ) {
		}*/
	}
	
}

void CHeapManager::updatePages( void * address, int size, int val )
{
	for( int pos_in_block = 0; pos_in_block < size; pos_in_block += pageSize ) {
		int page_index = getPage( (void*)((char*)address + pos_in_block) );
		pages[page_index] += val;
		/*if( pages[page_index] == 0 && page_index > minSize / pageSize ) {
			void* pageBeginning = (char*)heap + ((char*)address - (char*)heap) / pageSize*pageSize;
			
			// Ѕлижайший к левой границе свободный блок. ќн должен занимать всю закоммиченную страницу и, возможно, вылезать справа.
			void* minAddress = 0;
			for( int i = 0; i < 3; ++i ) {
				if( !freeBlocks[i].empty() ) {
					auto ub = freeBlocks[i].upper_bound( pageBeginning );
					auto iter = std::prev( ub );
					if( iter == freeBlocks[i].end() ) {
						continue;
					}
					if( *iter > minAddress ) {
						minAddress = *iter;
					}
				}
			}
			void* blockAddr;
			int blockSize;
			std::tie( blockAddr, blockSize ) = getBlockInfo( minAddress, false );
			if( isBigBlock( blockSize ) ) {
				bigBlocksSize.erase( blockAddr );
			}
			if( blockAddr == pageBeginning ) {
				freeBlocks[getFreeBlocksIndex( blockSize )].erase( blockAddr );
			} else {
				setBlockSize( blockAddr, (char*)pageBeginning - (char*)blockAddr );
			}

			void* nextPageBeginning = ((char*)pageBeginning + pageSize);
			if( (char*)nextPageBeginning < ((char*)blockAddr + blockSize) ) {
				void* newAddr = nextPageBeginning;
				int newSize = ((char*)blockAddr + blockSize) - (char*)nextPageBeginning;
				freeBlocks[getFreeBlocksIndex( newSize )].insert( newAddr );
				setBlockSize( newAddr, newSize );
			}

			if( VirtualFree( pageBeginning, 1, MEM_DECOMMIT ) == NULL ) {
				std::cout <<"Error " << GetLastError();
				throw HeapException( "Can't decommit memory" );
			}
		}*/
	}
}

void CHeapManager::addPagesForSize( int size )
{
	int addingSize = roundUpTo( size, pageSize );
	auto addr = VirtualAlloc( heapBorder, addingSize, MEM_COMMIT, PAGE_READWRITE );
	heapBorder = reinterpret_cast<void*>((char*)heapBorder + addingSize);
	if( addr == 0 ) {
		throw HeapException( "Not enough physical memory for addingSize" );
	}
	commitedHeaps.push_back( addr );
	freeBlocks[getFreeBlocksIndex( addingSize )].insert( addr );
	/*for( int i = 0; i < addingSize / pageSize; ++i ) {
		auto addr = VirtualAlloc( heapBorder, pageSize, MEM_COMMIT, PAGE_READWRITE );
		heapBorder = reinterpret_cast<void*>((char*)heapBorder + pageSize);
		if( addr == 0 ) {
			throw HeapException( "Not enough physical memory to resize" );
		}
		commitedHeaps.push_back( addr );
	}

	auto addr = commitedHeaps[commitedHeaps.size() - addingSize / pageSize];
	freeBlocks[getFreeBlocksIndex( addingSize )].insert( addr );*/
	// всегда больше pageSize, надо класть.
	bigBlocksSize.insert_or_assign( addr, addingSize );

}
