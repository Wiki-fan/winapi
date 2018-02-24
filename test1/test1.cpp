// test1.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <iostream>
#include <vector>
#include <ctime>
#include <Windows.h>

// Работает иногда.
void DescribeError(DWORD dwError)
{
	LPWSTR lpBuffer;
	int nResult = 0;
	if( dwError >= 12000 && dwError <= 12174 )
		nResult = FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_HMODULE,
			GetModuleHandle( L"wininet.dll" ), dwError, 0, (LPWSTR)&lpBuffer, 0, NULL );
	else
		nResult = FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS, NULL, dwError, MAKELANGID( LANG_ENGLISH, SUBLANG_ENGLISH_US ), (LPWSTR)&lpBuffer, 0, NULL );
	if( nResult ) {
		std::wcout << (LPWSTR)lpBuffer << std::endl;
		LocalFree( lpBuffer );
	} else {
		std::wcout << "Printing error message failed with " <<GetLastError();
	}
}

void TestVirtualAlloc( LPSYSTEM_INFO info)
{
	std::vector<LPVOID> addresses;
	for( int i = 0;; ++i ) {
		LPVOID addr = VirtualAlloc( NULL, info->dwPageSize, MEM_RESERVE, PAGE_READWRITE );
		if( addr == NULL ) {
			DWORD err = GetLastError();
			printf( "Allocated %d blocks, then error %d\n", i, err );
			DescribeError( err );
			break;
		} else {
			addresses.push_back( addr );
		}
	}
	std::cin.get();
	bool freedSuccessfully = true;
	for( int i = 0; i < addresses.size(); ++i ) {
		if( VirtualFree( addresses[i], 0, MEM_RELEASE ) == NULL ) {
			freedSuccessfully = false;
			DWORD err = GetLastError();
			printf( "Freed %d blocks, then error %d\n", i, err );
			DescribeError( err );
			break;
		}
	}
	if( freedSuccessfully ) {
		printf( "Freed %d blocks", addresses.size() );
	}

}

void TestVirtualRandomAlloc( LPSYSTEM_INFO info )
{
	std::cout << "RAND_MAX = " << RAND_MAX << std::endl;
	const int range = ((char*)(info->lpMaximumApplicationAddress) - (char*)(info->lpMinimumApplicationAddress)) / info->dwAllocationGranularity;
	printf( "Allocating in one of %d positions\n", range );
	std::vector<LPVOID> addresses;
	for( int i = 0;; ++i ) {
		LPVOID addr = VirtualAlloc( (char*) info->lpMinimumApplicationAddress + (rand() % range) * info->dwPageSize, rand() % info->dwPageSize, MEM_RESERVE, PAGE_READWRITE );
		if( addr == NULL ) {
			DWORD err = GetLastError();
			printf( "Allocated %d blocks, then error %d\n", i, err );
			DescribeError( err );
			break;
		} else {
			addresses.push_back( addr );
		}
	}
	std::cin.get();
	bool freedSuccessfully = true;
	for( int i = 0; i < addresses.size(); ++i ) {
		if( VirtualFree( addresses[i], 0, MEM_RELEASE ) == NULL ) {
			freedSuccessfully = false;
			DWORD err = GetLastError();
			printf( "Freed %d blocks, then error %d\n", i, err );
			DescribeError( err );
			break;
		}
	}
	if( freedSuccessfully ) {
		printf( "Freed %d blocks", addresses.size() );
	}

}

void TestHandles()
{
	HANDLE handle = CreateEvent( NULL, TRUE, TRUE, NULL );
	std::vector<HANDLE> handles;
	
	for( int i = 0; ; ++i ) {
		HANDLE newHandle;
		bool success = DuplicateHandle( GetCurrentProcess(),
			handle,
			GetCurrentProcess(),
			&newHandle,
			0,
			FALSE,
			DUPLICATE_SAME_ACCESS );
		if( success == false ) {
			DWORD err = GetLastError();
			printf( "Created %d handles, then error %d\n", i, err );
			DescribeError( err );
			break;
		} else {
			handles.push_back( newHandle );
		}
	}
	std::cin.get();
	bool closedSuccessfully = true;
	for( int i = 0; i < handles.size(); ++i ) {
		bool success = CloseHandle( handles[i] );
		if( success == false ) {
			closedSuccessfully = false;
			DWORD err = GetLastError();
			printf( "Closed %d handles, then error %d\n", i, err );
			DescribeError( err );
			break;
		}
	}
	if( !CloseHandle( handle ) ) {
		closedSuccessfully = false;
	}
	if( closedSuccessfully == true ) {
		printf( "Closed %d handles", handles.size() );
	}
	std::cin.get();
}


void TestGDI()
{
	
	std::vector<HPEN> pens;

	for( int i = 0; ; ++i ) {
		HPEN pen = CreatePen( PS_SOLID, 0, RGB( 127, 0, 0 ) );
		if( pen == NULL ) {
			DWORD err = GetLastError();
			printf( "Created %d pens, then error %d\n", i, err );
			DescribeError( err );
			break;
		} else {
			pens.push_back( pen );
		}
	}

	std::cin.get();

	bool deletedSuccessfully = true;
	for( int i = 0; i < pens.size(); ++i ) {
		bool success = DeleteObject( pens[i] );
		if( success == false ) {
			deletedSuccessfully = false;
			DWORD err = GetLastError();
			printf( "Deleted %d pens, then error %d\n", i, err );
			DescribeError( err );
			break;
		}
	}
	if( deletedSuccessfully == true ) {
		printf( "Deleted %d pens", pens.size() );
	}
	std::cin.get();
}

char *stackBase;
ptrdiff_t stackSize( void )
{
	char dummy2; //Будет лежать в начале стека в данный момент 
	// (уже после того, как на стеке оказалась dummy).
	// stackBase — адрес dummy
	return stackBase - &dummy2;
}

ptrdiff_t test() {
	return stackSize();
}

void TestEmptyFunctionCallSize()
{
	char dummy; // Будет лежать в начале стека в данный момент времени.
	stackBase = &dummy;

	ptrdiff_t smallerAddr = stackSize();
	std::wcout << "Stack bottom " << smallerAddr << std::endl;
	ptrdiff_t biggerAddr = test();
	std::wcout << "Stack top " << biggerAddr << std::endl;
	std::wcout << "Function size " <<biggerAddr - smallerAddr << std::endl;
}

ptrdiff_t test2()
{
	// 0 - 0
	// 1 - 12
	// 4 - 12
	// 5 - 28
	// 8 - 28
	// 9 - 32
	// 10 - 32
	char a[8];
	return stackSize();
}

void TestNonEmptyFunctionCallSize()
{
	char dummy; // Будет лежать в начале стека в данный момент времени.
	stackBase = &dummy;

	ptrdiff_t smallerAddr = stackSize();
	std::wcout << "Stack bottom " << smallerAddr << std::endl;
	ptrdiff_t biggerAddr = test2();
	std::wcout << "Stack top " << biggerAddr << std::endl;
	std::wcout << "Function size " << biggerAddr - smallerAddr << std::endl;
}

int main()
{
	srand( time( NULL ) );
	const DWORD size = 100 + 1;
	WCHAR buffer[size];

	printf( "Hello World" );
	SYSTEM_INFO info;
	GetSystemInfo( &info );
	printf( "Page size %d\nMin addr %d\nMax addr %d\nAllocation granularity %d\n\n", 
		info.dwPageSize,
		info.lpMinimumApplicationAddress,
		info.lpMaximumApplicationAddress, 
		info.dwAllocationGranularity );
	
	//TestVirtualAlloc( &info );
	TestVirtualRandomAlloc( &info );
	//TestHandles();
	//TestGDI();
	//TestEmptyFunctionCallSize();
	//TestNonEmptyFunctionCallSize();

	std::cin.get();

    return 0;
}

