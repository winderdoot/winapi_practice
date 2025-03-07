#include "app_simon.h"
#include <stdexcept>
#include <format>
#include "resource.h"

std::wstring const app_simon::s_class_name{ L"2048 Window" };

bool app_simon::register_class()
{
	WNDCLASSEXW desc{};
	if (GetClassInfoExW(m_instance, s_class_name.c_str(),
		&desc) != 0)
		return true;
	desc = {
		.cbSize = sizeof(WNDCLASSEXW),
		.lpfnWndProc = window_proc_static,
		.hInstance = m_instance,
		/*.hIcon = static_cast<HICON>(LoadImageW(
		m_instance,
		MAKEINTRESOURCEW(ID_APPICON),
		IMAGE_ICON,
		0, 0,
		LR_SHARED | LR_DEFAULTSIZE)),*/
		/*you can also use:
		LoadIconW(m_instance,
		MAKEINTRESOURCEW(ID_APPICON)) */
		.hCursor = LoadCursorW(nullptr, L"IDC_ARROW"),
		.hbrBackground =
		CreateSolidBrush(RGB(250, 247, 238)),
		.lpszMenuName =
		MAKEINTRESOURCEW(ID_MAINMENU), //add this
		.lpszClassName = s_class_name.c_str()
	};
	return RegisterClassExW(&desc) != 0;
}

HWND app_simon::create_window(DWORD style, HWND parent,
	DWORD ex_style)
{
	RECT size{ 0, 0, m_board.width, m_board.height };
	AdjustWindowRectEx(&size, style, true, 0);

	HWND window = CreateWindowExW(ex_style,
		s_class_name.c_str(), L"2048", style,
		CW_USEDEFAULT, 0,
		size.right - size.left, size.bottom - size.top,
		parent, nullptr, m_instance, this);

	/*for (auto& f : m_board.fields())
	{
		CreateWindowExW(
			0,
			L"STATIC",
			nullptr,
			WS_CHILD | WS_VISIBLE | SS_CENTER,
			f.position.left, f.position.top,
			f.position.right - f.position.left,
			f.position.bottom - f.position.top,
			window,
			nullptr,
			m_instance,	
			nullptr);
	}*/

	return window;
}

LRESULT app_simon::window_proc_static(
	HWND window,
	UINT message,
	WPARAM wparam,
	LPARAM lparam)
{
	app_simon* app = nullptr;
	if (message == WM_NCCREATE)
	{
		auto p = reinterpret_cast<LPCREATESTRUCTW>(lparam);
		app = static_cast<app_simon*>(p->lpCreateParams);
		SetWindowLongPtrW(window, GWLP_USERDATA,
			reinterpret_cast<LONG_PTR>(app));
	}
	else
	{
		app = reinterpret_cast<app_simon*>(
			GetWindowLongPtrW(window, GWLP_USERDATA));
	}
	if (app != nullptr)
	{
		return app->window_proc(window, message,
			wparam, lparam);
	}
	return DefWindowProcW(window, message, wparam, lparam);
}

LRESULT app_simon::window_proc(
	HWND window, UINT message,
	WPARAM wparam, LPARAM lparam)
{
	UINT width, height;
	switch (message) 
	{
		case WM_SIZE:
			width = LOWORD(lparam);
			height = HIWORD(lparam);
			on_resize(window, width, height);
			return 0;
		case WM_PAINT:
			on_paint(window);
			return 0;
		case WM_COMMAND:
			on_command(LOWORD(wparam));
			return 0;
		case WM_TIMER:
			on_timer();
			return 0;
		case WM_CTLCOLORSTATIC:
			return reinterpret_cast<INT_PTR>(m_field_brush);
		case WM_WINDOWPOSCHANGED:
			on_window_move(window,
				reinterpret_cast<LPWINDOWPOS>(lparam));
			return 0;
		case WM_CLOSE:
			DestroyWindow(window);
			return 0;
		case WM_DESTROY:
			if (window == m_main)
				PostQuitMessage(EXIT_SUCCESS);
			return 0;
	}
	return DefWindowProcW(window, message, wparam, lparam);
}

void app_simon::on_window_move(
	HWND window,
	LPWINDOWPOS params)
{
	//HWND other = (window == m_main) ? m_popup : m_main;
	//RECT other_rc;
	//GetWindowRect(other, &other_rc);
	////SIZE other_size{
	////.cx = other_rc.right - other_rc.left,
	////.cy = other_rc.bottom - other_rc.top }; -- old version
	//POINT this_board_center {
	//	board::width / 2,
	//	board::height / 2
	//};
	//if (!ClientToScreen(window, &this_board_center))
	//{
	//	return;
	//}
	//POINT other_board_center {
	//	board::width / 2,
	//	board::height / 2
	//};
	//if (!ClientToScreen(other, &other_board_center))
	//{
	//	return;
	//}
	///*
	//m_screen_size.x - (params->x + params->cx / 2) - other_size.cx / 2, - Old version
	//m_screen_size.y - (params->y + params->cy / 2) - other_size.cy / 2
	//*/
	//POINT new_pos {
	//	/*calculate the new position of the other window*/
	//	m_screen_size.x - this_board_center.x - (other_board_center.x - other_rc.left),
	//	m_screen_size.y - this_board_center.y - (other_board_center.y - other_rc.top)
	//};
	//if (new_pos.x == other_rc.left &&
	//	new_pos.y == other_rc.top)
	//	return;
	SetWindowPos(window, nullptr, params->x, params->y,
		0, 0, SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOZORDER);
	update_transparency();
}

app_simon::app_simon(HINSTANCE instance, int N)
	: m_instance{ instance }, m_main{}, m_board{N, N},
	m_field_brush{ CreateSolidBrush(RGB(204, 192, 174)) },
	m_screen_size{GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)}
{
	register_class();
	DWORD main_style = WS_OVERLAPPED | WS_SYSMENU |
		WS_CAPTION | WS_MINIMIZEBOX;
	DWORD popup_style = WS_OVERLAPPED | WS_CAPTION;
	m_main = create_window(main_style);
}

int app_simon::run(int show_command)
{
	ShowWindow(m_main, show_command);
	SetTimer(m_main, s_timer, 1000, nullptr);
	m_startTime = std::chrono::system_clock::now();


	HACCEL shortcuts = LoadAcceleratorsW(m_instance,
		MAKEINTRESOURCEW(ID_SHORTCUT));
	MSG msg{};
	BOOL result = TRUE;
	while ((result = GetMessageW(&msg, nullptr, 0, 0)) != 0)
	{
		if (result == -1)
			return EXIT_FAILURE;
		if (!TranslateAcceleratorW(msg.hwnd, shortcuts, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
	}
	return EXIT_SUCCESS;
}

void app_simon::update_transparency()
{
	//RECT main_rc, popup_rc, inter;
	//DwmGetWindowAttribute(m_main, DWMWA_EXTENDED_FRAME_BOUNDS, &main_rc, sizeof(RECT));
	//DwmGetWindowAttribute(m_popup, DWMWA_EXTENDED_FRAME_BOUNDS, &popup_rc, sizeof(RECT));
	//GetWindowRect(m_main, &main_rc);
	//GetWindowRect(m_popup, &popup_rc);
	//IntersectRect(&inter, &main_rc, &popup_rc);
	//BYTE a = IsRectEmpty(&inter) ? 255 : 255 * 50 / 100;
	//SetLayeredWindowAttributes(m_popup, 0, a, LWA_ALPHA);
}

void app_simon::on_timer()
{
	using namespace std::chrono;
	auto title = std::format(L"2048 {:%M:%S}", duration_cast<duration<int>>(
		system_clock::now() - m_startTime));
	SetWindowTextW(m_main, title.c_str());
	//SetWindowTextW(m_popup, title.c_str());
}

void app_simon::on_command(WORD cmdID)
{
	switch (cmdID)
	{
		case ID_NEWGAME:
			m_startTime = std::chrono::system_clock::now();
			on_timer();
			break;
	}
}

void app_simon::on_paint(HWND window)
{
	PAINTSTRUCT ps;
	HDC dc = BeginPaint(window, &ps); // Device Context
	auto oldBrush = SelectObject(dc, m_field_brush);
	auto oldPen = SelectObject(dc, GetStockObject(NULL_PEN));
	for (auto& f : m_board.fields())
	{
		RoundRect(dc, f.position.left, f.position.top,
			f.position.right, f.position.bottom, 11, 11);
	}
	SelectObject(dc, oldBrush);
	SelectObject(dc, oldPen);
	EndPaint(window, &ps);
}

void app_simon::on_resize(HWND window, UINT w, UINT h)
{
	return;
}
