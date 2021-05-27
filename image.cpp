#include <windows.h>
#include <fstream>
#include <iostream>
LRESULT CALLBACK WndProc(HWND app, UINT msg, WPARAM wparam, LPARAM lparam);
int WINAPI wWinMain(
	HINSTANCE hInstance,
	HINSTANCE previnstance,
	LPTSTR cmdline,
	int cmdshow
) {
	MSG msg;
	WNDCLASS window = {};
	window.lpfnWndProc = WndProc;
	window.lpszClassName = L"yeetboi2";
	window.hInstance = hInstance;
	RegisterClass(&window);
	std::ifstream tekst("images/download.bmp", std::fstream::binary);
	tekst.seekg(0, tekst.end);
	int length = tekst.tellg();
	tekst.seekg(0, tekst.beg);
	char* tekstbuf = new char[length];
	tekst.read(tekstbuf, length);
	int width = tekstbuf[18] + tekstbuf[19] * 256 + tekstbuf[20] * 65536 + tekstbuf[21] * 16777216;
	HWND app = CreateWindowEx(0, L"yeetboi2", L"yeetboi", WS_OVERLAPPEDWINDOW, 200, 200, width, width, NULL, NULL, hInstance, NULL);
	ShowWindow(app, cmdshow);
	PAINTSTRUCT paint;
	HDC pixel = BeginPaint(app, &paint);
	for (int i = tekstbuf[10]; i < length; i+=4) {
		int localCoord = (i - tekstbuf[10]) / 4;	
		SetPixel(pixel, localCoord % width, localCoord / width, RGB(tekstbuf[i + 2], tekstbuf[i + 1], tekstbuf[i]));
	}
	while (true) {
		
		GetMessage(&msg, NULL, 0, 0);
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}
LRESULT CALLBACK WndProc(HWND app, UINT msg, WPARAM wparam, LPARAM lparam) {
	return DefWindowProc(app, msg, wparam, lparam);
}
