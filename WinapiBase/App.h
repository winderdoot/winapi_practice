#pragma once
#include <windows.h>
#include <cstdlib>
#include <ctime>
#include <string>

#include <dwmapi.h>
#pragma comment (lib, "Dwmapi")

#include <winuser.h>
#include "board.h"

#ifndef __DEF_COLORS
#define __DEF_COLORS

#define BACKGROUND_COLOR RGB(250, 247, 238)
#define DEFAULT_TILE_COLOR RGB(204, 192, 174)

#endif

class App 
{
private:
	// Core methods
	bool register_class();
	static std::wstring const s_class_name;
	static LRESULT CALLBACK window_proc_static(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
	LRESULT window_proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
	HWND create_window(DWORD style, HWND parent = nullptr, DWORD ex_style = 0);

	// Extra client methods
	void on_window_move(HWND window, LPWINDOWPOS params);
	void update_transparency();
	void on_timer(HWND window);
	void on_command(WORD cmdID);
	void on_paint(HWND window);
	void on_resize(HWND window, UINT w, UINT h);

	// Members
	HINSTANCE m_instance;
	HWND m_main;
	Board m_board;
	HBRUSH m_field_brush;
	POINT m_screen_size;
	
	std::wstring m_window_title;

	static constexpr UINT_PTR s_timer = 1;

public:
	App(HINSTANCE instance);
	int run(int show_command);
};
