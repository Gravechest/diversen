#include <windows.h>
#include <intrin.h>
#include <stdio.h>
#include <cl/cl.h>

#define rgb(r,g,b) (r + (g << 4) + (b << 8))

#define resx 1024
#define resy 1024

#define res 1024

#define dataC res*res*4

#define par 3

cl_platform_id platformid[20];
cl_command_queue commandqueue;
cl_device_id deviceid;
cl_context context;
cl_program program;	
cl_kernel kernel;
cl_mem mem;
cl_mem memmap;
cl_mem memprop;

int platformC;
int deviceC;
int result;
int err;
int fsize;
int lightC;

const int parralel = 1 << par;
const int count = 2048 << par;
const int count2 = 1;

char *source;
char *data;
char *map;

typedef struct FLIGHTPROP{
	int x;
	int y;
	int prop;
}FLIGHTPROP;

typedef struct LIGHTPROP{
	float x;
	float y;
	float vx;
	float vy;
}LIGHTPROP;

FLIGHTPROP *flightprop;
LIGHTPROP  *lightprop;

BITMAPINFO bmi = { sizeof(BITMAPINFOHEADER),res,res,1,32,BI_RGB };

const char name[] = "epic lightengine 2.0";
HWND window;
HDC dc;
MSG Msg;

long _stdcall proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	return DefWindowProc(hwnd,msg,wParam,lParam);
}

WNDCLASS wndclass = {0,proc,0,0,0,0,0,0,name,name};

void renderLight(){
	clEnqueueWriteBuffer  (commandqueue,mem,    1,0,dataC  ,data                       ,0,0,0);
	clEnqueueWriteBuffer  (commandqueue,memprop,1,0,sizeof(FLIGHTPROP) * 128,flightprop,0,0,0);
	clEnqueueNDRangeKernel(commandqueue,kernel, 1,0,&count ,&count2                    ,0,0,0);
	clEnqueueReadBuffer   (commandqueue,mem,    1,0,dataC  ,data                       ,0,0,0);
}

void spawnLight(float x,float y,float vx,float vy,short prop){
	lightprop[lightC].x     = x;
	lightprop[lightC].y     = y;
	lightprop[lightC].vx    = vx;
	lightprop[lightC].vy    = vy;
	flightprop[lightC].prop = prop;
	lightC++;
	clSetKernelArg(kernel,3,4,&lightC);
}

void main(){
	HANDLE h = CreateFile("source.cl",GENERIC_READ,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
	fsize = GetFileSize(h,0);
	source = HeapAlloc(GetProcessHeap(),8,fsize);
	ReadFile(h,source,fsize,0,0);

	data       = HeapAlloc(GetProcessHeap(),8,dataC);
	map        = HeapAlloc(GetProcessHeap(),8,res*res);
	flightprop = HeapAlloc(GetProcessHeap(),8,sizeof(FLIGHTPROP) * 128);
	lightprop  = HeapAlloc(GetProcessHeap(),8,sizeof(LIGHTPROP) * 128);

	clGetPlatformIDs(20,platformid,&platformC);
	clGetDeviceIDs(platformid[0],CL_DEVICE_TYPE_DEFAULT,1,&deviceid,0);
	context = clCreateContext(0,1,&deviceid,0,0,0);
	commandqueue = clCreateCommandQueue(context,deviceid,0,0);
	program = clCreateProgramWithSource(context,1,(const char**)&source,0,0);
	clBuildProgram(program,0,0,0,0,0);
	kernel = clCreateKernel(program,"add",0);
	if(!kernel){
		printf("program has not compiled\n");
	}

	mem     = clCreateBuffer(context,0,dataC,0,0);
	memmap  = clCreateBuffer(context,0,res*res,0,0);
	memprop = clCreateBuffer(context,0,sizeof(FLIGHTPROP) * 128,0,0);

	clSetKernelArg(kernel,0,sizeof(mem),(void*)&mem);
	clSetKernelArg(kernel,1,sizeof(memmap),(void*)&memmap);
	clSetKernelArg(kernel,2,sizeof(memprop),(void*)&memprop);
	clSetKernelArg(kernel,3,4,&lightC);
	clSetKernelArg(kernel,4,4,&parralel);

	wndclass.hInstance = GetModuleHandle(0);
	RegisterClass(&wndclass);
	window = CreateWindowEx(0,name,name,0x10080000,0,0,resy + 16,resx + 32,0,0,wndclass.hInstance,0);
	dc = GetDC(window);

	clEnqueueWriteBuffer(commandqueue,memmap, 1,0,res*res,map,0,0,0);
	for(int i = 0;i < 32;i++){
		spawnLight(200,200,0,0,rgb(2,2,1));
	}
	for(;;){
		for(int i = 0;i < res*res;i++){
			if(map[i]){
				data[i*4] = 255;
			}
		}
		for(int i = 0;i < lightC;i++){
			flightprop[i].x = (short)lightprop[i].x;
			flightprop[i].y = (short)lightprop[i].y;
			lightprop[i].x += lightprop[i].vx;
			lightprop[i].y += lightprop[i].vy;
		}
		if(GetKeyState(VK_LBUTTON) & 0x80){
			POINT p;
			GetCursorPos(&p);
			ScreenToClient(window,&p);
			p.y = res - p.y;
			if(p.y < res && p.x < res && p.y > 0 && p.x > 0){
				map[(p.y << 10) + p.x] = 1;
				clEnqueueWriteBuffer(commandqueue,memmap, 1,0,res*res,map,0,0,0);
			}
		}
		long long timer = _rdtsc();
		renderLight();
		printf("%i\n",(_rdtsc() - timer) >> 22);
		StretchDIBits(dc,0,0,resx,resy,0,0,res,res,data,&bmi,0,SRCCOPY);
		memset(data,0,dataC);
		if(PeekMessage(&Msg,window,0,0,0)){
			GetMessage(&Msg,window,0,0);
			TranslateMessage(&Msg);
			DispatchMessageW(&Msg);
		}
		Sleep(13);
	}
}






























