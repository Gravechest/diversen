#include <windows.h>
#include <math.h>
#include <intrin.h>
#include <stdio.h>
#include <GL/gl.h>
#include <cl/cl.h>

#define GL_ARRAY_BUFFER 0x8892
#define GL_STATIC_DRAW 0x88E4
#define GL_FLOAT 0x1406
#define GL_FRAGMENT_SHADER 0x8B30
#define GL_VERTEX_SHADER 0x8B31
#define GL_COMPILE_STATUS 0x8B81
#define GL_INFO_LOG_LENGTH 0x8B84
#define GL_UNIFORM_BUFFER 0x8A11
#define GL_TEXTURE_3D 0x806F	

#define resx 256
#define resy 256

#define res 256

#define RENDERDISTANCE 32

#define MAPSZ 32
#define MAPRAM MAPSZ*MAPSZ*MAPSZ

#define VRAM res*res*4

#define gpu 0

#define PI_2 1.57079632679489661923

#define rendertechnique 1

unsigned int (*glCreateProgram)(void);
unsigned int (*glCreateShader)(int);
unsigned int (*glGetUniformBlockIndex)(int,char*);

void (*glCreateBuffers)(int,unsigned int*);
void (*glBindBuffer)(int,int);
void (*glBufferData)(int,int,void*,int);
void (*glEnableVertexAttribArray)(int);
void (*glShaderSource)(int,int,void*,int);
void (*glCompileShader)(int);
void (*glAttachShader)(unsigned int,int);
void (*glLinkProgram)(unsigned int);
void (*glUseProgram)(unsigned int);
void (*glVertexAttribPointer)(int,int,int,int,int,int);
void (*glGetShaderiv)(int,int,int*);
void (*glGetShaderInfoLog)(int,int,void*,char*);
void (*glUniform1iv)(int,int,void*);
void (*glBindBufferBase)(int,int,int);
void (*glUniformBlockBinding)(int,int,int);
void (*glTexImage3D)(int,int,int,int,int,int,int,int,int,void*);

int (*glGetUniformLocation)(int,char*);

PIXELFORMATDESCRIPTOR pfd = {sizeof(PIXELFORMATDESCRIPTOR), 1,
PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,PFD_TYPE_RGBA,
24,0, 0, 0, 0, 0, 0,0,0,0,
0,0,0,0,32,0,0,PFD_MAIN_PLANE,
0,0,0,0	};

float quad[12] = {1.0,1.0 ,-1.0,1.0 ,1.0,-1.0 ,-1.0,-1.0 ,-1.0,1.0 ,1.0,-1.0};

unsigned int shaderProgram;
unsigned int VBO;
unsigned int UBO;
unsigned int vertexShader;
unsigned int fragmentShader;
unsigned int mapText;

int shaderStatus;

typedef struct RAY{
	float x;
	float y;
	float z;
	float vx;
	float vy;
	float vz;
}RAY;

typedef struct PLAYERDATA{
	float xpitch;
	float ypitch;

	float xpos;
	float ypos;
	float zpos;

	float xfov;
	float yfov;

	float xvel;
	float yvel;
	float zvel;

	float richtingx;
	float richtingy;
	float richtingz;

	float leftx;
	float lefty;

	float downx;
	float downy;
}PLAYERDATA;

typedef struct PROPERTIES{
	int reso;
	int lvlSz;
	int renderDistance;
}PROPERTIES;

PLAYERDATA *player;
PROPERTIES *properties;

cl_platform_id platformid[20];
cl_command_queue commandqueue;
cl_device_id deviceid;
cl_context context;
cl_program program;	
cl_kernel kernel;

cl_mem mem;
cl_mem map_mem;
cl_mem player_mem;
cl_mem prop_mem;

int platformC;
int deviceC;
int result;
int err;
int fsize;
int count = VRAM/4;
int count2 = 1;

char *VERTsource;
char *FRAGsource;
char *CLsource;
char *data;
char *map;

char settings;
char touchStatus;
char threadStatus;

BITMAPINFO bmi = { sizeof(BITMAPINFOHEADER),res,res,1,32,BI_RGB };	

const char name[] = "window";
HWND window;
HDC dc;
MSG Msg;

char hitbox(float x,float y,float z){
	if(x < 0){
		touchStatus |= 0x01;
	}
	if(y < 0){
		touchStatus |= 0x02;
	}
	if(z < 0){
		touchStatus |= 0x04;
	}
	int m = (int)x + (int)y * properties->lvlSz + (int)z * properties->lvlSz * properties->lvlSz;
	if(map[m]){
		x -= player->xvel;
		m = (int)x + (int)y * properties->lvlSz + (int)z * properties->lvlSz * properties->lvlSz;
		if(!map[m]){
			touchStatus |= 0x08;
		}
		x += player->xvel;
		y -= player->yvel;
		m = (int)x + (int)y * properties->lvlSz + (int)z * properties->lvlSz * properties->lvlSz;
		if(!map[m]){
			touchStatus |= 0x10;
		}	
		y += player->yvel;		
		z -= player->zvel;
		m = (int)x + (int)y * properties->lvlSz + (int)z * properties->lvlSz * properties->lvlSz;
		if(!map[m]){
			touchStatus |= 0x20;
		}
		z += player->zvel;
	}
}

//functie die ervoor zorgt dat een ray 1 stap vooruit gaat

void rayItterate(RAY *ray){
	float dx,dy,dz;
	if(ray->vx > 0){
		dx = (int)ray->x + 1 - ray->x;
	}
	else{
		dx = (int)ray->x - ray->x;
		if(!dx){
			dx = -1;
		}
	}
	if(ray->vy > 0){
		dy = (int)ray->y + 1 - ray->y;
	}
	else{
		dy = (int)ray->y - ray->y;
		if(!dy){
			dy = -1;
		}
	}	
	if(ray->vz > 0){
		dz = (int)ray->z + 1 - ray->z;
	}
	else{
		dz = (int)ray->z - ray->z;
		if(!dz){
			dz = -1;
		}
	}
	if(dx / ray->vx < dy / ray->vy){
		if(dx / ray->vx < dz / ray->vz){
			ray->x += dx;
			ray->y += ray->vy * dx / ray->vx;
			ray->z += ray->vz * dx / ray->vx;
		}
		else{
			ray->x += ray->vx * dz / ray->vz;
			ray->y += ray->vy * dz / ray->vz;
			ray->z += dz;
		}
	}
	else if(dy / ray->vy < dz / ray->vz){
		ray->x += ray->vx * dy / ray->vy;
		ray->y += dy;
		ray->z += ray->vz * dy / ray->vy;
	}
	else{
		ray->x += ray->vx * dz / ray->vz;
		ray->y += ray->vy * dz / ray->vz;
		ray->z += dz;
	}
}

//windows messages kunnen hier worden behandeld

long _stdcall proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	switch(msg){
	case WM_KEYDOWN:
		if(GetKeyState(VK_F1) & 0x80){
			ShowCursor(settings & 1);
			settings ^= 1;
			if(settings & 1){
				SetCursorPos(50,50);
			}
		}
		break;
	case WM_MOUSEMOVE:;
		if(settings & 1){
			POINT curp;
			GetCursorPos(&curp);
			player->xpitch += (float)(curp.x - 50) / 130;
			player->ypitch -= (float)(curp.y - 50) / 130;
			if(player->ypitch < -2){
				player->ypitch = -2;
			}
			SetCursorPos(50,50);
		}
		break;
	case WM_LBUTTONDOWN:{
			RAY ray;
			ray.x = player->xpos;
			ray.y = player->ypos;
			ray.z = player->zpos;
			ray.vz = sin((float)(res/2) / res / player->yfov + player->ypitch);
			ray.vx = cos((float)(res/2) / res / player->xfov + player->xpitch) * cos((float)(res/2) / res / player->yfov + player->ypitch);
			ray.vy = sin((float)(res/2) / res / player->xfov + player->xpitch) * cos((float)(res/2) / res / player->yfov + player->ypitch);
			for(int i = 0;i < 12;i++){
				rayItterate(&ray);
				int block = (int)ray.x + (int)ray.y * properties->lvlSz + (int)ray.z * properties->lvlSz * properties->lvlSz;
				if(map[block]){
					if(ray.z - (int)ray.z == 0){
						map[block - properties->lvlSz * properties->lvlSz] = 1;
					}
					else if(ray.y - (int)ray.y == 0){
						map[block - properties->lvlSz] = 1;
					}
					else{
						map[block - 1] = 1;
					}
					break;
				}
				block = (int)(ray.x - 0.00001) + (int)(ray.y - 0.00001) * properties->lvlSz + (int)(ray.z - 0.00001) * properties->lvlSz * properties->lvlSz;
				if(map[block]){
					if(ray.z - (int)ray.z == 0){
						map[block + properties->lvlSz * properties->lvlSz] = 1;
					}
					else if(ray.y - (int)ray.y == 0){
						map[block + properties->lvlSz] = 1;
					}
					else{
						map[block + 1] = 1;
					}
					break;
				}
			}
			break;
		}
	case WM_RBUTTONDOWN:{
			RAY ray;
			ray.x = player->xpos;
			ray.y = player->ypos;
			ray.z = player->zpos;
			ray.vz = sin((float)(res/2) / res / player->yfov + player->ypitch);
			ray.vx = cos((float)(res/2) / res / player->xfov + player->xpitch) * cos((float)(res/2) / res / player->yfov + player->ypitch);
			ray.vy = sin((float)(res/2) / res / player->xfov + player->xpitch) * cos((float)(res/2) / res / player->yfov + player->ypitch);
			for(int i = 0;i < 12;i++){
				rayItterate(&ray);
				int block = (int)ray.x + (int)ray.y * properties->lvlSz + (int)ray.z * properties->lvlSz * properties->lvlSz;
				if(map[block]){
					map[block] = 0;
					break;
				}
			}
			break;
		}
	}
	return DefWindowProc(hwnd,msg,wParam,lParam);
}

WNDCLASS wndclass = {0,proc,0,0,0,0,0,0,name,name};

//de gpu code word hier uitgevoert

void openCL(){
	player->richtingx = cosf(player->xpitch);
	player->richtingy = sinf(player->xpitch);
	player->richtingz = sinf(player->ypitch);

	player->leftx = player->richtingy;
	player->lefty = -player->richtingx;

	player->downx = player->richtingz;
	player->downy = -player->richtingx;

	long long t = _rdtsc();	
	clEnqueueWriteBuffer(commandqueue,map_mem,1,0,MAPRAM,map,0,0,0);
	clEnqueueWriteBuffer(commandqueue,player_mem,1,0,sizeof(PLAYERDATA),player,0,0,0);
	clEnqueueWriteBuffer(commandqueue,prop_mem,1,0,sizeof(PROPERTIES),properties,0,0,0);
	clEnqueueWriteBuffer(commandqueue,mem,1,0,VRAM,data,0,0,0);
	clEnqueueNDRangeKernel(commandqueue,kernel,1,0,&count,&count2,0,0,0);
	clEnqueueReadBuffer(commandqueue,mem,1,0,VRAM,data,0,0,0);
}

void main(){
	//om de Sleep functie accurater te maken
	timeBeginPeriod(1);

	HANDLE h = CreateFile("source.cl",GENERIC_READ,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
	fsize = GetFileSize(h,0);
	CLsource = HeapAlloc(GetProcessHeap(),8,fsize);
	ReadFile(h,CLsource,fsize,0,0);
	CloseHandle(h);

	h = CreateFile("fragment.frag",GENERIC_READ,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
	fsize = GetFileSize(h,0);
	FRAGsource = HeapAlloc(GetProcessHeap(),8,fsize+1);
	ReadFile(h,FRAGsource,fsize+1,0,0);
	CloseHandle(h);

	h = CreateFile("vertex.vert",GENERIC_READ,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
	fsize = GetFileSize(h,0);
	VERTsource = HeapAlloc(GetProcessHeap(),8,fsize+1);
	ReadFile(h,VERTsource,fsize+1,0,0);
	CloseHandle(h);

	//hier word memory gealloceerd

	data       = HeapAlloc(GetProcessHeap(),8,VRAM);
	map        = HeapAlloc(GetProcessHeap(),8,MAPRAM);
	player     = HeapAlloc(GetProcessHeap(),8,sizeof(PLAYERDATA));
	properties = HeapAlloc(GetProcessHeap(),8,sizeof(PROPERTIES));

	//opencl standaard code 

	clGetPlatformIDs(20,platformid,&platformC);
	clGetDeviceIDs(platformid[gpu],CL_DEVICE_TYPE_DEFAULT,1,&deviceid,0);
	context = clCreateContext(0,1,&deviceid,0,0,0);
	commandqueue = clCreateCommandQueue(context,deviceid,0,0);
	program = clCreateProgramWithSource(context,1,(const char**)&CLsource,0,0);
	clBuildProgram(program,0,0,0,0,0);
	kernel = clCreateKernel(program,"add",0);
	if(!kernel){	
		printf("program has not compiled\n");
		ExitProcess(0);
	}

	mem        = clCreateBuffer(context,0,VRAM,0,0);
	map_mem    = clCreateBuffer(context,0,MAPRAM,0,0);
	player_mem = clCreateBuffer(context,0,sizeof(PLAYERDATA),0,0);
	prop_mem   = clCreateBuffer(context,0,sizeof(PROPERTIES),0,0);

	clSetKernelArg(kernel,0,sizeof(mem),(void*)&mem);
	clSetKernelArg(kernel,1,sizeof(map_mem),(void*)&map_mem);
	clSetKernelArg(kernel,2,sizeof(player_mem),(void*)&player_mem);
	clSetKernelArg(kernel,3,sizeof(prop_mem),(void*)&prop_mem);

	memset(map,1,MAPRAM);

	// hier word de window gemaakt

	wndclass.hInstance = GetModuleHandle(0);
	RegisterClass(&wndclass);
	window = CreateWindowEx(0,name,name,0x90080000,0,0,resy + 16,resx + 39,0,0,wndclass.hInstance,0);
	dc = GetDC(window);

	clEnqueueWriteBuffer(commandqueue,map_mem,1,0,MAPRAM,map,0,0,0);

	//initializatie

	player->xfov = 0.5;
	player->yfov = 1;
	player->zpos = 14;
	player->xpos = 6;
	player->ypos = 6;

	properties->lvlSz = MAPSZ;
	properties->renderDistance = RENDERDISTANCE;
	properties->reso = res*4;

	settings = 1;
	ShowCursor(0);

	//mapgencode kan hier
	
	map[0] = 1;

	//opengl initializing

	SetPixelFormat(dc, ChoosePixelFormat(dc, &pfd), &pfd);
	wglMakeCurrent(dc, wglCreateContext(dc));

	glGetShaderiv				 = wglGetProcAddress("glGetShaderiv");
	glCreateProgram 			 = wglGetProcAddress("glCreateProgram"); 
	glCreateShader  			 = wglGetProcAddress("glCreateShader");
	glCreateBuffers			     = wglGetProcAddress("glCreateBuffers");
	glBindBuffer                 = wglGetProcAddress("glBindBuffer"); 
	glCreateShader  			 = wglGetProcAddress("glCreateShader");
	glBufferData    			 = wglGetProcAddress("glBufferData"); 
	glEnableVertexAttribArray    = wglGetProcAddress("glEnableVertexAttribArray");
	glVertexAttribPointer		 = wglGetProcAddress("glVertexAttribPointer");
	glShaderSource 				 = wglGetProcAddress("glShaderSource"); 
	glCompileShader 			 = wglGetProcAddress("glCompileShader"); 
	glAttachShader 				 = wglGetProcAddress("glAttachShader");
	glLinkProgram 				 = wglGetProcAddress("glLinkProgram");
	glUseProgram 				 = wglGetProcAddress("glUseProgram");
	glGetShaderInfoLog			 = wglGetProcAddress("glGetShaderInfoLog");
	glUniform1iv                 = wglGetProcAddress("glUniform1iv");
	glGetUniformLocation		 = wglGetProcAddress("glGetUniformLocation");
	glGetUniformBlockIndex       = wglGetProcAddress("glGetUniformBlockIndex");
	glBindBufferBase             = wglGetProcAddress("glBindBufferBase");
	glUniformBlockBinding		 = wglGetProcAddress("glUniformBlockBinding");
	glTexImage3D                 = wglGetProcAddress("glTexImage3D");

	shaderProgram = glCreateProgram();

	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vertexShader,1,(const char**)&VERTsource,0);
	glShaderSource(fragmentShader,1,(const char**)&FRAGsource,0);

	glCompileShader(vertexShader);
	glCompileShader(fragmentShader);

	glAttachShader(shaderProgram,vertexShader);
	glAttachShader(shaderProgram,fragmentShader);

	glLinkProgram(shaderProgram);
	glUseProgram(shaderProgram);
	glEnable(GL_TEXTURE_3D);

	glGenTextures(1,&mapText);

	glBindTexture(GL_TEXTURE_3D,mapText);
	glTexImage3D(GL_TEXTURE_3D,0,GL_RED,MAPSZ,MAPSZ,MAPSZ,0,GL_RED,GL_UNSIGNED_BYTE,map);
	glCreateBuffers(1,&VBO);
	glBindBuffer(GL_ARRAY_BUFFER,VBO);
	glBufferData(GL_ARRAY_BUFFER,12 * sizeof(float),quad,GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,2,GL_FLOAT,0,2 * sizeof(float),0);

	for(;;){
		//movement
		if(GetKeyState(0x57) & 0x80){
			player->xvel += cosf(player->xpitch + 0.50 / player->xfov) / 120;
			player->yvel += sinf(player->xpitch + 0.50 / player->xfov) / 120;
		}
		if(GetKeyState(0x53) & 0x80){
			player->xvel -= cosf(player->xpitch + 0.50 / player->xfov) / 120;
			player->yvel -= sinf(player->xpitch + 0.50 / player->xfov) / 120;
		}
		if(GetKeyState(0x44) & 0x80){
			player->xvel += cosf(player->xpitch + 0.50 / player->xfov + PI_2) / 120;
			player->yvel += sinf(player->xpitch + 0.50 / player->xfov + PI_2) / 120;
		}
		if(GetKeyState(0x41) & 0x80){
			player->xvel -= cosf(player->xpitch + 0.50 / player->xfov + PI_2) / 120;
			player->yvel -= sinf(player->xpitch + 0.50 / player->xfov + PI_2) / 120;
		}
		if(!settings & 0x01){
			if(GetKeyState(VK_UP) & 0x80){
				player->ypitch += 0.03;
			}
			if(GetKeyState(VK_DOWN) & 0x80 && player->ypitch > -1){
				player->ypitch -= 0.03;
			}
			if(GetKeyState(VK_LEFT) & 0x80){
				player->xpitch -= 0.03;
			}
			if(GetKeyState(VK_RIGHT) & 0x80){
				player->xpitch += 0.03;
			}
		}

		player->zvel -= 0.03;

		player->xpos += player->xvel;
		player->ypos += player->yvel;
		player->zpos += player->zvel;

		//collision code 

		hitbox(player->xpos + 0.2,player->ypos + 0.2,player->zpos - 1.8);
		hitbox(player->xpos + 0.2,player->ypos + 0.2,player->zpos + 0.2);
		hitbox(player->xpos + 0.2,player->ypos - 0.2,player->zpos - 1.8);
		hitbox(player->xpos + 0.2,player->ypos - 0.2,player->zpos + 0.2);
		hitbox(player->xpos - 0.2,player->ypos + 0.2,player->zpos - 1.8);
		hitbox(player->xpos - 0.2,player->ypos + 0.2,player->zpos + 0.2);
		hitbox(player->xpos - 0.2,player->ypos - 0.2,player->zpos - 1.8);
		hitbox(player->xpos - 0.2,player->ypos - 0.2,player->zpos + 0.2);

		if(touchStatus & 0x01){
			player->xpos -= player->xvel;
			player->xvel = 0;
		}
		if(touchStatus & 0x02){
			player->ypos -= player->yvel;
			player->yvel = 0;
		}
		if(touchStatus & 0x04){
			player->zpos -= player->zvel;
			if(GetKeyState(VK_SPACE) & 0x80){
				player->zvel = 0.5;
			}
			else{
				player->zvel = 0;
			}
		}
		if(touchStatus & 0x08){
			player->xpos -= player->xvel;
			player->xvel = 0;
		}
		if(touchStatus & 0x10){
			player->ypos -= player->yvel;
			player->yvel = 0;
		}
		if(touchStatus & 0x20){
			player->zpos -= player->zvel;
			if(GetKeyState(VK_SPACE) & 0x80){
				player->zvel = 0.5;
			}
			else{
				player->zvel = 0;
			}
		}

		touchStatus = 0;

		player->xvel /= 1.07;
		player->yvel /= 1.07;
		player->zvel /= 1.07;

		//wat timing zooi hier

		switch(rendertechnique){
		case 0:{
				HANDLE renderThread = CreateThread(0,0,openCL,0,0,0);
				Sleep(15);
				WaitForSingleObject(renderThread,9999);
				StretchDIBits(dc,0,0,resy,resx,0,0,res,res,data,&bmi,0,SRCCOPY);
				break;
			}
		case 1:
    		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    		glClear(GL_COLOR_BUFFER_BIT);
			glDrawArrays(GL_TRIANGLES,0,12);
			SwapBuffers(dc);
			break;
		}

		//hier word de boel gerenderd op het scherm

		memset(data,0,VRAM);

		//de standaard message loop om windows tevreden te houden

		while(PeekMessage(&Msg,window,0,0,0)){
			GetMessage(&Msg,window,0,0);
			TranslateMessage(&Msg);
			DispatchMessageW(&Msg);
		}
	}
}
