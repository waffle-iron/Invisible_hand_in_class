#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "function.h"

#define PORT 22//�����ؾߵ�.
#define BUFSIZE 1024
#define IP "127.0.0.1"

int main(){
	//SOCKADDR_IN addr;
	//unsigned int count;
	

	printf("socket():���� ����(��Ʈ ��)�� ����.");
	printf("connect():������ �����Ѵ�.\n"); 
	printf("send():������ �����̸� �� ���� ������ �´��� Ȯ���ϰ� ������.\n");
	printf("recv():���� ������ �´��� Ȯ���Ѵ�.\n");
	printf("closesocket():������ �ݴ´�");
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