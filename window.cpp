#include <windows.h>
LRESULT CALLBACK WndProc(HWND app, UINT msg, WPARAM wparam, LPARAM lparam);
int WINAPI wWinMain(
	HINSTANCE hInstance,
	HINSTANCE previnstance,
	LPTSTR cmdline,
	int cmdshow
) {
	WNDCLASS window = {};
	window.lpfnWndProc = WndProc;
	window.lpszClassName = L"yeetboi2";
	window.hInstance = hInstance;
	RegisterClass(&window);
	HWND app = CreateWindowEx(0, L"yeetboi2", L"yeetboi", WS_OVERLAPPEDWINDOW, 400, 400, 400, 400, NULL, NULL, hInstance, NULL);
	ShowWindow(app, cmdshow);
	PAINTSTRUCT verf;
	HDC pixel = BeginPaint(app, &verf);
	HBRUSH redkwast = CreateSolidBrush(RGB(255, 0, 0));
	HBRUSH blackkwast = CreateSolidBrush(RGB(0, 0, 0));
	while (true) {

	}
}
LRESULT CALLBACK WndProc(HWND app, UINT msg, WPARAM wparam, LPARAM lparam) {
	return DefWindowProc(app, msg, wparam, lparam);
}
