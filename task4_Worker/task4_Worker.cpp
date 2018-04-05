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
#include "EventManager.h"
#include "Names.h"

const int END_OF_WORK_INDEX = 1;
const int TASK_PREPARED_INDEX = 0;

std::wstring getWord( WCHAR* text, int& position, int size )
{
	std::wstring s;
	for( int i = 0; i < size / sizeof( WCHAR ); ++i ) {
		WCHAR c = text[position / sizeof( WCHAR ) + i];
		if( iswspace( c ) ) {
			position = position + i * sizeof( WCHAR );
			return s;
		} else {
			s.append( 1, c );
		}
	}
	position = position + size;
	return s;
}

class OutputFileWriter {

public:
	HANDLE outputMapping;
	LPWSTR outputAddr;
	int pos;
	OutputFileWriter( HANDLE fileHandle, int size )
	{
		outputMapping = OpenFileMappingW(
			FILE_MAP_ALL_ACCESS,
			FALSE,
			(OUTPUT_FILE_MAPPING_NAME + std::to_wstring( GetCurrentProcessId() )).c_str() );

		if( outputMapping == NULL ) {
			printf( "%d", GetLastError() );
			assert( false );
		}
		outputAddr = (LPWSTR)MapViewOfFile( outputMapping, FILE_MAP_WRITE, 0, 0, 0 );
		if( outputAddr == NULL ) {
			printf( "%d", GetLastError() );
			assert( false );
		}

		pos = 0;
	}

	void append( std::wstring& s )
	{
		memcpy( outputAddr + pos, s.c_str(), s.length() * sizeof( WCHAR ) );
		pos += s.length();
		outputAddr[pos] = L' ';
		pos += 1;
	}

	void finalize()
	{
		outputAddr[pos] = L'\0';
	}

	~OutputFileWriter()
	{
		CloseHandle( outputMapping );
		UnmapViewOfFile( outputAddr );
	}

};

void processFile( int position, int size, std::unordered_set<std::wstring>& stopWords )
{
	HANDLE mapping = OpenFileMapping(
		FILE_MAP_ALL_ACCESS,
		FALSE,
		INPUT_FILE_MAPPING_NAME );

	LPVOID addr = MapViewOfFile( mapping, FILE_MAP_READ, 0, 0, 0 );
	if( addr == NULL ) {
		printf( "%d", GetLastError() );
		assert( false );
	}
	WCHAR* text = (WCHAR*)addr + 1;

	wprintf( L"arrived %.*s\n", size / sizeof( WCHAR ), text + position / sizeof( WCHAR ) );

	OutputFileWriter ofw( INVALID_HANDLE_VALUE, size );

	std::wstring s;
	s.clear();
	for( int i = 0; i < size / sizeof( WCHAR ); ++i ) {
		WCHAR c = text[position / sizeof( WCHAR ) + i];
		if( iswspace( c ) ) {
			if( !s.empty() ) {
				if( stopWords.find( s ) == stopWords.end() ) {
					ofw.append( s );
				}
			}
			s.clear();
		} else {
			s.append( 1, c );
		}
	}
	if( !s.empty() ) {
		if( stopWords.find( s ) == stopWords.end() ) {
			ofw.append( s );
		}
	}
	ofw.finalize();

	wprintf( L"str %s\n", ofw.outputAddr );
}

int wmain( int argc, wchar_t* argv[] )
{
	wchar_t* target_words = argv[0];
	int position = std::stoi( argv[1] );
	int size = std::stoi( argv[2] );

	std::unordered_set<std::wstring> stopWords;
	std::wifstream ifs( std::wstring( target_words ), std::ios_base::in );
	ifs.imbue( std::locale( ifs.getloc(), new std::codecvt_utf16<wchar_t, 0x10ffff, std::consume_header> ) );
	std::wstring word;
	while( ifs >> word ) {
		stopWords.insert( word );
	}

	wprintf( L"I'm worker %s, %d, %d\n", target_words, position, size );

	EventManager eventMgr;
	HANDLE events[2];
	/*events[TASK_PREPARED_INDEX] = eventMgr.createEvent( std::wstring( TASK_PREPARED ) + std::to_wstring( GetCurrentProcessId() ), false );
	events[END_OF_WORK_INDEX] = eventMgr.createEvent( std::wstring( END_OF_WORK ), true );
	HANDLE taskDoneEvent = eventMgr.createEvent( std::wstring( TASK_DONE ) + std::to_wstring( GetCurrentProcessId() ), false );*/
	events[TASK_PREPARED_INDEX] = eventMgr.openEvent( std::wstring( TASK_PREPARED ) + std::to_wstring( GetCurrentProcessId() ));
	events[END_OF_WORK_INDEX] = eventMgr.openEvent( std::wstring( END_OF_WORK ));
	HANDLE taskDoneEvent = eventMgr.openEvent( std::wstring( TASK_DONE ) + std::to_wstring( GetCurrentProcessId() ));

	bool working = true;
	while( working ) {
		DWORD dwWaitResult = WaitForMultipleObjects(2, events, false, INFINITE );

		switch( dwWaitResult ) {
			case WAIT_OBJECT_0 + TASK_PREPARED_INDEX:
				printf( "Thread %d processing\n", GetCurrentThreadId() );
				processFile( position, size, stopWords );
				eventMgr.setEvent( taskDoneEvent );
				break;
			case WAIT_OBJECT_0 + END_OF_WORK_INDEX:
				printf( "Thread %d stopping work\n", GetCurrentThreadId() );
				working = false;
				break;
			default:
				printf( "Wait error (%d)\n", GetLastError() );
				return 0;
		}
	}

	return 0;
}

