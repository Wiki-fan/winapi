#pragma once
#include <vector>
#include <set>
#include "common.h"

#include <Windows.h>
#include "Field.h"
#include "Cell.h"

const std::vector<COLORREF> allowedColors = { RGB( 255, 0, 0 ), RGB( 0 , 255, 0 ), RGB( 0, 0, 255 ), RGB( 255, 255, 0 ), RGB( 0, 255, 255 ), RGB( 255, 0, 255 ) };

struct FigureShape {
	char name;
	std::vector<Cell> cells;
};

struct Figure {
	std::vector<Cell> cells;
	char name;
	Field& field;
	Point pos = { 0,0 }; // center position
	COLORREF color;
	Figure(Field& field_) : field(field_) {
	}

	bool Create()
	{
		int i = rand() % allowedFigures.size();
		cells = allowedFigures[i].cells;
		name = allowedFigures[i].name;
		color = allowedColors[rand() % allowedColors.size()];
		for( auto& cell : cells ) {
			cell.color = color;
		}
		pos = Point( field.N / 2, 1 );
		for( auto& cell : cells ) {
			Point newP = Point( pos.x + cell.pos.x, pos.y + cell.pos.y );

			if( field.cellMatrix[newP.y][newP.x].isActive() ) {
				return false;
			}
		}
		return true;
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

	void Rotate()
	{
		if( name == 'O' ) {
			return;
		}
		for( auto& cell : cells ) {
			Point newP = Point( pos.x + (-cell.pos.y), pos.y + (cell.pos.x) );

			if( newP.x < 0 || newP.x >= field.N || newP.y < 0 || newP.y >= field.M || field.cellMatrix[newP.y][newP.x].isActive() ) {
				return;
			}
		}
		for( auto& cell : cells ) {
			Point newPos( -cell.pos.y, cell.pos.x );
			cell.pos = newPos;
		}
	}

	void Freeze()
	{
		std::set<int> updatedLines;
		for( auto& cell : cells ) {
			Point p = Point( pos.x + cell.pos.x, pos.y + cell.pos.y ); 
			cell.pos = p;
			field.cellMatrix[p.y][p.x] = cell;
			updatedLines.insert( p.y );
		}

		for( auto& y : updatedLines ) {
			bool allFilled = true;
			for( auto& cell : field.cellMatrix[y] ) {
				if( !cell.isActive() ) {
					allFilled = false;
					break;
				}
			}
			if( allFilled ) {
				for( int i = y; i > 0; --i ) {
					std::swap( field.cellMatrix[i], field.cellMatrix[i - 1] );
				}
				for( int i = 0; i < field.N; ++i ) {
					field.cellMatrix[0][i].setInactive();
				}
			}
		}
	}

	const static std::vector<FigureShape> allowedFigures;
};
