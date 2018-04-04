// task4_FilterText.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <Windows.h>
#include <cassert>
#include <string>
#include "Names.h"

SYSTEM_INFO systemInfo;

DWORD getNearestSpace( HANDLE file, LONG initialGuess )
{
	if( initialGuess == 0 ) {
		return 0;
	}
	// https://msdn.microsoft.com/ru-ru/library/windows/desktop/aa365541(v=vs.85).aspx
	if( SetFilePointer( file, initialGuess+sizeof(WCHAR), NULL, FILE_BEGIN ) == INVALID_SET_FILE_POINTER ) {
		printf( "%d", GetLastError() );
		assert( false );
	}
	WCHAR c;
	DWORD NumberOfBytesRead;
	do {
		// https://msdn.microsoft.com/en-us/library/windows/desktop/aa365467(v=vs.85).aspx
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

void FileMap( wchar_t* src_file_name, int* borders, int* sizes )
{
	// https://msdn.microsoft.com/en-us/library/windows/desktop/aa363858(v=vs.85).aspx
	HANDLE file = CreateFileW(src_file_name,GENERIC_READ,FILE_SHARE_READ,NULL,
		OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL );
	if( file == INVALID_HANDLE_VALUE ) {
		printf( "%d", GetLastError() );
		assert( false );
	}

	// https://msdn.microsoft.com/en-us/library/windows/desktop/aa366537(v=vs.85).aspx
	HANDLE mapping = CreateFileMappingW(file,NULL,PAGE_READONLY,
		0, 0, INPUT_FILE_MAPPING_NAME );
	if( mapping == NULL ) {
		printf( "%d", GetLastError() );
		assert( false );
	}

	// https://msdn.microsoft.com/en-us/library/windows/desktop/aa366761(v=vs.85).aspx
	LPVOID addr = MapViewOfFile( mapping, FILE_MAP_READ, 0, 0, 0 );
	if( addr == NULL ) {
		printf( "%d", GetLastError() );
		assert( false );
	}
	WCHAR* text = (WCHAR*)addr+1;

	wprintf( L"%c %c %c\n", text[0], text[1], text[2] );

	// https://msdn.microsoft.com/ru-ru/library/windows/desktop/aa364955(v=vs.85).aspx
	int fileSize = GetFileSize( file, NULL );

	for( int i = 0; i < N_PARTS; ++i ) {
		borders[i] = fileSize*i / N_PARTS;
		borders[i] = getNearestSpace( file, borders[i] );
	}

	printf( "%d %d %d %d\n", borders[0], borders[1], borders[2], borders[3] );
	// do deeds
	for( int i = 0; i < N_PARTS - 1; ++i ) {
		sizes[i] = borders[i + 1] - borders[i];
	}
	sizes[N_PARTS - 1] = fileSize - borders[N_PARTS - 1];
	printf( "%d %d %d %d\n", sizes[0], sizes[1], sizes[2], sizes[3] );

	/*for( int i = 0; i < N_PARTS; ++i )
	{
		WCHAR* text = (WCHAR*)addr+1;

		wprintf( L"::" );
		for( int j = 0; j < sizes[i] / sizeof( WCHAR ); ++j ) {
			wprintf( L"%c", text[borders[i]/sizeof(WCHAR) + j] );
		}
		wprintf( L"\n" );
	}*/
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
		// https://msdn.microsoft.com/en-us/library/windows/desktop/ms682396(v=vs.85).aspx
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
	FileMap( src_file_name, borders, sizes );

	// https://msdn.microsoft.com/ru-ru/library/windows/desktop/ms682425(v=vs.85).aspx
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
	createEventsForWorkers( taskPreparedEvents, L"TaskPrepared", false, procInfo );
	HANDLE taskDoneEvents[N_PARTS];
	createEventsForWorkers( taskDoneEvents, L"TaskDone", false, procInfo );

	HANDLE endOfWork = CreateEvent(
		NULL,
		true, false,
		(std::wstring( L"EndOfWork" )).c_str()
	);
	if( endOfWork == NULL ) {
		printf( "%d", GetLastError() );
		assert( false );
	}

	SetEventsForWorkers( taskPreparedEvents );

	if( WaitForMultipleObjects( N_PARTS, taskDoneEvents, true, 0 ) == WAIT_FAILED ) {
		printf( "%d", GetLastError() );
	}
	
	if( !SetEvent(endOfWork) ) {
		printf( "%d", GetLastError() );
		assert( false );
	}

	/*CloseEventsForWorkers( taskPreparedEvents );
	CloseEventsForWorkers( taskDoneEvents );*/

	if( WaitForMultipleObjects( N_PARTS, handles, true, 0 ) == WAIT_FAILED ) {
		printf( "%d", GetLastError() );
	}

	for( int i = 0; i < N_PARTS; ++i ) {
		CloseHandle( procInfo[i].hProcess );
		CloseHandle( procInfo[i].hThread );
	}
	

	scanf( "" );
	return 0;
}

