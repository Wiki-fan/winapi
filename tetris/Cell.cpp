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

	int x = static_cast<int>(static_cast<double>(field.width) / field.N*(pos.x));
	int y = static_cast<int>(static_cast<double>(field.height) / field.M*(pos.y));
	int tileWidth = static_cast<int>(static_cast<double>(field.width) / field.N);
	int tileHeight = static_cast<int>(static_cast<double>(field.height) / field.M);
	RECT rect = { x, y, x + tileWidth, y + tileHeight };
	if( !::FillRect( hDC, &rect, brush ) ) {
		HALT( "Rect draw failed" );
	}

	DeleteObject( brush );
}
