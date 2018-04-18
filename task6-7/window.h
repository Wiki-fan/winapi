#pragma once
#include <Windows.h>
#include <cstdio>
#include <string>
#include "Field.h"

const int IDT_TIMER1 = 1;
const int TIMEOUT = 50;

#define ERRMSG(TEXT, MSG) (TEXT+std::to_string(MSG)).c_str()
extern HINSTANCE hInstance;

extern const char* WINDOWCLASSNAME;

LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );

class COverlappedWindow {
public:
	COverlappedWindow(  ) : ellipse(&field, 100, 150)
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

		wcx.cbSize = sizeof( WNDCLASSEX );          // size of structure 
		wcx.style = CS_HREDRAW | CS_VREDRAW; // redraw if HORIZONTAL or VERTICAL size changes 
		wcx.lpfnWndProc = COverlappedWindow::windowProc; //COverlappedWindow::windowProc;     // points to window procedure 
		wcx.cbClsExtra = 0; // no extra class memory 
		wcx.cbWndExtra = sizeof(long); 
		wcx.hInstance = hInstance;         // handle to instance 
		wcx.hIcon = LoadIcon( hInstance, MAKEINTRESOURCE( IDI_APPLICATION ) ); // LoadIcon( NULL, IDI_APPLICATION ); // predefined app. icon 
		wcx.hCursor = LoadCursor( NULL, IDC_ARROW ); // predefined arrow 
		wcx.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);// static_cast<HBRUSH>(GetStockObject( WHITE_BRUSH )); // (HBRUSH)COLOR_BACKGROUND;
		wcx.lpszMenuName = NULL;    // name of menu resource 
		wcx.lpszClassName = WINDOWCLASSNAME;  // name of window class 
		wcx.hIconSm = LoadIcon( hInstance, MAKEINTRESOURCE( IDI_APPLICATION ) ); // LoadIcon( NULL, IDI_APPLICATION );

		return RegisterClassEx( &wcx );
	}

	// Создать экземпляр окна
	bool Create()
	{
		handle = CreateWindow(
			WINDOWCLASSNAME,        // name of window class 
			"Sample",            // title-bar string 
			WS_OVERLAPPEDWINDOW, // top-level window 
			100,       // default horizontal position  CW_USEDEFAULT
			100,       // default vertical position 
			640,       // default width 
			480,       // default height 
			(HWND)NULL,          // no owner window 
			(HMENU)NULL,         // use class menu 
			hInstance,           // handle to application instance 
			(LPVOID)NULL );      // no window-creation data 
		if( !handle ) {
			MessageBox( NULL, ERRMSG( "Window not created", GetLastError() ), NULL, MB_OK );
			getchar();
			exit(2);
		}
		SetLastError( 0 );
		DWORD err = GetLastError();
		LONG ret = SetWindowLongPtr( handle, 0, (LONG_PTR)this );
		if( ret == 0 && err != 0 ) {
			MessageBox( NULL, ERRMSG( "Long don't set %d", GetLastError() ), NULL, MB_OK );
			exit(3);
		}
	}

	// Показать окно
	void Show( int cmdShow )
	{
		MSG Msg;

		ShowWindow( handle, cmdShow );
		if( UpdateWindow( handle ) == 0 ) {
			MessageBox( NULL, ERRMSG( "UpdateWindow failed ", GetLastError() ), NULL, MB_OK );
			exit( 1 );
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
	}
	void OnNCCreate( const HWND handle ) {

	}
	void OnTimer() {
		ellipse.step();
		InvalidateRect( handle, NULL, TRUE);
		/*if( UpdateWindow( handle ) == 0 ) {
			MessageBox( NULL, ERRMSG( "UpdateWindow failed ", GetLastError() ), NULL, MB_OK );
			exit( 1 );
		}*/
		SetTimer( handle, 
			IDT_TIMER1,
			TIMEOUT,
			(TIMERPROC)NULL );
	}

	void OnSize()
	{
		RECT rect;
		if( !GetClientRect( handle, &rect ) ) {
			MessageBox( NULL, ERRMSG( "Client rect get failed", GetLastError() ), NULL, MB_OK );
			exit( 1 );
		}
		field.x = rect.right;
		field.y = rect.bottom;
	}

	void OnPaint() {
		static bool first_paint = true;
		if( first_paint ) {
			first_paint = false;
			SetTimer( handle,          // handle to main window 
				IDT_TIMER1,            // timer identifier 
				TIMEOUT,                   // 10-second interval 
				(TIMERPROC)NULL );     // no timer callback 
		}

		PAINTSTRUCT ps;
		HDC hDC = BeginPaint( handle, &ps );

		RECT rc;
		HDC hdcMem;
		HBITMAP hbmMem, hbmOld;
		HBRUSH hbrBkGnd;
		HFONT hfntOld;

		GetClientRect( handle, &rc );
		hdcMem = CreateCompatibleDC( hDC );
		hbmMem = CreateCompatibleBitmap( ps.hdc,
			field.x,
			field.y);
		// Select the bitmap into the off-screen DC.
		hbmOld = (HBITMAP)SelectObject( hdcMem, hbmMem );

		hbrBkGnd = CreateSolidBrush( GetSysColor( COLOR_WINDOW ) );
		FillRect( hdcMem, &rc, hbrBkGnd );
		DeleteObject( hbrBkGnd );

		// Render the image into the offscreen DC.
		SetBkMode( hdcMem, TRANSPARENT );
		ellipse.draw(hdcMem);

		// Blt the changes to the screen DC.
		BitBlt( ps.hdc,
			0, 0,
			field.x, field.y,
			hdcMem,
			0, 0,
			SRCCOPY );

		SelectObject( hdcMem, hbmOld );
		DeleteObject( hbmMem );
		DeleteDC( hdcMem );

		EndPaint( handle, &ps );
		DeleteDC( hDC );
	}

private:
	HWND handle; // хэндл окна
	static LRESULT __stdcall windowProc( HWND handle, UINT message, WPARAM wParam, LPARAM lParam )
	{
		COverlappedWindow* window = (COverlappedWindow*)GetWindowLongPtr( handle, 0 );
		switch( message ) {
			case WM_DESTROY:
				window->OnDestroy();
				break;
			case WM_CREATE:
				window->OnCreate();
				break;
			/*case WM_NCCREATE:
				window->OnNCCreate( handle );
				break;*/
			case WM_TIMER:
				window->OnTimer();
				break;
			case WM_SIZE:
				window->OnSize();
				break;
			case WM_PAINT:
				window->OnPaint();
				break;
			case WM_ERASEBKGND:
				return (LRESULT)1; // Say we handled it.
			default:
				// Process the left-over messages
				return DefWindowProc( handle, message, wParam, lParam );
		}
		return 0;
	}

	Field field;
	Object ellipse;
};
