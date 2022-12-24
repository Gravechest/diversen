#include <windows.h>
#include <stdio.h>

typedef char               i1;
typedef short              i2;
typedef int                i4;
typedef long long          i8;
typedef unsigned char      u1;
typedef unsigned short     u2;
typedef unsigned int       u4;
typedef unsigned long long u8;
typedef float              f4;
typedef double             f8;

typedef struct{
	u4 x;
	u4 y;
}TEXTPOINTER;

typedef struct{
	u1 b;
	u1 g;
	u1 r;
}RGB;

int proc(HWND hwnd,MSG msgl,LPARAM lparam,WPARAM wparam);

WNDCLASSA wndclass = {.lpfnWndProc = proc,.lpszClassName = "class"};
BITMAPINFO bmi = {sizeof(BITMAPINFOHEADER),8,8,1,24,BI_RGB};

HWND wnd;
HDC dc;
MSG msg;

u1 fontSize = 16;

TEXTPOINTER textPointer;

u1*  fontAtlas;
u1*  fontTexture;
u2*  lineSize;
RGB* fontCharRam;

int proc(HWND hwnd,UINT msgl,WPARAM wparam,LPARAM lparam){
	switch(msgl){
	case WM_KEYDOWN:
		if(wparam > 0x40 && wparam < 0x5b){
			u4 offset = (wparam-1)/10*80*8 + (wparam-1)%10*8;
			for(u4 i = 0;i < 8;i++){
				for(u4 j = 0;j < 8;j++){
					if(fontTexture[offset+i*80+j]){
						fontCharRam[(7-i)*8+j].r = 255;
					}
					else{
						fontCharRam[(7-i)*8+j].r = 0;
					}
				}
			}
			StretchDIBits(dc,textPointer.x*fontSize,textPointer.y*fontSize,fontSize,fontSize,0,0,8,8,fontCharRam,&bmi,DIB_RGB_COLORS,SRCCOPY);
			textPointer.x++;
		}
		else{
			switch(wparam){
			case VK_RETURN:
				textPointer.y++;
				textPointer.x = 0;
				break;
			case VK_SPACE:
				textPointer.x++;
				break;
			case VK_BACK:
				if(textPointer.x)      textPointer.x--;
				else if(textPointer.y) textPointer.y--;
				memset(fontCharRam,0,8*8*sizeof(RGB));
				StretchDIBits(dc,textPointer.x*fontSize,textPointer.y*fontSize,fontSize,fontSize,0,0,8,8,fontCharRam,&bmi,DIB_RGB_COLORS,SRCCOPY);
				break;
			}
		}
		break;
	}
	return DefWindowProcA(hwnd,msgl,wparam,lparam);
}

void main(){
	fontCharRam = HeapAlloc(GetProcessHeap(),8,8*8*sizeof(RGB));
	fontTexture = HeapAlloc(GetProcessHeap(),0,80*80);
	lineSize = HeapAlloc(GetProcessHeap(),8,1024);
	wndclass.hInstance = GetModuleHandleA(0);
	RegisterClassA(&wndclass);
	wnd = CreateWindowExA(0,"class","gravechest text editor",WS_VISIBLE | WS_SYSMENU,0,0,500,500,0,0,wndclass.hInstance,0);
	dc = GetDC(wnd);
	HANDLE font = CreateFileA("font.bmp",GENERIC_READ,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
	SetFilePointer(font,1078,0,FILE_CURRENT);
	ReadFile(font,fontTexture,80*80,0,0);
	CloseHandle(font);
	StretchDIBits(dc,0,0,500,500,0,0,1,1,fontCharRam,&bmi,DIB_RGB_COLORS,SRCCOPY);
	while(GetMessageA(&msg,wnd,0,0)){
		TranslateMessage(&msg);
		DispatchMessageA(&msg);
	}
}
