#include <WinSock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <chrono>
#include <random>
#include <vector>
#pragma comment(lib,"Ws2_32.lib")
int wormPos[2] = {20,100};
char id;
char yeet;
char map[100];
char msg;
char wormID[2] = {};
std::vector<SOCKET> clients;
char wormdir[2];
int random(int x) {
	std::uniform_int_distribution<int> dist(0, x);
	int time = std::chrono::high_resolution_clock().now().time_since_epoch().count();
	std::default_random_engine rand(time);
	int randf = dist(rand);
	return randf;
}
int main() {
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
	while (clients.size() < 2) {
		clients.push_back(accept(sock, &client, &clientsize));
		std::cout << "yeet" << std::endl;
	}
	msg = random(256);
	for (int i = 0; i < clients.size(); i++) {
		send(clients[i], &msg, 1, 0);
		id = i;
		send(clients[i], &id, 1, 0);
	}	
	while (true) {
		for (int i = 0; i < clients.size(); i++) {
			recv(clients[i], &wormdir[i], 1, 0);
		}
		for (int i = 0; i < clients.size(); i++) {
			switch (wormdir[i]) {
			case 0:
				wormPos[i] += 1;
				break;
			case 1:
				wormPos[i] -= 1;
				break;
			case 2:
				wormPos[i] += 20;
				break;
			case 3:
				wormPos[i] -= 20;
			}
		}
		for (int i = 0; i < clients.size(); i++) {
			send(clients[i], &wormdir[1 - i], 1, 0);
		}
		for (int i = 0; i < clients.size(); i++) {
			if (wormPos[i] == msg) {
				msg = random(256);
				for (int i2 = 0; i2	 < clients.size(); i2++) {
					send(clients[i2], &msg, 1, 0);
				}
			}
		}
	}
}
