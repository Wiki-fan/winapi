#pragma once

#ifdef PALINDROME_EXPORTS  
#define PALINDROME_API extern "C" __declspec(dllexport)   
#else  
#define PALINDROME_API extern "C" __declspec(dllimport)   
#endif  

//PALINDROME_API bool isPalindrome( std::wstring word );
PALINDROME_API int PalindromeCounter( const wchar_t* text );
