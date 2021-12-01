#include <windows.h>

int *heap;
const char name[] = "window";
HWND window;
HDC dc;
MSG Msg;

long _stdcall proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	return DefWindowProc(hwnd,msg,wParam,lParam);
}

WNDCLASS wndclass = {0,proc,0,0,0,0,0,0,name,name};

void main(){
	heap = HeapAlloc(GetProcessHeap(),0,30000);
	wndclass.hInstance = GetModuleHandle(0);
	RegisterClass(&wndclass);
	window = CreateWindowEx(0,name,name,0x10080000,0,0,100,100,0,0,wndclass.hInstance,0);
	dc = GetDC(window);
	for(;;){
		GetMessage(&Msg,window,0,0);
		TranslateMessage(&Msg);
		DispatchMessageW(&Msg);
	}
}
