#include <windows.h>
#include <gl/GL.h>
#include <math.h>
typedef float vec3[3];
unsigned char texture[250][250][3];
LRESULT CALLBACK WndProc(HWND app, UINT msg, WPARAM wparam, LPARAM lparam);
int WINAPI wWinMain(
	HINSTANCE hInstance,
	HINSTANCE previnstance,
	LPTSTR cmdline,
	int cmdshow

) {
	PIXELFORMATDESCRIPTOR pfd = {
	sizeof(PIXELFORMATDESCRIPTOR), 1,
	PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,PFD_TYPE_RGBA,
	24,0, 0, 0, 0, 0, 0,0,0,0,
	0,0,0,0,32,0,0,PFD_MAIN_PLANE,
	0,0,0,0
	};
	MSG msg;
	WNDCLASS window = {};
	window.lpfnWndProc = WndProc;
	window.lpszClassName = L"yeetboi2";
	window.hInstance = hInstance;
	RegisterClass(&window);
	HWND app = CreateWindowEx(0, L"yeetboi2", L"yeetboi", WS_OVERLAPPEDWINDOW, 200, 200, 517, 537, NULL, NULL, hInstance, NULL);
	ShowWindow(app, cmdshow);
	HDC pixel = GetDC(app);
	SetPixelFormat(pixel, ChoosePixelFormat(pixel, &pfd), &pfd);
	wglMakeCurrent(pixel, wglCreateContext(pixel));
	glPixelZoom(2, 2);
	vec3 rayxy[500];
	for (int i = 0; i < 500; i++) {
		rayxy[i][0] = cosf((float)i / 150);
		rayxy[i][1] = sinf((float)i / 150);
		rayxy[i][2] = (float)i / 150 - 0.5;
	}
	for (int i = 0; i < 500; i++) {
		for (int i2 = 0; i2 < 500; i2++) {
			unsigned char bounce = 1;
			vec3 addVal = { rayxy[i2][0],rayxy[i2][1],rayxy[i][2] };
			vec3 ray = { 0,0,0  };
			for (int i3 = 0; i3 < 20; i3++) {
				ray[0] += addVal[0];
				ray[1] += addVal[1];
				ray[2] += addVal[2];
				if (ray[2] < -1) {
					texture[i][i2][2] += 255;
					break;
				}
			}
		}
	}
	for (;;) {
		while (PeekMessage(&msg, app, 0, 0, 0) != 0) {
			GetMessage(&msg, app, 0, 0);
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
		glDrawPixels(500, 500, GL_RGB, GL_UNSIGNED_BYTE, &texture);
		SwapBuffers(pixel);
	}
}
LRESULT CALLBACK WndProc(HWND app, UINT msg, WPARAM wparam, LPARAM lparam) {
	return DefWindowProc(app, msg, wparam, lparam);
}
