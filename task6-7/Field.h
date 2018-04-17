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
		dx = 5;
		dy = 2;
	}

	void draw(HWND handle)
	{
		PAINTSTRUCT ps;
		HDC hDC = BeginPaint( handle, &ps );


		SelectObject( hDC, GetStockObject( GRAY_BRUSH ) );
		if( !Ellipse( hDC, x, y, x + w, y + h ) ) {
			MessageBox( NULL, ERRMSG( "Ellipse draw failed", GetLastError() ), NULL, MB_OK );
			exit( 1 );
		}
		EndPaint( handle, &ps );
		DeleteObject( hDC );
	}

	void step()
	{
		x += dx;
		y += dy;
		if(x <= 0 || x + w >= field->x) {
			dx *= -1;
		}
		if( y <= 0 || y + h >= field->y ) {
			dy *= -1;
		}
	}
};
