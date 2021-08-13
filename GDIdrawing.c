#include <windows.h>
#define resX 480
#define resY 640
unsigned char texture[resX][resY][3];
const char className[] = "myWindowClass";
HWND hwnd;
MSG Msg;
int Ptime;
unsigned int threadId;
BITMAPINFO bmi = { sizeof(BITMAPINFOHEADER),resY,resX,1,24,BI_RGB };
void WINAPI applyToScreen(HDC wdcontext) {
	for (;;) {
		StretchDIBits(wdcontext, 0, 0, 1920, 1080, 0, 0, resY, resX, &texture, &bmi, DIB_RGB_COLORS, SRCCOPY);
	}
}
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE:
		exit(0);
	case WM_DESTROY:
		exit(0);
		break;
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
	HDC wdcontext = GetDC(hwnd);
	UpdateWindow(hwnd);
	ShowCursor(0);
	CreateThread(0, 0, applyToScreen, wdcontext, 0, &threadId);
	float offset = 0;
	for (;;) {
		while (PeekMessage(&Msg, hwnd, 0, 0, 0)) {
			GetMessage(&Msg, hwnd, 0, 0);
			TranslateMessage(&Msg);
			DispatchMessageW(&Msg);
		}
		for (int i = 0; i < resX; i++) {
			for (int i2 = 0; i2 < resY; i2++) {
				float x = (float)i / resX / 2 - 1 + offset;
				float y = (float)i2 / resY / 2 - 1;
				float xa = 0;
				float ya = 0;
				int itt = 0;
				while (xa + ya < 256 && itt < 256) {
					float xtemp = xa * xa - ya * ya + x;
					ya = 2 * xa * ya + y;
					xa = xtemp;
					itt++;
				}
				texture[i][i2][0] = itt;
			}
		}
		offset += 0.01;
	}
	return Msg.wParam;
}
