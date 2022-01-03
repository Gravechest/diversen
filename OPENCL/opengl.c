#include <windows.h>
#include <glew.h>
#include <main.h>
#include <stdio.h>

char *VERTsource;
char *FRAGsource;

int glMesC;

OPENGLMESSAGE *glMes;

unsigned int shaderProgram;
unsigned int VBO;

unsigned int vertexShader;
unsigned int fragmentShader;
unsigned int mapText;
unsigned int VAO;

PIXELFORMATDESCRIPTOR pfd = {sizeof(PIXELFORMATDESCRIPTOR), 1,
PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,PFD_TYPE_RGBA,
24,0, 0, 0, 0, 0, 0,0,0,0,
0,0,0,0,32,0,0,PFD_MAIN_PLANE,
0,0,0,0	};

float quad[12] = {1.0,1.0 ,-1.0,1.0 ,1.0,-1.0 ,-1.0,-1.0 ,-1.0,1.0 ,1.0,-1.0};

void openGL(){
	glMes = HeapAlloc(GetProcessHeap(),8,sizeof(OPENGLMESSAGE) * 256);

	HANDLE h = CreateFile("vertex.vert",GENERIC_READ,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
	int fsize = GetFileSize(h,0);
	VERTsource = HeapAlloc(GetProcessHeap(),8,fsize+1);
	ReadFile(h,VERTsource,fsize+1,0,0);
	CloseHandle(h);

	h = CreateFile("fragment.frag",GENERIC_READ,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
	fsize = GetFileSize(h,0);
	FRAGsource = HeapAlloc(GetProcessHeap(),8,fsize+1);
	ReadFile(h,FRAGsource,fsize+1,0,0);
	CloseHandle(h);

	SetPixelFormat(dc, ChoosePixelFormat(dc, &pfd), &pfd);
	wglMakeCurrent(dc, wglCreateContext(dc));

	glewInit();

	shaderProgram = glCreateProgram();
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(vertexShader,1,(const char**)&VERTsource,0);
	glShaderSource(fragmentShader,1,(const char**)&FRAGsource,0);
	glCompileShader(vertexShader);
	glCompileShader(fragmentShader);
	char *bericht = HeapAlloc(GetProcessHeap(),8,100);
	glGetShaderInfoLog(fragmentShader,100,0,bericht);
	printf("%s\n",bericht);
	glAttachShader(shaderProgram,vertexShader);
	glAttachShader(shaderProgram,fragmentShader);
	glLinkProgram(shaderProgram);
	glUseProgram(shaderProgram);
	glGenVertexArrays(1,&VAO);
	glBindVertexArray(VAO);
	glGenTextures(1,&mapText);
	glBindTexture(GL_TEXTURE_3D,mapText);
	glTexImage3D(GL_TEXTURE_3D,0,GL_RED,properties->lvlSz,properties->lvlSz,properties->lvlSz,0,GL_RED,GL_UNSIGNED_BYTE,map);
	glGenerateMipmap(GL_TEXTURE_3D);
	glCreateBuffers(1,&VBO);
	glBindBuffer(GL_ARRAY_BUFFER,VBO);
	glBufferData(GL_ARRAY_BUFFER,12 * sizeof(float),quad,GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,2,GL_FLOAT,0,2 * sizeof(float),0);
	glUniform2i(glGetUniformLocation(shaderProgram,"reso"),properties->xres,properties->yres);
	glUniform2f(glGetUniformLocation(shaderProgram,"fov"),player->xfov,player->yfov);
	glUniform1i(glGetUniformLocation(shaderProgram,"mapSz"),properties->lvlSz);
	for(;;){
		while(glMesC > 0){
			glMesC--;
			switch(glMes[glMesC].id){
			case 0:	
				glViewport(0,0,properties->xres,properties->yres);
				glUniform2i(glGetUniformLocation(shaderProgram,"reso"),properties->xres,properties->yres);
				break;
			case 1:
				glTexSubImage3D(GL_TEXTURE_3D,0,glMes[glMesC].data1,glMes[glMesC].data2,glMes[glMesC].data3,1,1,1,GL_RED,GL_UNSIGNED_BYTE,map+glMes[glMesC].data1 + glMes[glMesC].data2 * properties->lvlSz + glMes[glMesC].data3 * properties->lvlSz * properties->lvlSz);
				break;
			}
		}
		glUniform2f(glGetUniformLocation(shaderProgram,"pitch"),player->xpitch,player->ypitch);
		glUniform3f(glGetUniformLocation(shaderProgram,"pos"),player->xpos,player->ypos,player->zpos);
		glClear(GL_COLOR_BUFFER_BIT);
		glDrawArrays(GL_TRIANGLES,0,12);
		SwapBuffers(dc);
	}
}
