#include <windows.h>

#include "ivec2.h"
#include "vec2.h"

#define winName "windows"

#pragma comment(lib,"winmm.lib")

typedef struct{
	unsigned char r;
	unsigned char g;
	unsigned char b;
}RGB;

typedef struct{
	VEC2 pos;
	VEC2 dir;
	RGB  *texture;
	unsigned int sz;
}BALL;

unsigned int ballC;
BALL *ball;

RGB *vram;

const IVEC2 reso  = {700,700};
const IVEC2 vreso = {700,700};

HWND window;
HDC dc;
MSG Msg;

BITMAPINFO bmi = {sizeof(BITMAPINFOHEADER),0,0,1,24,BI_RGB};

long _stdcall proc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam) {
	return DefWindowProcA(hwnd,msg,wParam,lParam);
}	

WNDCLASS wndclass = {0,proc,0,0,0,0,0,0,winName,winName};

void CreateBall(VEC2 pos,VEC2 dir,int sz){
	ball[ballC].texture = HeapAlloc(GetProcessHeap(),8,sz*sz*sizeof(RGB));
	IVEC2 middle = (IVEC2){sz/2,sz/2};
	for(int i = 0;i < sz*sz;i++){
		int d = 255-IVEC2dist((IVEC2){i/sz,i%sz},middle)*255/sz*2;
		if(d>0){
			ball[ballC].texture[i].r = d;
		}

	}
	ball[ballC].pos = pos;
	ball[ballC].dir = dir;
	ball[ballC].sz  = sz;
	ballC++;
}

void draw(){
	for(;;){
		for(int i = 0;i < ballC;i++) {
			int l = (int)ball[i].pos.x*vreso.x+ball[i].pos.y;
			for(int i2 = 0;i2 < ball[i].sz;i2++){
				for(int i3 = 0;i3 < ball[i].sz;i3++){
					vram[(int)i2*vreso.x+i3+l] = (RGB){0,0,0};
				}
			}
			VEC2addVEC2(&ball[i].pos, ball[i].dir);
			if(ball[i].pos.x < 0.0f || ball[i].pos.x + ball[i].sz > vreso.x) {
				VEC2subVEC2(&ball[i].pos, ball[i].dir);
				ball[i].dir.x = -ball[i].dir.x;
			}
			if(ball[i].pos.y < 0.0f || ball[i].pos.y + ball[i].sz > vreso.y) {
				VEC2subVEC2(&ball[i].pos, ball[i].dir);
				ball[i].dir.y = -ball[i].dir.y;
			}
			l = (int)ball[i].pos.x*vreso.x+ball[i].pos.y;
			for(int i2 = 0;i2 < ball[i].sz;i2++){
				for(int i3 = 0;i3 < ball[i].sz;i3++){
					vram[(int)i2*vreso.x+i3+l] = ball[i].texture[i2*ball[i].sz+i3];
				}
			}
		}
		Sleep(15);
	}
}

void render(){
	for(;;){
		StretchDIBits(dc,0,0,reso.y,reso.x,0,0,vreso.y,vreso.x,vram,&bmi,DIB_RGB_COLORS,SRCCOPY);
	}
}

void main(){
	timeBeginPeriod(1);
	bmi.bmiHeader.biHeight = vreso.x;
	bmi.bmiHeader.biWidth  = vreso.y;
	vram = HeapAlloc(GetProcessHeap(),8,IVEC2mulS(vreso) * sizeof(RGB));
	ball = HeapAlloc(GetProcessHeap(),8,256 * sizeof(BALL));
	wndclass.hInstance = GetModuleHandleA(0);
	RegisterClassA(&wndclass);
	window = CreateWindowExA(0,winName,winName,0x10080000,0,0,reso.y+16,reso.x+39,0,0,wndclass.hInstance,0);
	dc = GetDC(window);
	for(int i = 0;i < 10;i++){
		CreateBall((VEC2){i*40,0.0f},(VEC2){0.5f,1.0f},40);
	}
	CreateThread(0,0,draw,0,0,0);
	CreateThread(0,0,render,0,0,0);
	for(;;){
		while(PeekMessageA(&Msg,window,0,0,0)){
			GetMessageA(&Msg,window,0,0);
			TranslateMessage(&Msg);
			DispatchMessageW(&Msg);
		}
		Sleep(15);
	}
}
