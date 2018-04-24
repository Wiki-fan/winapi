#pragma once
#include <Windows.h>
#include <cstdio>
#include <string>
#include <vector>
#include "Field.h"
#include "Ellipse.h"
#include "EllipseWindow.h"
#include "common.h"

extern const char* WINDOWCLASSNAME;

const int N = 3;
const int M = 4;

class COverlappedWindow {
public:
	COverlappedWindow() :childWindows( N, std::vector<CEllipseWindow>( M ) )
	{
	}

	~COverlappedWindow() {
		CloseWindow( handle );
	}

	// Зарегистрировать класс окна
	static bool RegisterClass()
	{
		WNDCLASSEX wcx = { 0 };

		// Fill in the window class structure with parameters 
		// that describe the main window. 

		wcx.cbSize = sizeof( WNDCLASSEX ); 
		wcx.style = CS_HREDRAW | CS_VREDRAW;
		wcx.lpfnWndProc = COverlappedWindow::windowProc; 
		wcx.cbClsExtra = sizeof(LPVOID);
		wcx.cbWndExtra = sizeof(long); 
		wcx.hInstance = hInstance; 
		wcx.hIcon = LoadIcon( hInstance, MAKEINTRESOURCE( IDI_APPLICATION ) );
		wcx.hCursor = LoadCursor( NULL, IDC_ARROW ); 
		wcx.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
		wcx.lpszMenuName = NULL;
		wcx.lpszClassName = WINDOWCLASSNAME;
		wcx.hIconSm = LoadIcon( hInstance, MAKEINTRESOURCE( IDI_APPLICATION ) );

		return RegisterClassEx( &wcx );
	}

	void updateChildsSizes()
	{
		RECT winRect;
		if( !GetClientRect( handle, &winRect ) ) {
			HALT( "Get rect failed" );
		}
		int winW = winRect.right - winRect.left;
		int winH = winRect.bottom - winRect.top;
		float w = (float)winW / M;
		float h = (float)winH / N;

		for( int i = 0; i < childWindows.size(); ++i ) {
			for( int j = 0; j < childWindows[i].size(); ++j ) {
				RECT rect;
				rect.left = w*j;
				rect.right = rect.left + w;
				rect.top = h*i;
				rect.bottom = rect.top + h;
				childWindows[i][j].SetRect( rect );
			}
		}
	}

	// Создать экземпляр окна
	bool Create()
	{
		handle = CreateWindowA(
			WINDOWCLASSNAME,     // name of window class 
			"Sample",            // title-bar string 
			WS_OVERLAPPEDWINDOW, // top-level window 
			CW_USEDEFAULT,       // default horizontal position
			CW_USEDEFAULT,       // default vertical position
			640,       // width 
			480,       // height 
			static_cast<HWND>(NULL),
			static_cast<HMENU>(NULL),
			hInstance, 
			static_cast<LPVOID>(this) ); 
		if( !handle ) {
			HALT( "Window not created");
		}
	}

	// Показать окно
	void Show( int cmdShow )
	{
		MSG Msg;
		ShowWindow( handle, cmdShow );
		for( auto& row : childWindows ) {
			for( auto& win : row ) {
				win.Show(cmdShow);
			}
		}
		if( UpdateWindow( handle ) == 0 ) {
			HALT( "UpdateWindow main failed " );
		}

		while( GetMessage( &Msg, NULL, 0, 0 ) ) {
			TranslateMessage( &Msg );
			DispatchMessage( &Msg );
		}
	}

protected:
	void OnDestroy() {
		PostQuitMessage( 0 );
	}

	void OnCreate() {
		for( int i = 0; i < childWindows.size(); ++i ) {
			for( int j = 0; j < childWindows[i].size(); ++j ) {
				childWindows[i][j].Create( handle );
			}
		}

		updateChildsSizes();
	}

	void OnNCCreate( const HWND handle_ ) {
		SetLastError( 0 );
		DWORD err = GetLastError();
		LONG ret = SetWindowLongPtr( handle_, 0, reinterpret_cast<LONG_PTR>(this) );
		if( ret == 0 && err != 0 ) {
			MessageBox( NULL, ERRMSG( "Long don't set %d", GetLastError() ), NULL, MB_OK );
			exit( 3 );
		}
		handle = handle_;
	}

	void OnSize()
	{
		updateChildsSizes();
	}

private:
	HWND handle; // хэндл окна
	static LRESULT __stdcall windowProc( HWND handle, UINT message, WPARAM wParam, LPARAM lParam )
	{
		COverlappedWindow* window = reinterpret_cast<COverlappedWindow*>(GetWindowLongPtr( handle, 0 ));
		switch( message ) {
			case WM_DESTROY:
				window->OnDestroy();
				return 0;
			case WM_CREATE:
				window = (COverlappedWindow*)((LPCREATESTRUCT)lParam)->lpCreateParams;
				window->OnCreate();
				break;
			case WM_NCCREATE:
				window = (COverlappedWindow*)((LPCREATESTRUCT)lParam)->lpCreateParams;
				window->OnNCCreate( handle );
				return TRUE;
			case WM_SIZE:
				window->OnSize();
				break;
			default:
				return DefWindowProc( handle, message, wParam, lParam );
		}
		return 0;
	}

	std::vector<std::vector<CEllipseWindow>> childWindows;
};
