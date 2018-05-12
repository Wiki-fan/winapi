#include <Windows.h>
#include <cstdio>
#include "EllipseWindow.h"
#include <ctime>

HINSTANCE hInstance;

int WINAPI WinMain(HINSTANCE hInstance_, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	srand( time( NULL ) );
	hInstance = hInstance_;
	CEllipseWindow wnd;

	if( !CEllipseWindow::RegisterClass() ) {
		HALT( "Window not registered" );
	}

	wnd.Create();

	wnd.Show(nCmdShow);
	return 0;
}
