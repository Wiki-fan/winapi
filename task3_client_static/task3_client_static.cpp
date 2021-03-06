// task3_client.cpp: ���������� ����� ����� ��� ����������� ����������.
//

#include "stdafx.h"
#include "Palindrome.h"
#include <Windows.h>
#include <string>
#include <fstream>

int wmain( int argc, wchar_t* argv[] )
{
	if( argc < 2 ) {
		return 1;
	}

	wchar_t* filename = argv[1];
	std::wifstream ifs( filename );
	std::wstring content( (std::istreambuf_iterator<wchar_t>( ifs )),
		(std::istreambuf_iterator<wchar_t>()) );

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

