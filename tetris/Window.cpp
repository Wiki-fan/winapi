#include "EllipseWindow.h"
#include "Field.h"
#include "Figure.h"

const char* WINDOWCLASSNAME = "MainWClass";
const char* ELLIPSEWINDOWCLASSNAME = "EllipseWClass";

const std::vector<FigureShape> Figure::allowedFigures =
{
	//  O
	// O*O
	{'T', { { { 0,0 } },{ { 1,0 } },{ { -1,0 } },{ { 0,1 } } }},
	//  OO
	// OO
	{'S', { { { 0,0 } },{ { 0,-1 } },{ { 1,0 } },{ { 1,1 } } }},
	// OO
	//  OO
	{'Z', {{{0,0}},{{ -1,1 }}, {{0, 1}}, {{1, 0}}} },
	// OO
	// OO
	{'O', { {{0,0}}, {{1,1}}, {{1,0} }, { {0,1} } }},
	// O*OO
	{'I', { {{0,0}}, {{0,1}}, {{0,-1}}, {{0,2}} } },
	// O
	// O*O
	{'J', {{{0,0}}, {{1,0}}, {{-1,0}}, {{-1, 1}} }},
	//   O
	// O*O
	{'L', {{{0,0}}, {{-1,0}}, {{1, 0}}, {{1,1}}} }
};
