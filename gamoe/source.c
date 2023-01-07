#include <windows.h>
#include <GL/gl.h>
#include <intrin.h>
#include <stdio.h>
#include <math.h>

#pragma comment(lib,"Winmm.lib")
#pragma comment(lib,"opengl32.lib")

#define GL_FRAGMENT_SHADER 0x8B30
#define GL_VERTEX_SHADER 0x8B31
#define GL_ARRAY_BUFFER 0x8892
#define GL_DYNAMIC_DRAW 0x88E8

#define PR_FRICTION 0.9f

#define VK_W 0x57
#define VK_S 0x53
#define VK_A 0x41
#define VK_D 0x44

#define WNDOFFX 300
#define WNDOFFY 0

#define WNDX 512*2
#define WNDY 512*2

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

u4 (*glCreateProgram)();
u4 (*glCreateShader)(u4 shader);
u4 (*wglSwapIntervalEXT)(u4 status);

void (*glShaderSource)(u4 shader,i4 count,i1** string,i4* length);
void (*glCompileShader)(u4 shader);
void (*glAttachShader)(u4 program,u4 shader);
void (*glLinkProgram)(u4 program);
void (*glUseProgram)(u4 program);
void (*glEnableVertexAttribArray)(u4 index);
void (*glVertexAttribPointer)(u4 index,i4 size,u4 type,u1 normalized,u4 stride,void* pointer);
void (*glBufferData)(u4 target,u4 size,void* data,u4 usage);
void (*glCreateBuffers)(u4 n,u4 *buffers);
void (*glBindBuffer)(u4 target,u4 buffer);
void (*glGetShaderInfoLog)(u4 shader,u4 maxlength,u4 *length,u1 *infolog);
void (*glGenerateMipmap)(u4 target);

typedef struct{
	f4 x;
	f4 y;
}VEC2;

typedef struct{
	u4 x;
	u4 y;
}IVEC2;

typedef struct{
	union{
		f4 r;
		f4 x;
	};
	union{
		f4 g;
		f4 y;
	};
	union{
		f4 b;
		f4 z;
	};
}VEC3;

typedef struct{
	VEC2 p1;
	VEC2 tc1;
	VEC2 p2;
	VEC2 tc2;
	VEC2 p3;
	VEC2 tc3;
	VEC2 p4;
	VEC2 tc4;
	VEC2 p5;
	VEC2 tc5;
	VEC2 p6;
	VEC2 tc6;
}QUAD;

typedef struct{
	u1 r;
	u1 g;
	u1 b;
}RGB;

typedef struct{
	VEC2 vel;
	VEC2 pos;
	RGB* texture;
}PLAYER;

typedef struct{
	VEC2 vel;
	VEC2 pos;
}BULLET;

typedef struct{
	u4 count;
	BULLET* state;
	RGB* texture;
}BULLETHUB;

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

PIXELFORMATDESCRIPTOR pfd = {sizeof(PIXELFORMATDESCRIPTOR), 1,
PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
24,0, 0, 0, 0, 0, 0,0,0,0,
0,0,0,0,32,0,0,PFD_MAIN_PLANE,
0,0,0,0	};

HINSTANCE hinstance;
WNDCLASS wndclass = {.lpfnWndProc = proc,.lpszClassName = "class",.lpszMenuName = "class"};
HWND window;
HDC dc;
MSG Msg;
u4 VBO;
u4 texture;

QUAD quad = {.tc1={0.0f,0.0f},.tc2={0.0f,1.0f},.tc3={1.0f,0.0f},.tc4={1.0f,1.0f},.tc5={0.0f,1.0f},.tc6={1.0f,0.0f}};

VEC3* vramf;
RGB*  vram;

u1* map;
PLAYER player = {.pos = {RESX/2,RESY/2}};

BULLETHUB bullet;

u4 spriteShader,mapShader;

VEC2 VEC2absR(VEC2 p){
	p.x = p.x < 0.0f ? -p.x : p.x;
	p.y = p.y < 0.0f ? -p.y : p.y;
	return p;
}

VEC2 VEC2divFR(VEC2 p,f4 d){
	return (VEC2){d/p.x,d/p.y};
}

VEC2 VEC2subVEC2R(VEC2 p,VEC2 p2){
	return (VEC2){p.x - p2.x,p.y - p2.y};
}

VEC2 VEC2divR(VEC2 p,f4 d){
	return (VEC2){p.x/d,p.y/d};
}

void VEC2addVEC2(VEC2* p,VEC2 p2){
	p->x += p2.x;
	p->y += p2.y;
}

void VEC2mul(VEC2* p,f4 m){
	p->x *= m;
	p->y *= m;
}

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

i4 tmax(i4 p,i4 p2){
	return p > p2 ? p : p2;
}

i4 tmin(i4 p,i4 p2){
	return p < p2 ? p : p2;
}

f4 tmaxf(f4 p,f4 p2){
	return p > p2 ? p : p2;
}

f4 tminf(f4 p,f4 p2){
	return p < p2 ? p : p2;
}

f4 length(VEC2 p){
	return sqrtf(p.x*p.x+p.y*p.y);
}

VEC2 VEC2normalizeR(VEC2 p){
	f4 l = length(p);
	return (VEC2){p.x/l,p.y/l};
}

VEC2 mapCoordsToRenderCoords(VEC2 p){
	return (VEC2){p.x/(MAPX*2.0f)-1.0f,p.y/(MAPY*2.0f)-1.0f};
}

u1* loadFile(u1* name){
	HANDLE h = CreateFileA(name,GENERIC_READ,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
	u4 fsize = GetFileSize(h,0);
	u1* r = HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,fsize+1);
	ReadFile(h,r,fsize+1,0,0);
	CloseHandle(h);
	return r;
}

i4 proc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
	switch(msg){
	case WM_LBUTTONDOWN:{
	    POINT cursor;
		GetCursorPos(&cursor);
		ScreenToClient(window,&cursor);
		VEC2 vel = VEC2subVEC2R((VEC2){cursor.x/8,RESX-cursor.y/8},player.pos);
		bullet.state[bullet.count].pos = player.pos;
		bullet.state[bullet.count++].vel = VEC2normalizeR(vel,300.0f);
		break;
	}
	}
	return DefWindowProcA(hwnd,msg,wParam,lParam);
}

void illuminateMap(VEC3 color,VEC2 pos,u4 ammount){
	for(u4 i = 0;i < ammount;i++){
		RAY2D ray = ray2dCreate(pos,(VEC2){rnd()+rnd()-3.0f,rnd()+rnd()-3.0f});
		while(ray.roundPos.x > 0.0f && ray.roundPos.x < RESX && ray.roundPos.y > 0.0f && ray.roundPos.y < RESY){
			if(map[(u4)(ray.roundPos.y/2.0f)*MAPX/2+(u4)(ray.roundPos.x/2.0f)]){
				vramf[(u4)ray.roundPos.y*RESX+(u4)ray.roundPos.x].r+=color.r;
				vramf[(u4)ray.roundPos.y*RESX+(u4)ray.roundPos.x].g+=color.g;
				vramf[(u4)ray.roundPos.y*RESX+(u4)ray.roundPos.x].b+=color.b;
				break;
			}
			ray2dIterate(&ray);
		}
	}
}

void physics(){
	for(;;){
		u1 key_w = GetKeyState(VK_W) & 0x80;
		u1 key_a = GetKeyState(VK_A) & 0x80;
		u1 key_s = GetKeyState(VK_S) & 0x80;
		u1 key_d = GetKeyState(VK_D) & 0x80;
		if(key_w){
			if(key_d || key_a) player.vel.y+=0.04f * 0.7f;
			else               player.vel.y+=0.04f;
		}
		if(key_s){
			if(key_d || key_a) player.vel.y-=0.04f * 0.7f;
			else               player.vel.y-=0.04f;
		}
		if(key_d){
			if(key_s || key_w) player.vel.x+=0.04f * 0.7f;
			else               player.vel.x+=0.04f;
		}
		if(key_a){
			if(key_s || key_w) player.vel.x-=0.04f * 0.7f;
			else               player.vel.x-=0.04f;
		}
		VEC2addVEC2(&player.pos,player.vel);
		VEC2mul(&player.vel,PR_FRICTION);
		for(u4 i = 0;i < bullet.count;i++){
			VEC2addVEC2(&bullet.state[i].pos,bullet.state[i].vel);
		}
		Sleep(15);
	}
}

void drawSprite(VEC2 pos,VEC2 size,IVEC2 textureSize,RGB* texture){
	quad.p1 = (VEC2){pos.x-size.x,pos.y-size.y};
	quad.p2 = (VEC2){pos.x-size.x,pos.y+size.y};
	quad.p3 = (VEC2){pos.x+size.x,pos.y-size.y};
	quad.p4 = (VEC2){pos.x+size.x,pos.y+size.y};
	quad.p5 = (VEC2){pos.x-size.x,pos.y+size.y};
	quad.p6 = (VEC2){pos.x+size.x,pos.y-size.y};
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,textureSize.x,textureSize.y,0,GL_RGB,GL_UNSIGNED_BYTE,texture);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBufferData(GL_ARRAY_BUFFER,24 * sizeof(float),&quad,GL_DYNAMIC_DRAW);
	glDrawArrays(GL_TRIANGLES,0,24);
}

u4 loadShader(u1* fragment,u1* vertex){
	u1*	fragmentSource = loadFile(fragment);
	u1* vertexSource   = loadFile(vertex);
	u4 shaderProgram   = glCreateProgram();
	u4 vertexShader    = glCreateShader(GL_VERTEX_SHADER);
	u4 fragmentShader  = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader,1,(i1**)&fragmentSource,0);
	glShaderSource(vertexShader,1,(i1**)&vertexSource,0);
	glCompileShader(vertexShader);
	glCompileShader(fragmentShader);
	glAttachShader(shaderProgram,vertexShader);
	glAttachShader(shaderProgram,fragmentShader);
	glLinkProgram(shaderProgram);
	HeapFree(GetProcessHeap(),0,fragmentSource);
	HeapFree(GetProcessHeap(),0,vertexSource);
	return shaderProgram;
}

void render(){
	SetPixelFormat(dc,ChoosePixelFormat(dc,&pfd),&pfd);
	wglMakeCurrent(dc,wglCreateContext(dc));
	glCreateProgram		      = wglGetProcAddress("glCreateProgram");
	glCreateShader		      = wglGetProcAddress("glCreateShader");
	glShaderSource		      = wglGetProcAddress("glShaderSource");
	glCompileShader		      = wglGetProcAddress("glCompileShader");
	glAttachShader		      = wglGetProcAddress("glAttachShader");
	glLinkProgram		      = wglGetProcAddress("glLinkProgram");
	glUseProgram		      = wglGetProcAddress("glUseProgram");
	glEnableVertexAttribArray = wglGetProcAddress("glEnableVertexAttribArray");
	glVertexAttribPointer     = wglGetProcAddress("glVertexAttribPointer");
	glBufferData           	  = wglGetProcAddress("glBufferData");
	glCreateBuffers           = wglGetProcAddress("glCreateBuffers");
	glBindBuffer              = wglGetProcAddress("glBindBuffer");
	glGetShaderInfoLog        = wglGetProcAddress("glGetShaderInfoLog");
	glGenerateMipmap          = wglGetProcAddress("glGenerateMipmap");
	wglSwapIntervalEXT        = wglGetProcAddress("wglSwapIntervalEXT");

	wglSwapIntervalEXT(0);

	spriteShader = loadShader("sprite.fs","vertex.vs");
	mapShader    = loadShader("map.fs","vertex.vs");
	glUseProgram(spriteShader);

	glCreateBuffers(1,&VBO);
	glBindBuffer(GL_ARRAY_BUFFER,VBO);
	
	glGenTextures(1,&texture);
	glBindTexture(GL_TEXTURE_2D,texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,2,GL_FLOAT,0,4 * sizeof(float),(void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1,2,GL_FLOAT,0,4 * sizeof(float),(void*)(2 * sizeof(float)));

	for(;;){
		illuminateMap((VEC3){1.0f,0.1f,0.1f},player.pos,4048);
		for(u4 i = 0;i < bullet.count;i++){
			illuminateMap((VEC3){0.1f,1.0f,0.1f},bullet.state[i].pos,4048);
		}
		for(u4 i = 0;i < RESX*RESY;i++){
			vram[i].r = tminf(vramf[i].r,255.0f);
			vram[i].g = tminf(vramf[i].g,255.0f);
			vram[i].b = tminf(vramf[i].b,255.0f);
		}
		glUseProgram(mapShader);
		drawSprite((VEC2){0.0f,0.0f},(VEC2){1.0f,1.0f},(IVEC2){RESX,RESY},vram);
		glUseProgram(spriteShader);
		drawSprite(mapCoordsToRenderCoords(player.pos),(VEC2){0.02f,0.02f},(IVEC2){16,16},player.texture);
		for(u4 i = 0;i < bullet.count;i++){
			drawSprite(mapCoordsToRenderCoords(bullet.state[i].pos),(VEC2){0.01f,0.01f},(IVEC2){16,16},bullet.texture);
		}
		memset(vramf,0,sizeof(VEC3)*RESX*RESY);
		SwapBuffers(dc);
	}
}

void main(){
	timeBeginPeriod(1);
	vram   = HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(RGB)*RESX*RESY);
	vramf  = HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(VEC3)*RESX*RESY);
	map    = HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,MAPX*MAPY);
	bullet.state   = HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(BULLET)*255);
	bullet.texture = HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(RGB)*16*16);
	player.texture = HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(RGB)*16*16);
	wndclass.hInstance = GetModuleHandleA(0);
	RegisterClassA(&wndclass);
	window = CreateWindowExA(0,"class","hello",WS_VISIBLE,WNDOFFX,WNDOFFY,WNDY+15,WNDX+38,0,0,wndclass.hInstance,0);
	dc = GetDC(window);
	for(u4 i = 0;i < MAPX*MAPY;i++){
		if(rnd()<1.05f) map[i] = 1;
	}
	//generate player texture
	for(u4 x = 0;x < 16;x++){
		for(u4 y = 0;y < 16;y++){
			bullet.texture[x*16+y].g = tmax(255 - length((VEC2){fabsf(7.5f-x),fabsf(7.5f-y)}) * 32.0f,0);
			player.texture[x*16+y].r = 255 - length((VEC2){fabsf(7.5f-x),fabsf(7.5f-y)}) * 16.0f;
		}
	}
	CreateThread(0,0,render,0,0,0);
	CreateThread(0,0,physics,0,0,0);
	while(GetMessageA(&Msg,window,0,0)){
		TranslateMessage(&Msg);
		DispatchMessageA(&Msg);
	}
}
