#pragma once
#include <Windows.h>
#include <cstdio>
#include <string>
#include "Field.h"
#include "Ellipse.h"
#include "Figure.h"
#include "InputController.h"

const int IDT_TIMER1 = 1;
const int TIMEOUT = 200;

extern HINSTANCE hInstance;

extern const char* WINDOWCLASSNAME;

class CEllipseWindow {
public:
	CEllipseWindow() : ellipse(  100, 150 ), field(10, 20), figure(field), inputController(this)
	{
	}

	~CEllipseWindow()
	{
		CloseWindow( handle );
	}

	void Redraw()
	{
		InvalidateRect( handle, NULL, TRUE );
	}

	// Зарегистрировать класс окна
	static bool RegisterClass()
	{
		WNDCLASSEX wcx = { 0 };

		// Fill in the window class structure with parameters 
		// that describe the main window. 

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
			exit( 2 );
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
	void OnDestroy()
	{
		PostQuitMessage( 0 );
	}

	void OnCreate()
	{
		SetTimer( handle,
			IDT_TIMER1,
			TIMEOUT,
			static_cast<TIMERPROC>(NULL) );
		figure.Create();
		inputController.figure = &figure;
	}

	void OnNCCreate( const HWND handle_ )
	{
		SetLastError( 0 );
		DWORD err = GetLastError();
		LONG ret = SetWindowLongPtr( handle_, 0, reinterpret_cast<LONG_PTR>(this) );
		if( ret == 0 && err != 0 ) {
			HALT( "Long don't set %d" );
		}
		handle = handle_;
	}

	void OnTimer()
	{
		inputController.process();
		ellipse.step(&field);
		if( !figure.Move( Point( 0, 1 ) ) ) {
			figure.Create();
		}
		Redraw();
	}

	void OnSize()
	{
		RECT rect;
		if( !GetClientRect( handle, &rect ) ) {
			HALT( "Client rect get failed" );
		}
		field.width = rect.right;
		field.height = rect.bottom;
	}

	void DrawScene(HDC hdc)
	{
		field.draw( hdc );
		ellipse.draw( hdc, true );
		figure.draw(hdc);
	}

	void OnPaint()
	{
		PAINTSTRUCT ps;
		HDC hDC = BeginPaint( handle, &ps );

		RECT rc;
		HDC hdcMem;
		HBITMAP hbmMem, hbmOld;
		HBRUSH hbrBkGnd;

		GetClientRect( handle, &rc );
		hdcMem = CreateCompatibleDC( hDC );
		hbmMem = CreateCompatibleBitmap( ps.hdc,
			field.width,
			field.height );
		// Select the bitmap into the off-screen DC.
		hbmOld = static_cast<HBITMAP>(SelectObject( hdcMem, hbmMem ));

		hbrBkGnd = CreateSolidBrush( GetSysColor( COLOR_WINDOW ) );
		FillRect( hdcMem, &rc, hbrBkGnd );
		DeleteObject( hbrBkGnd );

		// Render the image into the offscreen DC.
		SetBkMode( hdcMem, TRANSPARENT );
		
		DrawScene(hdcMem);

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
		CEllipseWindow* window = reinterpret_cast<CEllipseWindow*>(GetWindowLongPtr( handle, 0 ));
		switch( message ) {
			case WM_DESTROY:
				window->OnDestroy();
				break;
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
			case WM_SIZE:
				window->OnSize();
				break;
			case WM_PAINT:
				window->OnPaint();
				break;
			case WM_ERASEBKGND:
				return (LRESULT)1;
			case WM_KEYDOWN:
				window->inputController.processKey( wParam );
				break;
			default:
				return DefWindowProc( handle, message, wParam, lParam );
		}
		return 0;
	}

	Field field;
	EllipseObject ellipse;
	Figure figure;
	InputController inputController;
};
