#include <windows.h>

#define STRBUF 10000

int strC;
char str[STRBUF];
int rstrC;
char rstr[STRBUF];
HANDLE ch;
HANDLE renderThread;

void u32ToStr(int p){
    int sz = 0;
	for(int i = 1;i <= p;i*= 10){
		sz++;
	}
	for(int i = 0,div = 1;i < sz;i++,div*=10){
		str[strC+sz-i] = p / div % 10 + 0x30;
	}
	str[strC+sz+1] = '\n';
    strC+=sz+1;
}

void print(){
    WriteConsoleA(ch,rstr,rstrC+1,0,0);
}

void main(){
    ch = GetStdHandle(STD_OUTPUT_HANDLE);
    for(int num = 0;;num++){
        if (num == 2 || num == 3){
			u32ToStr(num);
        }
        if (num <= 1 || num % 2 == 0 || num % 3 == 0){
            continue;
        }
        for (int check = 5; check * check <= num; check += 6){
            if (num % check == 0 || num % (check + 2) == 0){
                 goto con;
            }
        }
		u32ToStr(num);
        if(strC > STRBUF - 40){
            rstrC = strC;
            memcpy(rstr,str,strC);
            WaitForSingleObject(renderThread,INFINITE);
            renderThread = CreateThread(0,0,print,0,0,0);
            strC = 0;
        }
    con:
        continue;

    }
}

