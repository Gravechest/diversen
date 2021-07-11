#include <windows.h>
#include <cmath>
#include "global.h"
#include <chrono>
#include <random>

bool init = true;
float Rot[2];
char tiles[100][100];
char tilesbuf[100][100];
char tileType[100][100];

int random(int x) {
	std::uniform_int_distribution<int> dist(0, x);
	int time = std::chrono::high_resolution_clock().now().time_since_epoch().count();
	std::default_random_engine rand(time);
	return dist(rand);
	
}

void drawPic(int x, int y) {
	for (int i = x * 10; i < x * 10 + 10; i++) {
		for (int i2 = y * 10; i2 < y * 10 + 10; i2++) {
			for (int i3 = 0; i3 < 3; i3++) {
				pixels[i][i2][i3] = mountain[i3][i % 10][i2 % 10];
			}
		}
	}
}

void fillRect(int x,int y,int r,int g,int b) {
	for (int i = x * 10; i < x * 10 + 10; i++) {
		for (int i2 = y * 10; i2 < y * 10 + 10; i2++) {
			if (pixels[i][i2][0] == 0 && pixels[i][i2][1] == 0 && pixels[i][i2][2] == 0) {
				pixels[i][i2][0] = r;
				pixels[i][i2][1] = g;
				pixels[i][i2][2] = b;
			}
		}
	}
}

void initGame() {
	tiles[0][0] = 1;
	tiles[99][99] = 2;
	tiles[0][99] = 3;
	tiles[99][0] = 4;
	for(int i = 0;i < 18;i++)
	for (int i2 = 0; i2 < 100; i2++) {
		for (int i3 = 0; i3 < 100; i3++) {
			srand(std::chrono::high_resolution_clock().now().time_since_epoch().count());
			if (rand() % 6000 == 0) {
				tileType[i2][i3] = 1;
			}
			else if (rand() % 5 == 0 && (tileType[i2 - 1][i3] == 1 || tileType[i2 + 1][i3] == 1
			|| tileType[i2][i3 + 1] == 1 || tileType[i2][i3 - 1] == 1)) {
				tileType[i2][i3] = 1;
			}
		}
	}
}

void script(HWND app) {
	if (init) {
		initGame();
		init = false;
	}
	memcpy(tilesbuf, tiles, 10000);
	for (int i = 0; i < 100; i++) {
		srand(std::chrono::high_resolution_clock().now().time_since_epoch().count());
		for (int i2 = 0; i2 < 100; i2++) {
			int change = 50;
			switch (tileType[i][i2]) {
			case 1:
				change = 1200;
				break;
			}
			if (tiles[i][i2] != 0) {
				switch (rand() % change) {
				case 0:
					if (tiles[i + 1][i2] != tiles[i][i2] && i < 99) {
						tilesbuf[i + 1][i2] = tiles[i][i2];
					}
					else if (tiles[i][i2 + 1] != tiles[i][i2] && i2 < 99) {
						tilesbuf[i][i2 + 1] = tiles[i][i2];
					}
					else if (tiles[i - 1][i2] != tiles[i][i2] && i > 0) {
						tilesbuf[i - 1][i2] = tiles[i][i2];
					}
					else if (tiles[i][i2 - 1] != tiles[i][i2] && i2 > 0) {
						tilesbuf[i][i2 - 1] = tiles[i][i2];
					}
					break;
				case 1:
					if (tiles[i][i2 + 1] != tiles[i][i2] && i2 < 99) {
						tilesbuf[i][i2 + 1] = tiles[i][i2];
					}
					else if (tiles[i - 1][i2] != tiles[i][i2] && i > 0) {
						tilesbuf[i - 1][i2] = tiles[i][i2];
					}
					else if (tiles[i][i2 - 1] != tiles[i][i2] && i2 > 0) {
						tilesbuf[i][i2 - 1] = tiles[i][i2];
					}
					else if (tiles[i + 1][i2] != tiles[i][i2] && i < 99) {
						tilesbuf[i + 1][i2] = tiles[i][i2];
					}
					break;
				case 2:
					if (tiles[i - 1][i2] != tiles[i][i2] && i > 0) {
						tilesbuf[i - 1][i2] = tiles[i][i2];
					}
					else if (tiles[i][i2 - 1] != tiles[i][i2] && i2 > 0) {
						tilesbuf[i][i2 - 1] = tiles[i][i2];
					}
					else if (tiles[i + 1][i2] != tiles[i][i2] && i < 99) {
						tilesbuf[i + 1][i2] = tiles[i][i2];
					}
					else if (tiles[i][i2 + 1] != tiles[i][i2] && i2 < 99) {
						tilesbuf[i][i2 + 1] = tiles[i][i2];
					}
					break;
				case 3:
					if (tiles[i][i2 - 1] != tiles[i][i2] && i2 > 0) {
						tilesbuf[i][i2 - 1] = tiles[i][i2];
					}
					else if (tiles[i + 1][i2] != tiles[i][i2] && i < 99) {
						tilesbuf[i + 1][i2] = tiles[i][i2];
					}
					else if (tiles[i][i2 + 1] != tiles[i][i2] && i2 < 99) {
						tilesbuf[i][i2 + 1] = tiles[i][i2];
					}
					else if (tiles[i - 1][i2] != tiles[i][i2] && i > 0) {
							tilesbuf[i - 1][i2] = tiles[i][i2];
					}
					break;
				}
			}
		}
	}
	memcpy(tiles, tilesbuf, 10000);
	memset(pixels, 0, 3000000);
	for (int i = 0;i < 100;i++) {
		for (int i2 = 0;i2 < 100;i2++) {
			switch (tileType[i][i2]) {
			case 1:
				drawPic(i, i2);
				break;
			}
			switch (tiles[i][i2]) {
			case 1:
				fillRect(i,i2,255,0,0);
				break;
			case 2:
				fillRect(i, i2, 0, 255, 0);
				break;
			case 3:
				fillRect(i, i2, 255, 255, 0);
				break;
			case 4:
				fillRect(i, i2, 0, 0, 255);
				break;
			}
		}
	}
}
