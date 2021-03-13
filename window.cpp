#include <d2d1.h>
#include <windows.h>
LRESULT CALLBACK Windowproc(
	_In_ HWND hwnd,
	_In_ UINT uMsg,
	_In_ WPARAM wParam,
	_In_ LPARAM lparam
);
int CALLBACK WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE previnstance,
	_In_ LPSTR cmdline,
	_In_ int cmdshow
) {

	WNDCLASSEX window;

	HWND app = CreateWindowEx(NULL, L"yeetboi", L"yeetboi", NULL, 400, 400, 400, 400, NULL, NULL, hInstance, NULL);
}

