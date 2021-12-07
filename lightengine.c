#include <windows.h>
#include <stdio.h>
#include <immintrin.h>
#include <intrin.h>

#define resX 256
#define resY 256

#define cellC 3*1000000

unsigned char *heap;
unsigned char *map;

const char name[] = "meuk2";
HWND window;
HDC dc;
MSG Msg;
BITMAPINFO bmi = { sizeof(BITMAPINFOHEADER),resX,resY,1,32,BI_RGB };

long _stdcall proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	return DefWindowProc(hwnd,msg,wParam,lParam);
}

WNDCLASS wndclass = {0,proc,0,0,0,0,0,0,name,name};
unsigned int t = 0;

inline int mabs(int val){
	if(val < 0){
		return -val;
	}
	return val;
}

inline void fill(int i,int r,int g,int b){
	if(heap[i] < 255 - r){
		heap[i] += r;
	}
	else{
		heap[i] = 255;
	}
	if(heap[i+1] < 255 - g){
		heap[i+1] += g;
	}
	else{
		heap[i+1] = 255;
	}
	if(heap[i+2] < 255 - b){
		heap[i+2] += b;
	}
	else{
		heap[i+2] = 255;
	}
}

inline char collision(int x,int y){
	if(map[(x >> 7 & 0xffe0) + (y >> 12)]){
		if((x & 0x5f) > (y & 0x5f)){
			return 1;
		}
		else{
			return 2;
		}
	}
	return 0;
}

void light(unsigned char cx,unsigned char cy,char r,char g,char b){
	int ins = r + g + b << 4;
	int x = 0;
    int y = 0;
	int i = 0;
	for(;i < 512;i++){
		char rt = r;
		char gt = g;
		char bt = b;
		x = 512 - i;
		y = i;
		int val = mabs(x - y);
		for(int i2 = val;i2 < 512;i2+=5){
			x++;
			y++;
		}
		int rx = cx << 9;
		int ry = cy << 9;
		for(int i2 = 256;i2 > 0;i2--){
			rx += x;
			ry += y;
			if(rx > 0x1ffff || ry > 0x1ffff || rx < 0 || ry < 0){
				break;
			}
			switch(collision(rx,ry)){
			case 1:
				x = -x;
				fill(((rx >> 1 & 0xff00) + (ry >> 9)) << 2,bt << 2,gt << 2,rt << 2);
				rt >>= 1;
				gt >>= 1;
				bt >>= 1;
				break;
			case 2:
				y = -y;
				fill(((rx >> 1 & 0xff00) + (ry >> 9)) << 2,bt << 2,gt << 2,rt << 2);
				rt >>= 1;
				gt >>= 1;
				bt >>= 1;
				break;
			default:
				fill(((rx >> 1 & 0xff00) + (ry >> 9)) << 2,bt,gt,rt);
				break;
			}
		}
	}
	for(;i < 1024;i++){
		char rt = r;
		char gt = g;
		char bt = b;
		x = 512 - i;
		y = 1024 - i;
	    int val = mabs(mabs(x) - mabs(y));
		for(int i2 = val;i2 < 512;i2+=5){
			x--;
			y++;
		}
		int rx = cx << 9;
		int ry = cy << 9;
		for(int i2 = 256;i2 > 0;i2--){
			rx += x;
			ry += y;
			if(rx > 0x1ffff|| ry < 0 || rx < 0 || ry > 0x1ffff){
				break;
			}
			switch(collision(rx,ry)){
			case 1:
				x = -x;
				fill(((rx >> 1 & 0xff00) + (ry >> 9)) << 2,bt << 2,gt << 2,rt << 2);
				rt >>= 1;
				gt >>= 1;
				bt >>= 1;
				break;
			case 2:
				y = -y;
				fill(((rx >> 1 & 0xff00) + (ry >> 9)) << 2,bt << 2,gt << 2,rt << 2);
				rt >>= 1;
				gt >>= 1;
				bt >>= 1;
				break;
			default:
				fill(((rx >> 1 & 0xff00) + (ry >> 9)) << 2,bt,gt,rt);
				break;
			break;
			}
		}
	}
	for(;i < 1536;i++){
		char rt = r;
		char gt = g;
		char bt = b;
		x = i - 1536;
		y = 1024 - i;
		int val = mabs(x - y);
		for(int i2 = val;i2 < 512;i2+=5){
			x--;
			y--;
		}
		int rx = cx << 9;
		int ry = cy << 9;
		for(int i2 = 256;i2 > 0;i2--){
			rx += x;
			ry += y;
			if(rx < 0 || ry < 0 || rx > 0x1ffff || ry > 0x1ffff){
				break;
			}
			switch(collision(rx,ry)){
			case 1:
				x = -x;
				fill(((rx >> 1 & 0xff00) + (ry >> 9)) << 2,bt << 2,gt << 2,rt << 2);
				rt >>= 1;
				gt >>= 1;
				bt >>= 1;
				break;
			case 2:
				y = -y;
				fill(((rx >> 1 & 0xff00) + (ry >> 9)) << 2,bt << 2,gt << 2,rt << 2);
				rt >>= 1;
				gt >>= 1;
				bt >>= 1;
				break;
			default:
				fill(((rx >> 1 & 0xff00) + (ry >> 9)) << 2,bt,gt,rt);
				break;
			}
		}
	}
	for(;i < 2048;i++){
		char rt = r;
		char gt = g;
		char bt = b;
		x = i - 1536;
		y = i - 2048;
		int rx = cx << 9;
		int ry = cy << 9;
		int val = mabs(mabs(x) - mabs(y));
			for(int i2 = val;i2 < 512;i2+=5){
			x++;
			y--;
		}
		for(int i2 = 256;i2 > 0;i2--){
			rx += x;
			ry += y;
			if(rx < 0 || ry > 0x1ff00 || rx > 0x1ffff|| ry < 0){
				break;
			}
			switch(collision(rx,ry)){
			case 1:
				x = -x;
				fill(((rx >> 1 & 0xff00) + (ry >> 9)) << 2,bt << 2,gt << 2,rt << 2);
				rt >>= 1;
				gt >>= 1;
				bt >>= 1;
				break;
			case 2:
				y = -y;
				fill(((rx >> 1 & 0xff00) + (ry >> 9)) << 2,bt << 2,gt << 2,rt << 2);
				rt >>= 1;
				gt >>= 1;
				bt >>= 1;
				break;
			default:
				fill(((rx >> 1 & 0xff00) + (ry >> 9)) << 2,bt,gt,rt);
				break;
			}
		}
	}
}

void main(){
	timeBeginPeriod(1);
	heap = HeapAlloc(GetProcessHeap(),8,resX*resY*4+32*32);
	map = heap+resX*resY*4;
	map[32*4+10] = 1;
	map[32*4+11] = 1;
	map[32*4+12] = 1;
	map[32*4+13] = 1;
	map[32*4+14] = 1;
	wndclass.hInstance = GetModuleHandle(0);
	RegisterClass(&wndclass);
	window = CreateWindowEx(0,name,name,0x90080000,0,0,resX * 4,resY * 4,0,0,wndclass.hInstance,0);
	dc = GetDC(window);
	for(;;){
		light(80,80,10,1,1);
		light(10,100,1,10,1);
		light(40,150,1,1,10);
		t++;
		StretchDIBits(dc,0,0,resX * 4,resY * 4,0,0,resX,resY,heap,&bmi,0,SRCCOPY);
		if(PeekMessage(&Msg,window,0,0,0)){
			GetMessage(&Msg,window,0,0);
			TranslateMessage(&Msg);
			DispatchMessageW(&Msg);
		}
		Sleep(17);
		memset(heap,0,resX*resY*4);
	}
}