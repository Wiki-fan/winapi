// task3_client.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include "Palindrome.h"
#include <Windows.h>
#include <string>
#include <fstream>
#include <iostream>

typedef int( *f_funci )(const wchar_t * text);

int wmain( int argc, wchar_t* argv[] )
{
	if( argc < 2 ) {
		return 1;
	}

	wchar_t* filename = argv[1];
	std::wifstream ifs( filename );
	std::wstring content( (std::istreambuf_iterator<wchar_t>( ifs )),
		(std::istreambuf_iterator<wchar_t>()) );

	HMODULE lib = LoadLibrary( L"../Debug/task3_dynamic.dll" );

	if( !lib ) {
		std::cout << "could not load the dynamic library" << std::endl;
		return EXIT_FAILURE;
	}

	f_funci PalindromeCounter = (f_funci)GetProcAddress( lib, "PalindromeCounter" );
	if( !PalindromeCounter ) {
		std::cout << GetLastError() <<std::endl;
		std::cout << "could not locate the function" << std::endl;
		return EXIT_FAILURE;
	}

	auto palindromeCount = PalindromeCounter( content.c_str() );

	wchar_t buffer[1000];
	wsprintf( buffer, L"The text contains %d palindromes", palindromeCount );
	int msgboxID = MessageBox(
		NULL,
		(LPCWSTR)buffer,
		(LPCWSTR)L"PalindromeCount",
		MB_OK
	);
	return 0;
}

