#include "App.h"
#include <windows.h>

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE /*prevInstance*/, LPWSTR /*command_line*/, int show_command) 
{
	App app { instance };
	return app.run(show_command);
}