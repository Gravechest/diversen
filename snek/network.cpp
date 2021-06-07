#include "global.h"
#include <WinSock.h>

char wormID;
char msg;
char food;
WSADATA data;
SOCKET sock;
char enemie_direction;

char network_init() {
	WORD ver = MAKEWORD(2, 2);
	WSAStartup(ver, &data);
	sock = socket(AF_INET, SOCK_STREAM, 0);
	sockaddr_in adres;
	adres.sin_addr.S_un.S_addr = 788660234;
	adres.sin_port = htons(7779);
	adres.sin_family = AF_INET;
	connect(sock, (sockaddr*)&adres, sizeof(adres));
	recv(sock, &msg, 1, 0);
	recv(sock, &wormID, 1, 0);
	return msg;
}

int socket_data(char loc) {
	send(sock, &loc, 1, 0);
	recv(sock, &enemie_direction, 1, 0);
	return enemie_direction;
}

int new_food() {
	recv(sock, &food, 1, 0);
	return food;
}
