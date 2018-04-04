// task4_Worker.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <string>
#include <cassert>
#include <fstream>
#include <unordered_set>
#include <locale>
#include <codecvt>
#include <Windows.h>
#include "Names.h"

//const int TASK_DONE = 0;
const int END_OF_WORK = 1;
const int TASK_PREPARED = 0;

std::wstring getWord( WCHAR* text, int& position, int size )
{
	std::wstring s;
	for( int i = 0; i < size / sizeof( WCHAR ); ++i ) {
		WCHAR c = text[position / sizeof( WCHAR ) + i];
		if( iswspace( c ) ) {
			position = position + i*sizeof(WCHAR);
			return s;
		} else {
			s.append(1, c );
		}
	}
	position = position + size;
	return s;
}

void processFile(int position, int size, std::unordered_set<std::wstring>& stopWords )
{
	// https://msdn.microsoft.com/ru-ru/library/windows/desktop/aa366791(v=vs.85).aspx
	HANDLE mapping = OpenFileMapping(
		FILE_MAP_ALL_ACCESS,
		FALSE,
		INPUT_FILE_MAPPING_NAME );

	// https://msdn.microsoft.com/en-us/library/windows/desktop/aa366761(v=vs.85).aspx
	LPVOID addr = MapViewOfFile( mapping, FILE_MAP_READ, 0, 0, 0);
	if( addr == NULL ) {
		printf( "%d", GetLastError() );
		assert( false );
	}
	WCHAR* text = (WCHAR*)addr + 1;

	std::wstring s;
	for( int i = 0; i < size/sizeof(WCHAR); ++i ) {
		s.append(1, text[position/sizeof(WCHAR)+i] );
	}
	wprintf( L"\n%s\n", s.c_str());

	HANDLE outputMapping = CreateFileMapping(
		INVALID_HANDLE_VALUE,    // use paging file
		NULL,                    // default security
		PAGE_READWRITE,          // read/write access
		0,                       // maximum object size (high-order DWORD)
		size,                // maximum object size (low-order DWORD)
		(OUTPUT_FILE_MAPPING_NAME + std::to_wstring(GetCurrentProcessId() )).c_str());                 // name of mapping object

	LPVOID outputAddr = MapViewOfFile( outputMapping, FILE_MAP_WRITE, 0, 0, 0 );
	if( outputAddr == NULL ) {
		printf( "%d", GetLastError() );
		assert( false );
	}

	//std::wstring s;
	s.clear();
	for( int i = 0; i < size / sizeof( WCHAR ); ++i ) {
		WCHAR c = text[position / sizeof( WCHAR ) + i];
		if( iswspace( c ) ) {
			if( !s.empty() ) {
				if( stopWords.find(s) != stopWords.end() ) { 
					wprintf( L"\n::%s\n", s.c_str() ); }
			}
			s.clear();
		} else {
			s.append( 1, c );
		}
	}


}

int wmain( int argc, wchar_t* argv[] )
{
	wchar_t* target_words = argv[0];
	int position = std::stoi( argv[1] );
	int size = std::stoi( argv[2] );

	std::unordered_set<std::wstring> stopWords;
	std::wifstream ifs( std::wstring(target_words), std::ios_base::in );
	ifs.imbue( std::locale( ifs.getloc(), new std::codecvt_utf16<wchar_t, 0x10ffff, std::consume_header> ) );
	std::wstring word;
	while( ifs >> word ) {
		stopWords.insert( word );
	}

	wprintf( L"I'm worker %s, %d, %d\n", target_words, position, size );

	HANDLE events[2];

	std::wstring taskPreparedEventName = std::wstring( L"TaskPrepared" ) + std::to_wstring( GetCurrentProcessId() );
	events[TASK_PREPARED] = OpenEvent( EVENT_ALL_ACCESS, false, taskPreparedEventName.c_str() );
	if( events[TASK_PREPARED] == NULL ) {
		printf( "%d", GetLastError() );
		assert( false );
	}

	std::wstring endOfWorkEventName = std::wstring( L"EndOfWork" );
	events[END_OF_WORK] = OpenEvent( EVENT_ALL_ACCESS, false, endOfWorkEventName.c_str() );
	if( events[END_OF_WORK] == NULL ) {
		printf( "%d", GetLastError() );
		assert( false );
	}

	std::wstring taskDoneEventName = std::wstring( L"TaskDone" ) + std::to_wstring( GetCurrentProcessId() );
	HANDLE taskDoneEvent = OpenEvent( EVENT_ALL_ACCESS, false, taskDoneEventName.c_str() );
	if( taskDoneEvent == NULL ) {
		printf( "%d", GetLastError() );
		assert( false );
	}
	
	bool working = true;
	while( working ) {
		DWORD dwWaitResult = WaitForMultipleObjects(
			2, events, false, INFINITE );

		switch( dwWaitResult ) {
			// Event object was signaled
			case WAIT_OBJECT_0 + TASK_PREPARED:
				printf( "Thread %d reading from buffer\n", GetCurrentThreadId() );
				//process here
				processFile(position, size, stopWords);
				if( !SetEvent( taskDoneEvent ) ) {
					printf( "%d", GetLastError() );
					assert( false );
				}
				break;
			case WAIT_OBJECT_0 + END_OF_WORK:
				printf( "Thread %d stopping work\n", GetCurrentThreadId() );
				working = false;
				break;
			default:
				printf( "Wait error (%d)\n", GetLastError() );
				return 0;
		}
	}

	printf( "Thread %d stopped work\n", GetCurrentThreadId() );

	return 0;
}

