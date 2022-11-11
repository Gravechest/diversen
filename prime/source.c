#include <windows.h>

int strC;
char str[1000];

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

void main(){
    HANDLE ch = GetStdHandle(STD_OUTPUT_HANDLE);
    for(int num = 0;;num++){
        if (num == 2 || num == 3){
			u32ToStr(num);
            if(strC > 200){
                WriteConsoleA(ch,str,strC+1,0,0);
                strC = 0;
            }
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
        if(strC > 200){
            WriteConsoleA(ch,str,strC+1,0,0);
            strC = 0;
        }
    con:
        continue;
    }
}
