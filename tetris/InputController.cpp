#include "InputController.h"
#include "EllipseWindow.h"

void InputController::processKey( DWORD keyCode )
{
	switch( keyCode ) {
		case VK_LEFT:
			figure->Move( Point(-1, 0) );
			break;
		case VK_RIGHT:
			figure->Move( Point(1, 0) );
			break;
		case VK_UP:
			//movement = Direction::Up;
			break;
		case VK_DOWN:
			//movement = Direction::Down;
			break;
	}
	window->Redraw();
}
