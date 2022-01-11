#include <windows.h>
#include <math.h>
#include <intrin.h>
#include <stdio.h>
#include <glew.h>
#include <GL/gl.h>
#include <main.h>

#define resx 256
#define resy 256

#define RENDERDISTANCE 32

#define VRAM resx*resy*4

#define gpu 0

#define PI_2 1.57079632679489661923


PROPERTIES *properties;
PLAYERDATA *player;

int shaderStatus;

typedef struct RAY	{
	float x;
	float y;
	float z;
	float vx;
	float vy;
	float vz;
}RAY;

int platformC;
int deviceC;
int result;
int err;
int fsize;
int count = VRAM/4;
int count2 = 1;

char *CLsource;
unsigned char *map;

char settings;
char touchStatus;
char threadStatus;
char abilities;
char blockSel = 7;

float mousex;
float mousey;

BITMAPINFO bmi = { sizeof(BITMAPINFOHEADER),resx,resy,1,32,BI_RGB };	

HANDLE renderingThread;
HANDLE consoleThread;
HANDLE physicsThread;

const char name[] = "window";

HWND window;
HDC dc;
MSG Msg;

void hitboxZdown(float x,float y,float z){
	if(map[((int)x + (int)y * properties->lvlSz + (int)z * properties->lvlSz * properties->lvlSz) * 4]){
		z -= player->zvel;
		switch(!map[((int)x + (int)y * properties->lvlSz + (int)z * properties->lvlSz * properties->lvlSz) * 4]){
		case 0:
			break;
		case 12:
			player->zvel += 2.0;
			break;
		default:
			touchStatus |= 0x01;
			break;
		}
	}
	if(z < 0){
		player->xpos = 128.5;
		player->ypos = 128.5;
		player->zpos = 4;
		player->xvel = 0;
		player->yvel = 0;
		player->zvel = 0;
	}
}

void hitboxZup(int x,int y,float z){
	if(z > properties->lvlSz || map[(x + y * properties->lvlSz + (int)z * properties->lvlSz * properties->lvlSz) * 4]){
		z -= player->zvel;
		switch(!map[(x + y * properties->lvlSz + (int)z * properties->lvlSz * properties->lvlSz) * 4]){
		case 0:
			break;
		case 12:
			player->zvel += 2.0;
			break;
		default:
			touchStatus |= 0x02;
			break;
		}
	}
}

void hitboxXdown(float x,int y,int z){
	if(x < 0 || x > properties->lvlSz || map[((int)x + y * properties->lvlSz + z * properties->lvlSz * properties->lvlSz) * 4]){
		x -= player->xvel;
		switch(!map[((int)x + y * properties->lvlSz + z * properties->lvlSz * properties->lvlSz) * 4]){
		case 0:
			break;
		case 12:
			break;
		default:
			touchStatus |= 0x04;
			break;
		}
	}
}

void hitboxXup(float x,int y,int z){
	if(x < 0 || x > properties->lvlSz || map[((int)x + y * properties->lvlSz + z * properties->lvlSz * properties->lvlSz) * 4]){
		x -= player->xvel;
		switch(!map[((int)x + y * properties->lvlSz + z * properties->lvlSz * properties->lvlSz) * 4]){
		case 0:
			break;
		case 12:
			break;
		default:
			touchStatus |= 0x08;
			break;
		}

	}
}

void hitboxYdown(int x,float y,int z){
	if(y < 0 || y > properties->lvlSz || map[(x + (int)y * properties->lvlSz + z * properties->lvlSz * properties->lvlSz) * 4]){
		y -= player->yvel;
		switch(!map[(x + (int)y * properties->lvlSz + z * properties->lvlSz * properties->lvlSz) * 4]){
		case 0:
			break;
		case 12:
			break;
		default:
			touchStatus |= 0x10;
			break;
		}
	}
}

void hitboxYup(int x,float y,int z){
	if(y < 0 || y > properties->lvlSz || map[(x + (int)y * properties->lvlSz + z * properties->lvlSz * properties->lvlSz) * 4]){
		y -= player->yvel;
		switch(!map[(x + (int)y * properties->lvlSz + z * properties->lvlSz * properties->lvlSz) * 4]){
		case 0:
			break;
		case 12:
			break;
		default:
			touchStatus |= 0x20;
			break;
		}
	}
}

//functie die ervoor zorgt dat een ray 1 stap vooruit gaat

void rayItterate(RAY *ray){
	float dx,dy,dz;
	if(ray->vx > 0){
		dx = (int)ray->x + 1 - ray->x;
	}
	else{
		dx = (int)ray->x - ray->x;
		if(!dx){
			dx = -1;
		}
	}
	if(ray->vy > 0){
		dy = (int)ray->y + 1 - ray->y;
	}
	else{
		dy = (int)ray->y - ray->y;
		if(!dy){
			dy = -1;
		}
	}	
	if(ray->vz > 0){
		dz = (int)ray->z + 1 - ray->z;
	}
	else{
		dz = (int)ray->z - ray->z;
		if(!dz){
			dz = -1;
		}
	}
	if(dx / ray->vx < dy / ray->vy){
		if(dx / ray->vx < dz / ray->vz){
			ray->x += dx;
			ray->y += ray->vy * dx / ray->vx;
			ray->z += ray->vz * dx / ray->vx;
		}
		else{
			ray->x += ray->vx * dz / ray->vz;
			ray->y += ray->vy * dz / ray->vz;
			ray->z += dz;
		}
	}
	else if(dy / ray->vy < dz / ray->vz){
		ray->x += ray->vx * dy / ray->vy;
		ray->y += dy;
		ray->z += ray->vz * dy / ray->vy;
	}
	else{
		ray->x += ray->vx * dz / ray->vz;
		ray->y += ray->vy * dz / ray->vz;
		ray->z += dz;
	}
}

updateLight(int pos,unsigned char color){
	RAY ray;
	for(float i = 0;i <= 3.14*2;i+=0.03){
		for(float i2 = 0;i2 <= 3.14;i2+=0.03){
			ray.x  = pos % (properties->lvlSz*4) / 4 + 0.5;
			ray.y  = pos / (properties->lvlSz*4) * (properties->lvlSz*4) % (properties->lvlSz*properties->lvlSz*4) / (properties->lvlSz*4) + 0.5;
			ray.z  = pos / (properties->lvlSz*properties->lvlSz*4) + 0.5;
			ray.vx = sinf(i2) * cosf(i);
			ray.vy = cosf(i2) * cosf(i);
			ray.vz = sinf(i);
			for(int i = 0;i < 32;i++){
				rayItterate(&ray);
				if(ray.x < 0 || ray.y < 0 || ray.z < 0){
					break;
				}
				int block = ((int)ray.x + (int)ray.y*properties->lvlSz + (int)ray.z*properties->lvlSz*properties->lvlSz)*4;
				if(map[block]){
					if(ray.x - (int)ray.x == 0.0){
						ray.vx = -ray.vx;
					}
					else if(ray.y - (int)ray.y == 0.0){
						ray.vy = -ray.vy;
					}
					else if(ray.z - (int)ray.z == 0.0){
						ray.vz = -ray.vz;
					}
					continue;
				}
				if(map[block+1] != 255 && color & 0x01){
					map[block+1]++;
				}
				if(map[block+2] != 255 && color & 0x02){
					map[block+2]++;
				}
				if(map[block+3] != 255 && color & 0x04){
					map[block+3]++;
				}
			}
		}
	}
}

void updateLight2(){
	for(int i = 0;i < MAPRAM;i+=4){
		map[i+1] = 0;
		map[i+2] = 0;
		map[i+3] = 0;
	}
	for(int i = 0;i < MAPRAM;i+=4){
		switch(map[i]){
		case 8:
			map[i] = 0;
			updateLight(i+1,1);
			map[i] = 8;
			break;
		case 9:
			map[i] = 0;
			updateLight(i+1,2);
			map[i] = 9;
			break;	
		case 10:
			map[i] = 0;
			updateLight(i+1,4);
			map[i] = 10;
			break;	
		case 11:
			map[i] = 0;
			updateLight(i+1,7);
			map[i] = 11;
			break;	
		}
	}
}

void updateBlock(int pos){
	if(!settings & 4){
		switch(blockSel){
		case 8:
			updateLight(pos,1);
			map[pos] = blockSel;
			break;
		case 9:
			updateLight(pos,2);
			map[pos] = blockSel;
			break;	
		case 10:
			updateLight(pos,4);
			map[pos] = blockSel;
			break;	
		case 11:
			updateLight(pos,7);
			map[pos] = blockSel;
			break;
		default:
			map[pos] = blockSel;
			updateLight2();
			break;	
		}
	}
	else{
		map[pos] = blockSel;
	}
	glMes[glMesC].id = 3;
	glMesC++;
}

void deleteBlock(int pos){
	map[pos] = 0;
	if(!settings & 4){
		updateLight2();
	}
	glMes[glMesC].id = 3;
	glMesC++;
}

//windows messages kunnen hier worden behandeld

long _stdcall proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	switch(msg){
	case WM_ACTIVATE:
		switch(wParam){
		case WA_INACTIVE:
			consoleThread = CreateThread(0,0,commands,0,0,0);
			SuspendThread(physicsThread);
			break;
		default:
			TerminateThread(consoleThread,0);
			ResumeThread(physicsThread);
			break;
		}
	case WM_KEYDOWN:
		if(GetKeyState(VK_F1) & 0x80){
			settings ^= 1;
		}
		if(GetKeyState(0x46) & 0x80){
			settings ^= 2;
			if(settings & 2){
				SetWindowPos(window,0,0,0,GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN),0);	
				properties->xres = GetSystemMetrics(SM_CXSCREEN);
				properties->yres = GetSystemMetrics(SM_CYSCREEN);
				glMes[glMesC].id = 0;
				glMesC++;
			}
			else{
				SetWindowPos(window,0,0,0,256,256,0);	
				properties->xres = 256;
				properties->yres = 256;
				glMes[glMesC].id = 0;
				glMesC++;
			}
		}
		if(GetKeyState(VK_ADD) & 0x80){
			blockSel++;
		}
		if(GetKeyState(VK_SUBTRACT) & 0x80){
			blockSel--;
		}
		if(GetKeyState(VK_F2) & 0x80){
			settings ^= 4;
			if(settings & 0x04){
				for(int i = 0;i < MAPRAM;i+=4){
					if(i + i / properties->lvlSz + i / properties->lvlSz / properties->lvlSz & 4){
						map[i+1] = 120;
						map[i+2] = 120;
						map[i+3] = 120;
					}
					else{
						map[i+1] = 40;
						map[i+2] = 40;
						map[i+3] = 40;
					}

				}
				glMes[glMesC].id = 3;
				glMesC++;
			}
			else{
				updateLight2();
				glMes[glMesC].id = 3;
				glMesC++;
			}
		}
		if(GetKeyState(VK_F3) & 0x80){
			updateLight2();
			glMes[glMesC].id = 3;
			glMesC++;
		}
		if(settings & 0x01){
			if(GetKeyState(0x45) & 0x80){
				map[(int)player->xpos + (int)player->ypos * properties->lvlSz + ((int)player->zpos - 1) * properties->lvlSz * properties->lvlSz] = blockSel;
				glMes[glMesC].id = 1;
				glMes[glMesC].data1 = player->xpos;
				glMes[glMesC].data2 = player->ypos;
				glMes[glMesC].data3 = player->zpos - 1;
				glMesC++;
			}
		}
		break;
	case WM_MOUSEMOVE:{
			POINT curp;
			GetCursorPos(&curp);
			mousex = (float)(curp.x - 50) / 250;
			mousey = (float)(curp.y - 50) / 250;
			player->xangle += mousex;
			player->yangle -= mousey;
			if(player->yangle < -1.6){
				player->yangle = -1.6;
			}
			if(player->yangle > 1.6){
				player->yangle = 1.6;
			}
			SetCursorPos(50,50);
		}
		break;
	case WM_LBUTTONDOWN:{
			RAY ray;
			ray.x = player->xpos;
			ray.y = player->ypos;
			ray.z = player->zpos;
			ray.vz = sinf(player->yangle);
			ray.vx = cosf(player->xangle) * cosf(player->yangle);
			ray.vy = sinf(player->xangle) * cosf(player->yangle);
			for(int i = 0;i < 254;i++){
				rayItterate(&ray);
				if(ray.x < 0 || ray.y < 0 || ray.z < 0){
					break;
				}	
				int block = ((int)ray.x + (int)ray.y * properties->lvlSz + (int)ray.z * properties->lvlSz * properties->lvlSz) * 4;
				if(map[block]){
					if(ray.z - (int)ray.z == 0){
						updateBlock(block - properties->lvlSz * properties->lvlSz * 4);
					}
					else if(ray.y - (int)ray.y == 0){
						updateBlock(block - properties->lvlSz * 4);
					}
					else{
						updateBlock(block - 1 * 4);
					}
					break;
				}
				block = ((int)(ray.x - 0.00001) + (int)(ray.y - 0.00001) * properties->lvlSz + (int)(ray.z - 0.00001) * properties->lvlSz * properties->lvlSz) * 4;
				if(map[block]){
					if(ray.z - (int)ray.z == 0){
						updateBlock(block + properties->lvlSz * properties->lvlSz * 4);
					}
					else if(ray.y - (int)ray.y == 0){
						updateBlock(block + properties->lvlSz * 4);
					}
					else{
						updateBlock(block + 1 * 4);
					}
					break;
				}
			}
			break;
		}
	case WM_RBUTTONDOWN:{
			RAY ray;
			ray.x = player->xpos;
			ray.y = player->ypos;
			ray.z = player->zpos;
			ray.vz = sinf(player->yangle);
			ray.vx = cosf(player->xangle) * cosf(player->yangle);
			ray.vy = sinf(player->xangle) * cosf(player->yangle);
			for(int i = 0;i < 12;i++){
				rayItterate(&ray);
				int block = ((int)ray.x + (int)ray.y * properties->lvlSz + (int)ray.z * properties->lvlSz * properties->lvlSz) * 4;
				if(map[block]){
					deleteBlock(block);
					break;
				}
			}
			break;
		}
	}
	return DefWindowProc(hwnd,msg,wParam,lParam);
}

WNDCLASS wndclass = {0,proc,0,0,0,0,0,0,name,name};

void physics()
{
	player->xfov = 2;
	for (;;)
	{
		player->xydir = cosf(player->yangle);
		player->xdir  = cosf(player->xangle);
	 	player->ydir  = sinf(player->xangle);
	 	player->zdir  = sinf(player->yangle);
		 
		//movement

		if (settings & 0x01)
		{
			if (GetKeyState(0x57) & 0x80)
			{
				player->xpos += cosf(player->xangle) / 8;
				player->ypos += sinf(player->xangle) / 8;
			}
			if (GetKeyState(0x53) & 0x80)
			{
				player->xpos -= cosf(player->xangle) / 8;
				player->ypos -= sinf(player->xangle) / 8;
			}
			if (GetKeyState(0x44) & 0x80)
			{
				player->xpos += cosf(player->xangle + PI_2) / 8;
				player->ypos += sinf(player->xangle + PI_2) / 8;
			}
			if (GetKeyState(0x41) & 0x80)
			{
				player->xpos -= cosf(player->xangle + PI_2) / 8;
				player->ypos -= sinf(player->xangle + PI_2) / 8;
			}
			if (GetKeyState(VK_SPACE) & 0x80)
			{
				if(GetKeyState(VK_CONTROL) & 0x80){
					player->zpos += 0.45;
				}
				else{
					player->zpos += 0.15;
				}
			}
			if (GetKeyState(VK_SHIFT) & 0x80)
			{
				if(GetKeyState(VK_CONTROL) & 0x80){
					player->zpos -= 0.45;
				}
				else{
					player->zpos -= 0.15;
				}
			}
		}
		else
		{
			player->zvel -= 0.015;

			player->xpos += player->xvel;
			player->ypos += player->yvel;
			player->zpos += player->zvel;

			//collision code

			if (player->zvel < 0){
				hitboxZdown(player->xpos - 0.2, player->ypos - 0.2, player->zpos - 1.7);
				hitboxZdown(player->xpos + 0.2, player->ypos - 0.2, player->zpos - 1.7);
				hitboxZdown(player->xpos - 0.2, player->ypos + 0.2, player->zpos - 1.7);
				hitboxZdown(player->xpos + 0.2, player->ypos + 0.2, player->zpos - 1.7);
			}
			else{
				hitboxZup(player->xpos - 0.2, player->ypos - 0.2, player->zpos + 0.2);
				hitboxZup(player->xpos + 0.2, player->ypos - 0.2, player->zpos + 0.2);
				hitboxZup(player->xpos - 0.2, player->ypos + 0.2, player->zpos + 0.2);
				hitboxZup(player->xpos + 0.2, player->ypos + 0.2, player->zpos + 0.2);
			}
			if (player->xvel < 0){
				hitboxXdown(player->xpos - 0.2, player->ypos - 0.2, player->zpos + 0.2);
				hitboxXdown(player->xpos - 0.2, player->ypos - 0.2, player->zpos - 1.7);
				hitboxXdown(player->xpos - 0.2, player->ypos + 0.2, player->zpos + 0.2);
				hitboxXdown(player->xpos - 0.2, player->ypos + 0.2, player->zpos - 1.7);
				hitboxXdown(player->xpos - 0.2, player->ypos + 0.2, player->zpos - 0.7);
				hitboxXdown(player->xpos - 0.2, player->ypos - 0.2, player->zpos - 0.7);
			}
			else{
				hitboxXup(player->xpos + 0.2, player->ypos - 0.2, player->zpos + 0.2);
				hitboxXup(player->xpos + 0.2, player->ypos - 0.2, player->zpos - 1.7);
				hitboxXup(player->xpos + 0.2, player->ypos + 0.2, player->zpos + 0.2);
				hitboxXup(player->xpos + 0.2, player->ypos + 0.2, player->zpos - 1.7);
				hitboxXup(player->xpos + 0.2, player->ypos + 0.2, player->zpos - 0.7);
				hitboxXup(player->xpos + 0.2, player->ypos - 0.2, player->zpos - 0.7);
			}
			if (player->yvel < 0){
				hitboxYdown(player->xpos - 0.2, player->ypos - 0.2, player->zpos + 0.2);
				hitboxYdown(player->xpos - 0.2, player->ypos - 0.2, player->zpos - 1.7);
				hitboxYdown(player->xpos + 0.2, player->ypos - 0.2, player->zpos + 0.2);
				hitboxYdown(player->xpos + 0.2, player->ypos - 0.2, player->zpos - 1.7);
				hitboxYdown(player->xpos + 0.2, player->ypos - 0.2, player->zpos - 0.7);
				hitboxYdown(player->xpos - 0.2, player->ypos - 0.2, player->zpos - 0.7);
			}
			else{
				hitboxYup(player->xpos - 0.2, player->ypos + 0.2, player->zpos + 0.2);
				hitboxYup(player->xpos - 0.2, player->ypos + 0.2, player->zpos - 1.7);
				hitboxYup(player->xpos + 0.2, player->ypos + 0.2, player->zpos + 0.2);
				hitboxYup(player->xpos + 0.2, player->ypos + 0.2, player->zpos - 1.7);
				hitboxYup(player->xpos + 0.2, player->ypos + 0.2, player->zpos - 0.7);
				hitboxYup(player->xpos - 0.2, player->ypos + 0.2, player->zpos - 0.7);
			}
			if (touchStatus & 0x01){
				player->zpos -= player->zvel;
				player->zvel = 0;
				if (GetKeyState(VK_SPACE) & 0x80){
					player->zvel = 0.25;
				}
				if (GetKeyState(0x57) & 0x80)
				{
					player->xvel += cosf(player->xangle) / 60;
					player->yvel += sinf(player->xangle) / 60;
				}
				if (GetKeyState(0x53) & 0x80)
				{
					player->xvel -= cosf(player->xangle) / 60;
					player->yvel -= sinf(player->xangle) / 60;
				}
				if (GetKeyState(0x44) & 0x80)
				{
					player->xvel += cosf(player->xangle + PI_2) / 60;
					player->yvel += sinf(player->xangle + PI_2) / 60;
				}
				if (GetKeyState(0x41) & 0x80)
				{
					player->xvel -= cosf(player->xangle + PI_2) / 60;
					player->yvel -= sinf(player->xangle + PI_2) / 60;
				}
			}
			if (touchStatus & 0x02){
				player->zpos -= player->zvel;
				player->zvel = 0;
			}
			if (touchStatus & 0x04){
				player->xpos -= player->xvel;
				player->xvel = 0;
				if(player->zvel < -0.05){
					player->zvel = -0.05;
				}
				if(GetKeyState(VK_SPACE) & 0x80 && player->zvel > 0 && !(touchStatus & 0x01)){
					player->zvel = 0.25;
					player->xvel = 0.25;
				}
			}
			if (touchStatus & 0x08){
				player->xpos -= player->xvel;
				player->xvel = 0;
				if(player->zvel < -0.05){
					player->zvel = -0.05;
				}
				if(GetKeyState(VK_SPACE) & 0x80 && player->zvel > 0 && !(touchStatus & 0x01)){
					player->zvel = 0.25;
					player->xvel = -0.25;
				}
			}
			if (touchStatus & 0x10){
				player->ypos -= player->yvel;
				player->yvel = 0;
				if(player->zvel < -0.05){
					player->zvel = -0.05;
				}
				if(GetKeyState(VK_SPACE) & 0x80 && player->zvel > 0 && !(touchStatus & 0x01)){
					player->zvel = 0.25;
					player->yvel = 0.25;
				}
			}
			if (touchStatus & 0x20){
				player->ypos -= player->yvel;
				player->yvel = 0;
				if(player->zvel < -0.05){
					player->zvel = -0.05;
				}
				if(GetKeyState(VK_SPACE) & 0x80 && player->zvel > 0 && !(touchStatus & 0x01)){
					player->zvel = 0.25;
					player->yvel = -0.25;
				}
			}
			if (touchStatus){
				player->xvel /= 1.08;
				player->yvel /= 1.08;
			}
		}

		touchStatus = 0;

		player->zvel /= 1.01;
		Sleep(15);
	}
}

void main()
{
	//om de Sleep functie accurater te maken
	timeBeginPeriod(1);

	map        = HeapAlloc(GetProcessHeap(),8,MAPRAM);
	player     = HeapAlloc(GetProcessHeap(),8,sizeof(PLAYERDATA));
	properties = HeapAlloc(GetProcessHeap(),8,sizeof(PROPERTIES));

	wndclass.hInstance = GetModuleHandle(0);
	RegisterClass(&wndclass);
	window = CreateWindowEx(0,name,name,0x90080000,0,0,resy + 16,resx + 39,0,0,wndclass.hInstance,0);
	dc = GetDC(window);

	player->xfov = 16/9;
	player->yfov = 1;
	player->zpos = 10;
	player->xpos = 6;
	player->ypos = 6;

	properties->lvlSz = MAPSZ;
	properties->renderDistance = RENDERDISTANCE;

	properties->xres = resx;
	properties->yres = resy;

	settings = 1;
	ShowCursor(0);

	levelgen();

	renderingThread = CreateThread(0,0,openGL,0,0,0);
	physicsThread   = CreateThread(0,0,physics,0,0,0);

	for(;;){
		while(PeekMessage(&Msg,window,0,0,0)){
			GetMessage(&Msg,window,0,0);
			TranslateMessage(&Msg);
			DispatchMessageW(&Msg);
		}
		Sleep(1);
	}
}
