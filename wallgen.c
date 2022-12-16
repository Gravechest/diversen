#include <windows.h>
#include <intrin.h>
#include <stdio.h>

#include "ivec2.h"
#include "vec2.h"
#include "vec3.h" 

#define RESX 1080/2
#define RESY 1920/2

#define WNDX 1080
#define WNDY 1920

#define MAPX 1080/32
#define MAPY 1920/32

#define DIV 1.0f/16.0f

#define IMGX 1080
#define IMGY 1920

#define LIGHTRAYC 1
#define BOUNCESTRENGTH 0.999f
#define LIGHTSTRENGTH 255.0f
#define LIGHTC 16

typedef unsigned char  u1;
typedef unsigned short u2;
typedef unsigned int   u4;
typedef char           i1;
typedef short          i2;
typedef int			   i4;
typedef float		   f4;

typedef struct{
	VEC2 pos;
	f4 zoom;
}CAM;

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

BITMAPINFO bmi ={sizeof(BITMAPINFOHEADER),RESY,RESX,1,24,BI_RGB};

HINSTANCE hinstance;
WNDCLASS wndclass = {.lpfnWndProc = proc,.lpszClassName = "class",.lpszMenuName = "class"};
HWND window;
HDC dc;
MSG Msg;

RGB  *vram;
VEC3 *colbuf;

CAM cam = {.zoom = 1.0f};

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

void drawScene(){
	for(u4 x = 0;x < RESX;x++){
		for(u4 y = 0;y < RESY;y++){
			vram[x*RESY+y].r = fminf(colbuf[(i4)((x+cam.pos.x)/cam.zoom)*IMGY+(i4)((y+cam.pos.y)/cam.zoom)].r,255.0f);
			vram[x*RESY+y].g = fminf(colbuf[(i4)((x+cam.pos.x)/cam.zoom)*IMGY+(i4)((y+cam.pos.y)/cam.zoom)].g,255.0f);
			vram[x*RESY+y].b = fminf(colbuf[(i4)((x+cam.pos.x)/cam.zoom)*IMGY+(i4)((y+cam.pos.y)/cam.zoom)].b,255.0f);
		}
	}
	StretchDIBits(dc,0,0,WNDY,WNDX,0,0,RESY,RESX,vram,&bmi,DIB_RGB_COLORS,SRCCOPY);
}

long proc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
	static IVEC2 mousepos;
	switch(msg){
	case WM_MOUSEMOVE:
		if(GetKeyState(VK_LBUTTON)&0x80){
			f4 x = (mousepos.x - (lParam>>16)) * WNDX / RESX;
			f4 y = (mousepos.y - (lParam&0xffff)) * WNDY / RESY;
			cam.pos.x -= x;
			cam.pos.y += y;
			if(cam.pos.x < 0 || (cam.pos.x + RESX) / cam.zoom > IMGX)cam.pos.x += x;
			if(cam.pos.y < 0 || (cam.pos.y + RESY) / cam.zoom > IMGY)cam.pos.y -= y;
			drawScene();
		}
		mousepos.x = lParam>>16;
		mousepos.y = lParam&0xffff;
		break;
	case WM_MOUSEWHEEL:
		if((i2)(wParam>>16)==120){
			f4 x = (WNDX-mousepos.x) * cam.zoom / RESX * 160.0f;
			f4 y = mousepos.y * cam.zoom / RESY * 280.0f;
			cam.pos.x += x;
			cam.pos.y += y;
			cam.zoom *= 1.15f;
		}
		else if(cam.zoom >= 1.15){
			cam.zoom /= 1.15f;
			f4 x = (WNDX-mousepos.x) * cam.zoom / RESX * 160.0f;
			f4 y = mousepos.y * cam.zoom / RESY * 280.0f;
			cam.pos.x -= x;
			cam.pos.y -= y;
			if(cam.pos.x < 0)cam.pos.x = 0;
			if(cam.pos.y < 0)cam.pos.y = 0;
			if((cam.pos.x + IMGX) / cam.zoom > IMGX)cam.pos.x = IMGX - IMGX/cam.zoom;
			if((cam.pos.y + IMGY) / cam.zoom > IMGY)cam.pos.y = IMGY - IMGY/cam.zoom;
		}
		drawScene();
		break;
	}
	return DefWindowProcA(hwnd,msg,wParam,lParam);
}

void genLight(VEC3 col,VEC2 pos,u4 rayC){
	if(map[(u4)(pos.x/DIV)*MAPY+(u4)(pos.y/DIV)]) return;
	for(u4 i = 0;i < rayC;i++){
		VEC2 dir = {(rnd()+rnd()-3.0f)*2.0f,(rnd()+rnd()-3.0f)*2.0f};
		VEC3 color = col;
		RAY2D ray = ray2dCreate(pos,VEC2normalize(dir));
		while(ray.roundPos.x>=0&&ray.roundPos.y>=0&&ray.roundPos.x<IMGX&&ray.roundPos.y<IMGY){
			if(map[(u4)(ray.roundPos.x/DIV)*MAPY+(u4)(ray.roundPos.y/DIV)]){
				VEC3addVEC3(&colbuf[ray.roundPos.x*IMGY+ray.roundPos.y],VEC3mulR(color,4.0f));
				VEC3mul(&color,BOUNCESTRENGTH);
				if(color.r < 0.0003f && color.g < 0.0003f && color.b < 0.0003f) break;
				switch(ray.hitSide){
				case 0:
					if(ray.dir.x > 0.0f){
						dir = (VEC2){(rnd() + rnd() - 4.0f),(rnd() + rnd() - 3.0f) * 4.0f};
						ray = ray2dCreate((VEC2){ ray.roundPos.x,ray.roundPos.y+fract(ray.pos.y + (ray.side.x - ray.delta.x) * ray.dir.y) },dir);
					}
					else{
						dir = (VEC2){(rnd() + rnd() - 2.0f),(rnd() + rnd() - 3.0f) * 4.0f};
						ray = ray2dCreate((VEC2){ ray.roundPos.x+1.0f,ray.roundPos.y+fract(ray.pos.y + (ray.side.x - ray.delta.x) * ray.dir.y) },dir);
					}
					break;
				case 1:
					if(ray.dir.y > 0.0f){
						dir = (VEC2){(rnd() + rnd() - 3.0f) * 4.0f,(rnd() + rnd() - 4.0f)};
						ray = ray2dCreate((VEC2){ ray.roundPos.x+fract(ray.pos.x + (ray.side.y - ray.delta.y) * ray.dir.x),ray.roundPos.y },dir);
					}
					else{
						dir = (VEC2){(rnd() + rnd() - 3.0f) * 4.0f,(rnd() + rnd() - 2.0f)};
						ray = ray2dCreate((VEC2){ ray.roundPos.x+fract(ray.pos.x + (ray.side.y - ray.delta.y) * ray.dir.x),ray.roundPos.y+1.0f},dir);
					}
					break;
				}
			}
			VEC3addVEC3(&colbuf[ray.roundPos.x*IMGY+ray.roundPos.y],color);
			ray2dIterate(&ray);
		}
	}
}

void generateLighting(){
	for(u4 i = 0;i < LIGHTC;i++){
		genLight((VEC3){ (rnd()-1.0f)*LIGHTSTRENGTH,(rnd()-1.0f)*LIGHTSTRENGTH,(rnd()-1.0f)*LIGHTSTRENGTH},(VEC2){ (rnd()-1.0f)*IMGX,(rnd()-1.0f)*IMGY },LIGHTRAYC);
		drawScene();
	}
}

void main(){
	vram   = HeapAlloc(GetProcessHeap(),8,sizeof(RGB)*RESX*RESY);
	colbuf = HeapAlloc(GetProcessHeap(),8,sizeof(VEC3)*IMGX*IMGY);
	map    = HeapAlloc(GetProcessHeap(),8,MAPX*MAPY);
	wndclass.hInstance = GetModuleHandleA(0);
	RegisterClassA(&wndclass);
	window = CreateWindowExA(0,"class","hello",WS_VISIBLE | WS_POPUP,0,0,WNDY,WNDX,0,0,wndclass.hInstance,0);
	dc = GetDC(window);
	for(u4 i = 16;i >= 1;i /= 2){
		for(u4 j = 0;j < MAPX*MAPY/i/i;j++){
			if((irnd()%12)==1){
				for(u4 k = j/(MAPX/i)*i;k < i+j/(MAPX/i)*i;k++){
					for(u4 l = j%(MAPY/i)*i;l < i+j%(MAPY/i)*i;l++){
						map[k*MAPX+l] = 1;
					}
				}
			}
		}
	}
	CreateThread(0,0,generateLighting,0,0,0);
	while(GetMessageA(&Msg,window,0,0)>0){
		TranslateMessage(&Msg);	
		DispatchMessageA(&Msg);
	}
}
