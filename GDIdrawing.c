#include <windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
unsigned char texture[1920][1080][4];
const char g_szClassName[] = "myWindowClass";
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
        case WM_CLOSE:
			exit(0);
        break;
        case WM_DESTROY:
            PostQuitMessage(0);
        break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
{
	BITMAPINFO bmi = {sizeof(BITMAPINFOHEADER),1920,1080,1,32,BI_RGB};
    WNDCLASSEX wc;
    HWND hwnd;
    MSG Msg;
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = 0;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = 0;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = g_szClassName;
    wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);
	PIXELFORMATDESCRIPTOR pfd = {
	sizeof(PIXELFORMATDESCRIPTOR), 1,
	PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,PFD_TYPE_RGBA,
	24,0, 0, 0, 0, 0, 0,0,0,0,
	0,0,0,0,0,0,0,PFD_MAIN_PLANE,
	0,0,0,0
	};
    RegisterClassEx(&wc);
    hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        g_szClassName,
        "raycasting",
        WS_VISIBLE | WS_POPUP | WS_EX_TOPMOST,
        0, 0, 1920, 1080,
        NULL, NULL, hInstance, NULL);
	HDC wdcontext = GetDC(hwnd);
    UpdateWindow(hwnd);
	ShowCursor(0);
	SetPixelFormat(wdcontext, ChoosePixelFormat(wdcontext, &pfd), &pfd);
	wglMakeCurrent(wdcontext, wglCreateContext(wdcontext));
	memset(texture,100,sizeof(texture));
    for(;;){
		while (PeekMessage(&Msg, hwnd, 0, 0, 0)) {
			GetMessage(&Msg, hwnd, 0, 0);
			TranslateMessage(&Msg);
			DispatchMessageW(&Msg);
		}
		StretchDIBits(wdcontext,0,0,1920,1080,0,0,1920,1080,&texture,&bmi,DIB_RGB_COLORS,SRCCOPY);
    }
    return Msg.wParam;
}
