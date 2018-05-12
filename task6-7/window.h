#pragma once
#include <Windows.h>
#include <cstdio>
#include <string>
#include "Field.h"
#include "Ellipse.h"

const int IDT_TIMER1 = 1;
const int TIMEOUT = 10;

#define ERRMSG(TEXT, MSG) (TEXT+std::to_string(MSG)).c_str()
extern HINSTANCE hInstance;

extern const char* WINDOWCLASSNAME;

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
			MessageBox( NULL, ERRMSG( "Window not created", GetLastError() ), NULL, MB_OK );
			getchar();
			exit(2);
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
		SetTimer( handle,
			IDT_TIMER1,
			TIMEOUT,
			static_cast<TIMERPROC>(NULL) );
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

	void OnTimer() {
		ellipse.step();
		InvalidateRect( handle, NULL, TRUE);
		SetTimer( handle, 
			IDT_TIMER1,
			TIMEOUT,
			static_cast<TIMERPROC>(NULL) );
	}

	void OnSize()
	{
		RECT rect;
		if( !GetClientRect( handle, &rect ) ) {
			MessageBox( NULL, ERRMSG( "Client rect get failed", GetLastError() ), NULL, MB_OK );
			exit( 1 );
		}
		field.width = rect.right;
		field.height = rect.bottom;
	}

	void OnPaint() {
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
			field.width,
			field.height);
		// Select the bitmap into the off-screen DC.
		hbmOld = static_cast<HBITMAP>(SelectObject( hdcMem, hbmMem ));

		hbrBkGnd = CreateSolidBrush( GetSysColor( COLOR_WINDOW ) );
		FillRect( hdcMem, &rc, hbrBkGnd );
		DeleteObject( hbrBkGnd );

		// Render the image into the offscreen DC.
		SetBkMode( hdcMem, TRANSPARENT );
		ellipse.draw(hdcMem);

		// Blt the changes to the screen DC.
		BitBlt( ps.hdc,
			0, 0,
			field.width, field.height,
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
		COverlappedWindow* window = reinterpret_cast<COverlappedWindow*>(GetWindowLongPtr( handle, 0 ));
		switch( message ) {
			case WM_DESTROY:
				window->OnDestroy();
				break;
			case WM_CREATE:
				window = (COverlappedWindow*)((LPCREATESTRUCT)lParam)->lpCreateParams;
				window->OnCreate();
				break;
			case WM_NCCREATE:
				window = (COverlappedWindow*)((LPCREATESTRUCT)lParam)->lpCreateParams;
				window->OnNCCreate( handle );
				return TRUE;
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
				return (LRESULT)1; 
			default:
				return DefWindowProc( handle, message, wParam, lParam );
		}
		return 0;
	}

	Field field;
	EllipseObject ellipse;
};
