#include <windows.h>
#include <intrin.h>
#include <stdio.h>

#include "ivec2.h"
#include "vec2.h"
#include "vec3.h" 

#define RESX 500
#define RESY 500

#define WNDX 1920
#define WNDY 1080

#define MAPX 50
#define MAPY 50

#define DIVX 10
#define DIVY 10

typedef unsigned char u1;
typedef int			  i4;
typedef unsigned int  u4;
typedef float		  f4;

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

	int hitSide;
}RAY2D;

long proc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);

BITMAPINFO bmi ={sizeof(BITMAPINFOHEADER),RESX,RESY,1,24,BI_RGB};

HINSTANCE hinstance;
WNDCLASS wndclass = {.lpfnWndProc = proc,.lpszClassName = "class",.lpszMenuName = "class"};
HWND window;
HDC dc;
MSG Msg;

RGB  *vram;
VEC3 *colbuf;

u1 *map;

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

long proc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
	return DefWindowProcA(hwnd,msg,wParam,lParam);
}

void genLight(VEC3 col,VEC2 pos,u4 rayC){
	if(map[(u4)pos.x/DIVX*MAPX+(u4)pos.y/DIVY]){
		return;
	}
	for(u4 i = 0;i < rayC;i++){
		VEC2 dir;
		VEC3 color = col;
		for(u4 i=0;i<3;i++)VEC2addVEC2(&dir,(VEC2){ (rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f });
		RAY2D ray = ray2dCreate(pos,VEC2normalize(dir));
		while(ray.roundPos.x>=0&&ray.roundPos.y>=0&&ray.roundPos.x<RESX&&ray.roundPos.y<RESY){
			if(map[ray.roundPos.x/DIVX*MAPX+ray.roundPos.y/DIVY]){
				VEC3addVEC3(&colbuf[ray.roundPos.x*RESX+ray.roundPos.y],color);
				VEC3addVEC3(&colbuf[ray.roundPos.x*RESX+ray.roundPos.y],color);
				VEC3addVEC3(&colbuf[ray.roundPos.x*RESX+ray.roundPos.y],color);
				VEC3mul(&color,0.9f);
				if(color.r < 0.003f && color.g < 0.003f && color.b < 0.003f){
					break;
				}
				switch(ray.hitSide){
				case 0:
					if(ray.dir.x > 0.0f){
						dir = (VEC2){(rnd() - 2.0f),(rnd() - 1.5f) * 2.0f};
						for(u4 j=0;j<3;j++)VEC2addVEC2(&dir,(VEC2){ (rnd()-2.0f),(rnd()-1.5f)*2.0f });
					}
					else{
						dir = (VEC2){(rnd() - 1.0f),(rnd() - 1.5f) * 2.0f};
						for(u4 j=0;j<3;j++)VEC2addVEC2(&dir,(VEC2){ (rnd()-1.0f),(rnd()-1.5f)*2.0f });
					}
					ray = ray2dCreate((VEC2){ ray.roundPos.x,ray.roundPos.y+fract(ray.pos.y + (ray.side.x - ray.delta.x) * ray.dir.y) },dir);
					break;
				case 1:
					if(ray.dir.y > 0.0f){
						dir = (VEC2){(rnd() - 1.5f) * 2.0f,(rnd() - 2.0f)};
						for(u4 j=0;j<3;j++)VEC2addVEC2(&dir,(VEC2){ (rnd()-1.5f)*2.0f,(rnd()-2.0f) });
					}
					else{
						dir = (VEC2){(rnd() - 1.5f) * 2.0f,(rnd() - 1.0f)};
						for(u4 j=0;j<3;j++)VEC2addVEC2(&dir,(VEC2){ (rnd()-1.5f)*2.0f,(rnd()-1.0f) });
					}
					ray = ray2dCreate((VEC2){ ray.roundPos.x+fract(ray.pos.x + (ray.side.y - ray.delta.y) * ray.dir.x),ray.roundPos.y },dir);
					break;
				}
			}
			VEC3addVEC3(&colbuf[ray.roundPos.x*RESX+ray.roundPos.y],color);
			ray2dIterate(&ray);
		}
	}
}

void main(){
	vram   = HeapAlloc(GetProcessHeap(),8,sizeof(RGB)*RESX*RESY);
	colbuf = HeapAlloc(GetProcessHeap(),8,sizeof(VEC3)*RESX*RESY);
	map    = HeapAlloc(GetProcessHeap(),8,MAPX*MAPY);
	wndclass.hInstance = GetModuleHandleA(0);
	RegisterClassA(&wndclass);
	window = CreateWindowExA(0,"class","hello",WS_VISIBLE | WS_POPUP,0,0,WNDX,WNDY,0,0,wndclass.hInstance,0);
	ShowCursor(0);
	dc = GetDC(window);
	for(u4 i = 0;i < MAPX*MAPY;i++){
		if((irnd()&3)==1){
			map[i] = 1;
		}
	}
	for(u4 i = 0;i < 64;i++){
		genLight((VEC3){ (rnd()-1.0f)*0.04f,(rnd()-1.0f)*0.04f, (rnd()-1.0f)*0.04f},(VEC2){ (rnd()-1.0f)*RESX,(rnd()-1.0f)*RESX },irnd()&0xffff);
	}
	for(u4 i = 0;i < RESX*RESY;i++){
		vram[i].r = fminf(colbuf[i].r,255.0f);
		vram[i].g = fminf(colbuf[i].g,255.0f);
		vram[i].b = fminf(colbuf[i].b,255.0f);
	}
	StretchDIBits(dc,0,0,WNDX,WNDY,0,0,RESX,RESY,vram,&bmi,DIB_RGB_COLORS,SRCCOPY);
	for(;;){
		while(GetMessageA(&Msg,window,0,0)>0){
			TranslateMessage(&Msg);
			DispatchMessageW(&Msg);
		}
	}
}
