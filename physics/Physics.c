#include <windows.h>
#include <time.h>
#include <GL/gl.h>
#include <math.h>
#define resX 1080
#define resY 1920
typedef struct PARTICLE{float x;float y;float velx;float vely;} PARTICLE;
unsigned char texture[resX][resY][3];
unsigned char background[resX][resY][3];
const char className[] = "myWindowClass";
POINT mouse;
PIXELFORMATDESCRIPTOR pfd = {sizeof(PIXELFORMATDESCRIPTOR), 1,
	PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,PFD_TYPE_RGBA,
	24,0, 0, 0, 0, 0, 0,0,0,0,
	0,0,0,0,32,0,0,PFD_MAIN_PLANE,
	0,0,0,0	};
HWND hwnd;
MSG Msg;
HDC wdcontext;
int parC;
PARTICLE par[1000000];

void fillBack(int x,int y){
	x *= 10;
	y *= 10;
	for(int i = x;i < x + 10;i++){
		for(int i2 = y;i2 < y + 10;i2++){
			background[i][i2][2] = 255;
		}
	} 
}

void delPar(int ent){
	for(int i = ent;i < parC;i++){
		par[i].x = par[i + 1].x;
		par[i].y = par[i + 1].y;
		par[i].velx = par[i + 1].velx;
		par[i].vely = par[i + 1].vely;
	}
	parC--;
}
float direction;
void createPar(float x,float y,float velx,float vely){
	parC++;
	par[parC].x = x;
	par[parC].y = y;
	par[parC].velx = velx;
	par[parC].vely = vely;
}

void WINAPI Quarter1(){
	SetPixelFormat(wdcontext, ChoosePixelFormat(wdcontext, &pfd), &pfd);
	wglMakeCurrent(wdcontext, wglCreateContext(wdcontext));
	glPixelZoom(1920 / resY,1080 / resX);
	for(;;){
		if(GetAsyncKeyState(VK_LBUTTON)){
			
		}
		createPar(resX / 2,resY / 2,sinf(direction),cosf(direction));
		direction += 0.1;
		for(int i = 0;i < parC;i++){
			par[i].x += par[i].velx;
			par[i].y += par[i].vely;
			if(par[i].x < 0 || par[i].x > resX - 1 || par[i].y < 0 || par[i].y > resY - 1){
				delPar(i);
				i--;
				continue;
			}
			texture[(int)par[i].x][(int)par[i].y][0] = 255;
			texture[(int)par[i].x][(int)par[i].y][1] = (cosf(direction) + 1) * 127;
			texture[(int)par[i].x][(int)par[i].y][2] = (sinf(direction) + 1) * 127;

		}
		glDrawPixels(resY,resX,GL_RGB,GL_UNSIGNED_BYTE,&texture);
		SwapBuffers(wdcontext);  
		for(int i = 0;i < resX;i++){
			for(int i2 = 0;i2 < resY;i2++){
				if(texture[i][i2][0]){
					texture[i][i2][0]--;
				}
			}
		}
	}
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	switch (msg){
	case WM_LBUTTONDOWN:
		GetCursorPos(&mouse);
		texture[mouse.x][mouse.y][0] = 255;
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

	for(;;){
		while(PeekMessage(&Msg,hwnd,0,0,0)){
			GetMessage(&Msg, hwnd, 0, 0);
			TranslateMessage(&Msg);
			DispatchMessageW(&Msg);
		}
		Sleep(3);
	}
	return Msg.wParam;
}
