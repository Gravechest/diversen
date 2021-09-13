#include <windows.h>
#include <time.h>
#include <GL/gl.h>
#include <stdio.h>
#include <math.h>
#define resX 1080
#define resY 1920
typedef struct VEC2{float x;float y;} VEC2;
typedef struct VECI2{short int x;short int y;} VECI2;
typedef struct COLOR{char r;char g;char b;} COLOR;
char texture[resX][resY][3];
int imageSize;
char * image;
float divider;
char menuStatus = 1;
// woord en getal lengte, heeft een dubbel doel
char wordLng; 
char word[256];
int getal;
char * font;
char mouseInput;
COLOR color;
POINT mouse;
POINT mouseBuf;
COLOR colorBlack = {0,0,0};
PIXELFORMATDESCRIPTOR pfd = {sizeof(PIXELFORMATDESCRIPTOR), 1,
	PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,PFD_TYPE_RGBA,
	24,0, 0, 0, 0, 0, 0,0,0,0,
	0,0,0,0,32,0,0,PFD_MAIN_PLANE,
	0,0,0,0	};
HWND hwnd;
MSG Msg;
HDC wdcontext;

void drawSquare(int x, int y, int size,COLOR color){
	for(int i = x;i < x + size;i++){
		for(int i2 = y;i2 < y + size;i2++){
			texture[i][i2][0] = color.r;
			texture[i][i2][1] = color.g;
			texture[i][i2][2] = color.b;
		}
	}
}

void drawRect(int x,int y, int sx, int sy,COLOR color){
	for(int i = x;i < x + sx;i++){
		for(int i2 = y;i2 < y + sy;i2++){
			texture[i][i2][0] = color.r;
			texture[i][i2][1] = color.g;
			texture[i][i2][2] = color.b;
		}
	}
}

void fontDrawing(int x,int y,int offset){
	font += offset + 1000;
	for(int i = 0;i < 50;i+=10){
		for(int i2 = 0;i2 < 50;i2+=10){
			COLOR color = {font[2],font[1],font[0]};
			drawSquare(x + i,y + i2,10,color);
			font+=4;
		}
		font-=220;
	}
	font -= offset;
}

void drawPoint(int x,int y){
	
}

char * loadImage(const char * file){
	FILE * imageF = fopen(file,"rb+");
	fseek(imageF,0,SEEK_END);
	int size = ftell(imageF);
	fseek(imageF,0,SEEK_SET);
	char * data = malloc(14);
	fread(data,1,14,imageF);
	char * returndata = malloc(size);
	fseek(imageF,data[10] - 14,SEEK_CUR);
	fread(returndata,1,size - ftell(imageF),imageF);
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
		glDrawPixels(resY,resX,GL_RGB,GL_UNSIGNED_BYTE,&texture);
		SwapBuffers(wdcontext);
	}
}

void getNumberInput(){
	for(int i = 0x30;i < 0x40;i++){
		if(GetAsyncKeyState(i)){
			int offset = 0;
			if(i < 0x34){
				offset = 1360 + i * 20;
			}
			else{
				offset = 2360 + i * 20;
			}
			word[wordLng] = i - 0x30;
			fontDrawing(resX - 100,30 + wordLng * 50,offset);
			wordLng++;
		}
	}
	if(GetAsyncKeyState(VK_RETURN)){
		switch(menuStatus){
		case 1:
			drawRect(resX - 100,30,100,wordLng * 50,colorBlack);
			for(int i = 0;i < wordLng;i++){
				int subSize = word[i];
				for(int i2 = i + 1;i2 < wordLng;i2++){
					subSize *= 10;
				}
				imageSize += subSize;
			}
			image = malloc(imageSize * imageSize * 3);
			int x = resX - 200;
			divider = x / imageSize;
			memset(word,0,sizeof(word));
			menuStatus = 0;
			wordLng = 0;
			break;
		}
	}
	if(GetAsyncKeyState(VK_ESCAPE)){
		exit(0);
	}
	if(GetAsyncKeyState(0x53)){
		char pxLng[4] = {imageSize,imageSize >> 8,imageSize >> 16,imageSize >> 24};
		int imBytes = imageSize * imageSize * 3 + 54;
		char flSz[4] = {imBytes,imBytes >> 8,imBytes >> 16,imBytes >> 24};
		FILE * imageF = fopen("test.bmp","wb+");
		//Generally purposed bitmap header
		char Header[14] = {0x42,0x4d,flSz[0],flSz[1],flSz[2],flSz[3]
			,0x00,0x00,0x00,0x00,0x36,0x00,0x00,0x00};
		imBytes -= 54;
		flSz[0] = imBytes;
		flSz[1] = imBytes >> 8;
		flSz[2] = imBytes >> 16;
		flSz[3] = imBytes >> 32;
		//Medium sized bitmap header
		char bitmapHeader[40] = {0x28,0x00,0x00,0x00,pxLng[0],pxLng[1],pxLng[2],pxLng[3],pxLng[0],
			pxLng[1],pxLng[2],pxLng[3],0x01,0x00,0x18,0x00,0x00,0x00,0x00,0x00,flSz[0],flSz[1],flSz[2],
			flSz[3],0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
		fwrite(Header,1,14,imageF);
		fwrite(bitmapHeader,1,40,imageF);
		fwrite(image,1,imBytes,imageF);
		fclose(imageF);
	}
}	
void pixelConvertor(int x,int y){
	x /= divider;
	x *= divider;
	int offset = x * imageSize * 3 + y * 3;
	image[offset] = color.r;
	image[offset + 1] = color.g;
	image[offset + 2] = color.b;
	y /= divider;
	y *= divider;
	drawSquare(x,y,divider,color);
	
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
	WNDCLASSEX wc = { sizeof(WNDCLASSEX),0,WndProc,0,0,hInstance,0,0,0,0,"hendrik",0 };
	RegisterClassEx(&wc);
	hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, "hendrik", "Verf", WS_VISIBLE | WS_POPUP | WS_EX_TOPMOST,
		0, 0, 1920, 1080, NULL, NULL, hInstance, NULL);
	wdcontext = GetDC(hwnd);
	ShowCursor(1);
	CreateThread(0,0,Quarter1,0,0,0);
	for(;;){
		while(PeekMessage(&Msg,hwnd,0,0,0)){
			GetMessage(&Msg,hwnd,0,0);
			TranslateMessage(&Msg);
			DispatchMessageW(&Msg);
		}
		if(mouseInput & 1){
			GetCursorPos(&mouse);
			mouse.y = resX - mouse.y;
			if(mouse.y > 100 && mouse.x > 100 && mouse.y < resX - 100 && mouse.x < resY - 100){
				VEC2 normCoords = {mouse.y - mouseBuf.y,mouse.x - mouseBuf.x};
				VEC2 Coords = {mouse.y,mouse.x};
				short int minus = fmaxf(fabsf(normCoords.x),fabsf(normCoords.y));
				normCoords.x /= minus;
				normCoords.y /= minus;
				while(minus > 0){
					pixelConvertor((int)Coords.x,(int)Coords.y);
					Coords.x -= normCoords.x;
					Coords.y -= normCoords.y;
					minus--;
				}
				mouseBuf = mouse;
			}
		}
		Sleep(3);
	}
	return Msg.wParam;
}
