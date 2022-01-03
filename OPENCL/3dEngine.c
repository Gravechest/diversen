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

#define MAPSZ 256
#define MAPRAM MAPSZ*MAPSZ*MAPSZ

#define VRAM resx*resy*4

#define gpu 0

#define PI_2 1.57079632679489661923


PROPERTIES *properties;
PLAYERDATA *player;

int shaderStatus;

typedef struct RAY{
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
char *map;

char settings;
char touchStatus;
char threadStatus;
char blockSel = 1;

BITMAPINFO bmi = { sizeof(BITMAPINFOHEADER),resx,resy,1,32,BI_RGB };	

const char name[] = "window";

HWND window;
HDC dc;
MSG Msg;

char hitbox(float x,float y,float z){
	if(x < 0){
		touchStatus |= 0x01;
	}
	if(y < 0){
		touchStatus |= 0x02;
	}
	if(z < 0){
		touchStatus |= 0x04;
	}
	int m = (int)x + (int)y * properties->lvlSz + (int)z * properties->lvlSz * properties->lvlSz;
	if(map[m]){
		x -= player->xvel;
		m = (int)x + (int)y * properties->lvlSz + (int)z * properties->lvlSz * properties->lvlSz;
		if(!map[m]){
			touchStatus |= 0x08;
		}
		x += player->xvel;
		y -= player->yvel;
		m = (int)x + (int)y * properties->lvlSz + (int)z * properties->lvlSz * properties->lvlSz;
		if(!map[m]){
			touchStatus |= 0x10;
		}	
		y += player->yvel;
		z -= player->zvel;
		m = (int)x + (int)y * properties->lvlSz + (int)z * properties->lvlSz * properties->lvlSz;
		if(!map[m]){
			touchStatus |= 0x20;
		}
		z += player->zvel;
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

//windows messages kunnen hier worden behandeld

long _stdcall proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	switch(msg){
	case WM_KEYDOWN:
		if(GetKeyState(VK_F1) & 0x80){
			ShowCursor(settings & 1);
			settings ^= 1;
			if(settings & 1){
				SetCursorPos(50,50);
			}
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
		break;
	case WM_MOUSEMOVE:
		if(settings & 1){
			POINT curp;
			GetCursorPos(&curp);
			player->xpitch += (float)(curp.x - 50) / 250;
			player->ypitch -= (float)(curp.y - 50) / 250;
			if(player->ypitch < -2){
				player->ypitch = -2;
			}
			SetCursorPos(50,50);
		}
		break;
	case WM_LBUTTONDOWN:{
			RAY ray;
			ray.x = player->xpos;
			ray.y = player->ypos;
			ray.z = player->zpos;
			ray.vz = sinf(player->ypitch);
			ray.vx = cosf(player->xpitch) * cosf(player->ypitch);
			ray.vy = sinf(player->xpitch) * cosf(player->ypitch);
			for(int i = 0;i < 254;i++){
				rayItterate(&ray);
				if(ray.x < 0 || ray.y < 0 || ray.z < 0){
					break;
				}	
				int block = (int)ray.x + (int)ray.y * properties->lvlSz + (int)ray.z * properties->lvlSz * properties->lvlSz;
				if(map[block]){
					if(ray.z - (int)ray.z == 0){
						map[block - properties->lvlSz * properties->lvlSz] = blockSel;
						glMes[glMesC].id    = 1;
						glMes[glMesC].data1 = ray.x;
						glMes[glMesC].data2 = ray.y;
						glMes[glMesC].data3 = ray.z-1;
						glMesC++;
					}
					else if(ray.y - (int)ray.y == 0){
						map[block - properties->lvlSz] = blockSel;
						glMes[glMesC].id    = 1;
						glMes[glMesC].data1 = ray.x;
						glMes[glMesC].data2 = ray.y-1;
						glMes[glMesC].data3 = ray.z;
						glMesC++;
					}
					else{
						map[block - 1] = blockSel;
						glMes[glMesC].id    = 1;
						glMes[glMesC].data1 = ray.x-1;
						glMes[glMesC].data2 = ray.y;
						glMes[glMesC].data3 = ray.z;
						glMesC++;
					}
					break;
				}
				block = (int)(ray.x - 0.00001) + (int)(ray.y - 0.00001) * properties->lvlSz + (int)(ray.z - 0.00001) * properties->lvlSz * properties->lvlSz;
				if(map[block]){
					if(ray.z - (int)ray.z == 0){
						map[block + properties->lvlSz * properties->lvlSz] = blockSel;
						glMes[glMesC].id    = 1;
						glMes[glMesC].data1 = ray.x;
						glMes[glMesC].data2 = ray.y;
						glMes[glMesC].data3 = ray.z;
						glMesC++;
					}
					else if(ray.y - (int)ray.y == 0){
						map[block + properties->lvlSz] = blockSel;
						glMes[glMesC].id    = 1;
						glMes[glMesC].data1 = ray.x;
						glMes[glMesC].data2 = ray.y;
						glMes[glMesC].data3 = ray.z;
						glMesC++;
					}
					else{
						map[block + 1] = blockSel;
						glMes[glMesC].id    = 1;
						glMes[glMesC].data1 = ray.x;
						glMes[glMesC].data2 = ray.y;
						glMes[glMesC].data3 = ray.z;
						glMesC++;
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
			ray.vz = sinf(player->ypitch);
			ray.vx = cosf(player->xpitch) * cosf(player->ypitch);
			ray.vy = sinf(player->xpitch) * cosf(player->ypitch);
			for(int i = 0;i < 12;i++){
				rayItterate(&ray);
				int block = (int)ray.x + (int)ray.y * properties->lvlSz + (int)ray.z * properties->lvlSz * properties->lvlSz;
				if(map[block]){
					map[block] = 0;
					glMes[glMesC].id    = 1;
					glMes[glMesC].data1 = ray.x;
					glMes[glMesC].data2 = ray.y;
					glMes[glMesC].data3 = ray.z;
					glMesC++;
					break;
				}
			}
			break;
		}
	}
	return DefWindowProc(hwnd,msg,wParam,lParam);
}

WNDCLASS wndclass = {0,proc,0,0,0,0,0,0,name,name};


void physics(){
	for(;;){
		//movement
		if(GetKeyState(0x57) & 0x80){
			player->xvel += cosf(player->xpitch) / 120;
			player->yvel += sinf(player->xpitch) / 120;
		}
		if(GetKeyState(0x53) & 0x80){
			player->xvel -= cosf(player->xpitch) / 120;
			player->yvel -= sinf(player->xpitch) / 120;
		}
		if(GetKeyState(0x44) & 0x80){
			player->xvel += cosf(player->xpitch + PI_2) / 120;
			player->yvel += sinf(player->xpitch + PI_2) / 120;
		}
		if(GetKeyState(0x41) & 0x80){
			player->xvel -= cosf(player->xpitch + PI_2) / 120;
			player->yvel -= sinf(player->xpitch + PI_2) / 120;
		}
		if(GetKeyState(VK_SHIFT) & 0x80){
			player->zpos += 1;
		}
		if(!settings & 0x01){
			if(GetKeyState(VK_UP) & 0x80){
				player->ypitch += 0.03;
			}
			if(GetKeyState(VK_DOWN) & 0x80 && player->ypitch > -1){
				player->ypitch -= 0.03;
			}
			if(GetKeyState(VK_LEFT) & 0x80){
				player->xpitch -= 0.03;
			}
			if(GetKeyState(VK_RIGHT) & 0x80){
				player->xpitch += 0.03;
			}
		}

		player->zvel -= 0.015;

		player->xpos += player->xvel;
		player->ypos += player->yvel;
		player->zpos += player->zvel;

		//collision code 

		hitbox(player->xpos + 0.2,player->ypos + 0.2,player->zpos - 1.8);
		hitbox(player->xpos + 0.2,player->ypos + 0.2,player->zpos + 0.2);
		hitbox(player->xpos + 0.2,player->ypos - 0.2,player->zpos - 1.8);
		hitbox(player->xpos + 0.2,player->ypos - 0.2,player->zpos + 0.2);
		hitbox(player->xpos - 0.2,player->ypos + 0.2,player->zpos - 1.8);
		hitbox(player->xpos - 0.2,player->ypos + 0.2,player->zpos + 0.2);
		hitbox(player->xpos - 0.2,player->ypos - 0.2,player->zpos - 1.8);
		hitbox(player->xpos - 0.2,player->ypos - 0.2,player->zpos + 0.2);

		if(touchStatus & 0x01){
			player->xpos -= player->xvel;
			player->xvel = 0;
		}
		if(touchStatus & 0x02){
			player->ypos -= player->yvel;
			player->yvel = 0;
		}
		if(touchStatus & 0x04){
			player->zpos -= player->zvel;
			if(GetKeyState(VK_SPACE) & 0x80){
				player->zvel = 0.5;
			}
			else{
				player->zvel = 0;
			}
		}
		if(touchStatus & 0x08){
			player->xpos -= player->xvel;
			player->xvel = 0;
		}
		if(touchStatus & 0x10){
			player->ypos -= player->yvel;
			player->yvel = 0;
		}
		if(touchStatus & 0x20){
			player->zpos -= player->zvel;
			if(GetKeyState(VK_SPACE) & 0x80){
				player->zvel = 0.5;
			}
			else{
				player->zvel = 0;
			}
		}

		touchStatus = 0;

		player->xvel /= 1.08;
		player->yvel /= 1.08;
		player->zvel /= 1.02;
		Sleep(15);
	}
}

void main(){
	//om de Sleep functie accurater te maken
	timeBeginPeriod(1);

	map        = HeapAlloc(GetProcessHeap(),8,MAPRAM);
	player     = HeapAlloc(GetProcessHeap(),8,sizeof(PLAYERDATA));
	properties = HeapAlloc(GetProcessHeap(),8,sizeof(PROPERTIES));

	wndclass.hInstance = GetModuleHandle(0);
	RegisterClass(&wndclass);
	window = CreateWindowEx(0,name,name,0x90080000,0,0,resy + 16,resx + 39,0,0,wndclass.hInstance,0);
	dc = GetDC(window);

	player->xfov = 0.5;
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

	CreateThread(0,0,openGL,0,0,0);
	CreateThread(0,0,physics,0,0,0);

	for(;;){
		while(PeekMessage(&Msg,window,0,0,0)){
			GetMessage(&Msg,window,0,0);
			TranslateMessage(&Msg);
			DispatchMessageW(&Msg);
		}
		Sleep(1);
	}
}
