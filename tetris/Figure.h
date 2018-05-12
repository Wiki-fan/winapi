#pragma once
#include <vector>
#include "common.h"

#include <Windows.h>
#include "Field.h"
#include "Cell.h"

const std::vector<COLORREF> allowedColors = { RGB( 255, 0, 0 ), RGB( 0 , 255, 0 ), RGB( 0, 0, 255 ), RGB( 255, 255, 0 ), RGB( 0, 255, 255 ), RGB( 255, 0, 255 ) };

struct Figure {
	std::vector<Cell> cells;
	Field& field;
	Point pos = { 0,0 }; // center position
	COLORREF color;
	Figure(Field& field_) : field(field_) {
	}
	void Create()
	{
		int i = rand() % allowedFigures.size();
		cells = allowedFigures[i];
		color = allowedColors[rand() % allowedColors.size()];
		for( auto& cell : cells ) {
			cell.color = color;
		}
		pos = Point( field.N / 2, 1 );
	}

	void draw( HDC hDC )
	{
		for( auto& cell : cells ) {
			cell.draw( hDC, field, pos);
		}
	}

	bool Move( Point d )
	{
		for( auto& cell : cells ) {
			Point newP = Point(pos.x + cell.pos.x + d.x, pos.y + cell.pos.y + d.y);
			if( d.y == 0 ) {
				if( newP.x < 0 || newP.x >= field.N || field.cellMatrix[newP.y][newP.x].isActive() ) {
					return true;
				}
			} else {
				if( newP.y >= field.M || field.cellMatrix[newP.y][newP.x].isActive() ) {
					Freeze();
					return false;
				}
			}
		}
		pos.x += d.x;
		pos.y += d.y;
		return true;
	}

	void Freeze()
	{
		for( auto& cell : cells ) {
			Point p = Point( pos.x + cell.pos.x, pos.y + cell.pos.y ); 
			cell.pos = p;
			field.cellMatrix[p.y][p.x] = cell;
		}
	}

	const static std::vector<std::vector<Cell>> allowedFigures;
};

