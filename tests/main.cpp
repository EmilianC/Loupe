#define CATCH_CONFIG_RUNNER
#define CATCH_CONFIG_COLOUR_WINDOWS
#include "catch/catch.hpp"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow)
{
	int result = Catch::Session().run(__argc, __argv);
	if (result != 0)
	{
		system("pause");
	}
	else
	{
		Sleep(800);
	}

	return result;
}
