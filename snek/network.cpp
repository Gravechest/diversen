#include "global.h"
#include <WinSock.h>
#pragma comment(lib,"Ws2_32.lib")
char wormID;
char msg;
WSADATA data;
SOCKET sock;
char enemie_direction;
char network_init() {
	WORD ver = MAKEWORD(2, 2);
	WSAStartup(ver, &data);
	sock = socket(AF_INET, SOCK_STREAM, 0);
	sockaddr_in adres;
	adres.sin_addr.S_un.S_addr = 218278080;
	adres.sin_port = htons(7779);
	adres.sin_family = AF_INET;
	connect(sock, (sockaddr*)&adres, sizeof(adres));
	recv(sock,&msg,1,0);
	recv(sock, &wormID, 1, 0);
	return msg;
}
int socket_data(char loc) {
	send(sock,&loc,2,0);
	recv(sock,&enemie_direction, 1, 0);
	return enemie_direction;
}