#pragma once
#include <vector>
#include <windows.h>
struct field
{
	RECT position;
};

class board
{
public:
	LONG columns;
	LONG rows;
	//static constexpr LONG columns = 4;
	//static constexpr LONG rows = 4;
	static constexpr LONG margin = 5;
	LONG field_count;
	static constexpr LONG field_size = 90;
	LONG width;
	LONG height;
	using field_array = std::vector<field>;
	board(int columns, int rows);
	field_array const& fields() const { return m_fields; }
private:
	field_array m_fields;
};

