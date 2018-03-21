#include "Palindrome.h"
#include <string>
#include <sstream>

bool isPalindrome( std::wstring word )
{
	for( int i = 0; i < word.length() / 2 + 1; ++i ) {
		if( word[i] != word[word.length() - 1 - i] ) {
			return false;
		}
	}
	return true;
}

int PalindromeCounter( const wchar_t * text )
{
	std::wstring s( text );
	std::wstringstream ss( s );
	std::wstring word;
	int palindromeCount = 0;
	while( ss >> word ) {
		palindromeCount += isPalindrome(word);
	}

	return palindromeCount;
}
