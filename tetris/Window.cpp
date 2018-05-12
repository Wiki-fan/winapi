#include "EllipseWindow.h"
#include "Field.h"
#include "Figure.h"

const char* WINDOWCLASSNAME = "MainWClass";
const char* ELLIPSEWINDOWCLASSNAME = "EllipseWClass";

const std::vector<std::vector<Cell>> Figure::allowedFigures =
{
	{ { { 0,0 } },{ { 1,0 } },{ { -1,0 } },{ { 0,1 } } },
	{ { { 0,0 } },{ { 0,-1 } },{ { 1,0 } },{ { 1,1 } } }
};
