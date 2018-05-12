#include "Cell.h"
#include "Field.h"

const void Cell::draw( HDC hDC, Field & field, Point center )
{
	HBRUSH brush;
	brush = CreateSolidBrush( color );
	SelectObject( hDC, brush );

	int x = static_cast<int>(static_cast<double>(field.width) / field.N*(center.x + pos.x));
	int y = static_cast<int>(static_cast<double>(field.height) / field.M*(center.y + pos.y));
	int tileWidth = static_cast<int>(static_cast<double>(field.width) / field.N);
	int tileHeight = static_cast<int>(static_cast<double>(field.height) / field.M);
	RECT rect = { x, y, x + tileWidth, y + tileHeight };
	if( !::FillRect( hDC, &rect, brush ) ) {
		HALT( "Rect draw failed" );
	}

	DeleteObject( brush );
}
