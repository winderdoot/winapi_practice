#include <windows.h>
#include "app_simon.h"



int valdateArgs(LPWSTR args)
{
	/*size_t len = wcsnlen(args, )*/
	return 3;
}

int WINAPI wWinMain(HINSTANCE instance,
	HINSTANCE /*prevInstance*/,
	LPWSTR pCmdLine,
	int show_command)
{
	int N = valdateArgs(pCmdLine);
	app_simon app{ instance, N };
	return app.run(show_command);
}