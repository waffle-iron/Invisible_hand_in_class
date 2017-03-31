#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "function.h"

#define PORT 22//수정해야됨.
#define BUFSIZE 1024
#define IP "127.0.0.1"

int main(){
	//SOCKADDR_IN addr;
	//unsigned int count;
	

	printf("socket():먼저 소켓(포트 등)을 연다.");
	printf("connect():서버와 연결한다.\n"); 
	printf("send():보낼때 파일이름 및 파일 내용이 맞는지 확인하고 보낸다.\n");
	printf("recv():보낸 파일이 맞는지 확인한다.\n");
	printf("closesocket():소켓을 닫는다");
	client_function();
	//fd = socket(AF_UNIX,SOCK_STREAM,0);
	//if(listen_sock == INVALID_SOCKET) err_quit("socket()");
	//addr.sin_family = AF_INET;
	//add.sin_addr.S_un.S_addr = inet_addr(IP);
	//addr.sin_port =htons(PORT);
	//if(connect(s,(struc sockaddr *)&addr, sizeof(addr) ==  SDOCKET_ERROR){
	//	perror("connect : ");
	//	printf("fail to connect.\n);
	//	closesocket(s);
	//	return 1;
	//}
	
	
	return 0;
}