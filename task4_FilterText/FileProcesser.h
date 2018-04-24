#pragma once
#include <cstdio>
#include <cassert>
#include <Windows.h>
#include "Names.h"

class FileProcesser {
	HANDLE file;
	HANDLE mapping;
	LPVOID addr;
public:
	int borders[N_PARTS];
	int sizes[N_PARTS];

	DWORD getNearestSpace( LONG initialGuess )
	{
		if( initialGuess == 0 ) {
			return 0;
		}
		if( SetFilePointer( file, initialGuess + sizeof( WCHAR ), NULL, FILE_BEGIN ) == INVALID_SET_FILE_POINTER ) {
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
		return SetFilePointer( file, 0, NULL, FILE_CURRENT ) - sizeof( WCHAR );
	}

	LPVOID MapFile( wchar_t* src_file_name)
	{
		file = CreateFileW( src_file_name, GENERIC_READ, FILE_SHARE_READ, NULL,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
		if( file == INVALID_HANDLE_VALUE ) {
			printf( "%d", GetLastError() );
			assert( false );
		}

		mapping = CreateFileMappingW( file, NULL, PAGE_READONLY,
			0, 0, INPUT_FILE_MAPPING_NAME );
		if( mapping == NULL ) {
			printf( "%d", GetLastError() );
			assert( false );
		}

		addr = MapViewOfFile( mapping, FILE_MAP_READ, 0, 0, 0 );
		if( addr == NULL ) {
			printf( "%d", GetLastError() );
			assert( false );
		}
		return addr;
	}

	int ProcessFile(LPVOID addr)
	{
		WCHAR* text = (WCHAR*)addr + 1;

		int fileSize = GetFileSize( file, NULL );

		for( int i = 0; i < N_PARTS; ++i ) {
			borders[i] = fileSize*i / N_PARTS;
			borders[i] = getNearestSpace( borders[i] );
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

	~FileProcesser()
	{
		UnmapViewOfFile( addr );
		CloseHandle( mapping );
		CloseHandle( file );
	}
};