#include <windows.h>
#include <GL/gl.h>
#include <math.h>
#define resX 100
#define resY 140
unsigned char texture[resX][resY][3];
const char className[] = "myWindowClass";
POINT mouse;
PIXELFORMATDESCRIPTOR pfd = {sizeof(PIXELFORMATDESCRIPTOR), 1,
	PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,PFD_TYPE_RGBA,
	24,0, 0, 0, 0, 0, 0,0,0,0,
	0,0,0,0,32,0,0,PFD_MAIN_PLANE,
	0,0,0,0	};
unsigned char map[resX][resY][4];
unsigned char mapBuffer[resX][resY][4];
HWND hwnd;
MSG Msg;
HDC wdcontext;
BITMAPINFO bmi = { sizeof(BITMAPINFOHEADER),resY,resX,1,24,BI_RGB };
void WINAPI Quarter1(){
	SetPixelFormat(wdcontext, ChoosePixelFormat(wdcontext, &pfd), &pfd);
	wglMakeCurrent(wdcontext, wglCreateContext(wdcontext));
	glPixelZoom(1920 / resY,1080 / resX);
	for(;;){
		memset(mapBuffer,0,sizeof(mapBuffer));
		for(int i = 0;i < resX;i++){
			for(int i2 = 0;i2 < resY;i2++){
				if(texture[i][i2][0]){texture[i][i2][0]--;}
				if(rand() % 500 == 0 && (i == 0 || i2 == 0 || i == resX - 1 || i2 == resY - 1)){
					map[i][i2][0] = 1;
				}
				if(map[i][i2][0]){
					switch(rand() % 4){
					case 0:
						if(i != 0){
							mapBuffer[i - 1][i2][0] = 1;
							mapBuffer[i - 1][i2][1] = map[i][i2][1];
							mapBuffer[i - 1][i2][2] = map[i][i2][2];
							mapBuffer[i - 1][i2][3] = map[i][i2][3];
							texture[i - 1][i2][0] = map[i][i2][1];
							texture[i - 1][i2][1] = map[i][i2][2];
							texture[i - 1][i2][2] = map[i][i2][3];
						}
						break;
					case 1:
						if(i != resX - 1){
							mapBuffer[i + 1][i2][0] = 1;
							mapBuffer[i + 1][i2][1] = map[i][i2][1];
							mapBuffer[i + 1][i2][2] = map[i][i2][2];
							mapBuffer[i + 1][i2][3] = map[i][i2][3];
							texture[i + 1][i2][0] = map[i][i2][1];
							texture[i + 1][i2][1] = map[i][i2][2];
							texture[i + 1][i2][2] = map[i][i2][3];
						}
						break;
					case 2:
						if(i2 != 0){
							mapBuffer[i][i2 - 1][0] = 1;
							mapBuffer[i][i2 - 1][1] = map[i][i2][1];
							mapBuffer[i][i2 - 1][2] = map[i][i2][2];
							mapBuffer[i][i2 - 1][3] = map[i][i2][3];
							texture[i][i2 - 1][0] = map[i][i2][1];
							texture[i][i2 - 1][1] = map[i][i2][2];
							texture[i][i2 - 1][2] = map[i][i2][3];
						}
						break;
					case 3:
						if(i2 != resY - 1){
							mapBuffer[i][i2 + 1][0] = 1;
							mapBuffer[i][i2 + 1][1] = map[i][i2][1];
							mapBuffer[i][i2 + 1][2] = map[i][i2][2];
							mapBuffer[i][i2 + 1][3] = map[i][i2][3];
							texture[i][i2 + 1][0] = map[i][i2][1];
							texture[i][i2 + 1][1] = map[i][i2][2];
							texture[i][i2 + 1][2] = map[i][i2][3];
						}
						break;
					}
				}
			}
		}
		memcpy(map,mapBuffer,sizeof(map));
		glDrawPixels(resY,resX,GL_RGB,GL_UNSIGNED_BYTE,&texture);
		SwapBuffers(wdcontext); 
	}
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg){
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
	ShowCursor(0);
	CreateThread(0,0,Quarter1,0,0,0);
	for(int i = 0;i < resX;i++){
		for(int i2 = 0;i2 < resY;i2++){
			if(rand() % 300 == 0){
				map[i][i2][0] = 1;
				map[i][i2][1] = rand();
				map[i][i2][2] = rand();
				map[i][i2][3] = rand();
			}
		}
	}
	for(;;){
		while(PeekMessage(&Msg,hwnd,0,0,0)){
			GetMessage(&Msg, hwnd, 0, 0);
			TranslateMessage(&Msg);
			DispatchMessageW(&Msg);
		}
		Sleep(5);
	}
	return Msg.wParam;
}
