#include <windows.h>
#include <stdio.h>
#include <immintrin.h>
#include <intrin.h>
#include <time.h>

#define resX 512
#define resY 512

#define Bound 0x3ffff

unsigned char *heap;
unsigned char *map;
unsigned char *gui;

char selected;

const char name[] = "meuk2";
HWND window;
HDC dc;
MSG Msg;
BITMAPINFO bmi = { sizeof(BITMAPINFOHEADER),resY,resX,1,32,BI_RGB };
BITMAPINFO bmi2 = { sizeof(BITMAPINFOHEADER),128,resX*2,1,32,BI_RGB };

long _stdcall proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	switch(msg){
	case WM_CLOSE:
		exit(0);
	}
	return DefWindowProc(hwnd,msg,wParam,lParam);
}

WNDCLASS wndclass = {0,proc,0,0,0,0,0,0,name,name};
unsigned short t = 0;

inline int mabs(int val){
	if(val < 0){
		return -val;
	}
	return val;
}

inline void fill(int i,int r,int g,int b){
	if(heap[i] < 240){
		heap[i] += r;
	}
	else{
		heap[i] = 255;
	}
	if(heap[i+1] < 240){
		heap[i+1] += g;
	}
	else{
		heap[i+1] = 255;
	}
	if(heap[i+2] < 240){
		heap[i+2] += b;
	}
	else{
		heap[i+2] = 255;
	}
}

inline void lightAlgo(int rx,int ry,int x,int y,char rt,char gt,char bt){
	for(;;){
		rx += x;
		ry += y;
		if(map[(rx & 0x3fe00) + (ry >> 9)]){
			char val = map[(rx & 0x3fe00) + (ry >> 9)];
			if(val & 1){
				int tx = rx << 9;
				int ty = ry << 9;
				while(map[(tx >> 9 & 0x3fe00) + (ty >> 18)]){
					tx -= x;
					ty -= y;
				}
				tx += x;
				if(map[(tx >> 9 & 0x3fe00) + (ty >> 18)]){
					x = -x;
				}
				else{
					y = -y;
				}
				fill(((rx & 0x3fe00) + (ry >> 9)) << 2,bt << (val >> 1 & 3),gt << (val >> 3 & 3),rt << (val >> 5 & 3));
				rt >>= 4 - (val >> 5 & 3);
				gt >>= 4 - (val >> 3 & 3);
				bt >>= 4 - (val >> 1 & 3);
				if(rt + gt + bt == 0){
					break;
				}
			}
			else if(val & 0x80){
				break;
			}
			else{
				fill(((rx & 0x3fe00) + (ry >> 9)) << 2,bt << (val >> 1 & 3),gt << (val >> 3 & 3),rt << (val >> 5 & 3));
				rt >>= 1 - (val >> 5 & 3);
				gt >>= 1 - (val >> 3 & 3);
				bt >>= 1 - (val >> 1 & 3);
				if(rt + gt + bt == 0){
					break;
				}
			}
		}
		else{
			fill(((rx & 0x3fe00) + (ry >> 9)) << 2,bt,gt,rt);
		}
	}
}

void light(unsigned short cx,unsigned short cy,char r,char g,char b){
	int x = 0;
    int y = 0;
	int i = 0;
	for(;i < 512;i++){
		x = 512 - i;
		y = i;
		int val = mabs(x - y);
		for(int i2 = val;i2 < 512;i2+=5){
			x++;
			y++;
		}
		lightAlgo(cx << 9,cy << 9,x,y,r,g,b);
	}
	for(;i < 1024;i++){
		x = 512 - i;	
		y = 1024 - i;
	    int val = mabs(mabs(x) - mabs(y));
		for(int i2 = val;i2 < 512;i2+=5){
			x--;
			y++;
		}
		lightAlgo(cx << 9,cy << 9,x,y,r,g,b);
	}
	for(;i < 1536;i++){
		x = i - 1536;
		y = 1024 - i;
		int val = mabs(x - y);
		for(int i2 = val;i2 < 512;i2+=5){
			x--;
			y--;
		}
		lightAlgo(cx << 9,cy << 9,x,y,r,g,b);
	}
	for(;i < 2048;i++){
		x = i - 1536;
		y = i - 2048;
		int val = mabs(mabs(x) - mabs(y));
		for(int i2 = val;i2 < 512;i2+=5){
			x++;
			y--;
		}
		lightAlgo(cx << 9,cy << 9,x,y,r,g,b);	
	}
}

inline void square(int x,int y,int s,char r,char g,char b){
	x <<= 9;
	y <<= 2;
	for(int i = x;i < x + (s << 9);i += 512){
		for(int i2 = y;i2 < y + (s << 2);i2+=4){
			gui[i+i2] = r;
			gui[i+i2+1] = g;
			gui[i+i2+2] = b;
		}
	}
}

void updateScreen(){

}

void lightE(){
	for(int i = 0;i < 512;i++){
		map[i] = 0x80;
	}
	for(int i = 261632;i < 262144;i++){
		map[i] = 0x80;
	}
	for(int i = 512;i < 261632;i+=512){
		map[i] = 0x80;
	}
	for(int i = 511;i < 261632;i+=512){
		map[i] = 0x80;
	}
	for(int i = 0;i < resX*512;i+=512){
		gui[i] = 80;
		gui[i+1] = 80;
		gui[i+2] = 80;
	}
	updateScreen();
	for(;;){
		t++;
		if(GetKeyState(VK_LBUTTON) < 0){
			POINT point;
			GetCursorPos(&point);
			ScreenToClient(window,&point);
			point.y = resY * 2 - point.y;
			for(int i = point.y << 8 & 0x3fe00;i < (point.y << 8) + 4096;i+=512){
				for(int i2 = point.x >> 1 & 0x1ff;i2 < (point.x >> 1 & 0x1ff) + 8;i2++){
					map[i + i2] = selected;
				}
			}
		}
		for(int i = 0;i < 9;i++){
			if(GetAsyncKeyState(i+0x30) & 0x0001){
				selected ^= 1 << i;
				char col = (selected & 1) << 5;
				square(0,1,32,col,col,col);
				square(32,1,32,(selected & 0x06) << 5,0,0);
				square(64,1,32,0,(selected & 0x18) << 3,0);
				square(92,1,32,0,0,(selected & 0x60) << 1);
			}
		}
		if(GetAsyncKeyState(VK_BACK) & 0x0001){
			selected = 0;
			square(0,1,32,0,0,0);
			square(32,1,32,0,0,0);
			square(64,1,32,0,0,0);
			square(92,1,32,0,0,0);
		}
		if(GetAsyncKeyState(VK_SPACE) & 0x0001){
			selected = 0xff;
			char col = (selected & 1) << 5;
			square(0,1,32,col,col,col);
			square(32,1,32,(selected & 0x06) << 5,0,0);
			square(64,1,32,0,(selected & 0x18) << 3,0);
			square(92,1,32,0,0,(selected & 0x60) << 1);
		}
		light(256,256,6,6,6);
		StretchDIBits(dc,0,0,resX * 2,resY * 2,0,0,resX,resY,heap,&bmi,0,SRCCOPY);
		memset(heap,0,resX*resY*4);
		StretchDIBits(dc,resX * 2,0,resX * 2 + 128,resY*2,0,0,resX,512,gui,&bmi2,0,SRCCOPY);
	}
}

void main(){
	timeBeginPeriod(1);
	heap = HeapAlloc(GetProcessHeap(),8,resX*resY*4+512*512+256*resX*4);
	map = heap+resX*resY*4;
	gui = map+512*512;
	wndclass.hInstance = GetModuleHandle(0);
	RegisterClass(&wndclass);
	window = CreateWindowEx(0,name,name,0x10080000,0,0,resX * 2 + 256,resY * 2 + 39,0,0,wndclass.hInstance,0);
	dc = GetDC(window);
	CreateThread(0,0,lightE,0,0,0);
	for(;;){
		if(PeekMessage(&Msg,window,0,0,0)){
			GetMessage(&Msg,window,0,0);
			TranslateMessage(&Msg);
			DispatchMessageW(&Msg);
		}
		Sleep(10);
	}
}
