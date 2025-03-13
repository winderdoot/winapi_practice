#include "board.h"

board::board(int cols, int ros) : m_fields{ }
{
	columns = cols;
	rows = ros;
	field_count = rows * columns;
	width = columns * (field_size + margin) + margin;
	height = rows * (field_size + margin) + margin;
	m_fields = std::vector<field>();
	m_fields.resize(field_count);
	for (LONG row = 0; row < rows; ++row)
		for (LONG column = 0; column < columns; ++column)
		{
			//m_fields.insert(row * columns + column, field())
			auto& f = m_fields[row * columns + column];
			f.position.top =
				row * (field_size + margin) + margin;
			f.position.left =
				column * (field_size + margin) + margin;
			f.position.bottom = f.position.top + field_size;
			f.position.right = f.position.left + field_size;
		}
}