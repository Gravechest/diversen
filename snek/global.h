#pragma once
#include <windows.h>
#include <vector>
void newWorm();
void the_game(HDC pixel);
HDC init_game(HWND app,int food);
int network_init();
void socket_data(char data);
int new_food();
extern std::vector<int> wormLoc;
extern char wormID;
extern std::vector<char> wormDir;
