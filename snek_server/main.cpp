#include <WinSock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <iostream>
#include <vector>
#pragma comment(lib,"Ws2_32.lib")
char yeet;
char map[100];
char msg;
std::vector<SOCKET> clients;
int main() {
	WSADATA data;
	WORD ver = MAKEWORD(2, 2);
	WSAStartup(ver, &data);
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	sockaddr_in adress;
	adress.sin_addr.S_un.S_addr = INADDR_ANY;
	adress.sin_port = htons(7778);
	adress.sin_family = AF_INET;
	bind(sock, (sockaddr*)&adress, sizeof(adress));
	listen(sock, SOMAXCONN);
	sockaddr client;
	int clientsize = sizeof(client);
	while (clients.size() < 2) {
		clients.push_back(accept(sock, &client, &clientsize));
		std::cout << "yeet";
	}
	msg = 1;
	while (true) {
		for (int i = 0; i < clients.size(); i++) {
			send(clients[i], &msg, 1, 0);
		}
	}
}
