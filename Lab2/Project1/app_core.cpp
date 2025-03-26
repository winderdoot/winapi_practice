#include "App.h"
#include "resource.h"

std::wstring const App::s_class_name { L"Space Invaders - WINAPI" };;
POINT const App::m_enemy_size { 50, 40 };

LONG const App::m_enemy_off_max{ 25 };
LONG const App::m_enemy_step{ 3 };

POINT const App::m_player_size{50, 50};
LONG const App::m_player_step{12};



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
		.hbrBackground = m_main_brush,
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
	HDC hdc;
	UINT width, height;
	RECT rect;

	switch (message) 
	{
		case WM_KEYDOWN:
			on_keydown(window, wparam, lparam);
			return 0;
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
		case WM_CTLCOLORSTATIC:
		/*	for (auto& bullet : m_bullets)
			{
				if ((HWND)lparam == bullet.window)
				{
				}
			}
			return 0;*/
			return reinterpret_cast<UINT_PTR>(bullet_brush);

		case WM_ERASEBKGND:
			
			hdc = (HDC)wparam;
			GetClientRect(m_main, &rect);
			FillRect(hdc, &rect, m_main_brush);
			if (brush_changed)
			{
				capture_background(m_player);
				capture_background(m_enemies[0]);
				for (auto& enem : m_enemies)
					enem.background = m_enemies[0].background;
				brush_changed = false;
			}
			return 1;
		case WM_CLOSE:
			DestroyWindow(window);
			return 0;
		case WM_DESTROY:
			if (window == m_main)
				PostQuitMessage(EXIT_SUCCESS);
			return 0;
		default:
			return DefWindowProc(window, message, wparam, lparam);
	}
	return DefWindowProcW(window, message, wparam, lparam);
}

/*
	HINSTANCE m_instance;
	HWND m_main;
	POINT m_screen_size;
	bool m_has_focus;
	bool m_enemy_moving_right;
	agent_t m_player, m_enemy;

	std::wstring m_window_title;
*/

// Constructor
App::App(HINSTANCE instance)
	: m_instance { instance }, m_main {}, m_main_brush { CreateSolidBrush(BACKGROUND_COLOR) },
	m_screen_size { GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) },
	m_has_focus { true }, m_enemy_moving_right { true }, m_player {}, m_enemies {}, m_bullets {}, 
	m_window_title {L"Space Invaders 2137 - WINAPI"}
{
	srand((unsigned)time(nullptr));
	register_class();
	DWORD main_style = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX | WS_CLIPCHILDREN;
	m_main = create_window(main_style, nullptr, WS_EX_LAYERED);
	SetLayeredWindowAttributes(m_main, 0, 255, LWA_ALPHA);

	m_player.pos.x = m_player_def_pos.x;
	m_player.pos.y = m_player_def_pos.y;
	m_player.init_pos = m_player_def_pos;

	m_player.window = CreateWindowExW(
		0,
		L"STATIC",
		nullptr,
		WS_CHILD | WS_VISIBLE | SS_BITMAP,
		m_player.pos.x,
		m_player.pos.y,
		m_player_size.x,
		m_player_size.y,
		m_main,
		nullptr,
		m_instance,
		nullptr);
	
	m_player.bitmap = (HBITMAP)LoadImageW(
		instance,      // Instance handle
		MAKEINTRESOURCE(ID_PLAYER_BMP), // Path to the .bmp file
		IMAGE_BITMAP,   // Load as a bitmap
		0, 0,           // Size (0,0 for original size)W
		LR_DEFAULTCOLOR // Load from file
	);
	capture_background(m_player);
	//m_player.bitmap = remove_purple_background(m_player.bitmap);
	m_player.current_frame = get_next_frame(m_player, 0, 50, 50, 3, PLAYER_BACKGROUND);
	SendMessageW(m_player.window, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)m_player.current_frame);

	init_enemies();
	bullet_brush = CreateSolidBrush(RGB(10, 20, 15));
	for (int i = 0; i < MAX_BULLETS; i++)
	{
		memset(&m_bullets[i], 0, sizeof(agent_t));
	}
}

// Change if you need to add accelerators
int App::run(int show_command)
{
	center_window(m_main);
	SetWindowPos(m_main, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	ShowWindow(m_main, show_command);
	for (const agent_t& enem : m_enemies)
	{
		ShowWindow(enem.window, SW_HIDE);
	}
	capture_background(m_player);
	ShowWindow(m_player.window, SW_HIDE);

	capture_background(m_enemies[0]);
	for (auto& enem : m_enemies)
		enem.background = m_enemies[0].background;
	

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