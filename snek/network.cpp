#include "global.h"
#include <WinSock.h>
#pragma comment(lib,"Ws2_32.lib")
char network_init() {
	char msg;
	WSADATA data;
	WORD ver = MAKEWORD(2, 2);
	WSAStartup(ver, &data);
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	sockaddr_in adres;
	adres.sin_addr.S_un.S_addr = 33728704;
	adres.sin_port = htons(7778);
	adres.sin_family = AF_INET;
	connect(sock, (sockaddr*)&adres, sizeof(adres));
	recv(sock,&msg,1,0);
	return msg;
}
