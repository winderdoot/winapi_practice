#include "App.h"
#include <stdexcept>
#include <cstdlib>
#include "resource.h"

void App::on_window_move(
	HWND window,
	LPWINDOWPOS params) {
	RECT rect;
	GetWindowRect(window, &rect);
	SIZE size {
	.cx = rect.right - rect.left,
	.cy = rect.bottom - rect.top };
	POINT new_pos {rect.left, rect.top};

	SetWindowPos(window, nullptr, new_pos.x, new_pos.y,
		0, 0, SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOZORDER);
}

void App::update_transparency() 
{
	RECT main_rc, inter;
	//DwmGetWindowAttribute(m_main, DWMWA_EXTENDED_FRAME_BOUNDS, &main_rc, sizeof(RECT)); - by otrzymać dokładny rect okienka bez cienia
	/*GetWindowRect(m_popup, &popup_rc);
	IntersectRect(&inter, &main_rc, &popup_rc);
	BYTE a = IsRectEmpty(&inter) ? 255 : 255 * 50 / 100;*/
	BYTE a = 255 * 50 / 100;
	SetLayeredWindowAttributes(m_main, 0, a, LWA_ALPHA);
}

void App::on_timer(HWND window)
{
	int ind = rand() % m_board.field_count;
	COLORREF col = RGB(rand() % 256, rand() % 256, rand() % 256);
	m_board.set_colour(ind, col);
	InvalidateRect(window, NULL, false);
	on_paint(window);
	
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
	PAINTSTRUCT ps;
	HDC dc = BeginPaint(window, &ps); // Device Context
	auto oldPen = SelectObject(dc, GetStockObject(NULL_PEN));
	for (auto& f : m_board.fields()) 
	{
		auto newBrush = CreateSolidBrush(f.color);
		auto oldBrush = SelectObject(dc, newBrush);

		RoundRect(dc, f.position.left, f.position.top, f.position.right, f.position.bottom, 11, 11);

		DeleteObject(SelectObject(dc, newBrush));
		SelectObject(dc, oldBrush);
	}
	SelectObject(dc, oldPen);
	EndPaint(window, &ps);
}

void App::on_resize(HWND window, UINT w, UINT h) 
{
	return;
}