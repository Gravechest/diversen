#include <windows.h>
#include <time.h>
#include <GL/gl.h>
#include <stdio.h>
#include <math.h>
#define resX 1080
#define resY 1920
typedef struct PARTICLE{float x;float y;float velx;float vely;} PARTICLE;
typedef struct VEC2{float x;float y;} VEC2;
typedef struct COLOR{unsigned char r;unsigned char g;unsigned char b;} COLOR;
unsigned char texture[resX][resY][3];
unsigned char background[resX][resY][3];
const char className[] = "myWindowClass";
unsigned char menuStatus = 1;
// woord en getal lengte, heeft een dubbel doel
unsigned char wordLng; 
int word[256];
int getal;
char * font;
unsigned char mouseInput;
COLOR color;
POINT mouse;
POINT mouseBuf;
PIXELFORMATDESCRIPTOR pfd = {sizeof(PIXELFORMATDESCRIPTOR), 1,
	PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,PFD_TYPE_RGBA,
	24,0, 0, 0, 0, 0, 0,0,0,0,
	0,0,0,0,32,0,0,PFD_MAIN_PLANE,
	0,0,0,0	};
HWND hwnd;
MSG Msg;
HDC wdcontext;

void drawSquare(int x, int y, int size,unsigned char r,unsigned char g,unsigned char b){
	for(int i = x;i < x + size;i++){
		for(int i2 = y;i2 < y + size;i2++){
			texture[i][i2][0] = r;
			texture[i][i2][1] = g;
			texture[i][i2][2] = b;
		}
	}
}

void fontDrawing(int x,int y,int offset){
	font += offset + 1000;
	for(int i = 0;i < 50;i+=10){
		for(int i2 = 0;i2 < 50;i2+=10){
			drawSquare(x + i,y + i2,10,font[2],font[1],font[0]);
			font+=4;
		}
		font-=220;
	}
	font -= offset;
}

char * loadImage(const char * file){
	FILE * image = fopen(file,"rb+");
	fseek(image,0,SEEK_END);
	int size = ftell(image);
	fseek(image,0,SEEK_SET);
	char * data = malloc(14);
	fread(data,1,14,image);
	char * returndata = malloc(size);
	fseek(image,data[10] - 14,SEEK_CUR);
	fread(returndata,1,size - ftell(image),image);
	free(data);
	return returndata;
}

void drawPencil(){
	for(int i = 90;i < 140;i++){
		for(int i2 = 0;i2 < 50;i2++){
			texture[i][i2][0] = color.r;
			texture[i][i2][1] = color.g;
			texture[i][i2][2] = color.b;
		}
	}
}

void WINAPI Quarter1(){
	SetPixelFormat(wdcontext, ChoosePixelFormat(wdcontext, &pfd), &pfd);
	wglMakeCurrent(wdcontext, wglCreateContext(wdcontext));
	glPixelZoom(1920 / resY,1080 / resX);
	font = loadImage("font.bmp");
	for(int i = 0;i < 30;i++){
		for(int i2 = 0; i2 < 256;i2++){
			texture[i][i2][0] = i2;
		}
	}
	for(int i = 30;i < 60;i++){
		for(int i2 = -1; i2 < 255;i2++){
			texture[i][i2][1] = i2;
		}
	}
	for(int i = 60;i < 90;i++){
		for(int i2 = -2; i2 < 254;i2++){
			texture[i][i2][2] = i2;
		}
	}
	for(;;){
		for(int i = 0;i < wordLng;i++){
			fontDrawing(200,200 + i * 50,word[i]);
		}
		glDrawPixels(resY,resX,GL_RGB,GL_UNSIGNED_BYTE,&texture);
		SwapBuffers(wdcontext);
	}
}

void getNumberInput(){
	for(int i = 0x30;i < 0x40;i++){
		if(GetAsyncKeyState(i)){
			if(i < 0x34){
				word[wordLng] = 1360 + i * 20;
			}
			else{
				word[wordLng] = 2360 + i * 20;
			}
			wordLng++;
		}
	}
	if(GetAsyncKeyState(VK_ENTER)){
		wordLng = 0;
	}
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	switch (msg){
	case WM_KEYDOWN:
		getNumberInput();
	case WM_LBUTTONDOWN:
		GetCursorPos(&mouseBuf);
		mouseBuf.y = resX - mouseBuf.y;
		if(mouseBuf.y < 90 && mouseBuf.x < 255){
			if(mouseBuf.y < 30){
				color.r = mouseBuf.x;
			}
			else if(mouseBuf.y < 60){
				color.g = mouseBuf.x;
			}
			else{
				color.b = mouseBuf.x;
			}
			drawPencil();
		}
		else{
			mouseInput |= 1;
		}
		break;	
	case WM_LBUTTONUP:
		mouseInput &= ~1;
		break;
	case WM_CLOSE:
		exit(0);
	case WM_DESTROY:
		exit(0);
	default:	
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wc = { sizeof(WNDCLASSEX),0,WndProc,0,0,hInstance,0,0,0,0,className,0 };
	RegisterClassEx(&wc);
	hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, className, "raycasting", WS_VISIBLE | WS_POPUP | WS_EX_TOPMOST,
		0, 0, 1920, 1080, NULL, NULL, hInstance, NULL);
	wdcontext = GetDC(hwnd);
	ShowCursor(1);
	CreateThread(0,0,Quarter1,0,0,0);

	for(;;){
		while(PeekMessage(&Msg,hwnd,0,0,0)){
			GetMessage(&Msg, hwnd, 0, 0);
			TranslateMessage(&Msg);
			DispatchMessageW(&Msg);
		}
		if(mouseInput & 1){
			GetCursorPos(&mouse);
			mouse.y = resX - mouse.y;
			VEC2 normCoords = {mouse.y - mouseBuf.y,mouse.x - mouseBuf.x};
			VEC2 Coords = {mouse.y,mouse.x};
			short int minus = fmaxf(fabsf(normCoords.x),fabsf(normCoords.y));
			normCoords.x /= minus;
			normCoords.y /= minus;
			while(minus > 0){
				texture[(int)Coords.x][(int)Coords.y][0] = color.r;
				texture[(int)Coords.x][(int)Coords.y][1] = color.g;
				texture[(int)Coords.x][(int)Coords.y][2] = color.b;
				Coords.x -= normCoords.x;
				Coords.y -= normCoords.y;
				minus--;
			}
			mouseBuf = mouse;
		}
		Sleep(3);
	}
	return Msg.wParam;
}

