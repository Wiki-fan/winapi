#include "EllipseWindow.h"
#include "Field.h"
#include "Figure.h"

const char* WINDOWCLASSNAME = "MainWClass";
const char* ELLIPSEWINDOWCLASSNAME = "EllipseWClass";

const std::vector<std::vector<Cell>> Figure::allowedFigures =
{
	//  O
	// O*O
	{ { { 0,0 } },{ { 1,0 } },{ { -1,0 } },{ { 0,1 } } },
	//  OO
	// OO
	{ { { 0,0 } },{ { 0,-1 } },{ { 1,0 } },{ { 1,1 } } },
	// OO
	//  OO
	{{{0,0}},{{ -1,1 }}, {{0, 1}}, {{1, 0}}},
	// OO
	// OO
	{ {{0,0}}, {{1,1}}, {{1,0} }, { {0,1} } },
	// O*OO
	{ {{0,0}}, {{0,1}}, {{0,-1}}, {{0,2}} },
	// O
	// O*O
	{{{0,0}}, {{1,0}}, {{-1,0}}, {{-1, 1}} },
	//   O
	// O*O
	{{{0,0}}, {{-1,0}}, {{1, 0}}, {{1,1}}}
};
