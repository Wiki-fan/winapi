#include "InputController.h"
#include "EllipseWindow.h"

void InputController::processKeyDown( DWORD keyCode )
{
	if( window->gameover ) {
		window->field.Init();
		window->OnCreate(); 
		window->gameover = false;
		return;
	}
	switch( keyCode ) {
		case VK_LEFT:
			figure->Move( Point(-1, 0) );
			break;
		case VK_RIGHT:
			figure->Move( Point(1, 0) );
			break;
		case VK_UP:
			figure->Rotate();
			break;
		case VK_DOWN:
			if( !fast ) {
				window->SetTimerTimeout( FAST_TIMEOUT );
				fast = true;
			}
			break;
	}
	window->Redraw();
}

void InputController::processKeyUp( DWORD keyCode )
{
	if( window->gameover ) {
		return;
	}
	switch( keyCode ) {
		case VK_DOWN:
			window->SetTimerTimeout( 500 );
			fast = false;
			break;
	}
	window->Redraw();
}
