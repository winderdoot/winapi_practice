#pragma once
#include <windows.h>
#include <windowsx.h>

#include <cstdlib>
#include <ctime>
#include <string>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <array>
#include <cmath>
#include <unordered_map>

#include <dwmapi.h>
#pragma comment (lib, "Dwmapi")

#include <winuser.h>
#pragma comment (lib, "Msimg32")

#include <Commdlg.h>
#pragma comment (lib, "Comdlg32")

#include <wingdi.h>

#ifndef __DEF_COLORS
#define __DEF_COLORS

#define BACKGROUND_COLOR RGB(100, 255, 255)
#define ENEMY_COLOR RGB(54, 78, 236)
#define PLAYER_COLOR RGB(246, 78, 57)

#define PLAYER_OFF_X 10
#define MAX_BULLETS 200
#define SIZE_SMALL 600
#define SIZE_MEDIUM 800
#define SIZE_LARGE 1000

#define ENEMY_BACKGROUND 16711935
#define PLAYER_BACKGROUND 13369599

#endif

struct agent_t
{
	HWND window;
	POINT pos;
	POINT init_pos;
	HBITMAP current_frame; // This is horrible and I hate this and I won't change it
	HBITMAP previous_frame; // This shouldn't even be here??? what the hell?
	HBITMAP bitmap;
	HBITMAP background; // Just whatever fuck
};

enum class win_size
{
	smol, medium, large
};

enum class win_background
{
	solid, image
};

enum class win_display_mode
{
	center, fill, tile, fit
};

enum class game_state
{
	playing, not_playing, in_menu
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

	void init_enemies();
	void on_keydown(HWND window, WPARAM wparam, LPARAM lparam);
	//INT_PTR on_colorstatic(HWND window);
	HBITMAP get_next_frame(agent_t agent, int frameIndex, int frameWidth, int frameHeight, int framesPerRow, COLORREF backgroundColor);
	void draw_background(agent_t &agent);
	void capture_background(agent_t &agent);
	//HBITMAP remove_purple_background(HBITMAP hbitmap);
	void update_bullets();
	void add_bullet();
	void new_game();
	void enemy_reset_pos(bool resetY = false);
	void player_reset_pos();
	void on_menu_check(UINT menu, bool uncheckFirst = true);
	void check_enemy_death();
	void check_player_lost();
	//
	void uncheck_size();
	void uncheck_background();
	void uncheck_disp_mode();
	void user_set_background_color();

	// CONSTANTS
	LONG m_width = 4 * SIZE_MEDIUM / 3;
	LONG m_height = 3 * m_width / 4;
	win_size m_size = win_size::medium;
	//static std::unordered_map<win_size, UINT> 

	static POINT const m_enemy_size;
	POINT m_enemy_def_pos{ (m_width - m_enemy_size.x) / 2, (m_height - m_enemy_size.y) / 4 };
	static LONG const m_enemy_off_max;
	static LONG const m_enemy_step;
	static LONG const m_enemy_ystep = 10;

	static POINT const m_player_size;
	POINT m_player_def_pos{ (m_width - m_player_size.x) / 2, 3 * (m_height - m_player_size.y) / 4 };
	static LONG const m_player_step;

	static constexpr int enemies_rows = 3;
	static constexpr int enemies_cols = 5;

	static constexpr POINT bullet_size = { 5, 15 };
	static constexpr LONG bullet_step = 10;

	// Members
	HINSTANCE m_instance;
	HWND m_main;
	HBRUSH m_main_brush;
	bool brush_changed = false;
	//Board m_board;
	POINT m_screen_size;
	bool m_has_focus;
	bool m_enemy_moving_right;
	agent_t m_player;
	std::vector<agent_t> m_enemies;
	int m_playerFrame = 0;
	int m_enemyFrame = 0;
	std::array<agent_t, MAX_BULLETS> m_bullets; // Stupid dynamic cyclic buffer
	int bullet_count = 0;
	int bullet_beg = 0, bullet_end = 0;
	HBRUSH bullet_brush;
	game_state state = game_state::playing;
	win_background background = win_background::solid;
	win_display_mode disp_mode = win_display_mode::center;
	std::wstring m_window_title;

	static constexpr UINT_PTR s_timer = 1;

public:
	App(HINSTANCE instance);
	int run(int show_command);
};
