#pragma once

#include "common.h"

class Field;

struct Cell {
	Cell( Point pos_ ) : pos( pos_ ) {}

	bool isActive()
	{
		return pos.x != -1;
	}

	void setInactive()
	{
		pos.x = -1;
	}

	const void draw( HDC hDC, Field& field, Point center );

	COLORREF color;
	Point pos;
};
