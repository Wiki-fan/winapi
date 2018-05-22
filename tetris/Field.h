#pragma once

#include <vector>
#include "Cell.h"

class Field {
	HBRUSH borderBrush;
public:
	Field( int N_, int M_ ) :N( N_ ), M( M_ )
	{
		borderBrush = CreateSolidBrush( RGB( 0, 0, 0 ) );
	}

	void Init()
	{
		cellMatrix.clear();
		cellMatrix.resize( M, std::vector<Cell>( N, Cell( { 0,0 } ) ) );
		for( auto& row : cellMatrix ) {
			for( auto& cell : row ) {
				cell.setInactive();
			}
		}
	}

	~Field()
	{
		DeleteObject( borderBrush );
	}

	int width, height;
	int N, M; // cells horizontally and vertically

	std::vector<std::vector<Cell>> cellMatrix;

	void draw( HDC hDC)
	{
		SelectObject( hDC, borderBrush );
		for( int i = 0; i < M; ++i ) {
			int y = round(static_cast<double>(height) *i / M);
			MoveToEx( hDC, 0, y, NULL );
			LineTo( hDC, width, y );
		}
		for( int j = 0; j < N; ++j ) {
			int x = round(static_cast<double>(width) *j / N);
			MoveToEx( hDC, x, 0, NULL );
			LineTo( hDC, x, height);
		}
		
		for( int i = 0; i < M; ++i) {
			for( int j = 0; j < N; ++j) {
				if( cellMatrix[i][j].isActive() ) {
					cellMatrix[i][j].drawAt( hDC, *this, Point( j, i )) ;
				}
			}
		}
	}
};
