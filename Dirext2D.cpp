#include <windows.h>
#include <chrono>
#include <random>
#include <d2d1.h>
int random(int x) {
	std::uniform_int_distribution<int> dist(0, x);
	int time = std::chrono::high_resolution_clock().now().time_since_epoch().count();
	std::default_random_engine rand(time);
	int randf = dist(rand);
	return randf;
}
const int neighbours[8]{
	-41,
	-40,
	-39,
	-1,
	1,
	39,
	40,
	41
};
const int lvlsize = 1600;
int xdata[1600],ydata[1600],IDdata[1600],IDdatabuf[1600],cnt;
LRESULT CALLBACK WndProc(HWND app, UINT msg, WPARAM wparam, LPARAM lparam);
int WINAPI wWinMain(
	HINSTANCE hInstance,
	HINSTANCE previnstance,
	LPTSTR cmdline,
	int cmdshow
) {
	WNDCLASS window = {};
	window.lpfnWndProc = WndProc;
	window.lpszClassName = L"yeetboi2";
	window.hInstance = hInstance;
	RegisterClass(&window);
	HWND app = CreateWindowEx(0, L"yeetboi2", L"yeetboi", WS_OVERLAPPEDWINDOW, 400, 400, 400, 400, NULL, NULL, hInstance, NULL);
	ShowWindow(app, cmdshow);
	PAINTSTRUCT verf;
	HDC pixel = BeginPaint(app, &verf);
	HBRUSH redkwast = CreateSolidBrush(RGB(255, 0, 0));
	HBRUSH blackkwast = CreateSolidBrush(RGB(0, 0, 0));
	ID2D1Factory* factory;
	ID2D1RenderTarget* render;
	ID2D1HwndRenderTarget* scrnrender;
	ID2D1SolidColorBrush* kwast;
	D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &factory);
	factory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(app),&scrnrender);
	render->BeginDraw();
	render->CreateSolidColorBrush(D2D1::ColorF(1.0, 0.0, 0.0, 1.0), &kwast);
	render->DrawRectangle(D2D1::RectF(0.0, 0.5, 0.7, 0.3), kwast);
	for (int i = 0; i < lvlsize; i++) {
		xdata[i] = i % 40;
		ydata[i] = i / 40;
		IDdata[i] = random(1);
	}
	while (true) {
		for (int i = 0; i < lvlsize; i++) {
			if (IDdata[i] == 1) {
				RECT rect = { xdata[i] * 10,ydata[i] * 10,xdata[i] * 10 + 10,ydata[i] * 10 + 10 };
				FillRect(pixel, &rect, redkwast);
			}
			else {
				RECT rect = { xdata[i] * 10,ydata[i] * 10,xdata[i] * 10 + 10,ydata[i] * 10 + 10 };
				FillRect(pixel, &rect, blackkwast);
			}
		}
		for (int i = 0; i < lvlsize; i++) {
			cnt = 0;
			for (int i2 = 0; i2 < 8; i2++) {
				if (IDdata[i + neighbours[i2]] == 1) {
						cnt += 1;
					
				}
			}
			if (IDdata[i] == 0) {
				if (cnt == 3) {
					IDdatabuf[i] = 1;
				}
			}
			else {
				if (cnt != 2 && cnt != 3) {
					IDdatabuf[i] = 0;
				}
				else {
					IDdatabuf[i] = 1;
				}
			}
		}
		memcpy(IDdata, IDdatabuf, 6400);
	}
}
LRESULT CALLBACK WndProc(HWND app, UINT msg, WPARAM wparam, LPARAM lparam) {
	return DefWindowProc(app, msg, wparam, lparam);
}
