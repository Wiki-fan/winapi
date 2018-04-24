#pragma once
#define STRINGIZE_DETAIL(x) #x
#define STRINGIZE(x) STRINGIZE_DETAIL(x)
#define ERRMSG(TEXT, MSG) (TEXT+std::to_string(MSG)+"" STRINGIZE(__FILE__) " " STRINGIZE(__LINE__)).c_str()
#define HALT(TEXT) MessageBox( NULL, ERRMSG( (TEXT), GetLastError() ), NULL, MB_OK ); exit(1);
extern HINSTANCE hInstance;
