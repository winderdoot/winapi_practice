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
#define ENEMY_COLOR RGB(20, 95, 248)
#define PLAYER_COLOR RGB(236, 35, 45)

#endif

struct agent_t
{
	HWND window;
	HBRUSH brush;
	POINT pos;
};

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
	void enemy_move();
	INT_PTR on_colorstatic(HWND window);

	// CONSTANTS
	static LONG const m_width, m_height;

	static POINT const m_enemy_size;
	static POINT const m_enemy_def_pos;
	static LONG const m_enemy_off_max;
	static LONG const m_enemy_step;

	static POINT const m_player_size;
	static POINT const m_player_def_pos;
	//static LONG const m_enemy_off_max;
	//static LONG const m_enemy_step;

	// Members
	HINSTANCE m_instance;
	HWND m_main;
	POINT m_screen_size;
	bool m_has_focus;
	bool m_enemy_moving_right;
	agent_t m_player, m_enemy;
	
	std::wstring m_window_title;

	static constexpr UINT_PTR s_timer = 1;

public:
	App(HINSTANCE instance);
	int run(int show_command);
};
