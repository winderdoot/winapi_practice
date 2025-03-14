#include <stdexcept>
#include "App.h"
#include "resource.h"

std::wstring const App::s_class_name { L"MyAppWindow" };

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
	RECT size { 0, 0, m_board.width, m_board.height };
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
		case WM_PAINT:
			on_paint(window);
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
		//case WM_CTLCOLORSTATIC:
		//	return reinterpret_cast<INT_PTR>(m_field_brush);
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
	: m_instance { instance }, m_main {}, m_board {}, m_field_brush { CreateSolidBrush(DEFAULT_TILE_COLOR) },
	m_screen_size { GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) }, m_window_title {L"New App"}
{
	srand((unsigned)time(nullptr));
	register_class();
	DWORD main_style = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX;
	m_main = create_window(main_style, nullptr, WS_EX_LAYERED);
	SetLayeredWindowAttributes(m_main, 0, 255, LWA_ALPHA);
}

// Change if you need to add accelerators
int App::run(int show_command)
{
	ShowWindow(m_main, show_command);
	SetTimer(m_main, s_timer, 1000, nullptr);
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