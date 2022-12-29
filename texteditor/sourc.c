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
}IVEC2;

typedef struct{
	u1 b;
	u1 g;
	u1 r;
}RGB;

typedef struct{
	IVEC2 space;
	u4 size;
}FONT;

int proc(HWND hwnd,MSG msgl,LPARAM lparam,WPARAM wparam);

WNDCLASSA wndclass = {.lpfnWndProc = proc,.lpszClassName = "class"};
BITMAPINFO bmi = {sizeof(BITMAPINFOHEADER),8,8,1,24,BI_RGB};

HWND wnd;
HDC dc;
MSG msg;

FONT font = {2,5,16};

IVEC2 textPointer;

RGB* fontAtlas;
u1*  fontTexture;
u2*  lineSize;
RGB* fontCharRam;

int proc(HWND hwnd,UINT msgl,WPARAM wparam,LPARAM lparam){
	switch(msgl){
	case WM_KEYDOWN:
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
			StretchDIBits(dc,textPointer.x*(font.space.x+font.size),textPointer.y*(font.space.y+font.size),font.size,font.size,0,0,1,1,fontCharRam,&bmi,DIB_RGB_COLORS,SRCCOPY);
			break;
		case VK_SHIFT:
			break;
		default:
			if(GetKeyState(VK_SHIFT)&0x80){
				StretchDIBits(dc,textPointer.x*(font.space.x+font.size),textPointer.y*(font.space.y+font.size),font.size,font.size,0,0,8,8,fontAtlas+wparam*8*8+0x100*8*8,&bmi,DIB_RGB_COLORS,SRCCOPY);
			}
			else{
				StretchDIBits(dc,textPointer.x*(font.space.x+font.size),textPointer.y*(font.space.y+font.size),font.size,font.size,0,0,8,8,fontAtlas+wparam*8*8,&bmi,DIB_RGB_COLORS,SRCCOPY);
			}
			textPointer.x++;
		}
		break;
	}
	return DefWindowProcA(hwnd,msgl,wparam,lparam);
}

void main(){
	fontAtlas   = HeapAlloc(GetProcessHeap(),0,256*2*8*8*sizeof(RGB));
	fontCharRam = HeapAlloc(GetProcessHeap(),8,8*8*sizeof(RGB));
	fontTexture = HeapAlloc(GetProcessHeap(),0,80*80);
	lineSize    = HeapAlloc(GetProcessHeap(),8,1024);
	wndclass.hInstance = GetModuleHandleA(0);
	RegisterClassA(&wndclass);
	wnd = CreateWindowExA(0,"class","gravechest text editor",WS_VISIBLE | WS_SYSMENU,0,0,500,500,0,0,wndclass.hInstance,0);
	dc = GetDC(wnd);
	HANDLE font = CreateFileA("font.bmp",GENERIC_READ,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
	SetFilePointer(font,1078,0,FILE_CURRENT);
	ReadFile(font,fontTexture,80*80,0,0);
	CloseHandle(font);
	for(u4 i = 0;i < 10;i++){
		u4 offset = (i+15)/10*8*80 + (i+15)%10*8;
		for(u4 j = 0;j < 8;j++){
			for(u4 k = 0;k < 8;k++){
				if(fontTexture[offset+(7-j)*80+k]){
					fontAtlas[(i+0x30)*64+j*8+k].r = 255;
					fontAtlas[(i+0x30)*64+j*8+k].g = 255;
					fontAtlas[(i+0x30)*64+j*8+k].b = 255;
				}
			}
		}
	}
	for(u4 i = 0;i < 27;i++){
		u4 offset = (i+31)/10*8*80 + (i+31)%10*8;
		for(u4 j = 0;j < 8;j++){
			for(u4 k = 0;k < 8;k++){
				if(fontTexture[offset+(7-j)*80+k]){
					fontAtlas[(i+0x40+0x100)*64+j*8+k].r = 255;
					fontAtlas[(i+0x40+0x100)*64+j*8+k].g = 255;
					fontAtlas[(i+0x40+0x100)*64+j*8+k].b = 255;
				}
			}
		}
	}
	for(u4 i = 0;i < 27;i++){
		u4 offset = (i+63)/10*8*80 + (i+63)%10*8;
		for(u4 j = 0;j < 8;j++){
			for(u4 k = 0;k < 8;k++){
				if(fontTexture[offset+(7-j)*80+k]){
					fontAtlas[(i+0x40)*64+j*8+k].r = 255;
					fontAtlas[(i+0x40)*64+j*8+k].g = 255;
					fontAtlas[(i+0x40)*64+j*8+k].b = 255;
				}
			}
		}
	}
	StretchDIBits(dc,0,0,500,500,0,0,1,1,fontCharRam,&bmi,DIB_RGB_COLORS,SRCCOPY);
	while(GetMessageA(&msg,wnd,0,0)){
		TranslateMessage(&msg);
		DispatchMessageA(&msg);
	}
}
