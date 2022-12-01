#include <windows.h>

#define RESX 500
#define RESY 500

typedef struct{
	unsigned char r;
	unsigned char g;
	unsigned char b;
}RGB;

long proc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);

BITMAPINFO bmi ={sizeof(BITMAPINFOHEADER),RESX,RESY,1,24,BI_RGB};

HINSTANCE hinstance;
WNDCLASS wndclass = {.lpfnWndProc = proc,.lpszClassName = "class",.lpszMenuName = "class"};
HWND window;
HDC dc;
MSG Msg;

RGB *vram;

long proc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
	return DefWindowProcA(hwnd,msg,wParam,lParam);
}

void main(){
	vram = HeapAlloc(GetProcessHeap(),8,sizeof(RGB)*RESX*RESY);
	wndclass.hInstance = GetModuleHandleA(0);
	RegisterClassA(&wndclass);
	window = CreateWindowExA(0,"class","hello",WS_VISIBLE,100,100,500,500,0,0,wndclass.hInstance,0);
	dc = GetDC(window);
	StretchDIBits(dc,0,0,RESX,RESY,0,0,RESX,RESY,vram,&bmi,DIB_RGB_COLORS,SRCCOPY);
	for(;;){
		while(GetMessageA(&Msg,window,0,0)>0){
			TranslateMessage(&Msg);
			DispatchMessageW(&Msg);
		}
	}
}
