#include <windows.h>
#include <stdio.h>

#define INS_MOVMI 26
#define INS_ANDM  25
#define INS_AND   24
#define INS_DEC   23
#define INS_INCM  22
#define INS_INC   21
#define INS_JEQM  20
#define INS_JNTM  19
#define INS_JNT   18
#define INS_JEQ   17
#define INS_JMPM  16
#define INS_JMP   15
#define INS_DIVM  14
#define INS_DIV   13
#define INS_MULM  12
#define INS_MUL   11
#define INS_SUBM  10
#define INS_SUB   9
#define INS_ADDM  8
#define INS_ADD   7
#define INS_MOVRR 6
#define INS_MOV   5
#define INS_INP   4
#define INS_OTPM  3
#define INS_OTP   2
#define INS_PSZ   1
#define INS_HLT   0

#define TYPE_FLOAT 0x04
#define TYPE_ARRAY 0x08

typedef char i8;
typedef unsigned char u8;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef struct{
	u64 data;
	i8* name;
}VALDEF;

VALDEF* valdef;

u8* code;

const i8* insNames;

void* programMem;

i8* script;

i8* u32ToStr(u32 p){
	u32 sz = 0;
	for(u32 i = 1;i <= p;i*= 10){
		sz++;
	}
	i8* str = HeapAlloc(GetProcessHeap(),8,sz+1);
	for(u32 i = 0,div = 1;i < sz;i++,div*=10){
		str[sz-i] = p / div % 10 + 0x30;
	}
	return str;
}

u32 strToU32(i8* p,u32* itt){
	u32 r = 0;
	switch(p[0]){
	case '\'':
		*itt+=4;
		if(p[1] == '\\'){
			switch(p[2]){
			case 'n':
				return '\n';
			}
		}
		r = p[1];
		return r;
	default:
		for(u32 i = 0;p[i] >= '0' && p[i] <= '9';i++){
			r *= 10;
			r += p[i] - '0';
			*itt+=1;
		}
		*itt+=1;
		return r;
	}
}

u32 getWordSz(i8* str){
	u32 r = 0;
	while(str[r] >= 'a' && str[r] <= 'z'){
		r++;
	}
	return r;
}

u32 getStrSz(i8* str){
	u32 r = 0;
	while(str[r] != '"'){
		r++;
	}
	return r;
}

u32 skipWord(i8* p){
	u32 r = 0;
	while(p[r] >= 'a' && p[r] <= 'z'){
		r++;
	}
	return r;
}

u32 skipEmptyness(i8* p){
	u32 r = 3;
	while(p[r] == ' ' || p[r] == '\t'){
		r++;
	}
	return r;
}

void parseVal(u32* ptr,u32* codeC,u32* valdefC,u8 ins,u8 bptr){
	if(script[*ptr] == '@'){
		code[*codeC-bptr] = ins;
		(*ptr)++;
		if(script[*ptr] >= 'a' && script[*ptr] <= 'z'){
			u32 wsz = getWordSz(script+*ptr);
			for(u32 i = 0;i < *valdefC;i++){
				if(!memcmp(script+*ptr,valdef[i].name,wsz)){
					code[(*codeC)++] = valdef[i].data;
					break;
				}
			}
			ptr+=wsz+1;
		}
		code[(*codeC)++] = strToU32(script+*ptr,ptr);
	}
	else if(script[*ptr] >= 'a' && script[*ptr] <= 'z'){
		u32 wsz = getWordSz(script+*ptr);
		for(u32 i = 0;i < *valdefC;i++){
			if(!memcmp(script+*ptr,valdef[i].name,wsz)){
				code[(*codeC)++] = valdef[i].data;
				break;
			}
		}
		ptr+=wsz+1;
	}
	else{
		code[(*codeC)++] = strToU32(script+*ptr,ptr);
	}
}


void main(){
	code       = HeapAlloc(GetProcessHeap(),8,255);
	valdef	   = HeapAlloc(GetProcessHeap(),8,sizeof(VALDEF)*255);
	programMem = HeapAlloc(GetProcessHeap(),8,255);

	HANDLE file = CreateFileA("main.ts",GENERIC_READ,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
	HANDLE conHdl  = GetStdHandle(STD_OUTPUT_HANDLE);
	HANDLE conHdli = GetStdHandle(STD_INPUT_HANDLE);
	u32 size = GetFileSize(file,0);
	script = HeapAlloc(GetProcessHeap(),8,size);
	ReadFile(file,script,size,0,0);
	u8 varC = 0;
	u32 wordSz = 0,ptr = 0,codeC = 0,valdefC = 0;
	for(;ptr < size;ptr++){
		if(!memcmp("hlt ",script+ptr,4)){
			codeC++;
		}
		if(!memcmp("dec ",script+ptr,4) || !memcmp("inc ",script+ptr,4)){
			codeC+=2;
		}
		if(!memcmp("jnt ",script+ptr,4) || !memcmp("jeq ",script+ptr,4)){
			codeC+=4;
		}
		if(!memcmp("inp ",script+ptr,4) || !memcmp("otp ",script+ptr,4)){
			codeC+=2;
		}
		if(!memcmp("jmp ",script+ptr,4)){
			codeC+=2;
		}
		if(!memcmp("add ",script+ptr,4) || !memcmp("mul ",script+ptr,4) || !memcmp("div ",script+ptr,4) || !memcmp("sub ",script+ptr,4)){
			codeC+=3;
		}
		if(!memcmp("and ",script+ptr,4)){
			codeC+=3;
		}
		if(!memcmp("psz ",script+ptr,4)){
			codeC+=2;
		}
		if(!memcmp("mov ",script+ptr,4)){
			codeC+=3;
		}
		if(!memcmp("#df ",script+ptr,4)){
			ptr += skipEmptyness(script+ptr);
			u32 wsz = getWordSz(script+ptr);
			valdef[valdefC].name = HeapAlloc(GetProcessHeap(),8,wsz+1);
			memcpy(valdef[valdefC].name,script+ptr,wsz);
			ptr += wsz;
			ptr += skipEmptyness(script+ptr);
			valdef[valdefC++].data = strToU32(script+ptr,&ptr);
		}
		if(!memcmp("#lb ",script+ptr,4)){
			ptr += skipEmptyness(script+ptr);
			u32 wsz = getWordSz(script+ptr);
			valdef[valdefC].name = HeapAlloc(GetProcessHeap(),8,wsz+1);
			memcpy(valdef[valdefC].name,script+ptr,wsz);
			valdef[valdefC++].data = codeC;
			ptr += wsz;
		}
		if(!memcmp("#db ",script+ptr,4)){
			ptr += skipEmptyness(script+ptr);
			u32 memLoc = strToU32(script+ptr,&ptr);
			if(script[ptr] == '"'){
				ptr++;
				for(;script[ptr] != '"';ptr++,memLoc++){
					((i8*)programMem)[memLoc] = script[ptr];
				}
			}
		}
	}
	codeC = 0;
	ptr = 0;
	for(;ptr < size;ptr++){
		if(!memcmp("sub ",script+ptr,4)){
			ptr += skipEmptyness(script+ptr);
			code[codeC++] = INS_SUB;
			code[codeC++] = strToU32(script+ptr,&ptr);
			parseVal(&ptr,&codeC,&valdefC,INS_SUBM,2);
		}
		if(!memcmp("and ",script+ptr,4)){
			ptr += skipEmptyness(script+ptr);
			code[codeC++] = INS_AND;
			code[codeC++] = strToU32(script+ptr,&ptr);
			parseVal(&ptr,&codeC,&valdefC,INS_ANDM,2);
		}
		if(!memcmp("mul ",script+ptr,4)){
			ptr += skipEmptyness(script+ptr);
			code[codeC++] = INS_MUL;
			code[codeC++] = strToU32(script+ptr,&ptr);
			parseVal(&ptr,&codeC,&valdefC,INS_MULM,2);
		}
		if(!memcmp("div ",script+ptr,4)){
			ptr += skipEmptyness(script+ptr);
			code[codeC++] = INS_DIV;
			code[codeC++] = strToU32(script+ptr,&ptr);
			parseVal(&ptr,&codeC,&valdefC,INS_DIVM,2);
		}
		if(!memcmp("dec ",script+ptr,4)){
			ptr += skipEmptyness(script+ptr);
			code[codeC++] = INS_DEC;
			code[codeC++] = strToU32(script+ptr,&ptr);
		}
		if(!memcmp("inc ",script+ptr,4)){
			ptr += skipEmptyness(script+ptr);
			code[codeC++] = INS_INC;
			parseVal(&ptr,&codeC,&valdefC,INS_INCM,1);
		}
		if(!memcmp("jnt ",script+ptr,4)){
			ptr += skipEmptyness(script+ptr);
			code[codeC++] = INS_JNT;
			code[codeC++] = strToU32(script+ptr,&ptr);
			parseVal(&ptr,&codeC,&valdefC,INS_JNTM,2);
			u32 wsz = getWordSz(script+ptr);
			for(u32 i = 0;i < valdefC;i++){
				if(!memcmp(script+ptr,valdef[i].name,wsz)){
					code[codeC++] = valdef[i].data;
					break;
				}
			}
			ptr+=wsz;
		}
		if(!memcmp("jeq ",script+ptr,4)){
			ptr += skipEmptyness(script+ptr);
			code[codeC++] = INS_JEQ;
			code[codeC++] = strToU32(script+ptr,&ptr);
			parseVal(&ptr,&codeC,&valdefC,INS_JEQM,2);
			parseVal(&ptr,&codeC,&valdefC,0,3);
		}
		if(!memcmp("hlt ",script+ptr,4)){
			codeC++;
		}
		if(!memcmp("inp ",script+ptr,4)){
			ptr += skipEmptyness(script+ptr);
			code[codeC++] = INS_INP;
			code[codeC++] = strToU32(script+ptr,&ptr);
		}
		if(!memcmp("jmp ",script+ptr,4)){
			ptr += skipEmptyness(script+ptr);
			code[codeC++] = INS_JMP;
			parseVal(&ptr,&codeC,&valdefC,INS_JMPM,1);
		}
		if(!memcmp("add ",script+ptr,4)){
			ptr += skipEmptyness(script+ptr);
			code[codeC++] = INS_ADD;
			code[codeC++] = strToU32(script+ptr,&ptr);
			parseVal(&ptr,&codeC,&valdefC,INS_ADDM,2);
		}
		if(!memcmp("otp ",script+ptr,4)){
			ptr += skipEmptyness(script+ptr);
			code[codeC++] = INS_OTP;
			parseVal(&ptr,&codeC,&valdefC,INS_OTPM,1);
		}
		if(!memcmp("mov ",script+ptr,4)){
			ptr += skipEmptyness(script+ptr);
			code[codeC++] = INS_MOV;
			parseVal(&ptr,&codeC,&valdefC,INS_MOVMI,1);
			parseVal(&ptr,&codeC,&valdefC,INS_MOVRR,2);
		}
	}
	for(u32 i = 0;i < codeC;){
		switch(code[i++]){
		case INS_SUB:
			((i8*)programMem)[code[i]] -= code[i+1];
			i+=2;
			break;
		case INS_SUBM:
			((i8*)programMem)[code[i]] -= ((i8*)programMem)[code[i+1]];
			i+=2;
			break;
		case INS_INCM:
			((i8*)programMem)[((i8*)programMem)[code[i++]]]++;
			break;
		case INS_OTPM:
			WriteConsoleA(conHdl,&((i8*)programMem)[((i8*)programMem)[code[i++]]],1,0,0);
			break;
		case INS_ANDM:
			((i8*)programMem)[code[i]] &= ((i8*)programMem)[code[i+1]];
			i+=2;
			break;
		case INS_AND:
			((i8*)programMem)[code[i]] &= code[i+1];
			i+=2;
			break;
		case INS_DIVM:
			((i8*)programMem)[code[i]] /= ((i8*)programMem)[code[i+1]];
			i+=2;
			break;
		case INS_MULM:
			((i8*)programMem)[code[i]] *= ((i8*)programMem)[code[i+1]];
			i+=2;
			break;
		case INS_DIV:
			((i8*)programMem)[code[i]] /= code[i+1];
			i+=2;
			break;
		case INS_MUL:
			((i8*)programMem)[code[i]] *= code[i+1];
			i+=2;
			break;
		case INS_INC:
			((i8*)programMem)[code[i++]]++;
			break;
		case INS_DEC:
			((i8*)programMem)[code[i++]]--;
			break;
		case INS_JMPM:
			i = ((i8*)programMem)[code[i]];
			break;
		case INS_JEQM:
			if(((i8*)programMem)[code[i]] == ((i8*)programMem)[code[i+1]]){
				i = code[i+2];
			}
			else{
				i+=3;
			}
			break;
		case INS_JNTM:
			if(((i8*)programMem)[code[i]] != ((i8*)programMem)[code[i+1]]){
				i = code[i+2];
			}
			else{
				i+=3;
			}
			break;
		case INS_JNT:
			if(((i8*)programMem)[code[i]] != code[i+1]){
				i = code[i+2];
			}
			else{
				i+=3;
			}
			break;
		case INS_JEQ:
			if(((i8*)programMem)[code[i]] == code[i+1]){
				i = code[i+2];
			}
			else{
				i+=3;
			}
			break;
		case INS_MOVMI:
			((i8*)programMem)[((i8*)programMem)[code[i]]] = code[i+1];
			i+=2;
			break;
		case INS_MOVRR:
			((i8*)programMem)[code[i]] = ((i8*)programMem)[code[i+1]];
			i+=2;
			break;
		case INS_ADDM:
			((i8*)programMem)[code[i]] += ((i8*)programMem)[code[i+1]];
			i+=2;
			break;
		case INS_INP:{
			i8 b[4];
			int n;
			ReadConsoleA(conHdli,b,4,&n,0);
			((i8*)programMem)[code[i++]] = b[0];
			break;
		}
		case INS_JMP:
			i = code[i];
			break;
		case INS_ADD:
			((i8*)programMem)[code[i]] += code[i+1];
			i+=2;
			break;
		case INS_MOV:
			((i8*)programMem)[code[i]] = code[i+1];
			i+=2;
			break;
		case INS_OTP:
			WriteConsoleA(conHdl,(i8*)programMem+code[i++],1,0,0);
			break;
		case INS_HLT:
			ExitProcess(0);
		case INS_PSZ:
			programMem = HeapReAlloc(GetProcessHeap(),8,programMem,code[i++]);
			break;
		}
	}
}
