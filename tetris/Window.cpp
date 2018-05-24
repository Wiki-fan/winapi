#include "EllipseWindow.h"
#include "Field.h"
#include "Figure.h"

const char* WINDOWCLASSNAME = "MainWClass";
const char* ELLIPSEWINDOWCLASSNAME = "EllipseWClass";

const std::vector<FigureShape> Figure::allowedFigures =
{
	//  O
	// O*O
	{ 'T', true,{ { { 0,0 } },{ { 1,0 } },{ { -1,0 } },{ { 0,1 } } } },
	//  OO
	// OO
	{ 'S', true,{ { { 0,0 } },{ { 0,-1 } },{ { 1,0 } },{ { 1,1 } } } },
	// OO
	//  OO
	{ 'Z', true,{ { { 0,0 } },{ { -1,1 } },{ { 0, 1 } },{ { 1, 0 } } } },
	// OO
	// OO
	{ 'O', false,{ { { 0,0 } },{ { 1,1 } },{ { 1,0 } },{ { 0,1 } } } },
	// O*OO
	{ 'I', true,{ { { 0,0 } },{ { 0,1 } },{ { 0,-1 } },{ { 0,2 } } } },
	// O
	// O*O
	{ 'J', true,{ { { 0,0 } },{ { 1,0 } },{ { -1,0 } },{ { -1, 1 } } } },
	//   O
	// O*O
	{ 'L', true,{ { { 0,0 } },{ { -1,0 } },{ { 1, 0 } },{ { 1,1 } } } }
};
