#pragma once
#include "Field.h"

class EllipseObject {
	int x, y;
	int dx, dy;
	int w, h;
	Field* field;

public:
	EllipseObject( Field* field_, int w_, int h_ )
	{
		w = w_;
		h = h_;
		field = field_;
		x = 0;
		y = 0;
		dx = 1;
		dy = 1;
	}

	void draw( HDC hDC )
	{
		SelectObject( hDC, GetStockObject( GRAY_BRUSH ) );
		if( !::Ellipse( hDC, x, y, x + w, y + h ) ) {
			MessageBox( NULL, ERRMSG( "Ellipse draw failed", GetLastError() ), NULL, MB_OK );
			exit( 1 );
		}
	}

	void step()
	{
		x += dx;
		y += dy;
		if( x <= 0 ) {
			dx *= -1;
			x = 0;
		}
		if( x + w >= field->width ) {
			dx *= -1;
			x = field->width - w;
		}
		if( y <= 0 ) {
			dy *= -1;
			y = 0;
		}
		if( y + h >= field->height ) {
			dy *= -1;
			y = field->height - h;
		}
	}
};
