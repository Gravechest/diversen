#pragma once
#include <windows.h>
void the_game(HDC pixel);
HDC init_game(HWND app,char food);
char network_init();
int socket_data(char data);
int new_food();
extern char wormID;
