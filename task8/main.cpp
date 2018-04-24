#include <Windows.h>
#include <cstdio>
#include "window.h"
#include "EllipseWindow.h"

HINSTANCE hInstance;

int WINAPI WinMain(HINSTANCE hInstance_, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	hInstance = hInstance_;
	COverlappedWindow wnd;
	if (!COverlappedWindow::RegisterClass())
	{
		HALT( "Window not registered" );
	}
	if( !CEllipseWindow::RegisterClass() ) {
		HALT( "Window not registered" );
	}

	wnd.Create();

	wnd.Show(nCmdShow);
	return 0;
}
