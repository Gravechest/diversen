#include <windows.h>
#include <iostream>
HINSTANCE hInst;
LPSTR command = GetCommandLineA();
int main() {
	WinMain(hInst,NULL,command,0);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
int CALLBACK WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE previnstance,
	_In_ LPSTR cmdline,
	_In_ int cmdshow
) {
	WNDCLASSEX window;
	window.lpfnWndProc = WndProc;
	window.lpszClassName = L"yeetboi2";
	window.hInstance = hInstance;
	RegisterClassEx(&window);
	HWND app = CreateWindowEx(NULL, L"yeetboi2", L"yeetboi", WS_OVERLAPPEDWINDOW, 400, 400, 400, 400, NULL, NULL, hInstance, NULL);
	while (true) {
		ShowWindow(app, NULL);
	}
}
