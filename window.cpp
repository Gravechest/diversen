#include <windows.h>
#include <iostream>
LRESULT CALLBACK WndProc(HWND app, UINT msg, WPARAM wParam, LPARAM lParam);
int WINAPI wWinMain(
	HINSTANCE hInstance,
	HINSTANCE previnstance,	
	LPTSTR cmdline,
	int cmdshow
) {	
	WNDCLASS window;
	window.lpfnWndProc = WndProc;
	window.lpszClassName = L"yeetboi2";
	window.hInstance = hInstance;
	RegisterClass(&window);
	HWND app = CreateWindowEx(0, L"yeetboi2", L"yeetboi", WS_OVERLAPPEDWINDOW, 400, 400, 400, 400, NULL, NULL, hInstance, NULL);
	ShowWindow(app, cmdshow);
	while (true){

	}
}
LRESULT CALLBACK WndProc(HWND app, UINT msg, WPARAM wParam, LPARAM lParam) {
	return 0;
}
