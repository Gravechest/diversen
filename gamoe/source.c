#include <windows.h>
#include <intrin.h>
#include <stdio.h>

#include "ivec2.h"
#include "vec2.h"
#include "vec3.h"

#pragma comment(lib,"Winmm.lib")

#define PR_FRICTION 0.95f

#define VK_W 0x57
#define VK_S 0x53
#define VK_A 0x41
#define VK_D 0x44

#define WNDOFFX 500
#define WNDOFFY 250

#define WNDX 512
#define WNDY 512

#define RESX 512/4
#define RESY 512/4

#define MAPX 32
#define MAPY 32

typedef unsigned char      u1;
typedef unsigned short     u2;
typedef unsigned int       u4;
typedef unsigned long long u8;
typedef char               i1;
typedef short              i2;
typedef int			       i4;
typedef long long          i8;
typedef float		       f4;
typedef double             f8;

typedef struct{
	VEC2 vel;
	VEC2 pos;
}PLAYER;

typedef struct{
	u1 r;
	u1 g;
	u1 b;
}RGB;

typedef struct{
	VEC2 pos;
	VEC2 dir;
	VEC2 delta;
	VEC2 side;

	IVEC2 step;
	IVEC2 roundPos;

	i4 hitSide;
}RAY2D;

i4 proc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);

BITMAPINFO bmi ={sizeof(BITMAPINFOHEADER),RESY,RESX,1,24,BI_RGB};

HINSTANCE hinstance;
WNDCLASS wndclass ={.lpfnWndProc = proc,.lpszClassName = "class",.lpszMenuName = "class"};
HWND window;
HDC dc;
MSG Msg;

RGB*  vram;
VEC3* colbuf;

u1* map;
PLAYER player = {.pos = {RESX/2,RESY/2}};

RAY2D ray2dCreate(VEC2 pos,VEC2 dir){
	RAY2D ray;

	ray.pos = pos;
	ray.dir = dir;
	ray.delta = VEC2absR(VEC2divFR(ray.dir,1.0f));

	if(ray.dir.x < 0.0f){
		ray.step.x = -1;
		ray.side.x = (ray.pos.x-(int)ray.pos.x) * ray.delta.x;
	}
	else{
		ray.step.x = 1;
		ray.side.x = ((int)ray.pos.x + 1.0f - ray.pos.x) * ray.delta.x;
	}
	if(ray.dir.y < 0.0f){
		ray.step.y = -1;
		ray.side.y = (ray.pos.y-(int)ray.pos.y) * ray.delta.y;
	}
	else{
		ray.step.y = 1;
		ray.side.y = ((int)ray.pos.y + 1.0f - ray.pos.y) * ray.delta.y;
	}
	ray.roundPos.x = ray.pos.x;
	ray.roundPos.y = ray.pos.y;
	return ray;
}

void ray2dIterate(RAY2D *ray){
	if(ray->side.x < ray->side.y){
		ray->roundPos.x += ray->step.x;
		ray->side.x += ray->delta.x;
		ray->hitSide = 0;
	}
	else{
		ray->roundPos.y += ray->step.y;
		ray->side.y += ray->delta.y;
		ray->hitSide = 1;
	}
}

i4 hash(i4 x) {
	x += (x << 10);
	x ^= (x >> 6);
	x += (x << 3);
	x ^= (x >> 11);
	x += (x << 15);
	return x;
}

f4 rnd() {
	union p {
		f4 f;
		i4 u;
	}r;
	r.u = hash(__rdtsc());
	r.u &= 0x007fffff;
	r.u |= 0x3f800000;
	return r.f;
}

i4 irnd(){
	return hash(__rdtsc());
}

f4 fract(f4 p){
	return p - floorf(p);
}

i4 proc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
	switch(msg){
	}
	return DefWindowProcA(hwnd,msg,wParam,lParam);
}

void physics(){
	for(;;){
		if(GetKeyState(VK_W) & 0x80){
			player.vel.x+=0.02f;
		}
		if(GetKeyState(VK_S) & 0x80){
			player.vel.x-=0.02f;
		}
		if(GetKeyState(VK_D) & 0x80){
			player.vel.y+=0.02f;
		}
		if(GetKeyState(VK_A) & 0x80){
			player.vel.y-=0.02f;
		}
		VEC2addVEC2(&player.pos,player.vel);
		VEC2mul(&player.vel,PR_FRICTION);
		Sleep(15);
	}
}

void render(){
	for(;;){
		for(u4 x = player.pos.x-2;x <= player.pos.x+2;x++){
			for(u4 y = player.pos.y-2;y <= player.pos.y+2;y++){
				vram[x*RESX+y].r = 255;
			}
		}
		for(u4 i = 0;i < 2048;i++){
			RAY2D ray = ray2dCreate(player.pos,(VEC2){rnd()+rnd()-3.0f,rnd()+rnd()-3.0f});
			while(ray.roundPos.x > 0.0f && ray.roundPos.x < RESX && ray.roundPos.y > 0.0f && ray.roundPos.y < RESY){
				if(map[(u4)(ray.roundPos.x/2.0f)*RESX/2+(u4)(ray.roundPos.y/2.0f)]){
					vram[(u4)ray.roundPos.x*RESX+(u4)ray.roundPos.y].r++;
					break;
				}
				ray2dIterate(&ray);
			}
		}
		StretchDIBits(dc,0,0,WNDY,WNDX,0,0,RESY,RESX,vram,&bmi,DIB_RGB_COLORS,SRCCOPY);
		memset(vram,0,sizeof(RGB)*RESX*RESY);
	}
}

void main(){
	timeBeginPeriod(1);
	vram   = HeapAlloc(GetProcessHeap(),8,sizeof(RGB)*RESX*RESY);
	map    = HeapAlloc(GetProcessHeap(),8,MAPX*MAPY);
	wndclass.hInstance = GetModuleHandleA(0);
	RegisterClassA(&wndclass);
	window = CreateWindowExA(0,"class","hello",WS_VISIBLE,WNDOFFX,WNDOFFY,WNDY+15,WNDX+38,0,0,wndclass.hInstance,0);
	dc = GetDC(window);
	for(u4 i = 0;i < MAPX*MAPY;i++){
		if(rnd()<1.1f){
			map[i] = 1;
		}
	}
	CreateThread(0,0,render,0,0,0);
	CreateThread(0,0,physics,0,0,0);
	while(GetMessageA(&Msg,window,0,0)){
		TranslateMessage(&Msg);
		DispatchMessageA(&Msg);
	}
}
