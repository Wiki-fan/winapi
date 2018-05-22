#include "Cell.h"
#include "Field.h"

const void Cell::draw( HDC hDC, Field & field, Point center )
{
	drawAt( hDC, field, Point( center.x + pos.x, center.y + pos.y ) );
}

const void Cell::drawAt( HDC hDC, Field & field, Point pos )
{
	HBRUSH brush;
	brush = CreateSolidBrush( color );
	SelectObject( hDC, brush );

	int x = round(static_cast<double>(field.width)*(pos.x) / field.N);
	int y = round(static_cast<double>(field.height)*(pos.y) / field.M);
	int tileWidth = round(static_cast<double>(field.width) / field.N)+1;
	int tileHeight = round(static_cast<double>(field.height) / field.M)+1;
	RECT rect = { x, y, x + tileWidth, y + tileHeight };
	if( !::FillRect( hDC, &rect, brush ) ) {
		HALT( "Rect draw failed" );
	}

	DeleteObject( brush );
}
