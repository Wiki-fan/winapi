#pragma once
#include "Field.h"
#include "common.h"

class EllipseObject {
	int x, y;
	int dx, dy;
	int w, h;

public:
	EllipseObject( int w_, int h_ )
	{
		w = w_;
		h = h_;
		x = 0;
		y = 0;
		dx = 1;
		dy = 1;;
	}

	void draw( HDC hDC, bool isActive )
	{
		HBRUSH brush;
		if( isActive ) {
			brush = CreateSolidBrush( RGB( 255, 0, 0 ) );
		} else {
			brush = CreateSolidBrush( RGB( 127, 127, 127 ));
		}
		SelectObject( hDC, brush);
		if( !::Ellipse( hDC, x, y, x + w, y + h ) ) {
			HALT( "Ellipse draw failed" );
		}
		DeleteObject( brush );
	}

	void fixPosition( Field* field )
	{
		if( x <= 0 ) {
			x = 0;
		}
		if( x + w >= field->x ) {
			x = field->x - w;
		}
		if( y <= 0 ) {
			y = 0;
		}
		if( y + h >= field->y ) {
			y = field->y - h;
		}
	}

	void step(Field* field)
	{
		x += dx;
		y += dy;
		if( x <= 0 ) {
			dx *= -1;
			x = 0;
		}
		if( x + w >= field->x ) {
			dx *= -1;
			x = field->x - w;
		}
		if( y <= 0 ) {
			dy *= -1;
			y = 0;
		}
		if( y + h >= field->y ) {
			dy *= -1;
			y = field->y - h;
		}
	}
};
