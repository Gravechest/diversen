#include <windows.h>
#include <stdio.h>
#include <immintrin.h>
#include <intrin.h>
#include <time.h>

#define resX 512
#define resY 512

#define ampInt(x) (x<<7)
#define lightprop(r,g,b) (((r&15)<<1)+((g&15)<<5)+((b&15)<<9)+1)
#define blockprop(t,r,g,b,x,y) (((t&1)<<1)+((r&3)<<2)+((g&3)<<4)+((b&3)<<6)+((x&63)<<8)+((y&63)<<14))

#define Bound 0x3ffff

unsigned char *heap;
unsigned char *map;
unsigned char *gui;

unsigned char lightentC;
unsigned char blockentC;

typedef struct ENTITY{
	unsigned short x;
	unsigned short y;
	unsigned int prop;
	short vx;
	short vy;
}ENTITY;

ENTITY *entities;

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
	return val < 0 ? -val : val;
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
			if(val & 2){
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
				fill(((rx & 0x3fe00) + (ry >> 9)) << 2,bt << (val >> 2 & 3),gt << (val >> 4 & 3),rt << (val >> 6 & 3));
				rt >>= 4 - (val >> 6 & 3);
				gt >>= 4 - (val >> 4 & 3);
				bt >>= 4 - (val >> 2 & 3);
				if(rt + gt + bt == 0){
					break;
				}
			}
			else if(val & 1){
				break;
			}
			else{
				fill(((rx & 0x3fe00) + (ry >> 9)) << 2,bt << (val >> 2 & 3),gt << (val >> 4 & 3),rt << (val >> 6 & 3));
				rt >>= 1 - (val >> 6 & 3);
				gt >>= 1 - (val >> 4 & 3);
				bt >>= 1 - (val >> 2 & 3);
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

void spawnLight(short x,short y,short vx,short vy,int prop){
	entities[lightentC+blockentC].prop = prop;
	entities[lightentC+blockentC].x    = x;
	entities[lightentC+blockentC].y    = y;
	entities[lightentC+blockentC].vx   = vx;
	entities[lightentC+blockentC].vy   = vy;
	lightentC++;
}

void deleteLight(char id){
	for(int i = lightentC+blockentC;i > id+blockentC;i--){
		entities[i] = entities[i+1];
	}
	lightentC--;
}

void spawnBlock(short x,short y,short vx,short vy,int prop){
	for(int i = blockentC+lightentC;i > blockentC;i--){
		entities[i] = entities[i+1];
	}
	entities[blockentC].prop = prop;
	entities[blockentC].x    = x;
	entities[blockentC].y    = y;
	entities[blockentC].vx   = vx;
	entities[blockentC].vy   = vy;
	blockentC++;
}

void deleteBlock(char id){
	for(int i = lightentC+blockentC;i > id;i--){
		entities[i] = entities[i+1];
	}
	blockentC--;
}

void lightE(){
	spawnLight(ampInt(256),ampInt(400),0,250,lightprop(2,2,5));
	spawnBlock(ampInt(252),ampInt(495),0,0,blockprop(1,3,3,3,63,16));
	spawnBlock(ampInt(252),ampInt(1),0,0,blockprop(1,3,3,3,63,16));
	for(int i = 0;i < 512;i++){
		map[i] = 1;
	}
	for(int i = 261632;i < 262144;i++){
		map[i] = 1;
	}
	for(int i = 512;i < 261632;i+=512){
		map[i] = 1;
	}
	for(int i = 511;i < 261632;i+=512){
		map[i] = 1;
	}
	for(int i = 0;i < resX*512;i+=512){
		gui[i] = 80;
		gui[i+1] = 80;
		gui[i+2] = 80;
	}
	for(;;){
		t++;
		if(GetKeyState(VK_UP) < 0){
			entities[0].vx = 240;
		}
		else if(GetKeyState(VK_DOWN) < 0){
			entities[0].vx = -240;
		}
		else{
			entities[0].vx = 0;
		}
		if(GetKeyState(0x57) < 0){
			entities[1].vx = 240;
		}
		else if(GetKeyState(0x53) < 0){
			entities[1].vx = -240;
		}
		else{
			entities[1].vx = 0;
		}
		for(int i = 0;i < blockentC;i++){
			for(int i2 = entities[i].x<<2&0xffe00;i2 < (entities[i].x<<2&0xffe00) + ((entities[i].prop >> 8 & 63) << 9);i2+=512){
				for(int i3 = entities[i].y>>7;i3 < (entities[i].y>>7) + (entities[i].prop >> 14 & 63);i3++){
					map[i2+i3] = 0;
				}
			}
			entities[i].x += entities[i].vx;
			entities[i].y += entities[i].vy;
			for(int i2 = entities[i].x<<2&0xffe00;i2 < (entities[i].x<<2&0xffe00) + ((entities[i].prop >> 8 & 63) << 9);i2+=512){
				for(int i3 = entities[i].y>>7;i3 < (entities[i].y>>7) + (entities[i].prop >> 14 & 63);i3++){
					map[i2+i3] = entities[i].prop & 63;
				} 
			}
		}
		for(int i = blockentC;i < lightentC+blockentC;i++){
			entities[i].x += entities[i].vx;
			entities[i].y += entities[i].vy;
			if(map[(entities[i].x << 2&0xffe00) + (entities[i].y >> 7)]){
				for(int i2 = 0;i2 < blockentC;i2++){
					if(entities[i].x > entities[i2].x && entities[i].y > entities[i2].y 
					&& entities[i].x < entities[i2].x + ((entities[i2].prop >> 8 & 63) << 7)
					&& entities[i].y < entities[i2].y + ((entities[i2].prop >> 14) << 7)){
						entities[i].vy = -entities[i].vy + 23;
						entities[i].vx += (entities[i].x >> 6) - (entities[i2].x >> 6) - (entities[i2].prop >> 8 & 63);
						entities[i].x += entities[i].vx;
						entities[i].y += entities[i].vy;
						goto end;
					}
				}
				if(!entities[i].y & ampInt(511) || !~entities[i].y & ampInt(511)){
					exit(0);
				}
				else{
					entities[i].vx = -entities[i].vx;
					entities[i].x += entities[i].vx;
					entities[i].y += entities[i].vy;
				}
			}
		end:
			light(entities[i].x >> 7,entities[i].y >> 7,entities[i].prop >> 1 & 15,entities[i].prop >> 5 & 15,entities[i].prop >> 9 & 15);
		}
		StretchDIBits(dc,0,0,resX * 2,resY * 2,0,0,resX,resY,heap,&bmi,0,SRCCOPY);
		memset(heap,0,resX*resY*4);
		StretchDIBits(dc,resX * 2,0,resX * 2 + 128,resY*2,0,0,resX,512,gui,&bmi2,0,SRCCOPY);
		Sleep(10);
	}
}

void main(){
	timeBeginPeriod(1);
	heap = HeapAlloc(GetProcessHeap(),8,resX*resY*4 + 512*512 + 256*resX*4 + 256*10);
	map = heap+resX*resY*4;
	gui = map+512*512;
	entities = (ENTITY*)(gui+256*resX*4);
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
