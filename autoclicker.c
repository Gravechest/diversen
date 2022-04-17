#include <windows.h>
#include <stdio.h>

void clicker(){
	INPUT inp;
	inp.type = INPUT_MOUSE;
	for(;;){
		if(GetKeyState(VK_MBUTTON) & 0x80){
			inp.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN; 
			SendInput(1,&inp,sizeof(INPUT));
			inp.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
			SendInput(1,&inp,sizeof(INPUT));
		}
		if(GetKeyState(VK_RBUTTON) & 0x80){
			inp.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
			SendInput(1,&inp,sizeof(INPUT));
			inp.mi.dwFlags = MOUSEEVENTF_LEFTUP;	
			SendInput(1,&inp,sizeof(INPUT));
		}
		Sleep(1);
	}
}

void main(){
	timeBeginPeriod(1);
	for(int i = 0;i < 4;i++){
		CreateThread(0,0,clicker,0,0,0);
	}
	for(;;){
		Sleep(1);
	}
}
