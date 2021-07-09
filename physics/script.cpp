#include "global.h"
#include <Windows.h>
#include <cmath>

const int amm = 60;
const float friction = 1.0;
const float wallfriction = 2.0;
const float gravity = 0.3;

bool button[1];
bool init = true;
float circle[amm];
float acceleration[amm];
POINT mouse;
POINT mousebuf;

void drawBall(float x,float y) {
	for (int i = 0; i < 3; i++) {
		for (int i2 = 0; i2 < 32; i2++) {
			for (int i3 = 0; i3 < 32; i3++) {
				if (ball[i][i2][i3] != 0) {
					pixels[i3 + (int)x][i2 + (int)y][i] = ball[i][i2][i3];
				}
			}
		}
	}
}

void initgame() {
	for (int i = 0; i < amm; i+=2) {
		circle[i] = i * 10 % 968;
		circle[i + 1] = i * 16 % 968;
	}
	init = false;
}

void script(HWND app) {
	if (init) {
		initgame();
	}
	for (int i = 0; i < amm; i++) {
		acceleration[i] /= friction;
	}
	if (GetKeyState(VK_LBUTTON) < 0) {
		GetCursorPos(&mouse);
		ClientToScreen(app, &mouse);
		if (button[0] == false) {
			mousebuf = mouse;
		}
		button[0] = true;
		for (int i = 0; i < 2; i += 2) {
			acceleration[i] = -(mouse.y - mousebuf.y);
			acceleration[i + 1] = mouse.x - mousebuf.x;
		}
		
		mousebuf = mouse;
	}
	else {
		button[0] = false;
	}
	for (int i = 0; i < amm; i += 2) {
		float newPos[2] = { circle[i] + acceleration[i] + gravity,circle[i + 1] + acceleration[i + 1]};
		bool col = false;
		for (int i2 = 0; i2 < amm; i2 += 2) {
			float dist[2] = { newPos[0] - circle[i2], newPos[1] - circle[i2 + 1]};
			if (sqrt(dist[0] * dist[0] + dist[1] * dist[1]) < 32 && i != i2) {
				
				float speed = (abs(acceleration[i2]) + abs(acceleration[i2 + 1]) + abs(acceleration[i]) + abs(acceleration[i + 1])) / 128;
				
				acceleration[i] = (circle[i] - circle[i2]) * speed;
				acceleration[i + 1] = (circle[i + 1] - circle[i2 + 1]) * speed;
				acceleration[i2] = (circle[i2] - circle[i]) * speed;
				acceleration[i2 + 1] = (circle[i2 + 1] - circle[i + 1]) * speed;
				bool col = true;
			}
		}
		if (col == false) {
			acceleration[i] -= gravity;
			circle[i] += acceleration[i];
			circle[i + 1] += acceleration[i + 1];
		}
	}
	for (int i = 0; i < amm; i+=2) {
		if (circle[i] < 0) {
			circle[i] = 0;
			acceleration[i] -= acceleration[i] * wallfriction;
		}
		else if (circle[i] > 968) {
			acceleration[i] -= acceleration[i] * wallfriction;
			acceleration[i] -= 0.9;
			circle[i] = 968;
		}
		else {
		}
		if (circle[i + 1] < 0) {
			acceleration[i + 1] -= acceleration[i + 1] * wallfriction;
			circle[i + 1] = 0;
		}
		else if (circle[i + 1] > 968) {
			acceleration[i + 1] -= acceleration[i + 1] * wallfriction;
			circle[i + 1] = 968;
		}
	}
	memset(pixels, 0, 3000000);
	for (int i = 0; i < amm; i+=2) {
		drawBall(circle[i], circle[i + 1]);
	}
}
