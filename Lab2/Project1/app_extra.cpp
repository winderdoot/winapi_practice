#include "App.h"
#include "resource.h"

void App::center_window(HWND window)
{
	RECT rc = { 0 };
	GetWindowRect(window, &rc);

	int windowWidth = rc.right - rc.left;
	int windowHeight = rc.bottom - rc.top;

	int xPos = (m_screen_size.x - windowWidth) / 2;
	int yPos = (m_screen_size.y - windowHeight) / 2;

	SetWindowPos(window, NULL, xPos, yPos, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
}

void App::on_window_move(HWND window, LPWINDOWPOS params)
{
	RECT rect;
	GetWindowRect(window, &rect);
	SIZE size 
	{
		.cx = rect.right - rect.left,
		.cy = rect.bottom - rect.top 
	};
	POINT new_pos {rect.left, rect.top};

	SetWindowPos(window, nullptr, new_pos.x, new_pos.y,
		0, 0, SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOZORDER);
}

void App::update_transparency() 
{
	BYTE alpha;
	if (m_has_focus)
		alpha = 100;
	else
		alpha = 40;
	BYTE a = 255 * alpha / 100;
	SetLayeredWindowAttributes(m_main, 0, a, LWA_ALPHA);
}

//INT_PTR App::on_colorstatic(HWND window)
//{
//
//	if (window == m_player.window)
//	{
//		return reinterpret_cast<INT_PTR>(m_player.brush);
//	}
//	else if (window == m_enemy.window)
//	{
//		return reinterpret_cast<INT_PTR>(m_enemy.brush);
//	}
//	return reinterpret_cast<INT_PTR>(m_enemy.brush);
//}

void App::on_keydown(HWND window, WPARAM wparam, LPARAM lparam)
{
	LONG times = LOWORD(lparam);
	LONG newX;
	switch (wparam)
	{
	case VK_RIGHT:
		newX = m_player.pos.x + m_player_step;
		if (newX + m_player_size.x <= m_width - PLAYER_OFF_X) {
			m_player.pos.x = newX;
		}
		else {
			m_player.pos.x = m_width - PLAYER_OFF_X - m_player_size.x;
		}

		MoveWindow(m_player.window, m_player.pos.x, m_player.pos.y, m_player_size.x, m_player_size.y, true);
		//capture_background(m_player);
		SendMessageW(m_player.window, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)m_player.current_frame);
		return;
	case VK_LEFT:
		newX = m_player.pos.x - m_player_step;
		if (newX >= PLAYER_OFF_X) {
			m_player.pos.x = newX;
		}
		else {
			m_player.pos.x = PLAYER_OFF_X;
		}
		MoveWindow(m_player.window, m_player.pos.x, m_player.pos.y, m_player_size.x, m_player_size.y, true);
		//capture_background(m_player);
		SendMessageW(m_player.window, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)m_player.current_frame);
		return;
	case VK_SPACE:
		add_bullet();
		return;
	case VK_F2:
		new_game();
		return;
	}
}

void App::on_timer(HWND window)
{
	switch (state)
	{
	case game_state::not_playing:
		return;
	case game_state::playing:
		m_player.previous_frame = m_player.current_frame;
		m_playerFrame = (m_playerFrame + 1) % 3;

		m_player.current_frame = get_next_frame(m_player, m_playerFrame, m_player_size.x, m_player_size.y, 3, PLAYER_BACKGROUND);
		SendMessageW(m_player.window, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)m_player.current_frame);
		DeleteObject(m_player.previous_frame);

		enemy_move();
		update_bullets();
		check_enemy_death();
		check_player_lost();
	case game_state::in_menu:
		return;
	}
}

void App::on_command(WORD cmdID) 
{
	RECT rect;
	LONG groupXoffset = (enemies_cols - 1) * (m_enemy_size.x / 2);
	LONG groupYoffset = (enemies_rows - 1) * (m_enemy_size.y / 2);
	switch (cmdID)
	{
	case ID_NEWGAME:
		new_game();
		break;
	case ID_OPTIONS_SMALL:
		uncheck_size();
		on_menu_check(ID_OPTIONS_SMALL);
		m_width = 4 * SIZE_SMALL / 3;
		m_height = 3 * m_width / 4;
		rect = { 0, 0, m_width, m_height };
		AdjustWindowRectEx(&rect, WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX, true, 0);
		SetWindowPos(m_main, HWND_TOP, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER | SWP_DRAWFRAME | SWP_NOMOVE);
		center_window(m_main);
		m_size = win_size::smol;
		m_enemy_def_pos = { (m_width - m_enemy_size.x) / 2, (m_height - m_enemy_size.y) / 4 };
		m_player_def_pos = { (m_width - m_player_size.x) / 2, 3 * (m_height - m_player_size.y) / 4 };
		player_reset_pos();
		enemy_reset_pos();
		break;
	case ID_OPTIONS_MEDIUM:
		uncheck_size();
		on_menu_check(ID_OPTIONS_MEDIUM);
		m_width = 4 * SIZE_MEDIUM / 3;
		m_height = 3 * m_width / 4;
		rect = { 0, 0, m_width, m_height };
		AdjustWindowRectEx(&rect, WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX, true, 0);
		SetWindowPos(m_main, HWND_TOP, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER | SWP_DRAWFRAME | SWP_NOMOVE);
		center_window(m_main);
		m_size = win_size::medium;
		m_enemy_def_pos = { (m_width - m_enemy_size.x) / 2, (m_height - m_enemy_size.y) / 4 };
		m_player_def_pos = { (m_width - m_player_size.x) / 2, 3 * (m_height - m_player_size.y) / 4 };
		player_reset_pos();
		enemy_reset_pos();
		break;
	case ID_OPTIONS_LARGE:
		uncheck_size();
		on_menu_check(ID_OPTIONS_LARGE);
		m_width = 4 * SIZE_LARGE / 3;
		m_height = 3 * m_width / 4;
		rect = { 0, 0, m_width, m_height };
		AdjustWindowRectEx(&rect, WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX, true, 0);
		SetWindowPos(m_main, HWND_TOP, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER | SWP_DRAWFRAME | SWP_NOMOVE);
		center_window(m_main);
		m_size = win_size::large;
		m_enemy_def_pos = { (m_width - m_enemy_size.x) / 2, (m_height - m_enemy_size.y) / 4 };
		m_player_def_pos = { (m_width - m_player_size.x) / 2, 3 * (m_height - m_player_size.y) / 4 };
		player_reset_pos();
		enemy_reset_pos();
		break;
	case ID_EXIT:
		SendMessageW(m_main, WM_CLOSE, 0, 0);
		break;
	case ID_SOLIDCOLOR:
		state = game_state::in_menu;
		user_set_background_color();

		uncheck_background();
		on_menu_check(ID_SOLIDCOLOR);
		background = win_background::solid;
		state = game_state::playing;
		return;
	case ID_IMAGE:
		uncheck_background();
		on_menu_check(ID_IMAGE);
		background = win_background::image;

		return;
	case ID_BACKGROUND_CENTER:
		uncheck_disp_mode();
		on_menu_check(ID_BACKGROUND_CENTER);
		disp_mode = win_display_mode::center;

		return;
	case ID_BACKGROUND_FILL:
		uncheck_disp_mode();
		on_menu_check(ID_BACKGROUND_FILL);
		disp_mode = win_display_mode::fill;

		return;
	case ID_BACKGROUND_TILE:
		uncheck_disp_mode();
		on_menu_check(ID_BACKGROUND_TILE);
		disp_mode = win_display_mode::tile;

		return;
	case ID_BACKGROUND_FIT:
		uncheck_disp_mode();
		on_menu_check(ID_BACKGROUND_FIT);
		disp_mode = win_display_mode::fit;

		return;
	}
}

void App::on_paint(HWND window) 
{
	
}

void App::on_resize(HWND window, UINT w, UINT h) 
{
	//if (!menu_size_change)
	//	return;
	//menu_size_change = false;

	return;
}

void App::on_mouse_move(HWND window) // cursed code
{
	//POINT p { .x = 100, .y = 100};
	//ClientToScreen(window, &p);
	//SetCursorPos(p.x, p.y);
}

void App::on_char(HWND window, WPARAM wparam, LPARAM lparam)
{
}

HBITMAP App::get_next_frame(agent_t agent, int frameIndex, int frameWidth, int frameHeight, int totalFrames, COLORREF backPurple) 
{
	HBITMAP origBitmap = agent.bitmap;
	frameIndex = frameIndex % totalFrames;

	// Chyba potrzebne są oba konteksty z tego co ogarniam
	HDC hdcMemory = CreateCompatibleDC(NULL);
	HDC hdcOriginal = CreateCompatibleDC(NULL);
	HDC hdcbackground = CreateCompatibleDC(NULL);

	// starą bitmape do kontekstu walnąć, zapisać starą żeby potem znowu wybrac ją
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcOriginal, origBitmap);
	HBITMAP backoldbit = (HBITMAP)SelectObject(hdcbackground, agent.background);

	// Robimy bitmape na wybraną klatke animacji
	HBITMAP hFrameBitmap = CreateCompatibleBitmap(hdcOriginal, frameWidth, frameHeight);

	HBITMAP hOldFrameBitmap = (HBITMAP)SelectObject(hdcMemory, hFrameBitmap);
	COLORREF col = GetPixel(hdcOriginal, 0, 0);
	// PLayer: 13369599
	// Copy the frame data from the original bitmap to the new one
	BitBlt(hdcMemory, 0, 0, frameWidth, frameHeight, hdcOriginal, frameIndex * frameWidth, 0, SRCCOPY);
	for (int x = 0; x < frameWidth; x++)
	{
		for (int y = 0; y < frameHeight; y++)
		{
			//COLORREF tmp = RGB(0, 0, 0);
			COLORREF backCol = GetPixel(hdcbackground, x, y);
			COLORREF myCol = GetPixel(hdcMemory, x, y);
			if (myCol == backPurple)
			{
				SetPixel(hdcMemory, x, y, backCol);
			}
		}
	}


	//TransparentBlt(hdcMemory, 0, 0, frameWidth, frameHeight, hdcOriginal, frameIndex * frameWidth, 0, frameWidth, frameHeight, backPurple);
	// Enemy: 16711935

	SelectObject(hdcbackground, backoldbit);
	SelectObject(hdcOriginal, hOldBitmap);
	SelectObject(hdcMemory, hOldFrameBitmap);
	DeleteDC(hdcMemory);
	DeleteDC(hdcOriginal);
	DeleteDC(hdcbackground);

	return hFrameBitmap;

}

void App::on_mouse_click(HWND window, WPARAM wparam, LPARAM lparam)
{
	//ShowWindow(m_enemies[0].window, SW_HIDE);
	//if (!IsWindowVisible(m_enemies[0].window))
	//{
	//	ShowWindow(m_enemies[0].window, SW_SHOW);
	//}
}

void App::uncheck_size()
{
	switch (m_size)
	{
	case win_size::smol:
		on_menu_check(ID_OPTIONS_SMALL, false);
		break;
	case win_size::medium:
		on_menu_check(ID_OPTIONS_MEDIUM, false);
		break;
	case win_size::large:
		on_menu_check(ID_OPTIONS_LARGE, false);
		break;
	}
}

void App::uncheck_background()
{
	switch (background)
	{
	case win_background::solid:
		on_menu_check(ID_SOLIDCOLOR, false);
		break;
	case win_background::image:
		on_menu_check(ID_IMAGE, false);
		break;

	}
}

void App::uncheck_disp_mode()
{
	switch (disp_mode)
	{
	case win_display_mode::center:
		on_menu_check(ID_BACKGROUND_CENTER, false);
		break;
	case win_display_mode::fill:
		on_menu_check(ID_BACKGROUND_FILL, false);
		break;
	case win_display_mode::tile:
		on_menu_check(ID_BACKGROUND_TILE, false);
		break;
	case win_display_mode::fit:
		on_menu_check(ID_BACKGROUND_FIT, false);
		break;
	}
}

void App::on_menu_check(UINT menu, bool uncheckFirst)
{
	HMENU hmenu = GetMenu(m_main);

	MENUITEMINFO menuItem = { 0 };
	menuItem.cbSize = sizeof(MENUITEMINFO);
	menuItem.fMask = MIIM_STATE;

	GetMenuItemInfoW(hmenu, menu, FALSE, &menuItem);

	if (menuItem.fState == MFS_CHECKED) {
		menuItem.fState = MFS_UNCHECKED;
	}
	else 
	{
		menuItem.fState = MFS_CHECKED;
	}
	SetMenuItemInfoW(hmenu, menu, FALSE, &menuItem);
}

/*
typedef struct tagCHOOSECOLORW {
   DWORD        lStructSize;
   HWND         hwndOwner;
   HWND         hInstance;
   COLORREF     rgbResult;
   COLORREF*    lpCustColors;
   DWORD        Flags;
   LPARAM       lCustData;
   LPCCHOOKPROC lpfnHook;
   LPCWSTR      lpTemplateName;
} CHOOSECOLORW, *LPCHOOSECOLORW;
*/

void App::user_set_background_color()
{
	static COLORREF acrCustClr[16];
	CHOOSECOLORW color;
	color.lStructSize = sizeof(CHOOSECOLORW);
	color.hwndOwner = m_main;
	color.hInstance = nullptr;
	color.rgbResult = BACKGROUND_COLOR;
	color.Flags = CC_FULLOPEN | CC_RGBINIT;
	color.lpCustColors = acrCustClr;

	if (!ChooseColor(&color))
	{
		//error
		return;
	}
	DeleteObject(m_main_brush);
	m_main_brush = CreateSolidBrush(color.rgbResult);
	InvalidateRect(m_main, NULL, true);
	brush_changed = true;
}


