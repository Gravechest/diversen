#include <windows.h>
#include <GL/gl.h>
#include <math.h>
#include <intrin.h>

#define resX 480
#define resY 640

unsigned char texture[resX][resY*3];
const char className[] = "myWindowClass";
POINT mouse;
PIXELFORMATDESCRIPTOR pfd = {sizeof(PIXELFORMATDESCRIPTOR), 1,
	PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,PFD_TYPE_RGBA,
	24,0, 0, 0, 0, 0, 0,0,0,0,
	0,0,0,0,32,0,0,PFD_MAIN_PLANE,
	0,0,0,0	};
unsigned char textureB[resX][resY*3];
HWND hwnd;
MSG Msg;
HDC wdcontext;
BITMAPINFO bmi = { sizeof(BITMAPINFOHEADER),resY,resX,1,24,BI_RGB };

void colorSpawn(int x,int y){
	textureB[x][y+0] = _rdtsc()^rand();
	textureB[x][y+1] = _rdtsc()^rand();
	textureB[x][y+2] = _rdtsc()^rand();
}

void WINAPI Quarter1(){
	SetPixelFormat(wdcontext, ChoosePixelFormat(wdcontext, &pfd), &pfd);
	wglMakeCurrent(wdcontext, wglCreateContext(wdcontext));
	glPixelZoom((float)1920 / resY,(float)1080 / resX);
	for(;;){
		colorSpawn(resX/2,resY*3/2);
		colorSpawn(resX/1.5,resY*3/2);
		colorSpawn(resX/2.5,resY*3/2);
		for(int i = 1;i < resX-1;i++){
			for(int i2 = 3;i2 < resY*3-3;i2+=3){
				switch((rand()^_rdtsc())&0x7){
				case 4:
					textureB[i-1][i2+0] = texture[i][i2+0];
					textureB[i-1][i2+1] = texture[i][i2+1];
					textureB[i-1][i2+2] = texture[i][i2+2];
					break;
				case 5:
					textureB[i+1][i2+0] = texture[i][i2+0];
					textureB[i+1][i2+1] = texture[i][i2+1];
					textureB[i+1][i2+2] = texture[i][i2+2];
					break;
				case 6:
					textureB[i][i2+3] = texture[i][i2+0];
					textureB[i][i2+4] = texture[i][i2+1];
					textureB[i][i2+5] = texture[i][i2+2];
					break;
				case 7:
					textureB[i][i2-3] = texture[i][i2+0];
					textureB[i][i2-2] = texture[i][i2+1];
					textureB[i][i2-1] = texture[i][i2+2];
					break;
				}
			}
		}
		memcpy(texture,textureB,sizeof(texture));
		glDrawPixels(resY,resX,GL_RGB,GL_UNSIGNED_BYTE,&texture);
		SwapBuffers(wdcontext); 
	}
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg){
	case WM_CLOSE:
	case WM_DESTROY:
		ExitProcess(0);
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
	hwnd = CreateWindowEx(0, className, "raycasting", WS_VISIBLE | WS_POPUP | WS_EX_TOPMOST,0, 0, 1920, 1080, NULL, NULL, hInstance, NULL);
	wdcontext = GetDC(hwnd);
	ShowCursor(0);
	CreateThread(0,0,Quarter1,0,0,0);
	for(;;){
		while(PeekMessage(&Msg,hwnd,0,0,0)){
			GetMessage(&Msg, hwnd, 0, 0);
			TranslateMessage(&Msg);
			DispatchMessageW(&Msg);
		}
		Sleep(1);
	}
	return Msg.wParam;
}
