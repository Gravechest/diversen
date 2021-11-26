#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

STARTUPINFO startupinfo = {sizeof(STARTUPINFO),0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
PROCESS_INFORMATION process_info = {0,0,0,0};

const int headerSz = 0x000000fc;

int exedataSz;
int asmSz;
int dataSz;
int asmOffset;
int dataOffset;
int nameOffset;
int lblamm;
int dataGrote = 45;

char *asm;
char *fasm;
char *exedata;

FILE *file;

typedef struct VARIABLE{
	int size;
	int *ref;
	int count;
	char flags;
	char *data;
	char *name;
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

typedef struct LABEL{
	int pos;
	int refC;
	int *ref;
	int *loc;
	int nameSz;
	char *big;
	char *name;
	char *direct;
}LABEL;

typedef struct ASMFILE{
	char *name;
	char flags;
	char *file;
	int csize;
}ASMFILE;

typedef struct VARAF{
	char *name;
	char *name2;
}VARAF;

int libsCount;
int funcCount;
int linkCount;
int varCount;
int varafCount;
int lblCount;

LIBRARY *libs;
FUNCTION *func;
FLINKING *link;
VARIABLE *var;
VARAF *varaf;
LABEL *lbl;

ASMFILE asmfile;

void inline printx(int val){
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
	0x00,0x00,										//subSystem
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

void linkLabels(){
	char *resize[lblCount];
	for(int i = 0;i < lblCount;i++){
		resize[i] = calloc(lbl[i].refC,1);
		for(int i2 = 0;i2 < lbl[i].refC;i2++){
			if(lbl[i].big[i2] == 1 && (lbl[i].pos - lbl[i].ref[i2] > 127 || lbl[i].pos - lbl[i].ref[i2] < -127)){
				char b = 0;
				if(asm[lbl[i].ref[i2]-1] == 0xffffffeb){
					b = 3;
					asm[lbl[i].ref[i2]-1] = 0xe9;
					lbl[i].big[i2]+=3;
					resize[i][i2] = 4;
				}
				else{
					asm[lbl[i].ref[i2]] = asm[lbl[i].ref[i2]-1] + 0x10;
					asm[lbl[i].ref[i2]-1] = 0x0f;
					lbl[i].big[i2]+=4;
					b = 4;
					resize[i][i2] = 5;
				}
				asmOffset+=b;
				for(int i4 = 0;i4 < b;i4++){
					for(int i5 = asmOffset;i5 > lbl[i].ref[i2];i5--){	
						asm[i5] = asm[i5-1];	
					}
					
				}
				for(int i4 = 0;i4 < lblCount;i4++){
					for(int i5 = 0;i5 < lbl[i4].refC;i5++){
						if(lbl[i4].ref[i5] > lbl[i].ref[i2]){
							lbl[i4].ref[i5]+=b;
						}
					}
					if(lbl[i4].pos > lbl[i].ref[i2]){
						lbl[i4].pos+=b;
					}
				}
				for(int i4 = 0;i4 < linkCount;i4++){
					if(link[i4].pos > lbl[i].ref[i2]){
						link[i4].pos+=b;
					}
				}
				for(int i4 = 0;i4 < varCount;i4++){
					for(int i5 = 0;i5 < var[i4].count;i5++){
						if(var[i4].ref[i5] > lbl[i].ref[i2]){
							var[i4].ref[i5]+=b;
						}
					}
				}
			}
		}	
	}
	for(int i = 0;i < lblCount;i++){
		for(int i2 = 0;i2 < lbl[i].refC;i2++){
			char hlt = 0;
			for(int i3 = 0;i3 < lbl[i].big[i2];i3++){
				if(resize[i][i2] == 5 && !hlt){
					i3++;
					hlt = 1;
				}
				if(lbl[i].direct[i2]){;
					for(int i4 = 3;i4 >= 0;i4--){
						asm[lbl[i].ref[i2] + i4] = lbl[i].pos + 0x00400150 >> i4 * 8;
					}
					break;
				}
				else{
					if(resize[i][i2] && lbl[i].pos - lbl[i].ref[i2] - lbl[i].big[i2] < 0){
						int val = lbl[i].pos - lbl[i].ref[i2] - lbl[i].big[i2] >> (i3 - resize[i][i2]) * 8;
						asm[lbl[i].ref[i2] + i3] = val;
					}
					else{
						asm[lbl[i].ref[i2] + i3] = lbl[i].pos - lbl[i].ref[i2] - lbl[i].big[i2] >> i3 * 8;
					}
				}
				
			}
		}
	}
}

					
void createBootloader(char* name){
	asm = calloc(1474560,1);
	file = fopen(name,"wb");
}

void closeBootloader(){
	int bob = asmOffset;
	for(int i = 0;i < varCount;i++){
		for(int i2 = 0;i2 < var[i].count;i2++){
			asm[var[i].ref[i2]] = bob;
			if(var[i].flags & 0x01){
				for(int i3 = 0;i3 < var[i].size;i3++){
					asm[bob] = var[i].data[i3];
					bob++;
				}
				bob -= var[i].size - 1;
			}
		}
		bob += var[i].size;
	}
	asm[510] = 0x55;
	asm[511] = 0xaa;
	fwrite(asm,1474560,1,file);
	fclose(file);
	FILE *debug = fopen("debug.obj","wb");
	fwrite(asm,512,1,debug);
	fclose(debug);
}

void createSection(int tsz,int dsz){
	if(tsz % 4){
		tsz -= (tsz % 4) - 4;
	}
	int flags[2] = {0xe0000020,0xc0000040};
	char name[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	fasm = calloc(tsz,1);
	memcpy(fasm,asm,asmOffset);
	asmSz = tsz;
	dataSz = dsz;
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
	for(int i = 0;;i++){
		if(!asmfile.name[i]){
			memcpy(asmfile.name+i,".exe",4);
			break;
		}
	}
	asmfile.flags |= 0x08;
	file = fopen(name,"wb");
	optHeader[68] = type;
	asm = calloc(1474560,1);
}

void closeExe(){
	printx(libsCount);
	int adresstableSz = funcCount * 4 + libsCount * 4; 
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
	int subtract = bufOffset;
	for(int i = 0;i < funcCount;i++){
		if(libfunct != func[i].lib){
			libfunct = func[i].lib;
			bufOffset2 += 4;
			libs[libfunct].first = dataOffset + bufOffset2;
		}
		func[i].mempos = dataOffset + bufOffset2 + 0x00400000;
		exedata[bufOffset2] = bufOffset;
		exedata[bufOffset2 + 1] = bufOffset >> 8;
		if(func[i].namesize & 1){
			bufOffset += func[i].namesize + 5;
		}
		else{
			bufOffset += func[i].namesize + 6;
		}
		bufOffset2 += 4;
		
	}
	bufOffset2 += 4;
	bufOffset = subtract;
	libfunct = 0;
	for(int i = 0;i < funcCount;i++){
		if(libfunct != func[i].lib){
			libfunct = func[i].lib;
			bufOffset2 += 4;
		}
		exedata[bufOffset2] = bufOffset;
		exedata[bufOffset2 + 1] = bufOffset >> 8;
		bufOffset2 += 4;
		if(func[i].namesize & 1){
			bufOffset += func[i].namesize + 5;
		}
		else{
			bufOffset += func[i].namesize + 6;
		}
	}
	bufOffset2 += 4;
	for(int i = 0;i < libsCount;i++){
		int rel = libs[i].first + funcCount * 4 + libsCount * 4;
		exedata[bufOffset2] = rel;
		exedata[bufOffset2 + 1] = rel >> 8;
		bufOffset2 += 12;
		exedata[bufOffset2] = bufOffset;
		exedata[bufOffset2 + 1] = bufOffset >> 8;
		bufOffset2 += 4;
		rel -= funcCount * 4 + libsCount * 4;
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
	int fset = 0;
	for(int i = 0;i < varCount;i++){
		for(int i2 = 0;i2 < var[i].count;i2++){
			fasm[var[i].ref[i2]] = bob;
			fasm[var[i].ref[i2]+1] = bob >> 8;
			fasm[var[i].ref[i2]+2] = bob >> 16;
			fasm[var[i].ref[i2]+3] = bob >> 24;
		}
		for(int i2 = 0;i2 < var[i].size;i2++){
			exedata[bufOffset2 + fset] = var[i].data[i2];
			bufOffset2++;
		}
		if(var[i].flags & 0x01){
			bob++;
			fset++;
		}
		bob += var[i].size;
	}
	for(int i = 0;i < linkCount;i++){
		fasm[link[i].pos] = func[link[i].type].mempos;
		fasm[link[i].pos+1] = func[link[i].type].mempos >> 8;
		fasm[link[i].pos+2] = func[link[i].type].mempos >> 16;
		fasm[link[i].pos+3] = func[link[i].type].mempos >> 24;
	}
	fseek(file,0,SEEK_SET);
	fwrite(header,sizeof(header),1,file);
	fwrite(optHeader,sizeof(optHeader),1,file);
	fwrite(linking,sizeof(linking),1,file);
	fseek(file,0x54,SEEK_CUR);
	fwrite(fasm,asmSz,1,file);
	fwrite(exedata,exedataSz,1,file);
	fclose(file);
	printf("\nasm teveel\n");
	printf("%i",asmSz - asmOffset);
	printf("\ndata teveel\n");
	printf("%i",dataSz - bufOffset2);
	printf("\n");
}

void Asm(char val){
	asm[asmOffset] = val;
	asmOffset++;
}

void AsmEx(short val){
	asm[asmOffset] = val >> 8;
	asm[asmOffset+1] = val;
	asmOffset+=2;
}

void AsmExP(short val,char val2){
	asm[asmOffset] = val >> 8;
	asm[asmOffset+1] = val;
	asm[asmOffset+2] = val2;
	asmOffset+=3;
}

void AsmP(char val,char val2){
	asm[asmOffset] = val;
	asm[asmOffset+1] = val2;
	asmOffset+=2; 
}

void AsmPD(char val,short val2){
	asm[asmOffset] = val;
	asm[asmOffset+1] = val2;
	asm[asmOffset+2] = val2 >> 8;
	asmOffset+=3; 
}

void AsmPS(char val,char val2,char val3){
	asm[asmOffset] = val;
	asm[asmOffset+1] = val2;
	asm[asmOffset+2] = val3;
	asmOffset+=3; 
}

void AsmPSD(char val,char val2,short val3){
	asm[asmOffset] = val;
	asm[asmOffset+1] = val2;
	asm[asmOffset+2] = val3;
	asm[asmOffset+3] = val3 >> 8;
	asmOffset+=4; 
}

void AsmPSQ(char val,char val2,int val3){
	asm[asmOffset] = val;
	asm[asmOffset+1] = val2;
	asm[asmOffset+2] = val3;
	asm[asmOffset+3] = val3 >> 8;
	asm[asmOffset+4] = val3 >> 16;
	asm[asmOffset+5] = val3 >> 24;
	asmOffset+=6; 
}

void AsmPQ(char val,int val2){
	asm[asmOffset] = val;
	asm[asmOffset+1] = val2;
	asm[asmOffset+2] = val2 >> 8;
	asm[asmOffset+3] = val2 >> 16;
	asm[asmOffset+4] = val2 >> 24;
	asmOffset+=5; 
}

void callFunction(char *funct){
	AsmEx(0xff15);
	link = realloc(link,sizeof(linkCount) * (linkCount + 100));
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
	if(!libsCount){
		libs = malloc(sizeof(LIBRARY));
	}
	else{
		libs = realloc(libs,sizeof(LIBRARY) * (libsCount + 1));
	}
	int size = 0;
	for(int i = 0;;i++){
		if(name[i] == ' ' || name[i] == '\r' || name[i] == '\t' || name[i] == '\n'){
			size = i;
			break;
		}
	}
	libs[libsCount].name = malloc(size);
	libs[libsCount].namesize = size;
	memcpy(libs[libsCount].name,name,size);
	if(size & 1){
		dataGrote += 21 + size;
	}
	else{
		dataGrote += 22 + size;
	}
	libsCount++;
}

void addFunction(char *name,int type){
	dataGrote += 4;
	int size = 0;
	for(int i = 0;;i++){
		if(name[i] == ' ' || name[i] == '\r' || name[i] == '\t' || name[i] == '\n'){
			size = i;
			break;
		}
	}
	func = realloc(func,sizeof(FUNCTION) * (funcCount + 1));
	func[funcCount].lib = type;
	func[funcCount].name = calloc(40,1);
	func[funcCount].namesize = size;
	memcpy(func[funcCount].name,name,size);
	funcCount++;
	if(size & 1){
		dataGrote += 10 + size;
	}
	else{
		dataGrote += 11 + size;
	}
}

void createVar(char *name,int size,int initval){
	varCount++;
	var = realloc(var,sizeof(VARIABLE) * varCount * 200);
	int sz2 = 0;
	for(int i = 0;;i++){
		if(name[i] == ' '){
			sz2 = i;
			break;
		}
	}
	var[varCount-1].size = size;
	var[varCount-1].name = calloc(sz2 + 1,1);
	var[varCount-1].count = 0;
	var[varCount-1].data = malloc(size);
	for(int i = 0;i < size && i < 4;i++){
		var[varCount-1].data[i] = initval >> i * 8;
	}
	memcpy(var[varCount-1].name,name,sz2);
	dataGrote += size;
}

void createVarS(char *name,char *str){
	varCount++;
	var = realloc(var,sizeof(VARIABLE) * varCount * 20000);
	int sz2 = 0;
	for(int i = 0;;i++){
		if(name[i] == ' '){
			sz2 = i;
			break;
		}
	}
	int sz = 0;
	for(int i = 0;;i++){
		if(str[i] == '"'){
			sz = i;
			break;
		}
	}
	for(int i = 0;i < sz;i++){
		if(str[i] == '\\'){
			for(int i2 = i;i2 < sz - 1;i2++){
				str[i2] = str[i2+1];
			}
			sz--;
			switch(str[i]){
			case 'n':
				str[i] = 0x0a;
				break;
			}
		}
	}
	var[varCount-1].ref = malloc(1);
	var[varCount-1].size = sz + 1;
	var[varCount-1].data = calloc(sz + 1,1);
	var[varCount-1].flags |= 0x01;
	var[varCount-1].name = calloc(sz2 + 1,1);
	var[varCount-1].count = 0;
	memcpy(var[varCount-1].name,name,sz2);
	memcpy(var[varCount-1].data,str,sz);
	dataGrote += sz;
}

void accesVar(char *name){
	int sz = 0;
	for(int i = 0;;i++){
		if(name[i] == '\t' || name[i] == ' ' || name[i] == '\n' || name[i] == '\r' || name[i] == ','){
			sz = i;
			break;
		}
	}
	int vari = 0;
	for(int i = 0;;i++){
		if(!memcmp(var[i].name,name,sz)){
			vari = i;
			break;
		}
	}
	if(!var[vari].count){
		var[vari].ref = malloc(4);
	}
	else{
		var[vari].ref = realloc(var[vari].ref,sizeof(int) * (var[vari].count + 1));
	}
	var[vari].ref[var[vari].count] = asmOffset;
	var[vari].count++;
	asmOffset+=4;
}

void accesVar8b(char *name){
	int vari = 0;
	for(int i = 0;;i++){
		if(!memcmp(var[i].name,name,strlen(name))){
			vari = i;
			break;
		}
	}
	var[vari].ref = realloc(var[vari].ref,sizeof(int) * (var[vari].count + 1));
	var[vari].ref[var[vari].count] = asmOffset;
	var[vari].count++;
	var[vari].flags |= 0x04;
	asmOffset++;
}

void createLabel(char *name){
	lbl = realloc(lbl,sizeof(LABEL) * (lblCount+10000));
	int nmsz = strlen(name);
	lbl[lblCount].nameSz = nmsz;
	lbl[lblCount].name = malloc(nmsz);
	memcpy(lbl[lblCount].name,name,nmsz);
	lbl[lblCount].pos = asmOffset;
	lbl[lblCount].refC = 0;
	lblCount++;
}

void label(int id,int size){
	if(!lbl[id].refC){
		lbl[id].ref = malloc(4);
		lbl[id].big = malloc(4);
		lbl[id].direct = malloc(4);
	}
	else{
		lbl[id].ref = realloc(lbl[id].ref,sizeof(int) * (lbl[id].refC + 1));
		lbl[id].big = realloc(lbl[id].big,sizeof(int) * (lbl[id].refC + 1));
		lbl[id].direct = realloc(lbl[id].direct,sizeof(int) * (lbl[id].refC + 1));
	}
	if(!size){
		lbl[id].big[lbl[id].refC] = 4;
		lbl[id].ref[lbl[id].refC] = asmOffset;
		lbl[id].direct[lbl[id].refC] = 1;
		lbl[id].refC++;
		asmOffset+=4;
	}
	else{
		lbl[id].big[lbl[id].refC] = size;
		lbl[id].ref[lbl[id].refC] = asmOffset;
		lbl[id].direct[lbl[id].refC] = 0;
		lbl[id].refC++;
		asmOffset+=size;
	}
}

int exp10(int val,int times){
	for(int i = 1;i < times;i++){
		val *= 10;
	}
	return val;
}

int exp16(int val,int times){
	for(int i = 1;i < times;i++){
		val *= 16;
	}
	return val;
}

int asciiToInt(int p){
	int sz = 0;
	char hex = 0;
	if(asmfile.file[p] == 'h'){
		p++;
		hex = 1;
	}
	for(int i = p;;i++){
		if(asmfile.file[i] == ' ' || asmfile.file[i] == '\n' || asmfile.file[i] == '\r'){
			sz = i;
			break;
		}
		if( asmfile.file[i] == ','){
			sz = i;
			break;
		}
	}
	int result = 0;
	if(hex){
		for(int i = p;i < sz;i++){
			if(asmfile.file[i] < 0x30 || asmfile.file[i] > 0x39){
				result += exp16(asmfile.file[i] - 0x57,sz - i);
			}
			else{
				result += exp16(asmfile.file[i] - 0x30,sz - i);
			}
		}
	}
	else{
		for(int i = p;i < sz;i++){
			result += exp10(asmfile.file[i] - 0x30,sz - i);
		}
	}
	return result;	
}

char decodeReg(char v1,char v2){
	int result = 0;
	switch(v1){
	case 'c':
		result += 1;
		break;
	case 'd':
		result += 2;
		break;
	case 'b':
		result += 3;
		break;
	}
	switch(v2){
	case 'e':
	case 'a':
		result += 192;
		break;
	case 'c':
		result += 200;
		break;
	case 's':
	case 'd':
		result += 208;
		break;
	case 'b':
		result += 216;
		break;
	case 'f':
		result += 224;
		break;
	case 'g':
		result += 232;
	}
	return result;
}

char decodeReg2(char v1,char v3,char v4,char v2){
	int result = 0;
	switch(v1){
	case 'b':
		switch(v3){
		case 'x':
			switch(v4){
			case 'd':
				result += 1;
				break;
			case 's':
				break;
			default:
				result += 7;
				break;
			}
			break;
		case 'p':
			switch(v4){
			case 'd':
				result += 3;
				break;
			case 's':
				result += 2;
				break;
			break;
			}
		}
		break;
	case 's':
		result += 4;
		break;
	case 'd':
		result += 5;
		break;
	}
	switch(v2){
	case 'e':
	case 'c':
		result += 8;
		break;
	case 's':
	case 'd':
		result += 16;
		break;
	case 'b':
		result += 24;
		break;
	case 'f':
		result += 32;
		break;
	case 'g':
		result += 40;
	}
	return result;
}

char decodeReg3(char v1,char v2){
	switch(v2){
	default:
		switch(v1){
		case 'a':
			return 0x06;
		case 'b':
			return 0x1e;
		case 'c':
			return 0x0e;
		case 'd':
			return 0x16;
		}
	case 'h':
		switch(v1){
		case 'a':
			return 0x26;
		case 'b':
			return 0x3e;
		case 'c':
			return 0x2e;
		case 'd':
			return 0x36;
	}
	case 'i':
		switch(v1){
		case 'd':
			return 0x3e;
		case 's':
			return 0x36;
		break;
	}
	case 'p':
		switch(v1){
		case 's':
			return 0x26;
		case 'b':
			return 0x2e;
		break;
		}
	}
}

char decodeI(char v1){
	switch(v1){
	case 'd':
		return 7;
	case 's':
		return 6;
	}
}

char decodeP(char v1){
	switch(v1){
	case 'b':
		return 5;
	case 's':
		return 4;
	}
}

char decodeRegReg(char v1,char v2,char v3,char v4){
	int result = 0xc0;
	switch(v1){
	case 'b':
		switch(v2){
		case 'p':
			result += 5;
			break;	
		default:	
			result += 3;
			break;
		}
		break;
	case 'c':
		result += 1;
		break;
	case 'd':
		switch(v2){
		case 'i':
			result += 7;
			break;
		default:
			result += 2;
			break;
		}
		break;
	case 's':
		switch(v2){
		case 'i':
			result += 6;
			break;
		case 'p':
			result += 4;
			break;
		}
	}
	switch(v3){
	case 'a':
		switch(v4){
		case 'h':
			result += 4;
			break;
		}
		break;
	case 'b':
		switch(v4){
		case 'x':	
			result += 24;
			break;
		case 'p':
			result += 40;	
		}
		break;
	case 'c':
		result += 8;
		break;
	case 'd':
		switch(v4){
		case 'i':
			result += 56;
			break;
		default:
			result += 16;
			break;
		}
		break;
	case 's':
		switch(v4){
		case 'i':
			result += 48;
			break;
		case 'p':
			result += 32;
			break;
		}
	}
	return result;
}

char decodeReg4(char v1,char v2){
	switch(v1){
	case 'a':
		return 0;
	case 'b':
		switch(v2){
		case 'p':
			return 5;
		case 'x':
			return 3;
		}
	case 'c':
		return 1;
	case 'd':
		switch(v2){
		case 'i':
			return 7;
		case 'x':
			return 2;
		}
	case 's':
		switch(v2){
		case 'i':
			return 6;
		case 'p':
			return 4;
		}
	}
}

void commonIns(int i,char op){
	if(asmfile.file[i] == 'a' && (asmfile.file[i+3] == 'h' || (asmfile.file[i+3] > 0x2f && asmfile.file[i+3] < 0x3a))){
		switch(asmfile.file[i]){
		case 'a':
			switch(asmfile.file[i+1]){
			case 'l':
				AsmP(op+4,asciiToInt(i+3));
				break;
			case 'x':
				AsmPD(op+5,asciiToInt(i+3));
				break;
			}
			break;
		case 'e':
			AsmPD(op+5,asciiToInt(i+4));
			break;
		}
	}
	else if(asmfile.file[i] == 'e' && asmfile.file[i+2] != ','){
		if(asmfile.file[i+4] == 'h' || (asmfile.file[i+4] > 0x2f && asmfile.file[i+4] < 0x3a)){
			int val = asciiToInt(i+4);
			if(val < 128){
				switch(asmfile.file[i+2]){
				case 'i':
					AsmPS(0x83,decodeI(asmfile.file[i+1]) + 0xc0 + op,asciiToInt(i+4));
					break;
				case 'p':
					AsmPS(0x83,decodeP(asmfile.file[i+1]) + 0xc0 + op,asciiToInt(i+4));
					break;
				case 'x':
					AsmPS(0x83,decodeReg(asmfile.file[i+1],0) + 0xc0 + op,asciiToInt(i+4));
					break;
				}
			}
			else{
				switch(asmfile.file[i+2]){
				case 'i':
					AsmPSQ(0x81,decodeI(asmfile.file[i+1]) + 0xc0 + op,asciiToInt(i+4));
					break;
				case 'p':
					AsmPSQ(0x81,decodeP(asmfile.file[i+1]) + 0xc0 + op,asciiToInt(i+4));
					break;
				case 'x':
					AsmPSQ(0x81,decodeReg(asmfile.file[i+1],0) + 0xc0 + op,asciiToInt(i+4));
					break;
				}
			}
		}
		else{
			if(asmfile.file[i+6] == ' ' || asmfile.file[i+6] == '\n' || asmfile.file[i+6] == '\r' || asmfile.file[i+6] == '\t'){
				AsmP(op+1,decodeRegReg(asmfile.file[i+1],asmfile.file[i+2],asmfile.file[i+4],asmfile.file[i+5]));
			}
			else{
				AsmP(op+1,decodeRegReg(asmfile.file[i+1],asmfile.file[i+2],asmfile.file[i+5],asmfile.file[i+6]));
			}
			
		}
	}
	else if(asmfile.file[i] == '*'){
		if(asmfile.file[i] == 'h' || (asmfile.file[i] > 0x2f && asmfile.file[i] < 0x3a)){
			int adres = asciiToInt(i+1);
			i+=2;
			for(;asmfile.file[i] != ',';i++){}
			switch(asmfile.file[i+2]){
			case 'h':
			case 'l':
				AsmPSD(op,decodeReg3(asmfile.file[i+1],asmfile.file[i+2]),adres);
				break;
			default:
				AsmPSD(op+1,decodeReg3(asmfile.file[i+1],asmfile.file[i+2]),adres);
				break;
			}
		}
		else{
			if(asmfile.file[i+4] == ','){
				if(asmfile.file[i+7] == ' ' || asmfile.file[i+7] == '\t' || asmfile.file[i+7] == '\n' || asmfile.file[i+7] == '\r'){
					AsmP(0x38,decodeRegReg(asmfile.file[i+2],asmfile.file[i+3],asmfile.file[i+5],asmfile.file[i+6]) - 0xc0);
				}
				else{
					AsmP(0x39,decodeRegReg(asmfile.file[i+2],asmfile.file[i+3],asmfile.file[i+6],asmfile.file[i+7]) - 0xc0);
				}
			}
		}
	}
	else if(asmfile.file[i+3] == 'h' || (asmfile.file[i+3] > 0x2f && asmfile.file[i+3] < 0x3a)){
		switch(asmfile.file[i+1]){
		case 'i':
			AsmPSD(0x81,decodeI(asmfile.file[i]) + 0xc0 + op,asciiToInt(i+3));
			break;
		case 'p':
			AsmPSD(0x81,decodeP(asmfile.file[i]) + 0xc0 + op,asciiToInt(i+3));
			break;
		case 'l':
			AsmPS(0x80,decodeReg(asmfile.file[i],0) + 0xc0 + op,asciiToInt(i+3));
			break;
		case 'h':
			AsmPS(0x80,decodeReg(asmfile.file[i],0) + 0xe0 + op,asciiToInt(i+3));
			break;
		case 'x':
			AsmPSD(0x81,decodeReg(asmfile.file[i],0) + 0xc0 + op,asciiToInt(i+3));
			break;
		}
	}
	else if(asmfile.file[i+3] == '*'){
		if(asmfile.file[i+4] == 'h' || (asmfile.file[i+4] > 0x2f && asmfile.file[i+4] < 0x3a)){
			int adres = asciiToInt(i+4);
			switch(asmfile.file[i+1]){
			case 'l':
			case 'h':
				AsmPSD(op+2,decodeReg3(asmfile.file[i],asmfile.file[i+1]),adres);
				break;
			default:
				AsmPSD(op+3,decodeReg3(asmfile.file[i],asmfile.file[i+1]),adres);
				break;
			}
		}
		else{
			AsmP(op+2,decodeRegReg(asmfile.file[i+5],asmfile.file[i+6],asmfile.file[i],asmfile.file[i+1]) - 0xc0);
		}
	}
	else{
		switch(asmfile.file[i+1]){
		case 'l':			
			AsmP(op,decodeRegReg(asmfile.file[i],asmfile.file[i+1],asmfile.file[i+3],asmfile.file[i+4]));
			break;
		case 'h':
			AsmP(op,decodeRegReg(asmfile.file[i],asmfile.file[i+1],asmfile.file[i+3],asmfile.file[i+4]) + 0x20);
			break;
		default:
			AsmP(op+1,decodeRegReg(asmfile.file[i],asmfile.file[i+1],asmfile.file[i+3],asmfile.file[i+4]));
			break;
		}
	}
}

void commonIns2(int i,int op){
	if(asmfile.file[i+2] == ','){
		switch(asmfile.file[i+4]){
		case 'c':
			switch(asmfile.file[i+1]){
			case 'l':
				AsmP(0xd2,decodeRegReg(asmfile.file[i],0,0,0) + 0x20 + op);
				break;
			case 'h':
				AsmP(0xd2,decodeRegReg(asmfile.file[i],0,0,0) + 0x24 + op);
				break;
			default:
				AsmP(0xd3,decodeRegReg(asmfile.file[i],asmfile.file[i+1],0,0) + 0x20 + op);
				break;
			}
			break;
		default:
			switch(asmfile.file[i+1]){
			case 'l':
				AsmPS(0xc0,decodeRegReg(asmfile.file[i],0,0,0) + 0x20 + op,asciiToInt(i+3));
				break;
			case 'h':
				AsmPS(0xc0,decodeRegReg(asmfile.file[i],0,0,0) + 0x24 + op,asciiToInt(i+3));
				break;
			default:
				AsmPS(0xc1,decodeRegReg(asmfile.file[i],asmfile.file[i+1],0,0) + 0x20 + op,asciiToInt(i+3));
				break;
			}
			break;
		}
	}
	else{
		switch(asmfile.file[i+1]){
		case 'l':
			AsmP(0xd0,decodeRegReg(asmfile.file[i],0,0,0) + 0x20 + op);
			break;
		case 'h':
			AsmP(0xd0,decodeRegReg(asmfile.file[i],0,0,0) + 0x24 + op);
			break;
		default:
			AsmP(0xd1,decodeRegReg(asmfile.file[i],asmfile.file[i+1],0,0) + 0x20 + op);
			break;
		}
	}
}

void commonIns3(int i,int op){
	if(asmfile.file[i+1] == 'x'){
		AsmP(0xf7,decodeReg(asmfile.file[i],0) + 0xe0 + op);
	}
	else if(asmfile.file[i+2] != '\t' && asmfile.file[i+2] != '\r' && asmfile.file[i+2] != '\n' && asmfile.file[i+2] != ' '){
		AsmP(0xf7,decodeReg(asmfile.file[i+1],0) + 0xe0 + op);
	}
	else{
		AsmP(0xf6,decodeReg(asmfile.file[i],0) + 0xe0 + op);
	}
}

void main(){
	printf("%i\n",sizeof(WNDCLASSEX));
	int buf = 0;
	FILE *f = fopen("file.txt","rb");
	fseek(f,0,SEEK_END);
	int size = ftell(f);
	fseek(f,0,SEEK_SET);
	asmfile.file = calloc(size + 20,1);
	fread(asmfile.file,size,1,f);
	fclose(f);
	for(int i = 0;i < size;i++){
		if(!memcmp(asmfile.file+i,"%bootloader",11)){
			asmfile.flags |= 0x01;
		}
		else if(!memcmp(asmfile.file+i,"%console",8)){
			asmfile.flags |= 0x02;
		}
		else if(!memcmp(asmfile.file+i,"%window",7)){
			asmfile.flags |= 0x04;
		}
		else if(!memcmp(asmfile.file+i,"%name",5)){
			i+=6;
			int sz = 0;
			for(;asmfile.file[i] == ' ';i++){}
			for(;asmfile.file[i+sz] != ' ' && asmfile.file[i+sz] != '\n';sz++){}
			sz--;
			asmfile.name = calloc(sz+4,1);
			memcpy(asmfile.name,asmfile.file+i,sz);
		}
		else if(!memcmp(asmfile.file+i,"%lib",4)){
			i+=4;
			for(;asmfile.file[i] == ' ';i++){}
			addLibrary(asmfile.file+i);
		}
		else if(!memcmp(asmfile.file+i,"%func",5)){
			i+=5;
			for(;asmfile.file[i] == ' ';i++){}
			addFunction(asmfile.file+i,libsCount-1);
		}
		else if(!memcmp(asmfile.file+i,"%var",4)){
			i+=4;
			for(;asmfile.file[i] == ' ';i++){}
			int x = i;
			for(;asmfile.file[i] != ' ';i++){}
			int sz = i - x;
			i = x;
			char *nm = calloc(sz + 1,1);
			memcpy(nm,asmfile.file + i,sz+1);
			for(;asmfile.file[i] != ' ';i++){}
			for(;asmfile.file[i] == ' ';i++){}
			if(asmfile.file[i] == '"'){
				createVarS(nm,asmfile.file+i+1);
			}
			else{
				int vl = asciiToInt(i);
				for(;asmfile.file[i] > 0x2f && asmfile.file[i] < 0x3a;i++){}
				for(;(asmfile.file[i] < 0x2f || asmfile.file[i] > 0x3a) && asmfile.file[i] != '~';i++){}
				int vl2 = 0;
				if(asmfile.file[i] == '~'){
					i++;
					if(!varafCount){
						varaf = malloc(sizeof(VARAF));
					}
					else{
						varaf = realloc(varaf,sizeof(VARAF) * (varafCount + 1));
					}
					int sz2 = i;
					for(;asmfile.file[sz2] != ' ' && asmfile.file[sz2] != '\n' && asmfile.file[sz2] != '\r' && asmfile.file[sz2] != '\t';sz2++){}
					sz2 -= i;
					varaf[varafCount].name = calloc(sz2+1,1);
					varaf[varafCount].name2 = calloc(sz+1,1);
					memcpy(varaf[varafCount].name,asmfile.file+i,sz2);
					memcpy(varaf[varafCount].name2,nm,sz);
					varafCount++;
				}
				else{
					vl2 = asciiToInt(i);
				}
				createVar(nm,vl,vl2);
			}
		}
		else if(!memcmp(asmfile.file+i,"%label",6)){
			i+=6;
			int sz = 0;
			for(;asmfile.file[i] == ' ';i++){}
			for(;asmfile.file[i+sz] != ' ' && asmfile.file[i+sz] != '\r' && asmfile.file[i+sz] != '\n';sz++){}
			char *name = calloc(sz + 1,1);
			memcpy(name,asmfile.file+i,sz);
			createLabel(name);
			free(name);
		}
		for(;asmfile.file[i] != '\n' && i < size;i++){}
	}
	if(asmfile.flags & 0x01){
		for(int i = 0;;i++){
			if(!asmfile.name[i]){
				memcpy(asmfile.name+i,".img",4);
				break;
			}
		}
		createBootloader(asmfile.name);
	}
	else if(asmfile.flags & 0x02){
		createExe(asmfile.name,3);
	}
	else if(asmfile.flags & 0x04){
		createExe(asmfile.name,2);
	}
	for(int i = 0;i < size;i++){
		switch(asmfile.file[i]){
		case '%':
			if(!memcmp(asmfile.file+i,"%label",6)){
				lbl[lblamm].pos = asmOffset;
				lblamm++;
			}
			if(!memcmp(asmfile.file+i,"%skip",5)){
				i+=5;
				for(;asmfile.file[i] == ' ';i++){}
				asmOffset += asciiToInt(i);
			}
			if(!memcmp(asmfile.file+i,"%string",7)){
				i+=7;
				for(;asmfile.file[i] == ' ';i++){}
				int adress = asciiToInt(i);
				for(;asmfile.file[i] != ' ';i++){}
				for(;asmfile.file[i] == ' ';i++){}
				i++;
				int f = 0;
				for(;asmfile.file[i] != '"';i++){
					asm[adress+f] = asmfile.file[i];
					f++;
				}
			}
			while(asmfile.file[i] != '\n'){
				i++;
			}
			break;
		case 'a':
			switch(asmfile.file[i+1]){
			case 'a':
				switch(asmfile.file[i+2]){
				case 'a':
					Asm(0x37);
					break;
				case 's':
					Asm(0x3f);
					break;
				}
			case 'd':
				switch(asmfile.file[i+2]){
				case 'd':
					i+=4;
					for(;asmfile.file[i] == ' ' || asmfile.file[i] == '\t';i++){}
					commonIns(i,0);
					break;
				case 'c':
					i+=4;
					for(;asmfile.file[i] == ' ' || asmfile.file[i] == '\t';i++){}
					commonIns(i,16);
					break;
				}
				break;
			case 'n':
				i+=4;
				for(;asmfile.file[i] == ' ' || asmfile.file[i] == '\t';i++){}
				commonIns(i,32);
				break;
			}
			
			break;
		case 'c':
			switch(asmfile.file[i+1]){
			case 'a':
				i+=5;
				for(;asmfile.file[i] == ' ' || asmfile.file[i] == '\t';i++){}
				for(int i2 = 0;i2 < funcCount;i2++){
					if(!memcmp(asmfile.file+i,func[i2].name,func[i2].namesize)){
						callFunction(func[i2].name);
						goto done;
					}
				}
				Asm(0xe8);
				for(int i2 = 0;i2 < lblCount;i2++){
					if(!memcmp(asmfile.file+i,lbl[i2].name,lbl[i2].nameSz - 1)){
						if(asmfile.flags & 0x08){
							label(i2,4);
						}
						else{
							label(i2,2);
						}
						break;
					}
				}
			done:
				break;
			case 'l':
				switch(asmfile.file[i+2]){
				case 'c':
					Asm(0xf8);
					break;
				case 'd':
					Asm(0xfc);
					break;
				case 'i':
					Asm(0xfa);
					break;
				}
				break;
			case 'm':
				i+=4;
				for(;asmfile.file[i] == ' ' || asmfile.file[i] == '\t';i++){}
				commonIns(i,56);
				break;
			}
			break;
		case 'd':
			switch(asmfile.file[i + 1]){
			case 'e':
				i+=4;
				for(;asmfile.file[i] == ' ' || asmfile.file[i] == '\t';i++){}
				if(asmfile.file[i] == 'e'){
					if(!asmfile.flags & 0x08){
						Asm(0x66);
					}
					Asm(decodeReg4(asmfile.file[i+1],asmfile.file[i+2]) + 0x48);
				}
				else{
					if(asmfile.flags & 0x08){
						Asm(0x66);
					}
					Asm(decodeReg4(asmfile.file[i],asmfile.file[i+1]) + 0x48);
				}
				break;
			case 'i':
				i+=4;
				for(;asmfile.file[i] == ' ' || asmfile.file[i] == '\t';i++){}
				commonIns3(i,0x10);
				break;
			} 
			break;
		case 'h':
			Asm(0xf4);
			break;
		case 'i':
			switch(asmfile.file[i+2]){
			case 'c':
				i+=4;
				for(;asmfile.file[i] == ' ' || asmfile.file[i] == '\t';i++){}
				if(asmfile.file[i] == 'e'){
					if(!asmfile.flags & 0x08){
						Asm(0x66);
					}
					Asm(decodeReg4(asmfile.file[i+1],asmfile.file[i+2]) + 0x40);
				}
				else{
					if(asmfile.flags & 0x08){
						Asm(0x66);
					}
					Asm(decodeReg4(asmfile.file[i],asmfile.file[i+1]) + 0x40);
				}
				break;
			case 'd':
				i+=5;
				for(;asmfile.file[i] == ' ' || asmfile.file[i] == '\t';i++){}
				commonIns3(i,0x18);
				break;
			case 't':
				i+=4;
				for(;asmfile.file[i] == ' ' || asmfile.file[i] == '\t';i++){}
				AsmP(0xcd,asciiToInt(i));
				break;	
			case ' ':
				i+=3;
				for(;asmfile.file[i] == ' ';i++){}
				switch(asmfile.file[i+1]){
				case 'l':
					AsmP(0xe4,asciiToInt(i+3));
					break;
				case 'x':
					AsmP(0xe5,asciiToInt(i+3));
					break;
				}
				break;
			}
			break;
		case 'j':
			switch(asmfile.file[i+1]){
			case 'a':
				buf = 0x77;
				i+=3;
				break;
			case 'b':
				switch(asmfile.file[i+2]){
				case ' ':
					buf = 0x72;
					i+=3;
					break;
				case 'e':
					buf = 0x76;
					i+=4;
					break;
				}
				break;
			case 'c':
				buf = 0xe3;
				i+=5;
				break;
			case 'e':
				buf = 0x74;
				i+=2;
				break;
			case 'g':
				switch(asmfile.file[i+2]){
				case ' ':
					buf = 0x7f;
					i+=3;
					break;
				case 'e':
					buf = 0x7d;
					i+=4;
					break;
				}
				break;
			case 'l':
				switch(asmfile.file[i+2]){
				case ' ':
					buf = 0x7c;
					i+=3;
					break;
				case 'e':
					buf = 0x7e;
					i+=4;
					break;
				}
				break;
			case 'n':
				switch(asmfile.file[i+2]){
				case 'b':
					buf = 0x7b;
					i+=4;
					break;
				case 'e':
					buf = 0x75;
					i+=4;
					break;
				case 'o':
					buf = 0x71;
					i+=4;
					break;
				case 's':
					buf = 0x79;
					i+=4;
					break;
				}
				break;
			case 'm':
				buf = 0xeb;
				i+=4;
				break;
			case 'o':
				buf = 0x70;
				i+=3;
				break;
			case 'p':
				switch(asmfile.file[i+2]){
				case 'e':
					buf = 0x7a;
					i+=4;
					break;
				case 'o':
					buf = 0x7b;
					i+=4;
					break;
				}
				break;
			case 's':
				buf = 0x78;
				i+=3;
				break;
			}
			for(;asmfile.file[i] == ' ' || asmfile.file[i] == '\t';i++){}		
			Asm(buf);
			for(int i2 = 0;i2 < lblCount;i2++){
				if(!memcmp(asmfile.file+i,lbl[i2].name,lbl[i2].nameSz)){
					label(i2,1);
					break;
				}
			}
			break;
		case 'l':
			switch(asmfile.file[i+2]){
			case 'o':
				i+=5;
				for(;asmfile.file[i] == ' ' || asmfile.file[i] == '\t';i++){}
				Asm(0xe2);
				for(int i2 = 0;i2 < lblCount;i2++){
					if(!memcmp(asmfile.file+i,lbl[i2].name,lbl[i2].nameSz)){
						label(i2,1);
						break;
					}
				}
				break;
			case 'd':
				switch(asmfile.file[i+4]){
				case 'b':
					Asm(0xac);
					break;
				case 'w':
					Asm(0xad);
					break;
				}
				break;
			}
			break;
		case 'm':
			switch(asmfile.file[i+1]){
			case 'o':
				switch(asmfile.file[i+3]){
				case 'z':
					i+=6;
					for(;asmfile.file[i] == ' ' || asmfile.file[i] == '\t';i++){}
					if(asmfile.file[i+3] == ','){
						if(asmfile.file[i+4] == '*'){
							
						}
					}
					break;
				default:
					i+=4;
					for(;asmfile.file[i] == ' ' || asmfile.file[i] == '\t';i++){}
					if(asmfile.file[i+2] == ','){
						if((asmfile.file[i+3] < 0x2f || asmfile.file[i+3] > 0x3a) && asmfile.file[i+3] != 'h'){
							if(asmfile.file[i+1] == 's'){
								if(asmfile.file[i] == 'd'){
									AsmP(0x8e,0xd8);
								}
								else{
									AsmP(0x8e,decodeReg(asmfile.file[i+3],asmfile.file[i]));
								}
							}
							else if(asmfile.file[i+4] == 's'){
								if(asmfile.file[i] == 'd'){
									AsmP(0x8c,0xd8);
								}
								else{
									AsmP(0x8c,decodeReg(asmfile.file[i],asmfile.file[i+3]));
								}
							}
							else if(asmfile.file[i+3] == '*'){
								if(asmfile.file[i+4] == '$'){
									switch(asmfile.file[i+1]){
									case 'l':
									case 'h':
										AsmP(0x8a,decodeReg3(asmfile.file[i],asmfile.file[i+1]) - 1);
										break;
									default:
										AsmP(0x8b,decodeReg3(asmfile.file[i],asmfile.file[i+1]) - 1);
										break;
									}
									accesVar(asmfile.file+i+5);
								}
								else{
									if(asmfile.file[i] == 'a' && ((asmfile.file[i+4] > 0x2f && asmfile.file[i+4] < 0x3a) || asmfile.file[i+4] == 'h')){
										if(asmfile.file[i+1] == 'l'){
											AsmPD(0xa0,asciiToInt(i+4));
										} 
										else if(asmfile.file[i+1] == 'h'){
											AsmPSD(0x8a,0x26,asciiToInt(i+4));
										}
										else{
											AsmPD(0xa1,asciiToInt(i+4));
										}
									}
									else if((asmfile.file[i+4] > 0x2f && asmfile.file[i+4] < 0x3a) || asmfile.file[i+4] == 'h'){
		
										switch(asmfile.file[i+1]){
										case 'l':
										case 'h':									
											AsmPSD(0x8a,decodeReg3(asmfile.file[i],asmfile.file[i+1]),asciiToInt(i+4));
											break;
										default:
											AsmPSD(0x8b,decodeReg3(asmfile.file[i],asmfile.file[i+1]),asciiToInt(i+4));
											break;
										}
									}
									else{
										if(asmfile.file[i+6] != ' ' && asmfile.file[i+6] != '\t' && asmfile.file[i+6] != '\r' && asmfile.file[i+6] != '\n'){
											if(asmfile.file[i+1] == 'x'){
												AsmP(0x8b,decodeRegReg(asmfile.file[i+5],asmfile.file[i+6],asmfile.file[i],asmfile.file[i+1]) - 0xc0);
											}
											else{
												AsmP(0x8a,decodeRegReg(asmfile.file[i+5],asmfile.file[i+6],asmfile.file[i],asmfile.file[i+1]) - 0xc0);
											}
										}
										else{
											if(asmfile.file[i+1] == 'x'){
												AsmP(0x8b,decodeReg2(asmfile.file[i+4],asmfile.file[i+5],0,asmfile.file[i]));
											}
											else{
												AsmP(0x8a,decodeReg2(asmfile.file[i+4],asmfile.file[i+5],0,asmfile.file[i]));
											}
										}
									}
								}
							}
							else if(asmfile.file[i] == '*'){
								if(asmfile.file[i+3] == 'a'){
									if(asmfile.file[i+4] == 'l'){
										AsmPD(0xa0,asciiToInt(i+1));
									}
									else if(asmfile.file[i+4] == 'h'){
										AsmPSD(0x8a,0x26,asciiToInt(i+1));
									}
									else{
										AsmPD(0xa1,asciiToInt(i+1));
									}
								}
							}
							else{
								
								switch(asmfile.file[i+1]){
								case 'l':
									AsmP(0x88,decodeRegReg(asmfile.file[i],asmfile.file[i+1],asmfile.file[i+3],asmfile.file[i+4]));
									break;
								case 'h':
									AsmP(0x88,decodeRegReg(asmfile.file[i],asmfile.file[i+1],asmfile.file[i+3],asmfile.file[i+4]) + 0x20);
									break;
								default:
									AsmP(0x89,decodeRegReg(asmfile.file[i],asmfile.file[i+1],asmfile.file[i+3],asmfile.file[i+4]));
									break;
								}	
							}
						}
						else{
							int buf = 0xb0;
							switch(asmfile.file[i+1]){
							case 'i':
								buf += 0x0d;
								break;
							case 'l':
								break;
							case 'h':
								buf += 4;
								break;
							default:
								buf += 8;
							}
							switch(asmfile.file[i]){
							case 's':
							case 'c':
								buf++;
								break;
							case 'd':
								buf+=2;
								break;
							case 'b':
								buf+=3;
								break;
							}
							if(buf >= 0xb8){
								AsmPD(buf,asciiToInt(i+3));
							}
							else{
								AsmP(buf,asciiToInt(i+3));
							}
						}
					}
					else if(asmfile.file[i] == '*'){
						i++;
						if((asmfile.file[i] > 0x2f && asmfile.file[i] < 0x3a) || asmfile.file[i] == 'h'){
							int val = asciiToInt(i);
							for(;(asmfile.file[i] > 0x2f && asmfile.file[i] < 0x3a) || asmfile.file[i] == 'h' || (asmfile.file[i] > 0x60 && asmfile.file[i] < 0x67);i++){}
							if(asmfile.file[i+1] == 'a'){
								switch(asmfile.file[i+2]){
								case 'l':
									AsmPD(0xa2,val);
									break;
								case 'x':
									AsmPD(0xa3,val);
									break;
								}
							}
							else{
								switch(asmfile.file[i+2]){
								case 'l':
								case 'h':
									AsmPSD(0x88,decodeReg3(asmfile.file[i+1],asmfile.file[i+2]),val);
									break;
								default:
									AsmPSD(0x89,decodeReg3(asmfile.file[i+1],asmfile.file[i+2]),val);
									break;
								}
							}
						}
						else if(asmfile.file[i] == 'e' && asmfile.file[i+3] == ','){
							if(asmfile.file[i+6] == ' ' || asmfile.file[i+6] == '\n' || asmfile.file[i+6] == '\r' || asmfile.file[i+6] == '\t'){
								AsmP(0x88,decodeRegReg(asmfile.file[i+1],asmfile.file[i+2],asmfile.file[i+4],asmfile.file[i+5]) - 0xc0);
							}
							else{
								AsmP(0x89,decodeRegReg(asmfile.file[i+1],asmfile.file[i+2],asmfile.file[i+5],asmfile.file[i+6]) - 0xc0);
							}
						}
						else{
							if(asmfile.file[i+2] == ','){
								if(asmfile.file[i+3] == 's' && asmfile.file[i+4] == 's'){
									AsmP(0x8c,0x17);
								}
								else if(asmfile.file[i+3] == 'e' && asmfile.file[i+4] == 's'){
									AsmP(0x8c,0x07);
								}
								else if(asmfile.file[i+4] == 'x'){
									AsmP(0x89,decodeReg2(asmfile.file[i],asmfile.file[i+1],0,asmfile.file[i+3]));
								}
								else{
									AsmP(0x88,decodeReg2(asmfile.file[i],asmfile.file[i+1],0,asmfile.file[i+3]));
								}
								
							}
							else if(asmfile.file[i+3] == ','){
								if(asmfile.file[i+6] != ' ' && asmfile.file[i+6] != '\t' && asmfile.file[i+6] != '\n' && asmfile.file[i+6] != '\r'){
									AsmP(0x89,decodeReg2(asmfile.file[i],asmfile.file[i+1],0,asmfile.file[i+5]));
								}
								else{
									switch(asmfile.file[i+5]){
									case 'l':
										AsmP(0x88,decodeReg2(asmfile.file[i],asmfile.file[i+1],0,asmfile.file[i+4]));
										break;
									case 'h':
										AsmP(0x88,decodeReg2(asmfile.file[i],asmfile.file[i+1],0,asmfile.file[i+4]) + 0x20);
										break;
									}
								}
							}
							else{
								AsmP(0x89,decodeReg2(asmfile.file[i+1],asmfile.file[i+2],asmfile.file[i+4],asmfile.file[i+7]));
							}
						}
					}
					else if(asmfile.file[i] == 'e'){
						if(asmfile.file[i+4] == '*'){
							if(asmfile.file[i+5] == '$'){
								if(asmfile.file[i+1] == 'a'){
									Asm(0xa1);
								}
								else{
									AsmP(0x8b,decodeReg3(asmfile.file[i+1],asmfile.file[i+2]) - 1);
								}
								accesVar(asmfile.file+i+6);
							}
							else{
								AsmP(0x8b,decodeRegReg(asmfile.file[i+5],asmfile.file[i+6],asmfile.file[i+1],asmfile.file[i+2]) - 0xc0);
							}
						}
						else if(asmfile.file[i+4] == '$'){
							int buf = 0xb0;
							switch(asmfile.file[i+2]){
							case 'i':
								buf += 0x0d;
								break;
							case 'l':
								break;
							case 'h':
								buf += 4;
								break;
							default:
								buf += 8;
							}
							switch(asmfile.file[i+1]){
							case 's':
							case 'c':
								buf++;
								break;
							case 'd':
								buf+=2;
								break;
							case 'b':
								buf+=3;
								break;
							}
							Asm(buf);
							accesVar(asmfile.file+i+5);
						}
						else if(asmfile.file[i+4] == '~'){
							int buf = 0xb0;
							switch(asmfile.file[i+2]){
							case 'i':
								buf += 0x0d;
								break;
							case 'l':
								break;
							case 'h':
								buf += 4;
								break;
							default:
								buf += 8;
							}
							switch(asmfile.file[i+1]){
							case 's':
							case 'c':
								buf++;
								break;
							case 'd':
								buf+=2;
								break;
							case 'b':
								buf+=3;
								break;
							}
							Asm(buf);
							i+=5;
							for(int i2 = 0;i2 < lblCount;i2++){
								if(!memcmp(asmfile.file+i,lbl[i2].name,lbl[i2].nameSz)){
									label(i2,0);
									break;
								}
							}
						}
						else if(asmfile.file[i+4] == 'e'){
							AsmP(0x89,decodeRegReg(asmfile.file[i+1],asmfile.file[i+2],asmfile.file[i+5],asmfile.file[i+6]));
						}
						else{
							int buf = 0xb0;
							switch(asmfile.file[i+2]){
							case 'i':
								buf += 0x0d;
								break;
							case 'p':
								buf+=10;
								break;
							case 'l':
								break;
							case 'h':
								buf += 4;
								break;
							default:
								buf += 8;
							}
							switch(asmfile.file[i+1]){
							case 's':
							case 'c':
								buf++;
								break;
							case 'd':
								buf+=2;
								break;
							case 'b':
								buf+=3;
								break;
							}
							AsmPQ(buf,asciiToInt(i+4));
						}
					}
					else if(asmfile.file[i] == '$'){
						int t = i;
						for(;asmfile.file[i] != ',';i++){}
						if(asmfile.file[i+1] == 'e' && asmfile.file[i+3] != ' ' && asmfile.file[i+3] != '\t' && asmfile.file[i+3] != '\n' && asmfile.file[i+3] != '\r'){
							if(asmfile.file[i+2] == 'a'){
								Asm(0xa3);
								accesVar(asmfile.file+t+1);
							}
							else{
								AsmP(0x89,decodeReg3(asmfile.file[i+2],asmfile.file[i+3]) - 1);
								accesVar(asmfile.file+t+1);
							}
						}
						else{
							if(asmfile.file[i+2] == 'a'){
								Asm(0xa2);
								accesVar(asmfile.file+t+1);
							}
							else{
								AsmP(0x88,decodeReg3(asmfile.file[i+1],asmfile.file[i+2]) - 1);
								accesVar(asmfile.file+t+1);
							}
						}
					}
					break;
				}
				break;
			case 'u':
				i+=4;
				for(;asmfile.file[i] == ' ' || asmfile.file[i] == '\t';i++){}
				commonIns(i,0);
				break;
			}
			break;
		case 'n':
			Asm(0x90);
			break;
		case 'o':
			switch(asmfile.file[i+1]){
			case 'r':
				i+=3;
				for(;asmfile.file[i] == ' ' || asmfile.file[i] == '\t';i++){}
				commonIns(i,8);
				break;
			case 'u':
				i+=4;
				for(;asmfile.file[i] == ' ' || asmfile.file[i] == '\t';i++){}
				int val = asciiToInt(i);
				for(;(asmfile.file[i] > 0x2f && asmfile.file[i] < 0x3a) || asmfile.file[i] == 'h';i++){}
				switch(asmfile.file[i+2]){
				case 'l':
					AsmP(0xe6,val);
					break;
				case 'x':
					AsmPD(0xe7,val);
					break;
				}
				break;
			}
			
			break;
		case 'p':
			switch(asmfile.file[i+1]){
			case 'u':
				i+=5;
				for(;asmfile.file[i] == ' ' || asmfile.file[i] == '\t';i++){}
				if((asmfile.file[i] > 0x2f && asmfile.file[i] < 0x39) || asmfile.file[i] == 'h'){
					int buf = asciiToInt(i);
					int cmp = 0;
					if(asmfile.file[i] == 'h'){
						cmp = 256;
					}
					else{
						cmp = 128;
					}
					if(buf < cmp){
						AsmP(0x6a,buf);
					}
					else{
						if(asmfile.flags & 0x08){
							AsmPQ(0x68,buf);
						}
						else{
							AsmPD(0x68,buf);
						}
					}
				}
				else if(asmfile.file[i] == '$'){
					Asm(0x68);
					accesVar(asmfile.file+i+1);
				}
				else if(asmfile.file[i] == '*'){
					if(asmfile.file[i+1] == 'e' && asmfile.file[i+3] != ' ' && asmfile.file[i+3] != '\t' && asmfile.file[i+3] != '\n' && asmfile.file[i+3] != '\r'){
						if(asmfile.file[i+2] == 'b' && asmfile.file[i+3] == 'p'){
							AsmPS(0xff,0x75,asciiToInt(i+5));
						}
						else{
							AsmP(0xff,decodeReg4(asmfile.file[i+2],asmfile.file[i+3]) + 0x30);
						}
					}
				}
				else{
					char sw = 0;
					char sw2 = 0;
					if(asmfile.file[i] == 'e' && asmfile.file[i+2] != ' ' && asmfile.file[i+2] != '\t' && asmfile.file[i+2] != '\n' && asmfile.file[i+2] != '\r'){
						if(!asmfile.flags & 0x08){
							Asm(0x66);
						}
						sw = asmfile.file[i+1];
						sw2 = asmfile.file[i+2];
					}
					else{
						if(asmfile.flags & 0x08){
							Asm(0x66);
						}
						sw = asmfile.file[i];
						sw2 = asmfile.file[i+1];
					}
					switch(sw){
					case 'a':
						Asm(0x50);
						break;
					case 'c':
						Asm(0x51);
						break;
					case 'd':
						switch(sw2){
						case 'x':
							Asm(0x52);
							break;
						case 'i':
							Asm(0x57);
							break;
						}
						break;
					case 'b':
						switch(sw2){
						case 'x':
							Asm(0x53);
							break;
						case 'p':
							Asm(0x55);
							break;
						}
						break;
					case 's':
						switch(sw2){
						case 'p':
							Asm(0x54);
							break;
						case 'i':
							Asm(0x56);
							break;
						}
						break;
					case 'e':
						Asm(0x06);
						break;
					}
					break;
				}
				break;
			case 'o':
				i+=4;
				for(;asmfile.file[i] == ' ' || asmfile.file[i] == '\t';i++){}
				char sw = 0;
				char sw2 = 0;
				if(asmfile.file[i] == 'e' && asmfile.file[i+1] != 'p' && asmfile.file[i+1] != 'i'){
				
					if(!asmfile.flags & 0x08){
						Asm(0x66);
					}
					sw = asmfile.file[i+1];
					sw2 = asmfile.file[i+2];
				}
				else{
					if(asmfile.flags & 0x08){
						Asm(0x66);
					}
					sw = asmfile.file[i];
					sw2 = asmfile.file[i+1];
				}
				switch(sw){
				case 'a':
					Asm(0x58);
					break;
				case 'c':
					Asm(0x59);
					break;
				case 'd':
					switch(sw2){
					case 's':
						Asm(0x1f);
						break;
					case 'x':
						Asm(0x5a);
						break;
					case 'i':
						Asm(0x5f);
						break;
					}
					break;
				case 'b':
					switch(sw2){
					case 'x':
						Asm(0x5b);
						break;
					case 'p':
						Asm(0x5d);
						break;
					}
					break;
				case 's':
					switch(sw2){
					case 'p':
						Asm(0x5c);
						break;
					case 'i':
						Asm(0x5e);
					}
					Asm(0x54);
					break;
				case 'e':
					Asm(0x07);
					break;
				}
				break;
			}
			break;
		case 'r':
			switch(asmfile.file[i+2]){
			case 't':
				Asm(0xc3);
				break;
			case 'r':
				i+=6;
				for(;asmfile.file[i] == ' ' || asmfile.file[i] == '\t';i++){}
				AsmPS(0x0f,0xc7,decodeReg(asmfile.file[i+1],asmfile.file[i+2])+0xf0);
				break;
			case 'p':
				Asm(0xf3);
				i+=4;
				for(;asmfile.file[i] == ' ' || asmfile.file[i] == '\t';i++){}
				i--;
				continue;
			}
			break;
		case 's':
			switch(asmfile.file[i+1]){
			case 'b':
				i+=4;
				for(;asmfile.file[i] == ' ' || asmfile.file[i] == '\t';i++){}
				commonIns(i,24);
				break;
			case 'h':
				switch(asmfile.file[i+2]){
				case 'l':
					i+=4;
					for(;asmfile.file[i] == ' ' || asmfile.file[i] == '\t';i++){}
					commonIns2(i,0);
					break;
				case 'r':
					i+=4;
					for(;asmfile.file[i] == ' ' || asmfile.file[i] == '\t';i++){}
					commonIns2(i,8);
					break;
				}
			case 't':
				switch(asmfile.file[i+2]){
				case 'c':
					Asm(0xf9);
					break;
				case 'd':
					Asm(0xfd);
					break;
				case 'i':
					Asm(0xfb);
					break;
				case 'o':
					switch(asmfile.file[i+4]){
					case 'b':
						Asm(0xaa);
						break;
					case 'w':
						Asm(0xab);
						break;
					}
					break;
				}
				break;
			case 'u':
				i+=4;
				for(;asmfile.file[i] == ' ' || asmfile.file[i] == '\t';i++){}
				commonIns(i,40);
				break;
			}
			break;
		case 't':
			i+=5;
			for(;asmfile.file[i] == ' ' || asmfile.file[i] == '\t';i++){}
			switch(asmfile.file[i]){
			case 'a':
				switch(asmfile.file[i+1]){
					case 'l':
						AsmP(0xa8,asciiToInt(i+3));
						break;
					case 'x':
						AsmPD(0xa9,asciiToInt(i+3));
						break;
				}
				break;
			case 'e':
				AsmPD(0xa9,asciiToInt(i+4));
				break;
			}
			break;
		case 'x':
			switch(asmfile.file[i+1]){
			case 'o':
				i+=4;
				for(;asmfile.file[i] == ' ' || asmfile.file[i] == '\t';i++){}
				commonIns(i,48);
				break;
			case 'c':
				i+=5;
				for(;asmfile.file[i] == ' ' || asmfile.file[i] == '\t';i++){}
				if(asmfile.file[i+2] == ','){
					if(asmfile.file[i] == 'a'){
						Asm(decodeReg(asmfile.file[i+3],0) + 0x90);
					}
					else if(asmfile.file[i+3] == 'a'){
						Asm(decodeReg(asmfile.file[i],0) + 0x90);
					}
				}
				break;
			}
			break;
		}
		while(asmfile.file[i] != '\n' && i < size){
			i++;
		}
	}
	for(int i = 0;i < varafCount;i++){
		for(int i2 = 0;i2 < varCount;i2++){
			if(!memcmp(varaf[i].name2,var[i2].name,var[i2].size)){
				for(int i3 = 0;i3 < lblCount;i3++){
					if(!memcmp(varaf[i].name,lbl[i3].name,lbl[i3].nameSz)){
						int bob = lbl[i3].pos + 0x00400000 + 0x150;
						var[i2].data[0] = bob;
						var[i2].data[1] = bob >> 8;
						var[i2].data[2] = bob >> 16;
						var[i2].data[3] = bob >> 24;
						break;
					}
				}
				break;
			}
		}
	}
	linkLabels();
	if(asmfile.flags & 0x01){
		closeBootloader();
	}
	else{
		createSection(asmOffset,dataGrote);
		closeExe();
	}
	if(optHeader[68] == 3){
		CreateProcessA(asmfile.name,0,0,0,0,0x00000010,0,0,&startupinfo,&process_info);
	}
	else if(optHeader[68] == 2){
		CreateProcessA(asmfile.name,0,0,0,0,0,0,0,&startupinfo,&process_info);
	}
}
