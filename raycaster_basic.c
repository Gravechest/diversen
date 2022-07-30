#include <windows.h>
#include <GL/gl.h>
#include <stdio.h> 
#include <math.h>
#include <intrin.h>

#include "ivec2.h"
#include "vec2.h"
#include "ray.h"

#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"winmm.lib")

#define RESOX 800
#define RESOY 1200

#define MAPSZ  10
#define MAPRAM MAPSZ*MAPSZ

#define PI_2   6.28318530718f
#define PI     3.14159265358f
#define PI_05  1.57079632679f
#define PI_025 0.78539816339f

typedef char               i8;
typedef short              i16;
typedef int                i32;
typedef long long          i64;

typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned int       u32;
typedef unsigned long long u64;

typedef float              f32;
typedef double             f64;

typedef struct{
	unsigned char r;
	unsigned char g;
	unsigned char b;
}RGB;

typedef struct{
	VEC2 pos;
	VEC2 dir;
	VEC2 plane;
}PLAYER;

PLAYER player;
const char name[] = "window";
HWND window;
HDC dc;
MSG Msg;

RGB *vram;
u8  *map;

inline i32 hash(i32 x) {
	x += (x << 10);
	x ^= (x >> 6);
	x += (x << 3);
	x ^= (x >> 11);
	x += (x << 15);
	return x;
}

inline f32 rnd() {
	union p {
		float f;
		i32 u;
	}r;
	r.u = hash(__rdtsc());
	r.u &= 0x007fffff;
	r.u |= 0x3f800000;
	return r.f;
}

inline i32 irnd(){
	return hash(__rdtsc());
}

inline f32 fract(f32 p){
	return p - (u32)p;
}

void rotVEC2(VEC2 *p,float rot){
    float t = cosf(rot) * p->x - sinf(rot) * p->y;
    p->y = sinf(rot) * p->x + cosf(rot) * p->y;
	p->x = t;
}

VEC2 rotVEC2R(VEC2 p,float rot){
    VEC2 r;
    r.x = cosf(rot) * p.x - sinf(rot) * p.y;
    r.y = sinf(rot) * p.x + cosf(rot) * p.y;
    return r;
}

PIXELFORMATDESCRIPTOR pfd = {sizeof(PIXELFORMATDESCRIPTOR), 1,
	PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,PFD_TYPE_RGBA,
	24,0, 0, 0, 0, 0, 0,0,0,0,
	0,0,0,0,32,0,0,PFD_MAIN_PLANE,
	0,0,0,0	};

long _stdcall proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	switch(msg){
	case WM_KEYDOWN:
		switch(wParam){
		break;
		}
	case WM_MOUSEMOVE:{
		POINT p;
		GetCursorPos(&p);
		rotVEC2(&player.dir,-(float)(500-p.x)/255.0f);
		player.plane = rotVEC2R(player.dir,PI_05);
		SetCursorPos(500,500);
		break;
		}
	}
	return DefWindowProcA(hwnd,msg,wParam,lParam);
}

WNDCLASS wndclass = {sizeof(WNDCLASS),proc,0,0,0,0,0,0,name,name};

void physics(){
	for(;;){
		if(GetKeyState(0x57)&0x80){
			VEC2addVEC2(&player.pos,VEC2divR(player.dir,80.0f));
		}
		if(GetKeyState(0x53)&0x80){
			VEC2subVEC2(&player.pos,VEC2divR(player.dir,80.0f));
		}
		if(GetKeyState(0x44)&0x80){
			VEC2 ang = rotVEC2R(player.dir,PI_05);
			VEC2addVEC2(&player.pos,VEC2divR(ang,80.0f));
		}
		if(GetKeyState(0x41)&0x80){
			VEC2 ang = rotVEC2R(player.dir,PI_05);
			VEC2subVEC2(&player.pos,VEC2divR(ang,80.0f));
		}
		Sleep(15);
	}
}

void render(){
	SetPixelFormat(dc, ChoosePixelFormat(dc, &pfd), &pfd);
	wglMakeCurrent(dc, wglCreateContext(dc));
	for(u32 i = 0;i < MAPRAM;i++){
		map[i] = irnd();
	}
	for(;;){
		for(u32 i = 0;i < RESOY;i++){
			float cam = 2.0f * (float)i / RESOY - 1.0f;
 			RAY2D ray = ray2dCreate(player.pos,rotVEC2R(player.dir,cam));
			while(ray.ix>=0&&ray.ix<=MAPSZ&&ray.iy>=0&&ray.iy<=MAPSZ){
				if(map[ray.ix*MAPSZ+ray.iy]>200){
					float wall = 0.0f;
					float dst = 0.0f;
					switch(ray.sid){
					case 0:
						wall = ray.pos.y + (ray.side.x - ray.delta.x) * ray.dir.y;
						if(ray.dir.x>0.0f){
							dst = VEC2dist(player.pos,(VEC2){ray.ix,ray.iy+fract(wall)});
						}
						else{
							dst = VEC2dist(player.pos,(VEC2){ray.ix+1.0f,ray.iy+fract(wall)});
						}
						break;
					case 1:
						wall = ray.pos.x + (ray.side.y - ray.delta.y) * ray.dir.x;
						if(ray.dir.y>0.0f){
							dst = VEC2dist(player.pos,(VEC2){ray.ix+fract(wall),ray.iy});
						}
						else{
							dst = VEC2dist(player.pos,(VEC2){ray.ix+fract(wall),ray.iy+1.0f});
						}
						break;
					}
					if(dst){
						dst = RESOX/dst/4;
						if(dst > RESOX){
							dst = RESOX;
						}
						for(u32 i2 = 0;i2 < dst;i2++){
							switch(ray.sid){
							case 0:
								vram[i2*RESOY+i+(RESOY*RESOX/2-(u32)(dst/2.0f)*RESOY)].r = 128.0f;
								break;
							case 1:
								vram[i2*RESOY+i+(RESOY*RESOX/2-(u32)(dst/2.0f)*RESOY)].g = 128.0f;
								break;
							}
						}
					}
					break;
				}
				ray2dItterate(&ray);
			}
		}
		glDrawPixels(RESOY,RESOX,GL_RGB,GL_UNSIGNED_BYTE,vram);
		SwapBuffers(dc); 
		ZeroMemory(vram,sizeof(RGB)*RESOX*RESOY);
	}
}

void main(){
	player.dir.x = 1.0f;
	timeBeginPeriod(1);
	vram = HeapAlloc(GetProcessHeap(),8,sizeof(RGB)*RESOX*RESOY);
	map  = HeapAlloc(GetProcessHeap(),8,MAPRAM);
	wndclass.hInstance = GetModuleHandleA(0);
	RegisterClassA(&wndclass);
	window = CreateWindowExA(0,name,name,0x10080000,100,100,RESOY,RESOX,0,0,wndclass.hInstance,0);
	ShowCursor(0);
	dc = GetDC(window);
	CreateThread(0,0,render,0,0,0);
	CreateThread(0,0,physics,0,0,0);
	for(;;){
		while (PeekMessageA(&Msg, window, 0, 0, 0)) {
			GetMessageA(&Msg, window, 0, 0);
			TranslateMessage(&Msg);
			DispatchMessageW(&Msg);
		}
		Sleep(15);
	}
}
