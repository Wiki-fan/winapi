// task4_FilterText.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <Windows.h>
#include <cassert>
#include <string>
#include <cstring>
#include "Names.h"

DWORD getNearestSpace( HANDLE file, LONG initialGuess )
{
	if( initialGuess == 0 ) {
		return 0;
	}
	if( SetFilePointer( file, initialGuess+sizeof(WCHAR), NULL, FILE_BEGIN ) == INVALID_SET_FILE_POINTER ) {
		printf( "%d", GetLastError() );
		assert( false );
	}
	WCHAR c;
	DWORD NumberOfBytesRead;
	do {
		bool bResult = ReadFile( file, &c, sizeof( WCHAR ), &NumberOfBytesRead, NULL );
		if( bResult == false ) {
			printf( "%d", GetLastError() );
			assert( false );
		}
		if( iswspace( c ) ) {
			break;
		}
	} while( NumberOfBytesRead != 0 );
	return SetFilePointer( file, 0, NULL, FILE_CURRENT )-sizeof(WCHAR);
}

int FileMap( wchar_t* src_file_name, int* borders, int* sizes )
{
	HANDLE file = CreateFileW(src_file_name,GENERIC_READ,FILE_SHARE_READ,NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,NULL );
	if( file == INVALID_HANDLE_VALUE ) {
		printf( "%d", GetLastError() );
		assert( false );
	}

	HANDLE mapping = CreateFileMappingW(file, NULL, PAGE_READONLY,
		0, 0, INPUT_FILE_MAPPING_NAME );
	if( mapping == NULL ) {
		printf( "%d", GetLastError() );
		assert( false );
	}

	LPVOID addr = MapViewOfFile( mapping, FILE_MAP_READ, 0, 0, 0 );
	if( addr == NULL ) {
		printf( "%d", GetLastError() );
		assert( false );
	}
	WCHAR* text = (WCHAR*)addr+1;

	int fileSize = GetFileSize( file, NULL );

	for( int i = 0; i < N_PARTS; ++i ) {
		borders[i] = fileSize*i / N_PARTS;
		borders[i] = getNearestSpace( file, borders[i] );
	}

	//printf( "%d %d %d %d\n", borders[0], borders[1], borders[2], borders[3] );
	// do deeds
	for( int i = 0; i < N_PARTS - 1; ++i ) {
		sizes[i] = borders[i + 1] - borders[i];
	}
	sizes[N_PARTS - 1] = fileSize - borders[N_PARTS - 1];
	//printf( "%d %d %d %d\n", sizes[0], sizes[1], sizes[2], sizes[3] );

	return fileSize;
}

void processResultFiles( HANDLE* mappings, LPWSTR target_name, PROCESS_INFORMATION* procInfo, int size )
{
	FILE* f = _wfopen( target_name, L"w" );

	for( int i = 0; i < N_PARTS; ++i ) {
		/*HANDLE mapping = OpenFileMappingW(
			FILE_MAP_READ,
			FALSE,
			(OUTPUT_FILE_MAPPING_NAME+std::to_wstring(procInfo[i].dwProcessId)).c_str());
		*/
		HANDLE mapping = mappings[i];

		LPVOID addr = MapViewOfFile( mapping, FILE_MAP_READ, 0, 0, 0 );
		if( addr == NULL ) {
			printf( "%d", GetLastError() );
			assert( false );
		}
		WCHAR* text = (WCHAR*)addr;
		
		fwprintf( f, L"%s ", text );
		//wprintf( L"out %s\n", text );
	}
	fclose( f );
}

/*void FreeFile()
{
	UnmapViewOfFile( addr );
	CloseHandle( mapping );
	CloseHandle( file );
}*/

void createEventsForWorkers( HANDLE* events, LPCWSTR name, bool manualReset, PROCESS_INFORMATION* procInfo )
{
	for( int i = 0; i < N_PARTS; ++i ) {
		events[i] = CreateEvent(
			NULL,
			manualReset, false,
			(std::wstring( name ) + std::to_wstring( procInfo[i].dwProcessId )).c_str()
		);
		if( events[i] == NULL ) {
			printf( "%d", GetLastError() );
			assert( false );
		}
	}
}

void SetEventsForWorkers(HANDLE* events)
{
	for( int i = 0; i < N_PARTS; ++i ) {
		if( !SetEvent(events[i] ) ) {
			printf( "%d", GetLastError() );
			assert( false );
		}
	}
}

void CloseEventsForWorkers( HANDLE* events )
{
	for( int i = 0; i < N_PARTS; ++i ) {
		if( !CloseHandle( events[i] ) ) {
			printf( "%d", GetLastError() );
			assert( false );
		}
	}
}

int wmain( int argc, wchar_t* argv[] )
{
	std::wstring target_words( argv[1] );
	wchar_t* src_file_name = argv[2];
	wchar_t* target_name = argv[3];

	int borders[N_PARTS];
	int sizes[N_PARTS];
	int fileSize = FileMap( src_file_name, borders, sizes );

	STARTUPINFO startupInfo[N_PARTS];
	ZeroMemory( startupInfo, N_PARTS * sizeof( STARTUPINFO ) );
	PROCESS_INFORMATION procInfo[N_PARTS];
	ZeroMemory( procInfo, N_PARTS * sizeof( PROCESS_INFORMATION ) );
	for( int i = 0; i < N_PARTS; ++i ) {
		startupInfo[i].cb = sizeof( STARTUPINFO );
		if( !CreateProcess(
			L"C:\\Users\\ph134\\Documents\\Visual Studio 2015\\Projects\\abbyy\\Debug\\task4_Worker.exe",
			const_cast<LPWSTR>((target_words+L" "+std::to_wstring(borders[i])+L" "+std::to_wstring(sizes[i])).c_str()),
			NULL,
			NULL,
			false,
			0,
			NULL,
			NULL,
			&startupInfo[i],
			&procInfo[i]
		) ) {
			printf( "%d", GetLastError() );
			assert( false );
		}
	}

	HANDLE handles[N_PARTS];
	for( int i = 0; i < N_PARTS; ++i ) {
		handles[i] = procInfo[i].hProcess;
	}

	HANDLE taskPreparedEvents[N_PARTS];
	createEventsForWorkers( taskPreparedEvents, TASK_PREPARED, false, procInfo );
	HANDLE taskDoneEvents[N_PARTS];
	createEventsForWorkers( taskDoneEvents, TASK_DONE, false, procInfo );

	HANDLE endOfWork = CreateEvent(
		NULL,
		true, false,
		(std::wstring( END_OF_WORK )).c_str()
	);
	if( endOfWork == NULL ) {
		printf( "%d", GetLastError() );
		assert( false );
	}

	HANDLE mappings[N_PARTS];
	for( int i = 0; i < N_PARTS; ++i ) {
		/*HANDLE mapping = OpenFileMappingW(
		FILE_MAP_READ,
		FALSE,
		(OUTPUT_FILE_MAPPING_NAME+std::to_wstring(procInfo[i].dwProcessId)).c_str());
		*/
		HANDLE mapping = CreateFileMappingW(
			INVALID_HANDLE_VALUE,    // use paging file
			NULL,                    // default security
			PAGE_READWRITE,          // read/write access
			0,                       // maximum object size (high-order DWORD)
			fileSize,                // maximum object size (low-order DWORD)
			(OUTPUT_FILE_MAPPING_NAME + std::to_wstring( procInfo[i].dwProcessId )).c_str() );
		if( mapping == NULL ) {
			printf( "%d", GetLastError() );
			assert( false );
		}
		mappings[i] = mapping;
	}

	SetEventsForWorkers( taskPreparedEvents );

	Sleep( 100 );
	if( WaitForMultipleObjects( N_PARTS, taskDoneEvents, true, INFINITY ) == WAIT_FAILED ) {
		printf( "%d", GetLastError() );
		assert( false );
	}
	
	processResultFiles( mappings, target_name, procInfo, fileSize );

	if( !SetEvent(endOfWork) ) {
		printf( "%d", GetLastError() );
		assert( false );
	}

	if( WaitForMultipleObjects( N_PARTS, handles, true, INFINITY ) == WAIT_FAILED ) {
		printf( "%d", GetLastError() );
		assert( false );
	}

	CloseEventsForWorkers( taskPreparedEvents );
	CloseEventsForWorkers( taskDoneEvents );
	for( int i = 0; i < N_PARTS; ++i ) {
		CloseHandle( procInfo[i].hProcess );
		CloseHandle( procInfo[i].hThread );
	}
	

	scanf( "" );
	return 0;
}
