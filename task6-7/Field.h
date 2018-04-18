#pragma once
#define ERRMSG(TEXT, MSG) (TEXT+std::to_string(MSG)).c_str()

class Field {
public:
	int x, y;

};

class Object {
	int x, y;
	int dx, dy;
	int w, h;
	Field* field;

public:
	Object( Field* field_, int w_, int h_ )
	{
		w = w_;
		h = h_;
		field = field_;
		x = 0;
		y = 0;
		dx = 3;
		dy = 3;
	}

	void draw(HDC hDC)
	{
		SelectObject( hDC, GetStockObject( GRAY_BRUSH ) );
		if( !Ellipse( hDC, x, y, x + w, y + h ) ) {
			MessageBox( NULL, ERRMSG( "Ellipse draw failed", GetLastError() ), NULL, MB_OK );
			exit( 1 );
		}
	}

	void step()
	{
		x += dx;
		y += dy;
		if(x <= 0 ) {
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
