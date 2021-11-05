#include <ws2tcpip.h> 
#include <winsock2.h>
#include <stdio.h>

WSADATA data;
SOCKET sock;
SOCKADDR_IN adres;
char rbuf[9999];
char message[] = "yeet";

int main() {
	WORD ver = MAKEWORD(2, 2);
	WSAStartup(ver, &data);
	sock = socket(AF_INET, SOCK_STREAM, 0);
    adres.sin_family = AF_INET;
	adres.sin_port = htons(80);
	inet_pton(AF_INET,"72.1.32.168",&adres.sin_addr);
	connect(sock,(SOCKADDR*)&adres,sizeof(adres));
	printf("%i",sock);
	for(;;){
		send(sock,message,sizeof(message),0);
		Sleep(1);
	}
}
