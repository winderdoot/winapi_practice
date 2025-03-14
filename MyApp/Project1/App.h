#pragma once
#include <windows.h>
#include <windowsx.h>
#include <cstdlib>
#include <ctime>
#include <string>
#include <stdexcept>
#include <cstdlib>

#include <dwmapi.h>
#pragma comment (lib, "Dwmapi")

#include <winuser.h>

#ifndef __DEF_COLORS
#define __DEF_COLORS

#define BACKGROUND_COLOR RGB(255, 255, 255)
#define ENEMY_COLOR 0x4646FF

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
	void center_window(HWND window);
	void on_window_move(HWND window, LPWINDOWPOS params);
	void update_transparency();
	void on_timer(HWND window);
	void on_command(WORD cmdID);
	void on_paint(HWND window);
	void on_resize(HWND window, UINT w, UINT h);
	void on_mouse_move(HWND window);
	void on_char(HWND window, WPARAM wparam, LPARAM lparam);
	void on_mouse_click(HWND window, WPARAM wparam, LPARAM lparam);

	// CONSTANTS
	static LONG const m_width, m_height;
	static POINT const m_enemy_size;

	// Members
	HINSTANCE m_instance;
	HWND m_main, m_enemy;
	//Board m_board;
	HBRUSH m_enemy_brush;
	POINT m_screen_size;
	bool m_has_focus;
	POINT m_enemy_pos;
	
	std::wstring m_window_title;

	static constexpr UINT_PTR s_timer = 1;

public:
	App(HINSTANCE instance);
	int run(int show_command);
};
