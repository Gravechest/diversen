#include <Windows.h>
#include <chrono>
#include <random>
#include <thread>
#include "global.h" 
int size = 3;
int direction = 0;
int id[400];
int lifespan[400];
int idbuf[400];
int lifespanbuf[400];
PAINTSTRUCT verf;
RECT rect;
HBRUSH kwast;
int random(int x) {
    std::uniform_int_distribution<int> dist(0, x);
    int time = std::chrono::high_resolution_clock().now().time_since_epoch().count();
    std::default_random_engine rand(time);
    int randf = dist(rand);
    return randf;
}
HDC init_game(HWND app) {
    memset(id, 0, 1600);
    memset(lifespan, 0, 1600);
    memset(idbuf, 0, 1600);
    memset(lifespanbuf, 0, 1600);
    id[random(400)] = 2;
    id[20] = 1;
    lifespan[20] = size;
    return BeginPaint(app, &verf);
}
void snake_mov(int itt,int dir) {
    int nwpcs = itt + dir;
    if (itt % 20 == 19 && nwpcs % 20 == 0) {
        return;
    }
    if (itt % 20 == 0 && nwpcs % 20 == 19) {
        return;
    }
    if (id[nwpcs] == 1) {
        return;
    }
    if (id[nwpcs] == 2) {
        size += 1;
        idbuf[random(400)] = 2;
    }
    idbuf[nwpcs] = 1;
    lifespanbuf[nwpcs] = size;
}
void the_game(HDC pixel) {
    memcpy(lifespanbuf, lifespan, 1600);
    memcpy(idbuf, id, 1600);
    for (int i = 0; i < 400; i++) {
        if (lifespan[i] == size) {
            switch (direction) {
            case 0:
                snake_mov(i, 1);
                break;
            case 1:
                snake_mov(i, -1);
                break;
            case 2:
                snake_mov(i, 20);
                break;
            case 3:
                snake_mov(i, -20);
                break;
            }
        }
        if (lifespan[i] > 0) {
            lifespanbuf[i] = lifespan[i] - 1;
            if (lifespanbuf[i] == 0) {
                idbuf[i] = 0;
            }
        }
    }
    memcpy(lifespan, lifespanbuf, 1600);
    memcpy(id, idbuf, 1600);
    rect = { 0,0,400,400 };
    FillRect(pixel, &rect, CreateSolidBrush(RGB(255, 255, 255)));
    for (int i = 0; i < 400; i++) { 
        if (id[i] != 0) {
            switch (id[i]) {
            case 1:
                kwast = CreateSolidBrush(RGB(0, 0, 0));
                break;
            case 2:
                kwast = CreateSolidBrush(RGB(255, 0, 0));
                break;
            }
            int offsety = i * 20 % 400;
            int offsetx = i / 20 * 20;
            rect = { offsety,offsetx ,offsety + 20,offsetx + 20 };
            FillRect(pixel, &rect, kwast);
        }
    }
    if (GetKeyState(0x44) < 0) {
        direction = 0;
    }
    if (GetKeyState(0x41) < 0) {
        direction = 1;
    }
    if (GetKeyState(0x53) < 0) {
        direction = 2;
    }
    if (GetKeyState(0x57) < 0) {
        direction = 3;
    }
    Sleep(100);
}
