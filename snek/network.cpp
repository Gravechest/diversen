#include "global.h"
#include <WinSock.h>
#include <vector>
std::vector<char> wormDir = {0};
std::vector<char> wormLoc_init;
char buf;
char wormloc[2];
char wormCount;
char new_Worm_loc[2];
char new_Worm;
int nw_food;
char wormID;
char msg[2];
char food[2];
WSADATA data;
SOCKET sock;

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
	recv(sock, &wormCount, 1, 0);
	for (int i = 0; i < wormCount; i++) {
		recv(sock, wormloc, 2, 0);
		wormLoc_init.push_back(wormloc[0] + wormloc[1] * 127);
	}
	for (int i = 0; i < wormLoc_init.size(); i+=2) {
		wormLoc.push_back(wormLoc_init[i] + wormLoc_init[i + 1] * 127);
		wormDir.push_back(0);
	}
	nw_food = msg[0] + msg[1] * 127;
	return nw_food;
}

void newWorm() {
	recv(sock, &new_Worm, 1, 0);
	if (new_Worm == 1) {
		recv(sock, new_Worm_loc, 2, 0);
		wormLoc.push_back(new_Worm_loc[0] + new_Worm_loc[1] * 127);
		wormDir.push_back(0);	
	}
}

void socket_data(char loc) {
	send(sock, &loc, 1, 0);
	wormDir.clear();
	for (int i = 0; i < wormLoc.size(); i++) {
		recv(sock, &buf, 1, 0);
		wormDir.push_back(buf);
	}
}

int new_food() {
	recv(sock, food, 2, 0);
	nw_food = food[0] + food[1] * 127;
	return nw_food;
}
