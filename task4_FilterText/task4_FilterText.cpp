// task4_FilterText.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <Windows.h>
#include <cassert>
#include <string>
#include <cstring>
#include "EventManager.h"
#include "FileProcesser.h"
#include "Names.h"


void processResultFiles( HANDLE* mappings, LPWSTR target_name, PROCESS_INFORMATION* procInfo, int size )
{
	FILE* f = _wfopen( target_name, L"w" );

	for( int i = 0; i < N_PARTS; ++i ) {
		HANDLE mapping = mappings[i];

		LPVOID addr = MapViewOfFile( mapping, FILE_MAP_READ, 0, 0, 0 );
		if( addr == NULL ) {
			printf( "%d", GetLastError() );
			assert( false );
		}
		WCHAR* text = (WCHAR*)addr;

		fwprintf( f, L"%s ", text );
		wprintf( L"out %s\n", text );
	}
	fclose( f );
}

int wmain( int argc, wchar_t* argv[] )
{
	std::wstring target_words( argv[1] );
	wchar_t* src_file_name = argv[2];
	wchar_t* target_name = argv[3];

	FileProcesser fileProcesser;
	LPVOID addr = fileProcesser.MapFile( src_file_name);
	int fileSize = fileProcesser.ProcessFile( addr );

	STARTUPINFO startupInfo[N_PARTS];
	ZeroMemory( startupInfo, N_PARTS * sizeof( STARTUPINFO ) );
	PROCESS_INFORMATION procInfo[N_PARTS];
	ZeroMemory( procInfo, N_PARTS * sizeof( PROCESS_INFORMATION ) );
	for( int i = 0; i < N_PARTS; ++i ) {
		startupInfo[i].cb = sizeof( STARTUPINFO );
		if( !CreateProcessW(
			L"Debug\\task4_Worker.exe",
			const_cast<LPWSTR>((target_words + L" " + std::to_wstring( fileProcesser.borders[i] ) + L" " + std::to_wstring( fileProcesser.sizes[i] )).c_str()),
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

	PartsEventManager eventMgr;
	HANDLE taskPreparedEvents[N_PARTS];
	eventMgr.createEventsForWorkers( taskPreparedEvents, TASK_PREPARED, false, procInfo );
	HANDLE taskDoneEvents[N_PARTS];
	eventMgr.createEventsForWorkers( taskDoneEvents, TASK_DONE, false, procInfo );

	HANDLE endOfWork = eventMgr.createEvent(std::wstring( END_OF_WORK ), true);
	
	HANDLE mappings[N_PARTS];
	for( int i = 0; i < N_PARTS; ++i ) {
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

	eventMgr.SetEventsForWorkers( taskPreparedEvents );

	int res = WaitForMultipleObjects( N_PARTS, taskDoneEvents, true, INFINITE );
	
	if( res == WAIT_FAILED) {
		printf( "%d, %d", res, GetLastError() );
		assert( false );
	}

	//Sleep( 100 );
	processResultFiles( mappings, target_name, procInfo, fileSize );
	eventMgr.setEvent( endOfWork );


	if( WaitForMultipleObjects( N_PARTS, handles, true, INFINITY ) == WAIT_FAILED ) {
		printf( "%d", GetLastError() );
		assert( false );
	}

	for( int i = 0; i < N_PARTS; ++i ) {
		CloseHandle( procInfo[i].hProcess );
		CloseHandle( procInfo[i].hThread );
	}

	scanf( "" );
	return 0;
}
