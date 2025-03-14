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

void App::enemy_move()
{
	//LONG newX = m_enemy_pos.x;
	//if (m_enemy_moving_right)
	//{
	//	newX = newX + m_enemy_step;
	//	if (newX > m_enemy_def_pos.x + m_enemy_off_max)
	//	{
	//		newX = m_enemy_def_pos.x + m_enemy_off_max;
	//		m_enemy_moving_right = false;
	//		
	//		
	//	}
	//}
}

INT_PTR App::on_colorstatic(HWND window)
{

	if (window == m_player.window)
	{
		return reinterpret_cast<INT_PTR>(m_player.brush);
	}
	else if (window == m_enemy.window)
	{
		return reinterpret_cast<INT_PTR>(m_enemy.brush);
	}
	return reinterpret_cast<INT_PTR>(m_enemy.brush); // Doesn't matter
}

void App::on_timer(HWND window)
{

	//center_window(window);
}

void App::on_command(WORD cmdID) 
{
	switch (cmdID) {
	case ID_NEWGAME:
		// something
		break;
	}
}

void App::on_paint(HWND window) 
{
	
}

void App::on_resize(HWND window, UINT w, UINT h) 
{
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

void App::on_mouse_click(HWND window, WPARAM wparam, LPARAM lparam)
{

}
