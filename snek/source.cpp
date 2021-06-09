#include <windows.h>
#include <thread>
#include "global.h"
LRESULT CALLBACK WndProc(HWND app, UINT umsg, WPARAM wparam, LPARAM lparam);
int WINAPI wWinMain(
	HINSTANCE hInstance,
	HINSTANCE previnstance,
	LPTSTR cmdline,
	int cmdshow
) {
	MSG msg;
	WNDCLASS window = {};
	window.lpfnWndProc = WndProc;
	window.lpszClassName = L"snek";
	window.hInstance = hInstance;
	RegisterClass(&window);
	HWND app = CreateWindowEx(0, L"snek", L"snek", WS_OVERLAPPEDWINDOW, 0, 0, 850, 850, NULL, NULL, hInstance, NULL);
	ShowWindow(app, cmdshow);
	int food = network_init();
	HDC pixel = init_game(app, food);
	while (true) {
		if (PeekMessage(&msg, app, 0, 0, 0) != 0) {
			GetMessage(&msg, app, 0, 0);
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
		the_game(pixel);
		if (PeekMessage(&msg, app, 0, 0, 0) != 0) {
			GetMessage(&msg, app, 0, 0);
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
	}
}
LRESULT CALLBACK WndProc(HWND app, UINT msg, WPARAM wparam, LPARAM lparam) {
	if (WM_CLOSE == msg) {
		TerminateProcess(app, 1);
	}
	return DefWindowProc(app, msg, wparam, lparam);
}
