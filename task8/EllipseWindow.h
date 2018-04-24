#pragma once
#include "Ellipse.h"
#include "Field.h"
#include <Windows.h>
#include <cstdio>
#include <iostream>
#include <string>
#include "common.h"

const int IDT_TIMER1 = 1;
const int TIMEOUT = 10;

extern const char* ELLIPSEWINDOWCLASSNAME;
extern HINSTANCE hInstance;

class CEllipseWindow {
public:
	CEllipseWindow() : ellipse( 10, 15 )
	{
	}

	~CEllipseWindow()
	{
		CloseWindow( handle );
	}

	// Зарегистрировать класс окна
	static bool RegisterClass() 
	{
		WNDCLASSEX wcx = { 0 };

		wcx.cbSize = sizeof( WNDCLASSEX );
		wcx.style = CS_HREDRAW | CS_VREDRAW;
		wcx.lpfnWndProc = CEllipseWindow::windowProc;
		wcx.cbClsExtra = sizeof( LPVOID );
		wcx.cbWndExtra = sizeof( long );
		wcx.hInstance = hInstance;
		wcx.hIcon = LoadIcon( hInstance, MAKEINTRESOURCE( IDI_APPLICATION ) );
		wcx.hCursor = LoadCursor( NULL, IDC_ARROW );
		wcx.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
		wcx.lpszMenuName = NULL;
		wcx.lpszClassName = ELLIPSEWINDOWCLASSNAME;
		wcx.hIconSm = LoadIcon( hInstance, MAKEINTRESOURCE( IDI_APPLICATION ) );

		return RegisterClassEx( &wcx );
	}

	// Создать экземпляр окна
	bool Create(HWND parent)
	{
		handle = CreateWindowA(
			ELLIPSEWINDOWCLASSNAME,   // name of window class 
			"Sample",				  // title-bar string 
			WS_CHILD | WS_VISIBLE | WS_BORDER, // top-level window 
			CW_DEFAULT,				  // default horizontal position
			CW_DEFAULT,				  // default vertical position
			CW_DEFAULT,       // width 
			CW_DEFAULT,       // height 
			parent,
			static_cast<HMENU>(NULL),
			hInstance,
			static_cast<LPVOID>(this) );
		if( !handle ) {
			MessageBox( NULL, ERRMSG( "Window not created", GetLastError() ), NULL, MB_OK );
			getchar();
			exit( 2 );
		}
	}

	// Показать окно
	void Show( int cmdShow )
	{
		MSG Msg;

		ShowWindow( handle, cmdShow );
	}

protected:
	void SetRect( RECT rect )
	{
		if( !SetWindowPos( handle, HWND_BOTTOM,
			rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
			NULL ) ) {
			HALT( "SetWindowPos" );
		}
		field.x = rect.right - rect.left;
		field.y = rect.bottom - rect.top;
		ellipse.fixPosition(&field);
	}

	void StartTimer()
	{
		SetTimer( handle,
			IDT_TIMER1,
			TIMEOUT,
			static_cast<TIMERPROC>(NULL) );
	}

	void OnDestroy()
	{
		PostQuitMessage( 0 );
	}

	void OnCreate()
	{
		StartTimer();
		TRACKMOUSEEVENT mouseEvent;
		mouseEvent.cbSize = sizeof( TRACKMOUSEEVENT );
		mouseEvent.hwndTrack = handle;
		mouseEvent.dwHoverTime = 100;
		mouseEvent.dwFlags = TME_HOVER;
		if( !TrackMouseEvent( &mouseEvent ) ) {
			HALT( "TrackMouse" );
		}
	}

	void OnNCCreate( const HWND handle_ )
	{
		SetLastError( 0 );
		DWORD err = GetLastError();
		LONG ret = SetWindowLongPtr( handle_, 0, reinterpret_cast<LONG_PTR>(this) );
		if( ret == 0 && err != 0 ) {
			MessageBox( NULL, ERRMSG( "Long don't set %d", GetLastError() ), NULL, MB_OK );
			exit( 3 );
		}
		handle = handle_;
	}

	void OnTimer()
	{
		ellipse.step(&field);
		InvalidateRect( handle, NULL, TRUE );
		if( GetFocus() != handle ) {
			KillTimer( handle, IDT_TIMER1 );
		}
	}

	void OnPaint()
	{
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
			field.y );
		// Select the bitmap into the off-screen DC.
		hbmOld = static_cast<HBITMAP>(SelectObject( hdcMem, hbmMem ));

		hbrBkGnd = CreateSolidBrush( GetSysColor( COLOR_WINDOW ) );
		FillRect( hdcMem, &rc, hbrBkGnd );
		DeleteObject( hbrBkGnd );

		// Render the image into the offscreen DC.
		SetBkMode( hdcMem, TRANSPARENT );
		ellipse.draw( hdcMem, (GetFocus() == handle));

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
		CEllipseWindow* window = reinterpret_cast<CEllipseWindow*>(GetWindowLongPtr( handle, 0 ));
		switch( message ) {
			case WM_DESTROY:
				window->OnDestroy();
				return 1;
			case WM_CREATE:
				window = (CEllipseWindow*)((LPCREATESTRUCT)lParam)->lpCreateParams;
				window->OnCreate();
				break;
			case WM_NCCREATE:
				window = (CEllipseWindow*)((LPCREATESTRUCT)lParam)->lpCreateParams;
				window->OnNCCreate( handle );
				return TRUE;
			case WM_TIMER:
				window->OnTimer();
				break;
			case WM_PAINT:
				window->OnPaint();
				break;
			case WM_LBUTTONDOWN:
				SetFocus( handle );
				window->StartTimer();
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

	friend class COverlappedWindow;
};
