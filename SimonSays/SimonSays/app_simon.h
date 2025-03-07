#pragma once
#include <windows.h>
#include <string>
#include <chrono>

#include <dwmapi.h>
#pragma comment (lib, "Dwmapi")

#include <winuser.h>
#include "board.h"


class app_simon {
private:
	bool register_class();
	static std::wstring const s_class_name;
	static LRESULT CALLBACK window_proc_static(
		HWND window, UINT message,
		WPARAM wparam, LPARAM lparam);
	LRESULT window_proc(
		HWND window, UINT message,
		WPARAM wparam, LPARAM lparam);
	HWND create_window(DWORD style, HWND parent = nullptr,
		DWORD ex_style = 0);
	void on_window_move(HWND window, LPWINDOWPOS params);
	void update_transparency();

	HINSTANCE m_instance;
	HWND m_main;
	board m_board;
	HBRUSH m_field_brush;
	POINT m_screen_size;

	static constexpr UINT_PTR s_timer = 1;
	std::chrono::time_point<std::chrono::system_clock> m_startTime;
	void on_timer();
	void on_command(WORD cmdID);
	void on_paint(HWND window);

public:
	app_simon(HINSTANCE instance, int N);
	int run(int show_command);

};