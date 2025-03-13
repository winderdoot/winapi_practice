#pragma once
#include <vector>
#include <windows.h>

struct field 
{
	RECT position;
	COLORREF color;
};

class Board 
{
public:
	using field_array = std::vector<field>;

	static constexpr LONG margin = 5;
	static constexpr LONG field_size = 90;
	LONG columns;
	LONG rows;
	LONG field_count;
	LONG width;
	LONG height;

	Board(int columns = 5, int rows = 5);
	field_array const& fields() const { return m_fields; }
	void set_colour(int i, COLORREF col);

private:
	field_array m_fields;
};

