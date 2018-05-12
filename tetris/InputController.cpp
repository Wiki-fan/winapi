#include "InputController.h"
#include "EllipseWindow.h"

void InputController::processKeyDown( DWORD keyCode )
{
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
				window->SetTimerTimeout( 100 );
				fast = true;
			}
			//figure->Move( Point( 0, 1 ) );
			break;
	}
	window->Redraw();
}

void InputController::processKeyUp( DWORD keyCode )
{
	switch( keyCode ) {
		case VK_DOWN:
			window->SetTimerTimeout( 500 );
			fast = false;
			break;
	}
	window->Redraw();
}
