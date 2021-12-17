#include <windows.h>
#include <stdio.h>
#include <cl/cl.h>

#define resx 512
#define resy 512

#define res 512

#define gpu 0

cl_platform_id platformid[20];
cl_command_queue commandqueue;
cl_device_id deviceid;
cl_context context;
cl_program program;	
cl_kernel kernel;
cl_mem mem;

int platformC;
int deviceC;
int result;
int err;
int fsize;
int count = dataC;
int count2 = 1;
char *source;
char *data;

BITMAPINFO bmi = { sizeof(BITMAPINFOHEADER),resx,resy,1,32,BI_RGB };

const char name[] = "window";
HWND window;
HDC dc;
MSG Msg;

long _stdcall proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	return DefWindowProc(hwnd,msg,wParam,lParam);
}

WNDCLASS wndclass = {0,proc,0,0,0,0,0,0,name,name};

void openCL(){
	clEnqueueWriteBuffer(commandqueue,mem,1,0,dataC,data,0,0,0);
	clEnqueueNDRangeKernel(commandqueue,kernel,1,0,&count,&count2,0,0,0);
	clEnqueueReadBuffer(commandqueue,mem,1,0,dataC,data,0,0,0);
}

void main(){
	HANDLE h = CreateFile("source.cl",GENERIC_READ,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
	fsize = GetFileSize(h,0);
	source = HeapAlloc(GetProcessHeap(),8,fsize);
	ReadFile(h,source,fsize,0,0);

	data = HeapAlloc(GetProcessHeap(),8,res*res*4);

	clGetPlatformIDs(20,platformid,&platformC);
	clGetDeviceIDs(platformid[gpu],CL_DEVICE_TYPE_DEFAULT,1,&deviceid,0);
	context = clCreateContext(0,1,&deviceid,0,0,0);
	commandqueue = clCreateCommandQueue(context,deviceid,0,0);
	mem = clCreateBuffer(context,0,dataC,0,&err);
	clEnqueueWriteBuffer(commandqueue,mem,CL_FALSE,0,dataC,data,0,0,0);
	program = clCreateProgramWithSource(context,1,(const char**)&source,0,0);
	clBuildProgram(program,0,0,0,0,0);
	kernel = clCreateKernel(program,"add",0);

	if(!kernel){
		printf("program has not compiled\n");
	}

	clSetKernelArg(kernel,0,sizeof(mem),(void*)&mem);
	clEnqueueNDRangeKernel(commandqueue,kernel,1,0,&count,&count2,0,0,0);
	clEnqueueReadBuffer(commandqueue,mem,1,0,dataC,data,0,0,0);

	wndclass.hInstance = GetModuleHandle(0);
	RegisterClass(&wndclass);
	window = CreateWindowEx(0,name,name,0x10080000,0,0,resy + 16,resx + 39,0,0,wndclass.hInstance,0);
	dc = GetDC(window);

	for(;;){
		openCL();
		StretchDIBits(dc,0,0,resx,resy,0,0,res,res,data,&bmi,0,SRCCOPY);
		while(PeekMessage(&Msg,window,0,0,0)){
			GetMessage(&Msg,window,0,0);
			TranslateMessage(&Msg);
			DispatchMessageW(&Msg);
		}
	}
}





























