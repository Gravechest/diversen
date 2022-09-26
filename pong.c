#include <windows.h>
#include <GL/gl.h>
#include <stdio.h>

#include "ivec2.h"
#include "vec2.h"
#include "vec3.h"

#define winName "pong"

#define GL_FRAGMENT_SHADER 0x8B30
#define GL_VERTEX_SHADER 0x8B31
#define GL_ARRAY_BUFFER 0x8892
#define GL_DYNAMIC_DRAW 0x88E8

#define GL_RGBA32F 0x8814

#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"opengl32.lib")

unsigned int VBO;

unsigned int shaderprogram;
unsigned int vertexshader;
unsigned int fragmentshader;

unsigned int (*glCreateProgram)();
unsigned int (*glCreateShader)(unsigned int shader);
unsigned int (*wglSwapIntervalEXT)(unsigned int satus);

int (*glGetUniformLocation)(unsigned int program,const char *name);

void (*glShaderSource)(unsigned int shader,int count,const char **string,int *length);
void (*glCompileShader)(unsigned int shader);
void (*glAttachShader)(unsigned int program,unsigned int shader);
void (*glLinkProgram)(unsigned int program);
void (*glUseProgram)(unsigned int program);
void (*glEnableVertexAttribArray)(unsigned int index);
void (*glVertexAttribPointer)(unsigned int index,int size,unsigned int type,unsigned char normalized,unsigned int stride,const void *pointer);
void (*glBufferData)(unsigned int target,unsigned int size,const void *data,unsigned int usage);
void (*glCreateBuffers)(unsigned int n,unsigned int *buffers);
void (*glBindBuffer)(unsigned int target,unsigned int buffer);
void (*glGetShaderInfoLog)(unsigned int shader,unsigned int maxlength,unsigned int *length,unsigned char *infolog);
void (*glGenerateMipmap)(unsigned int target);
void (*glUniform2i)(int loc,int v1,int v2);
void (*glUniform1i)(int loc,int v1);

PIXELFORMATDESCRIPTOR pfd = {sizeof(PIXELFORMATDESCRIPTOR), 1,
PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
24,0, 0, 0, 0, 0, 0,0,0,0,
0,0,0,0,32,0,0,PFD_MAIN_PLANE,
0,0,0,0	};

char *VERTsource;
char *FRAGsource;

unsigned int entityText;

typedef struct{
	unsigned char r;
	unsigned char g;
	unsigned char b;
}RGB;

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

unsigned int quadC;
QUAD *quad;

IVEC2 reso  = {400,800};

HWND window;
HDC dc;
MSG Msg;

typedef struct{
	VEC3 pos;
	float radius;
	VEC3 col;
	float id;
	VEC3 reserved2;
	float reserve3;
}SPHEREGPU;

typedef struct{
	VEC3 pos;
	float reserve1;
	VEC3 size;
	float id;
	VEC3 col;
	float reserve3;
}BOXGPU;

typedef struct{
	union{
		SPHEREGPU sphere;
		BOXGPU box;
	};
}GPUENTITYDATA;

typedef struct{
	VEC3 vel;
	unsigned char id;
}CPUENTITYDATA;

typedef struct{
	GPUENTITYDATA *GPU;
	CPUENTITYDATA *CPU;
}ENTITY;

unsigned char glMes[16];
unsigned char glMesC;

ENTITY entity;

unsigned char entityC;

unsigned char scorep1,scorep2;

long _stdcall proc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam) {
	return DefWindowProcA(hwnd,msg,wParam,lParam);
}	

WNDCLASS wndclass = {0,proc,0,0,0,0,0,0,winName,winName};

inline int hash(int x) {
	x += (x << 10);
	x ^= (x >> 6);
	x += (x << 3);
	x ^= (x >> 11);
	x += (x << 15);
	return x;
}	

inline float rnd(){
	union p{
		float f;
		int u;
	}r;
	r.u = hash(__rdtsc());
	r.u &= 0x007fffff;
	r.u |= 0x3f800000;
	return r.f;
}

float PointBoxDistance(VEC2 spherePos,VEC2 boxPos,VEC2 boxSZ){ 
	VEC2 p = VEC2subVEC2R(spherePos,boxPos);
	VEC2 q = VEC2subVEC2R(VEC2absR(p),boxSZ);
	return VEC2length(VEC2maxR(q,0.0f)) + fminf(fmaxf(q.x,q.y),0.0f);
}

VEC2 PointBoxNormal(VEC2 spherePos,VEC2 boxPos,VEC2 boxSz){
	VEC2 p = VEC2subVEC2R(spherePos,boxPos);
	if(p.x > 0.0f){
		p.x -= boxSz.x;
	}
	else{
		p.x = fabsf(p.x);
	}
	if(p.y > 0.0f){
		p.y -= boxSz.y;
	}
	else{
		p.y = fabsf(p.y);
	}
	return VEC2maxR(p,0.0f);
}

char *loadFile(const char *name){
	char *r;
	HANDLE h = CreateFileA(name,GENERIC_READ,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
	int fsize = GetFileSize(h,0);
	r = HeapAlloc(GetProcessHeap(),8,fsize+1);
	ReadFile(h,r,fsize+1,0,0);
	CloseHandle(h);
	return r;
}

void initBall(int ball){
	entity.GPU[ball].sphere.radius = rnd()/2.0f;
	do{
		entity.CPU[ball].vel.x = (rnd()-1.5f)/50.0f;
		entity.CPU[ball].vel.y = (rnd()-1.5f)/50.0f;
	}
	while(entity.CPU[ball].vel.x < 0.0075f);
}

void createEntity(unsigned int type,VEC3 pos){
	entity.CPU[entityC].id = type;
	switch(type){
	case 0:
		entity.GPU[entityC].sphere.pos = pos;
		entity.GPU[entityC].sphere.id = 0;
		initBall(entityC);
		break;
	case 1:
	case 2:
		entity.GPU[entityC].box.pos = pos;
		entity.GPU[entityC].box.id = 1;
		entity.GPU[entityC].box.size = (VEC3){0.8f,0.2f,0.2f};
		entity.GPU[entityC].box.col = (VEC3){rnd()-1.0f,rnd()-1.0f,rnd()-1.0f};
		break;
	case 3:
		entity.GPU[entityC].box.pos = pos;
		entity.GPU[entityC].box.id = 1;
		entity.GPU[entityC].box.size = (VEC3){0.1f,0.1f,0.1f};
		entity.GPU[entityC].box.col = (VEC3){rnd()-1.0f,rnd()-1.0f,rnd()-1.0f};
		break;
	}
	entityC++;
	glMes[glMesC] = 0;
	glMesC++;
}

void physics(){
	createEntity(0,(VEC3){0.0f,0.0f,18.0f});
	createEntity(1,(VEC3){0.0f,-15.0f,18.0f});
	createEntity(2,(VEC3){0.0f,15.0f,18.0f});
	for(;;){
		for(int i = 0;i < entityC;i++){
			switch(entity.CPU[i].id){
			case 1:
				if(GetKeyState(0x53)&0x80){
					entity.CPU[i].vel.x += 0.0007f;
				}
				if(GetKeyState(0x57)&0x80){
					entity.CPU[i].vel.x -= 0.0007f;
				}
				if(GetKeyState(0x44)&0x80){
					entity.CPU[i].vel.y += 0.0007f;
				}
				if(GetKeyState(0x41)&0x80){
					entity.CPU[i].vel.y -= 0.0007f;
				}
				break;
			case 2:
				if(GetKeyState(VK_DOWN)&0x80){
					entity.CPU[i].vel.x += 0.0007f;
				}
				if(GetKeyState(VK_UP)&0x80){
					entity.CPU[i].vel.x -= 0.0007f;
				}
				if(GetKeyState(VK_RIGHT)&0x80){
					entity.CPU[i].vel.y += 0.0007f;
				}
				if(GetKeyState(VK_LEFT)&0x80){
					entity.CPU[i].vel.y -= 0.0007f;
				}
				break;
			}
		}
		for(int i = 0;i < entityC;i++){
			switch(entity.CPU[i].id){
			case 0:
				VEC3addVEC3(&entity.GPU[i].sphere.pos,entity.CPU[i].vel);
				if(entity.GPU[i].sphere.pos.x < -7.5f - entity.GPU[i].sphere.radius || entity.GPU[i].sphere.pos.x > 7.5f + entity.GPU[i].sphere.radius){
					entity.GPU[i].sphere.pos.x -= entity.CPU[i].vel.x;
					entity.CPU[i].vel.x = -entity.CPU[i].vel.x * 0.8f;
					entity.CPU[i].vel.y *= 1.2f;
				}
				if(entity.GPU[i].sphere.pos.y < -18.0f){
					createEntity(3,(VEC3){-4.0f,-1.0f-scorep1,6.0f});
					scorep1++;
					entity.GPU[i].sphere.pos = (VEC3){0.0f,0.0f,18.0f};
					initBall(i);
				}
				if(entity.GPU[i].sphere.pos.y > 18.0f){
					createEntity(3,(VEC3){-4.0f,1.0f+scorep2,6.0f});
					scorep2++;
					entity.GPU[i].sphere.pos = (VEC3){0.0f,0.0f,18.0f};
					initBall(i);
				}
				for(int i2 = 0;i2 < entityC;i2++){
					switch(entity.CPU[i2].id){
					case 1:
					case 2:{
						float coli = PointBoxDistance((VEC2){entity.GPU[i].sphere.pos.x,entity.GPU[i].sphere.pos.y},(VEC2){entity.GPU[i2].box.pos.x,entity.GPU[i2].box.pos.y},(VEC2){entity.GPU[i2].box.size.x,entity.GPU[i2].box.size.y});
						if(coli<entity.GPU[i].sphere.radius){
							VEC2 colNorm = PointBoxNormal((VEC2){entity.GPU[i].sphere.pos.x,entity.GPU[i].sphere.pos.y},(VEC2){entity.GPU[i2].box.pos.x,entity.GPU[i2].box.pos.y},(VEC2){entity.GPU[i2].box.size.x,entity.GPU[i2].box.size.y});
							VEC3subVEC3(&entity.GPU[i].sphere.pos,entity.CPU[i].vel);
							entity.CPU[i].vel.x = -entity.CPU[i].vel.x * colNorm.x / entity.GPU[i].sphere.radius + entity.CPU[i2].vel.x / 1.5f;
							entity.CPU[i].vel.y = -entity.CPU[i].vel.y * colNorm.y / entity.GPU[i].sphere.radius + entity.CPU[i2].vel.y / 1.5f;
							VEC3subVEC3(&entity.GPU[i2].box.pos,entity.CPU[i2].vel);
							VEC3subVEC3(&entity.GPU[i2].box.pos,entity.CPU[i2].vel);
						}
						}
						break;
					}
				}
				break;
			case 1:
				VEC3addVEC3(&entity.GPU[i].box.pos,entity.CPU[i].vel);
				if(entity.GPU[i].sphere.pos.x < -8.0f || entity.GPU[i].sphere.pos.x > 8.0f){
					entity.GPU[i].sphere.pos.x -= entity.CPU[i].vel.x;
					entity.CPU[i].vel.x = 0.0f;
				}
				VEC3mul(&entity.CPU[i].vel,0.97f);
				break;
			case 2:
				VEC3addVEC3(&entity.GPU[i].box.pos,entity.CPU[i].vel);
				if(entity.GPU[i].sphere.pos.x < -8.0f || entity.GPU[i].sphere.pos.x > 8.0f){
					entity.GPU[i].sphere.pos.x -= entity.CPU[i].vel.x;
					entity.CPU[i].vel.x = 0.0f;
				}
				VEC3mul(&entity.CPU[i].vel,0.97f);
				break;
			}
		}
		Sleep(1);
	}
}

void render(){
	FRAGsource = loadFile("shaders/fragment.frag");
	VERTsource = loadFile("shaders/vertex.vert");

	SetPixelFormat(dc, ChoosePixelFormat(dc, &pfd), &pfd);
	wglMakeCurrent(dc, wglCreateContext(dc));

	glCreateProgram			  = wglGetProcAddress("glCreateProgram");
	glCreateShader			  = wglGetProcAddress("glCreateShader");
	glShaderSource			  = wglGetProcAddress("glShaderSource");
	glCompileShader			  = wglGetProcAddress("glCompileShader");
	glAttachShader			  = wglGetProcAddress("glAttachShader");
	glLinkProgram			  = wglGetProcAddress("glLinkProgram");
	glUseProgram			  = wglGetProcAddress("glUseProgram");
	glEnableVertexAttribArray = wglGetProcAddress("glEnableVertexAttribArray");
	glVertexAttribPointer     = wglGetProcAddress("glVertexAttribPointer");
	glBufferData           	  = wglGetProcAddress("glBufferData");
	glCreateBuffers           = wglGetProcAddress("glCreateBuffers");
	glBindBuffer              = wglGetProcAddress("glBindBuffer");
	glGetShaderInfoLog        = wglGetProcAddress("glGetShaderInfoLog");
	glGetUniformLocation      = wglGetProcAddress("glGetUniformLocation");
	glGenerateMipmap          = wglGetProcAddress("glGenerateMipmap");
	glUniform1i               = wglGetProcAddress("glUniform1i");
	glUniform2i				  = wglGetProcAddress("glUniform2i");

	wglSwapIntervalEXT        = wglGetProcAddress("wglSwapIntervalEXT");

	wglSwapIntervalEXT(1);

	shaderprogram   = glCreateProgram();
	vertexshader    = glCreateShader(GL_VERTEX_SHADER);
	fragmentshader  = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vertexshader,1,(const char**)&VERTsource,0);
	glShaderSource(fragmentshader,1,(const char**)&FRAGsource,0);
	glCompileShader(vertexshader);
	glCompileShader(fragmentshader);
	glAttachShader(shaderprogram,vertexshader);
	glAttachShader(shaderprogram,fragmentshader);
	glLinkProgram(shaderprogram);
	glUseProgram(shaderprogram);

	glGenTextures(1,&entityText);

	glBindTexture(GL_TEXTURE_1D,entityText);

	glCreateBuffers(1,&VBO);
	glBindBuffer(GL_ARRAY_BUFFER,VBO);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,2,GL_FLOAT,0,4 * sizeof(float),(void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1,2,GL_FLOAT,0,4 * sizeof(float),(void*)(2 * sizeof(float)));

	glUniform2i(glGetUniformLocation(shaderprogram,"reso"),reso.x,reso.y);
	
	for(;;){
		while(glMesC > 0){
			glMesC--;
			switch(glMes[glMesC]){
			case 0:
				glUniform1i(glGetUniformLocation(shaderprogram,"entityC"),entityC);
				break;
			}
		}
		glTexImage1D(GL_TEXTURE_1D,0,GL_RGBA32F,entityC*sizeof(SPHEREGPU),0,GL_RGBA,GL_FLOAT,entity.GPU);
		glGenerateMipmap(GL_TEXTURE_1D);
		glClear(GL_COLOR_BUFFER_BIT);
		glBufferData(GL_ARRAY_BUFFER,24 * sizeof(float)*quadC,quad,GL_DYNAMIC_DRAW);
		glDrawArrays(GL_TRIANGLES,0,24*quadC);
		SwapBuffers(dc);
	}
}

void main(){
	quad = HeapAlloc(GetProcessHeap(),8,sizeof(QUAD)*256);

	reso.y = GetSystemMetrics(SM_CXSCREEN);
	reso.x = GetSystemMetrics(SM_CYSCREEN);

	entity.CPU = HeapAlloc(GetProcessHeap(),8,sizeof(CPUENTITYDATA)*16);
	entity.GPU = HeapAlloc(GetProcessHeap(),8,sizeof(GPUENTITYDATA)*16);

	quad[0].p1 = (VEC2){-1.0f,-1.0f};
	quad[0].p2 = (VEC2){-1.0f, 1.0f};
	quad[0].p3 = (VEC2){ 1.0f,-1.0f};
	quad[0].p4 = (VEC2){ 1.0f, 1.0f};
	quad[0].p5 = (VEC2){-1.0f, 1.0f};	
	quad[0].p6 = (VEC2){ 1.0f,-1.0f};

	quad[0].tc1 = (VEC2){0.0f,0.0f};
	quad[0].tc2 = (VEC2){0.0f,1.0f};
	quad[0].tc3 = (VEC2){1.0f,0.0f};
	quad[0].tc4 = (VEC2){1.0f,1.0f};
	quad[0].tc5 = (VEC2){0.0f,1.0f};
	quad[0].tc6 = (VEC2){1.0f,0.0f};

	quadC++;

	timeBeginPeriod(1);
	wndclass.hInstance = GetModuleHandleA(0);
	RegisterClassA(&wndclass);
	window = CreateWindowExA(0,winName,winName,0x90000000,0,0,reso.y,reso.x,0,0,wndclass.hInstance,0);
	dc = GetDC(window);

	ShowCursor(0);

	CreateThread(0,0,render,0,0,0);
	CreateThread(0,0,physics,0,0,0);
	
	for(;;){
		while(GetMessageA(&Msg,window,0,0)>0){
			TranslateMessage(&Msg);
			DispatchMessageW(&Msg);
		}
	}
}
