// test1.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <Windows.h>

/// Работает иногда.
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

void TestVirtualAlloc( LPSYSTEM_INFO info, DWORD allocationType, std::string allocationTypeDescr, DWORD protect, std::string protectionDescr)
{
	std::vector<LPVOID> addresses;
	for( int i = 0;; ++i ) {
		LPVOID addr = VirtualAlloc( NULL, info->dwPageSize, allocationType, protect );
		if( addr == NULL ) {
			DWORD err = GetLastError();
			std::cout << "Allocated " << i << " blocks with " << allocationTypeDescr << " and " << protectionDescr 
				<< " then error " << err << std::endl;
			DescribeError( err );
			break;
		} else {
			addresses.push_back( addr );
		}
	}
	std::cin.get();
	for( int i = 0; i < 10; ++i ) {
		std::cout << addresses[i] << ' ';
	}

	bool freedSuccessfully = true;
	for( int i = 0; i < addresses.size(); ++i ) {
		if( VirtualFree( addresses[i], 0, MEM_RELEASE ) == NULL ) {
			freedSuccessfully = false;
			DWORD err = GetLastError();
			std::wcout << L"Freed " <<i << L"blocks, then error "<< err;
			DescribeError( err );
			break;
		}
	}
	if( freedSuccessfully ) {
		std::wcout << "Freed " << addresses.size() <<"blocks" <<std::endl;
	}
	std::cin.get();

}

void TestVirtualRandomAlloc( LPSYSTEM_INFO info )
{
	std::cout << "RAND_MAX = " << RAND_MAX << std::endl;
	// Возможный интервал для изменения параметра (в единицах dwAllocationGranurality)
	//const int range = ((char*)((long long int)info->lpMaximumApplicationAddress*15) - (char*)(info->lpMinimumApplicationAddress)) / info->dwAllocationGranularity;
	//printf( "Allocating in one of %d positions\n", range );
	
	std::vector<LPVOID> addresses;
	for( long long int i = 0; i<10000; ++i ) {
		DWORD allocationType;
		switch( rand()%2 ) {
			case 0:
				allocationType = MEM_RESERVE;
				break;
			case 1:
				allocationType = MEM_RESERVE | MEM_COMMIT;
				break;
		}
		DWORD protect;
		switch( rand() % 3 ) {
			case 0:
				protect = PAGE_READONLY;
				break;
			case 1:
				protect = PAGE_READWRITE;
				break;
			case 2:
				protect = PAGE_EXECUTE_READWRITE;
				break;
		}

		DWORD guard;
		switch( rand() % 2 ) {
			case 0:
				guard = PAGE_GUARD;
				break;
			case 1:
				guard = 0;
				break;
		}
		LPVOID addr = VirtualAlloc(
			//(char*)info->lpMinimumApplicationAddress+1000*1024*1024 + (i) * info->dwAllocationGranularity,
			(LPVOID)(200 * 1024 * 1024 +rand()*info->dwPageSize),
			info->dwPageSize, allocationType, protect|guard);
		if( addr == NULL ) {
			//DWORD err = GetLastError();
			//std::wcout << "On" << i << " block error " << err << std::endl;
			//DescribeError( err );
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
		printf( "Freed %d blocks\n", addresses.size() );
	}
	std::cin.get();

}

void TestHandles()
{
	HANDLE handle = CreateEvent( NULL, TRUE, TRUE, NULL );
	std::vector<HANDLE> handles;
	
	for( unsigned long long int i = 0; ; ++i ) {
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
	for( unsigned long long int i = 0; i < handles.size(); ++i ) {
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
		printf( "Closed %d handles\n", handles.size() );
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

	std::wcout << "Stack bottom " << stackSize() << std::endl;
	std::wcout << "Stack top " << test() << std::endl;
	std::wcout << "Function size " <<test() - stackSize() << std::endl;
}

ptrdiff_t test2()
{
	// Debug
	// 0 - 0
	// 1 - 12
	// 4 - 12
	// 5 - 28
	// 8 - 28
	// 9 - 32
	// 10 - 32
	// 12 - 32
	// 14 - 36
	// 16 - 36
	// Release
	// 1 - 4
	// 2 - 4
	// 5 - 16
	volatile char a[5];
	return stackSize();
}

void TestNonEmptyFunctionCallSize()
{
	char dummy; // Будет лежать в начале стека в данный момент времени.
	stackBase = &dummy;

	std::wcout << "Stack bottom " << stackSize() << std::endl;
	std::wcout << "Stack top " << test2() << std::endl;
	std::wcout << "Function size " << test2() - stackSize() << std::endl;
	std::wcout << "Function difference " << (test2() - stackSize()) - 8 <<std::endl;
}

void TestLongFilenames()
{
	std::wstring dirName = L"C:\\";
	
	for( int i = 1; i < 1000; ++i ) {
		dirName += L'a';
		//std::cout << (LPCTSTR)(dirName+'\\').c_str();
		bool created = CreateDirectory( (LPCTSTR)(dirName+L'\\').c_str(), 0 );
		if( !created ) {
			DWORD err = GetLastError();
			std::wcout << "Directory name length " << i+4 << ", error " << err << std::endl;
			DescribeError( err );
			break;
		}

		bool removed = RemoveDirectory( (LPCTSTR)(dirName+L'\\').c_str() );
		if( !removed ) {
			DWORD err = GetLastError();
			std::wcout << "Remove error "<<i <<std::endl;
			DescribeError( err );
			break;
		}
	}
}

int main()
{
	srand( time( NULL ) );
	const DWORD size = 100 + 1;
	WCHAR buffer[size];

	std::cout <<"Hello World";
	SYSTEM_INFO info;
	GetSystemInfo( &info );
	printf( "Page size %d\nMin addr %d\nMax addr %d\nAllocation granularity %d\n\n", 
		info.dwPageSize,
		info.lpMinimumApplicationAddress,
		info.lpMaximumApplicationAddress, 
		info.dwAllocationGranularity );
	
	//TestVirtualAlloc( &info, MEM_RESERVE, "MEM_RESERVE", PAGE_READWRITE, "PAGE_READWRITE" );
	//TestVirtualAlloc( &info, MEM_RESERVE|MEM_COMMIT, "MEM_RESERVE|MEM_COMMIT", PAGE_READWRITE, "PAGE_READWRITE" );
	//TestVirtualRandomAlloc( &info );
	//TestHandles();
	//TestGDI();
	TestEmptyFunctionCallSize();
	TestNonEmptyFunctionCallSize();
	//TestLongFilenames();

	std::cin.get();

    return 0;
}

