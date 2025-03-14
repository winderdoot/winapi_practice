#include "App.h"
#include "resource.h"

std::wstring const App::s_class_name { L"Space Invaders - WINAPI" };
LONG const App::m_width = 800;
LONG const App::m_height = 600;

POINT const App::m_enemy_size { 50, 40 };
POINT const App::m_enemy_def_pos{ (m_width - m_enemy_size.x) / 2, (m_height - m_enemy_size.y) / 4 };
LONG const App::m_enemy_off_max{ 25 };
LONG const App::m_enemy_step{ 5 };

POINT const App::m_player_size{ 50, 50 };
POINT const App::m_player_def_pos{ (m_width - m_enemy_size.x) / 2 , 3 * (m_height - m_enemy_size.y) / 4 };


bool App::register_class() 
{
	WNDCLASSEXW desc {};
	if (GetClassInfoExW(m_instance, s_class_name.c_str(), &desc) != 0)
		return true;
	desc = 
	{
		.cbSize = sizeof(WNDCLASSEXW),
		.lpfnWndProc = window_proc_static,
		.hInstance = m_instance,
		.hCursor = LoadCursorW(nullptr, L"IDC_ARROW"),
		.hbrBackground = CreateSolidBrush(BACKGROUND_COLOR),
		.lpszMenuName = MAKEINTRESOURCEW(ID_MAINMENU),
		.lpszClassName = s_class_name.c_str(),
	};
	return RegisterClassExW(&desc) != 0;
}

HWND App::create_window(DWORD style, HWND parent, DWORD ex_style) 
{
	RECT size { 0, 0, m_width, m_height };
	AdjustWindowRectEx(&size, style, true, 0);
	HWND window = CreateWindowExW(ex_style,
		s_class_name.c_str(), m_window_title.c_str(), style,
		CW_USEDEFAULT, 0,
		size.right - size.left, size.bottom - size.top,
		parent, nullptr, m_instance, this);
	//for (auto &f : m_board.fields()) 
	//{
	//	CreateWindowExW(
	//		0,
	//		L"STATIC",
	//		nullptr,
	//		WS_CHILD | WS_VISIBLE | SS_CENTER,
	//		f.position.left, f.position.top,
	//		f.position.right - f.position.left,
	//		f.position.bottom - f.position.top,
	//		window,
	//		nullptr,
	//		m_instance,
	//		nullptr);
	//}
	return window;
}

LRESULT App::window_proc_static(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	App *app = nullptr;
	if (message == WM_NCCREATE)
	{
		auto p = reinterpret_cast<LPCREATESTRUCTW>(lparam);
		app = static_cast<App *>(p->lpCreateParams);
		SetWindowLongPtrW(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(app));
	}
	else 
	{
		app = reinterpret_cast<App *>(GetWindowLongPtrW(window, GWLP_USERDATA));
	}
	if (app != nullptr) {
		return app->window_proc(window, message, wparam, lparam);
	}
	return DefWindowProcW(window, message, wparam, lparam);
}

/// =========== ADD THINGS BELOW ================== ///

// Here is where all the messages are handled
LRESULT App::window_proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	UINT width, height;
	switch (message) 
	{
		case WM_LBUTTONDOWN:
			on_mouse_click(window, wparam, lparam);
			return 0;
		case WM_CHAR:
			on_char(window, wparam, lparam);
			return 0;
		case WM_MOUSEMOVE:
			on_mouse_move(window);
			return 0;
		case WM_SIZE:
			width = LOWORD(lparam);
			height = HIWORD(lparam);
			on_resize(window, width, height);
			return 0;
		case WM_COMMAND:
			on_command(LOWORD(wparam));
			return 0;
		case WM_TIMER:
			on_timer(window);
			return 0;
		case WM_WINDOWPOSCHANGED:
			on_window_move(window, reinterpret_cast<LPWINDOWPOS>(lparam));
			return 0;
		case WM_SETFOCUS:
			m_has_focus = true;
			update_transparency();
			break;
		case WM_KILLFOCUS:
			m_has_focus = false;
			update_transparency();
			break;
		//case WM_CTLCOLORSTATIC:
		//	return on_colorstatic(window);
			
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

// Constructor
App::App(HINSTANCE instance)
	: m_instance{ instance }, m_main{},
	m_screen_size { GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) },
	m_has_focus{ true }, m_enemy_moving_right{ true }, m_player{ }, m_enemy{ }, m_window_title{ L"New App" }
{
	//m_enemy.brush = CreateSolidBrush(ENEMY_COLOR);
	//m_enemy.pos.x = m_enemy_def_pos.x;
	//m_enemy.pos.y = m_enemy_def_pos.y;

	//m_player.brush = CreateSolidBrush(PLAYER_COLOR);
	//m_player.pos.x = m_player_def_pos.x;
	//m_player.pos.y = m_player_def_pos.y;

	srand((unsigned)time(nullptr));
	register_class();
	DWORD main_style = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX;
	m_main = create_window(main_style, nullptr, WS_EX_LAYERED);
	//m_enemy.window	= CreateWindowExW(
	//	0,
	//	L"STATIC",
	//	nullptr,
	//	WS_CHILD | WS_VISIBLE | SS_CENTER,
	//	m_player.pos.x,
	//	m_player.pos.y,
	//	m_enemy_size.x,
	//	m_enemy_size.y,
	//	m_main,
	//	nullptr,
	//	m_instance,
	//	nullptr);
	//m_player.window = CreateWindowExW(
	//	0,
	//	L"STATIC",
	//	nullptr,
	//	WS_CHILD | WS_VISIBLE | SS_CENTER,
	//	m_player.pos.x,
	//	m_player.pos.y,
	//	m_enemy_size.x,
	//	m_enemy_size.y,
	//	m_main,
	//	nullptr,
	//	m_instance,
	//	nullptr);
	SetLayeredWindowAttributes(m_main, 0, 255, LWA_ALPHA);
}

// Change if you need to add accelerators
int App::run(int show_command)
{
	center_window(m_main);
	SetWindowPos(m_main, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	//ShowWindow(m_enemy.window, show_command);
	//ShowWindow(m_player.window, show_command);
	SetTimer(m_main, s_timer, 50, nullptr);
	MSG msg {};
	BOOL result = TRUE;
	HACCEL shortcuts = LoadAcceleratorsW(m_instance, MAKEINTRESOURCEW(ID_SHORTCUTS));
	while ((result = GetMessageW(&msg, nullptr, 0, 0)) != 0) 
	{
		if (result == -1)
			return EXIT_FAILURE;
		if (!TranslateAcceleratorW(
			msg.hwnd, shortcuts, &msg)) {
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
	}
	return EXIT_SUCCESS;
}