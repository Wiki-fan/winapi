#include <Windows.h>
#include <cstdio>
#include "window.h"

HINSTANCE hInstance;

int WINAPI WinMain(HINSTANCE hInstance_, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	hInstance = hInstance_;
	COverlappedWindow wnd;
	if (!wnd.RegisterClass())
	{
		MessageBox( NULL, ERRMSG( "Window not registered", GetLastError()), NULL, MB_OK );
		return 1;
	}

	wnd.Create();

	wnd.Show(nCmdShow);
	return 0;

}
