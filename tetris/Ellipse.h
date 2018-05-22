#pragma once
#include "Field.h"
#include "common.h"

class EllipseObject {
	int x, y;
	int dx, dy;
	int w, h;
	HFONT font;
	int FONTSIZE = 24;
	std::string label;

public:
	EllipseObject( int w_, int h_, std::string& label_ ) : label(label_)
	{
		w = w_;
		h = h_;
		x = 0;
		y = 0;
		dx = 3;
		dy = 3;
		font = CreateFontA( FONTSIZE, 12, 0, 0, FW_DONTCARE, FALSE, TRUE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
			CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT( "Times New Roman" ) );
	}

	~EllipseObject()
	{
		DeleteObject( font );
	}

	void draw( HDC hDC)
	{
		HBRUSH brush;
		brush = CreateSolidBrush( RGB( 255, 0, 0 ) );
		SelectObject( hDC, brush);
		if( !::Ellipse( hDC, x, y, x + w, y + h ) ) {
			HALT( "Ellipse draw failed" );
		}


		SelectObject( hDC, font );
		int margin = h/2 - FONTSIZE;
		RECT rect = { x, y+margin, x+w, y+h };
		if( DrawText( hDC, label.c_str(), -1, &rect, DT_CENTER) == 0 ) {
			HALT( "DrawText" );
		}

		DeleteObject( brush );
	}

	void fixPosition( Field* field )
	{
		if( x <= 0 ) {
			x = 0;
		}
		if( x + w >= field->width ) {
			x = field->width - w;
		}
		if( y <= 0 ) {
			y = 0;
		}
		if( y + h >= field->height ) {
			y = field->height - h;
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
