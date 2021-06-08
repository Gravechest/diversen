#include "global.h"
#include <WinSock.h>
int nw_food;
char wormID;
char msg[2];
char food[2];
WSADATA data;
SOCKET sock;
char enemie_direction;

int network_init() {
	WORD ver = MAKEWORD(2, 2);
	WSAStartup(ver, &data);
	sock = socket(AF_INET, SOCK_STREAM, 0);
	sockaddr_in adres;
	adres.sin_addr.S_un.S_addr = 167946432;
	adres.sin_port = htons(7779);
	adres.sin_family = AF_INET;
	connect(sock, (sockaddr*)&adres, sizeof(adres));
	recv(sock, msg, 2, 0);
	recv(sock, &wormID, 1, 0);
	nw_food = msg[0] + msg[1] * 256;
	return nw_food;
}

int socket_data(char loc) {
	send(sock, &loc, 1, 0);
	recv(sock, &enemie_direction, 1, 0);
	return enemie_direction;
}

int new_food() {
	recv(sock, food, 2, 0);
	nw_food = food[0] + food[1] * 256;
	return nw_food;
}
