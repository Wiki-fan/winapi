#pragma once

#include "Figure.h"
class CEllipseWindow;

enum class Direction{None, Left, Right};
struct InputController {

	InputController( CEllipseWindow* window_ ) :window( window_ ) {}

	Direction movement = Direction::None;
	Figure* figure;
	CEllipseWindow* window;
	bool fast = false;

	void processKeyUp( DWORD keyCode );
	void processKeyDown( DWORD keyCode );

	void process()
	{
		/*if( movement == Direction::Left ) {
			figure->pos.x -= 1;
		} else if( movement == Direction::Right ) {
			figure->pos.x += 1;
		}*/

	}
};
