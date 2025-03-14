#include "board.h"
#include "App.h"

Board::Board(int cols, int ros) 
	: m_fields { } 
{
	columns = cols;
	rows = ros;
	field_count = rows * columns;
	width = columns * (field_size + margin) + margin;
	height = rows * (field_size + margin) + margin;
	m_fields = std::vector<field>(field_count);
	//m_fields.resize(field_count);
	for (LONG row = 0; row < rows; ++row)
		for (LONG column = 0; column < columns; ++column) 
		{
			auto& f = m_fields[row * columns + column];
			f.position.top =
				row * (field_size + margin) + margin;
			f.position.left =
				column * (field_size + margin) + margin;
			f.position.bottom = f.position.top + field_size;
			f.position.right = f.position.left + field_size;
			f.color = DEFAULT_TILE_COLOR;
		}
}

void Board::set_colour(int i, COLORREF col) 
{
	if (i < 0 || i >= field_count)
		return;
	m_fields[i].color = col;
}

int Board::get_tile_index(LONG x, LONG y)
{
	LONG len = margin + field_size;
	LONG xind = x / len;
	LONG yind = y / len;
	int index = xind + yind * columns;
	return index;
}