#include <windows.h>
#include <immintrin.h>
#include <intrin.h>

#define cellC 3*1000000

unsigned short *heap;
unsigned char *cells;

const char name[] = "meuk2";
HWND window;
HDC dc;
MSG Msg;
BITMAPINFO bmi = { sizeof(BITMAPINFOHEADER),256,256,1,16,BI_RGB };

long _stdcall proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	return DefWindowProc(hwnd,msg,wParam,lParam);
}

WNDCLASS wndclass = {0,proc,0,0,0,0,0,0,name,name};
unsigned char t = 0;

void main(){
	heap = (short*)HeapAlloc(GetProcessHeap(),8,131072 + cellC);
	cells = (char*)heap + 131072;
	wndclass.hInstance = GetModuleHandle(0);
	RegisterClass(&wndclass);
	window = CreateWindowEx(0,name,name,0x10080000,0x40,0x40,524,551,0,0,wndclass.hInstance,0);
	dc = GetDC(window);
	for(;;){
		for(int i = 0;i < 256;i++){
			heap[i + ((i | t) << 8)] = 255;
		}
		t++;
		StretchDIBits(dc,0,0,512,512,0,0,256,256,heap,&bmi,0,SRCCOPY);
		if(PeekMessage(&Msg,window,0,0,0)){
			GetMessage(&Msg,window,0,0);
			TranslateMessage(&Msg);
			DispatchMessageW(&Msg);
		}
		Sleep(100);
	}
}
