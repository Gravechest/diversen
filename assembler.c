#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

STARTUPINFO startupinfo = {sizeof(STARTUPINFO),0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
PROCESS_INFORMATION process_info = {0,0,0,0};

const int headerSz = 0x000000fc;

int exedataSz;
int asmSz;
int asmOffset;
int dataOffset;
int nameOffset;

char *asm;
char *exedata;

FILE *file;

typedef struct VARIABLE{
	int size;
	int *ref;
	int count;
	char flags;
	char *data;
}VARIABLE;

typedef struct LIBRARY{
	int namesize;
	int first;
	char *name;
}LIBRARY;

typedef struct FUNCTION{
	int namesize;
	int lib;
	int mempos;
	char *name;
}FUNCTION;

typedef struct FLINKING{
	int pos;
	int type;
}FLINKING;

int libsCount;
int funcCount;
int linkCount;
int varCount;

LIBRARY *libs;
FUNCTION *func;
FLINKING *link;
VARIABLE *var;

inline void printx(int val){
	printf("%x",val);
	printf("\n");
}

char header[28] = {
	0x4d,0x5a,0x00,0x00,     									//MZ
	0x50,0x45,0x00,0x00,      									//beginning of file header
	0x4c,0x01,													//machine type
	0x02,0x00,													//number of sections 
	0xff,0xff,0xff,0xff,										//buildDate
	0x00,0x00,0x00,0x00,										
	0x00,0x00,0x00,0x00,										
	0xe0,0x00,													//optional header size
	0x03,0x01
};

char optHeader[] = {						
	0x0b,0x01,										//executable flags
	0x08,
	0x00,
	0x05,0x00,0x00,0x00,							//size of the code
	0x00,0x00,0x00,0x00,							//size of init data
	0x00,0x00,0x00,0x00,							//size of uninit data
	0x24,0x01,0x00,0x00,							//adress waar assembly begint
	0x24,0x01,0x00,0x00,							//base of code
	0x28,0x01,0x00,0x00,							//base of data
	0x00,0x00,0x40,0x00,							//image base ???
	0x04,0x00,0x00,0x00,							//section alignment
	0x04,0x00,0x00,0x00,							//file alignment
	0x04,0x00,										//major OS version
	0x00,0x00,										//minor OS version
	0x00,0x00,										//Major image version
	0x00,0x00,										//Minor Image version
	0x04,0x00,										//Major Subsystem Version
	0x00,0x00,										//Minor Subsystem Version
	0x00,0x00,0x00,0x00,							//win32 version value
	0x29,0x01,0x00,0x00,                         	//groote van .exe file
	0x24,0x01,0x00,0x00,							//groote van de headers
	0x00,0x00,0x00,0x00,							//checksum
	0x03,0x00,										//subSystem
	0x00,0x40,										//dll caracteristics
	0x00,0x00,0x01,0x00,							
	0x00,0x01,0x00,0x00,
	0x00,0x00,0x01,0x00,
	0x00,0x01,0x00,0x00,
	0x00,0x00,0x00,0x00,
	0x10,0x00,0x00,0x00};

char linking[] = {
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,        //export dir							
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,		//import dir
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,		//resource dir
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,		//exeption dir
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,		//security dir
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,		//base relocation dir
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,		//debug dir
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,		//architecure specific data
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,		//rva of gp
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,		//tls dir
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,		//load config dir
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,		//boudn import dir
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,		//import address table
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,		//delay import descriptors
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,		//com runtime discriptor
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00      	//reserverd
	
};

void createSection(int tsz,int dsz){
	int flags[2] = {0x60000020,0xc0000040};
	char name[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	asm = calloc(tsz,1);
	asmSz = tsz;
	int size = headerSz + 0x54;
	optHeader[16] = size;
	optHeader[17] = size >> 8; 
	optHeader[20] = size;
	optHeader[21] = size >> 8;
	optHeader[4] = tsz;
	optHeader[5] = tsz >> 8;
	optHeader[60] = size;
	optHeader[61] = size >> 8;
	optHeader[56] = size + tsz + dsz;
	optHeader[57] = (size + tsz + dsz) >> 8;
	memcpy(name,".text",5);
	fseek(file,headerSz,SEEK_SET);
	fwrite(name,8,1,file);
	fwrite(&tsz,1,4,file);
	fwrite(&size,1,4,file);
	fwrite(&tsz,1,4,file);
	fwrite(&size,1,4,file);
	fseek(file,12,SEEK_CUR);
	fwrite(&flags[0],1,4,file);
	size += tsz;
	optHeader[8] = dsz;
	optHeader[9] = dsz >> 8;
	optHeader[24] = size;
	optHeader[25] = size >> 8;
	memcpy(name,".data",5);
	exedata = calloc(dsz,1);
	exedataSz = dsz;
	dataOffset = size;
	fwrite(name,8,1,file);
	fwrite(&dsz,1,4,file);
	fwrite(&size,1,4,file);
	fwrite(&dsz,1,4,file);
	fwrite(&size,1,4,file);
	fseek(file,12,SEEK_CUR);
	fwrite(&flags[1],1,4,file);	
}

void createExe(char *name,int type){
	file = fopen(name,"w");
	optHeader[68] = type;
}

void closeExe(){
	int adresstableSz = funcCount * 8;
	int importdirSz = libsCount * 20 + 32;
	if(libsCount + funcCount){
		linking[96] = dataOffset;
		linking[97] = dataOffset >> 8;
		linking[100] = adresstableSz;
		linking[101] = adresstableSz >> 8;
		dataOffset += adresstableSz * 2;
		linking[8] = dataOffset;
		linking[9] = dataOffset >> 8;
		linking[12] = importdirSz;
		linking[13] = importdirSz >> 8;
		
		dataOffset -= adresstableSz * 2;
	}

	int bufOffset = dataOffset + adresstableSz * 2 + importdirSz;
	int bufOffset2 = 0;
	if(libsCount){
		libs[0].first = dataOffset;
	}
	int libfunct = 0;
	for(int i = 0;i < funcCount;i++){
		func[i].mempos = dataOffset + bufOffset2 + 0x00400000;
		if(libfunct != func[i].lib){
			libfunct = func[i].lib;
			libs[libfunct].first = dataOffset + (i * 8);
			printx(libfunct);
		}
		exedata[bufOffset2] = bufOffset;
		exedata[bufOffset2 + 1] = bufOffset >> 8;
		bufOffset2 += 8;
	}
	for(int i = 0;i < funcCount;i++){
		exedata[bufOffset2] = bufOffset;
		exedata[bufOffset2 + 1] = bufOffset >> 8;
		bufOffset2 += 8;
		if(func[i].namesize & 1){
			bufOffset += func[i].namesize + 5;
		}
		else{
			bufOffset += func[i].namesize + 6;
		}
	}
	printx(bufOffset);
	for(int i = 0;i < libsCount;i++){
		int rel = libs[i].first + (funcCount * 8);
		exedata[bufOffset2] = rel;
		exedata[bufOffset2 + 1] = rel >> 8;
		bufOffset2 += 12;
		exedata[bufOffset2] = bufOffset;
		exedata[bufOffset2 + 1] = bufOffset >> 8;
		bufOffset2 += 4;
		rel -= funcCount * 8;
		exedata[bufOffset2] = rel;
		exedata[bufOffset2 + 1] = rel >> 8;	
		bufOffset2 += 4;
		if(libs[i].namesize & 1){
			bufOffset += libs[i].namesize + 1;
		}
		else{
			bufOffset += libs[i].namesize + 2;
		}
	}

	bufOffset2 += 32;

	for(int i = 0;i < funcCount;i++){
		bufOffset2 += 2;
		memcpy(exedata + bufOffset2,func[i].name,func[i].namesize);
		if(func[i].namesize & 1){
			bufOffset2 += func[i].namesize + 3;
		}
		else{
			bufOffset2 += func[i].namesize + 4;
		}
	}
	
	for(int i = 0;i < libsCount;i++){
		memcpy(exedata + bufOffset2,libs[i].name,libs[i].namesize);
		if(libs[i].namesize & 1){
			bufOffset2 += libs[i].namesize + 1;
		}
		else{
			bufOffset2 += libs[i].namesize + 2;
		}
	}
	int bob = bufOffset2 + 0x00400000 + dataOffset;
	for(int i = 0;i < varCount;i++){
		for(int i2 = 0;i2 < var[i].count;i2++){
			printf("%x",var[i].ref[i2]);
			asm[var[i].ref[i2]] = bob;
			asm[var[i].ref[i2]+1] = bob >> 8;
			asm[var[i].ref[i2]+2] = bob >> 16;
			asm[var[i].ref[i2]+3] = bob >> 24;
			if(var[i].flags & 0x01){
				for(int i3 = 0;i3 < var[i].size;i3++){
					exedata[bufOffset2+i3] = var[i].data[i3];
				}
				bob++;
				
			}
		}
		bob += var[i].size;
	}
	for(int i = 0;i < linkCount;i++){
		asm[link[i].pos] = func[link[i].type].mempos;
		asm[link[i].pos+1] = func[link[i].type].mempos >> 8;
		asm[link[i].pos+2] = func[link[i].type].mempos >> 16;
		asm[link[i].pos+3] = func[link[i].type].mempos >> 24;
	}
	fseek(file,0,SEEK_SET);
	fwrite(header,sizeof(header),1,file);
	fwrite(optHeader,sizeof(optHeader),1,file);
	fwrite(linking,sizeof(linking),1,file);
	fseek(file,0x54,SEEK_CUR);
	fwrite(asm,asmSz,1,file);
	fwrite(exedata,exedataSz,1,file);
	fclose(file);
}

void addAsm(char val){
	asm[asmOffset] = val;
	asmOffset++;
}

void addAsmEx(short val){
	asm[asmOffset] = val >> 8;
	asm[asmOffset+1] = val;
	asmOffset+=2;
}
void addAsmP(char val,char val2){
	asm[asmOffset] = val;
	asm[asmOffset+1] = val2;
	asmOffset+=2; 
}

void addAsmPQ(char val,int val2){
	asm[asmOffset] = val;
	asm[asmOffset+1] = val2 >> 24;
	asm[asmOffset+2] = val2 >> 16;
	asm[asmOffset+3] = val2 >> 8;
	asm[asmOffset+4] = val2;
	asmOffset+=5; 
}

void callFunction(char *funct){
	addAsmEx(0xff15);
	link = realloc(link,sizeof(linkCount) * (linkCount + 1));
	for(int i = 0;i < funcCount;i++){
		if(!memcmp(funct,func[i].name,strlen(funct))){
			link[linkCount].pos = asmOffset;
			link[linkCount].type = i;
			linkCount++;
			asmOffset += 4;
			break;
		}
	}
}

void addLibrary(char *name){
	libs = realloc(libs,sizeof(libs) * (libsCount + 1));
	libs[libsCount].name = malloc(strlen(name));
	libs[libsCount].namesize = strlen(name);
	memcpy(libs[libsCount].name,name,strlen(name));
	libsCount++;
}

void addFunction(char *name,int type){
	func = realloc(func,sizeof(func) * (funcCount + 1));
	func[funcCount].lib = type;
	func[funcCount].name = malloc(strlen(name));
	func[funcCount].namesize = strlen(name);
	memcpy(func[funcCount].name,name,strlen(name));
	funcCount++;
}

void createVar(int size){
	varCount++;
	var = realloc(var,sizeof(VARIABLE) * varCount);
	var[varCount-1].size = size;
}

void createVarS(char *str){
	varCount++;
	var = realloc(var,sizeof(VARIABLE) * varCount);
	int sz = strlen(str);
	var[varCount-1].size = sz;
	var[varCount-1].data = malloc(sz);
	var[varCount-1].flags |= 0x01;
	memcpy(var[varCount-1].data,str,sz);
}

void acessVar(int vari){
	var[vari].ref = realloc(var[vari].ref,sizeof(int) * (var[vari].count + 1));
	var[vari].ref[var[vari].count] = asmOffset;
	var[vari].count++;
	asmOffset += 4;
}

/*
list
---------------------------------------
0x6a = push byte
0x68 = push int
0x50 = push eax
0x51 = push ecx
0x52 = push edx
0x53 = push ebx
0xb8 = mov int->eax
0xa3 = mov eax->mem
*/

void main(){
	createExe("gert.exe",2);
	createSection(40,200);
	addLibrary("KERNEL32.dll");
	addLibrary("USER32.dll");
	addFunction("GetStdHandle",0);
	addFunction("MessageBoxA",1);
	createVarS("hello world!");
	addAsmP(0x6a,0x00);
	addAsm(0x68);
	acessVar(0);
	addAsmP(0x6a,0x00);	
	addAsmP(0x6a,0x00);
	callFunction("MessageBoxA");
	closeExe();
	CreateProcessA("gert.exe",0,0,0,0,0,0,0,&startupinfo,&process_info);
	return 10;
}













