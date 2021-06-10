#include <WinSock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <chrono>
#include <random>
#include <vector>
#include <thread>
#pragma comment(lib,"Ws2_32.lib")
std::vector<int> wormPos;
std::vector<char> wormdir;
std::vector<char> wormSize;
char chbuf;
int buf;
char rndloc[2];
int rand_food;
int playerCount = 0;
int playerCountbuf = 0;
char newWorm;
char id;
char yeet;
char msg[2];
std::vector<SOCKET> clients;
int random(int x) {
	std::uniform_int_distribution<int> dist(0, x);
	int time = std::chrono::high_resolution_clock().now().time_since_epoch().count();
	std::default_random_engine rand(time);
	int randf = dist(rand);
	return randf;
}
void accept_con(SOCKET sock, sockaddr client, int clientsize) {
	while (true) {
		clients.push_back(accept(sock, &client, &clientsize));
		std::cout << "yeet" << std::endl;
		newWorm = clients.size();
		char joinnm = wormPos.size();
		id = wormPos.size();
		send(clients[joinnm], msg, 2, 0);
		send(clients[joinnm], &id, 1, 0);
		send(clients[joinnm], &joinnm, 1, 0);
		for (int i = 0; i < joinnm; i++) {	
			char wormen[2] = {wormPos[i] % 127,wormPos[i] / 127};
			chbuf = wormSize[i];
			send(clients[joinnm], wormen, 2, 0);
			send(clients[joinnm], &chbuf, 1, 0);
		}
		playerCountbuf += 1;
	}
}
int main() {
	rand_food = random(1600);
	msg[0] = rand_food % 127;
	msg[1] = rand_food / 127;
	WSADATA data;
	WORD ver = MAKEWORD(2, 2);
	WSAStartup(ver, &data);
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	sockaddr_in adress;
	adress.sin_addr.S_un.S_addr = INADDR_ANY;
	adress.sin_port = htons(7779);
	adress.sin_family = AF_INET;
	bind(sock, (sockaddr*)&adress, sizeof(adress));
	listen(sock, SOMAXCONN);
	sockaddr client;
	int clientsize = sizeof(client);	
	std::thread first (accept_con,sock, client, clientsize);
	while (true) {
		if (playerCount != playerCountbuf) {
			wormdir.push_back(0);
		}
		playerCount = playerCountbuf;
		for (int i = 0; i < playerCount; i++) {
			send(clients[i], &newWorm, 1, 0);
			if (newWorm != 0) {
				if (newWorm == clients.size()) {
					buf = random(1600);
					rndloc[0] = buf % 127;
					rndloc[1] = buf / 127;
					wormPos.push_back(buf);
					wormSize.push_back(3);
				}
				send(clients[i], rndloc, 2, 0);
				newWorm -= 1;
			}
		}
		for (int i = 0; i < playerCount; i++) {
			recv(clients[i], &wormdir[i], 1, 0);
		}
		for (int i = 0; i < playerCount; i++) {
			switch (wormdir[i]) {
			case 0:
				wormPos[i] += 1;
				break;
			case 1:
				wormPos[i] -= 1;
				break;
			case 2:
				wormPos[i] += 40;
				break;
			case 3:
				wormPos[i] -= 40;
				break;
			}
		}
		for (int i = 0; i < playerCount; i++) {
			for (int i2 = 0; i2 < wormdir.size(); i2++) {
				send(clients[i], &wormdir[i2], 1, 0);
			}
		}
		for (int i = 0; i < playerCount; i++) {
			if (wormPos[i] == rand_food) {
				rand_food = random(1600);
				msg[0] = rand_food % 127;
				msg[1] = rand_food / 127;
				for (int i2 = 0; i2	 < playerCount; i2++) {
					send(clients[i2], msg, 2, 0);
				}
			}
		}
	}
}
