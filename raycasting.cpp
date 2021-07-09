#include <Windows.h>
#include "global.h"
#include <cmath>
bool init = true;
char level[100][100];
float Pos[2] = {0,0};
float Rot = 0;
int Ptime;
POINT mouse;
POINT mousebuf;

char pixelsbuf[simSize][simSize][3];

void gameinit() {
	for (int i = 0; i < 100; i++) {
		for (int i2 = 0; i2 < 100; i2++) {
			if (rand() % 5 == 0) {
				level[i][i2] = rand() % 3;
			}
		}
	}
	GetCursorPos(&mouse);
	mousebuf = mouse;
	ShowCursor(false);
}

void Movement(float x,float y) {
	if (level[(int)(Pos[0] + x)][(int)(Pos[1] + y)] == 0) {
		Pos[0] += x / 10;
		Pos[1] += y / 10;
	}
	else if (level[(int)(Pos[0] + x)][(int)Pos[1]] == 0) {
		Pos[0] += y / 10;
	}
	else if (level[(int)Pos[0]][(int)(Pos[1] + y)] == 0) {
		Pos[1] += x / 10;
	}
}

void script(HWND app) {
	if (init) {
		gameinit();
		init = false;
	}
	GetCursorPos(&mouse);
	Rot += (float)(mouse.x - mousebuf.x) / 90;
	mousebuf = mouse;
	memset(pixels, 0, 3000000);
	if (GetKeyState(0x57) < 0) {
		Movement(cos(Rot), sin(Rot + 0.6));
	}
	if (GetKeyState(0x53) < 0) {
		Movement(-cos(Rot), -sin(Rot + 0.6));
	}
	if (GetKeyState(0x41) < 0) {
		Movement(cos(Rot - 0.9), sin(Rot - 0.9));
	}
	if (GetKeyState(0x44) < 0) {
		Movement(-cos(Rot - 0.9), -sin(Rot - 0.9));
	}
	for (float i = 0; i < 500; i++) {
		float ray[2] = {Pos[0],Pos[1]}; 
		float raydist[2] = {cos(i / 600 + Rot) / 100,sin(i / 600 + Rot) / 100};
		for (int i2 = 0; i2 < 10000; i2++) {
			ray[0] += raydist[0];

			ray[1] += raydist[1];
			if (ray[0] > 100 || ray[1] > 100 || ray[0] < 0 || ray[1] < 0) {
				continue;
			}
			if(level[(int)ray[0]][(int)ray[1]] != 0) {
				for (int i3 = 500 - 500 / ((float)i2 / 50 + 1); i3 < 500 + 500 / ((float)i2 / 50 + 1); i3++) {
					for (int i4 = 0; i4 < 2; i4++) {
						switch (level[(int)ray[0]][(int)ray[1]]) {
						case 1:
							pixels[i3][(int)i * 2 + i4][0] = 255 / ((float)i2 / 400 + 1);
							break;
						case 2:
							pixels[i3][(int)i * 2 + i4][1] = 255 / ((float)i2 / 400 + 1);
							break;
						}
					}
				}
				break;
			}
		}
	}
	Ptime++;
}
