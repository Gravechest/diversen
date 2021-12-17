#include <windows.h>
#include <intrin.h>
#include <stdio.h>
#include <cl/cl.h>

#define resx 512
#define resy 512

#define res 512

#define MAPSZ 1 << (3 * 3)

#define VRAM res*res*4

#define gpu 0

typedef struct PLAYERDATA{
	float xpitch;
	float ypitch;
}PLAYERDATA;

PLAYERDATA *player;

cl_platform_id platformid[20];
cl_command_queue commandqueue;
cl_device_id deviceid;
cl_context context;
cl_program program;	
cl_kernel kernel;
cl_mem mem;
cl_mem map_mem;
cl_mem player_mem;

int platformC;
int deviceC;
int result;
int err;
int fsize;
int count = VRAM/4;
int count2 = 1;

char *source;
char *data;
char *map;

BITMAPINFO bmi = { sizeof(BITMAPINFOHEADER),res,res,1,32,BI_RGB };

const char name[] = "window";
HWND window;
HDC dc;
MSG Msg;

long _stdcall proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	switch(msg){
	case WM_KEYDOWN:
		if(GetKeyState(VK_UP) & 0x80){
			player->ypitch += 0.3;
		}
		else if(GetKeyState(VK_DOWN) & 0x80){
			player->ypitch -= 0.3;
		}
		else if(GetKeyState(VK_LEFT) & 0x80){
			player->xpitch -= 0.3;
		}
		else if(GetKeyState(VK_RIGHT) & 0x80){
			player->xpitch += 0.3;
		}
		break;
	}
	return DefWindowProc(hwnd,msg,wParam,lParam);
}

WNDCLASS wndclass = {0,proc,0,0,0,0,0,0,name,name};

void openCL(){
	clEnqueueWriteBuffer(commandqueue,map_mem,1,0,MAPSZ,map,0,0,0);
	clEnqueueWriteBuffer(commandqueue,player_mem,1,0,sizeof(PLAYERDATA),player,0,0,0);
	clEnqueueWriteBuffer(commandqueue,mem,1,0,VRAM,data,0,0,0);
	clEnqueueNDRangeKernel(commandqueue,kernel,1,0,&count,&count2,0,0,0);
	clEnqueueReadBuffer(commandqueue,mem,1,0,VRAM,data,0,0,0);
}

void main(){
	HANDLE h = CreateFile("source.cl",GENERIC_READ,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
	fsize = GetFileSize(h,0);
	source = HeapAlloc(GetProcessHeap(),8,fsize);
	ReadFile(h,source,fsize,0,0);

	data   = HeapAlloc(GetProcessHeap(),8,VRAM);
	map    = HeapAlloc(GetProcessHeap(),8,MAPSZ);
	player = HeapAlloc(GetProcessHeap(),8,sizeof(PLAYERDATA));

	clGetPlatformIDs(20,platformid,&platformC);
	clGetDeviceIDs(platformid[gpu],CL_DEVICE_TYPE_DEFAULT,1,&deviceid,0);
	context = clCreateContext(0,1,&deviceid,0,0,0);
	commandqueue = clCreateCommandQueue(context,deviceid,0,0);
	program = clCreateProgramWithSource(context,1,(const char**)&source,0,0);
	clBuildProgram(program,0,0,0,0,0);
	kernel = clCreateKernel(program,"add",0);
	if(!kernel){
		printf("program has not compiled\n");
		exit(0);
	}

	mem        = clCreateBuffer(context,0,VRAM,0,0);
	map_mem    = clCreateBuffer(context,1,MAPSZ,0,0);
	player_mem = clCreateBuffer(context,2,sizeof(PLAYERDATA),0,0);

	clSetKernelArg(kernel,0,sizeof(mem),(void*)&mem);
	clSetKernelArg(kernel,1,sizeof(map_mem),(void*)&map_mem);
	clSetKernelArg(kernel,2,sizeof(player_mem),(void*)&player_mem);

	wndclass.hInstance = GetModuleHandle(0);
	RegisterClass(&wndclass);
	window = CreateWindowEx(0,name,name,0x10080000,0,0,resy + 16,resx + 39,0,0,wndclass.hInstance,0);
	dc = GetDC(window);

	for(int i = 0;i < MAPSZ;i++){
		if(!_rdtsc() & 15){
			map[i] = 1;
		}
	}

	for(;;){
		long long timer = _rdtsc();
		openCL();
		printf("%i\n",_rdtsc() - timer >> 22);
		StretchDIBits(dc,0,0,resx,resy,0,0,res,res,data,&bmi,0,SRCCOPY);
		memset(data,0,VRAM);
		while(PeekMessage(&Msg,window,0,0,0)){
			GetMessage(&Msg,window,0,0);
			TranslateMessage(&Msg);
			DispatchMessageW(&Msg);
		}
	}
}





























