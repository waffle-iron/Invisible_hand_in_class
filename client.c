#include<stdio.h>
#include "function.h"

#define PORT 22// 바꿔야됨
#define BUFSIZE 1024

int main(){
	
	printf("socket():먼저 소켓(포트 등)을 연다.");
	printf("connect()\n:서버와 연결한다."); 
	printf("send()\n:보낼때 파일이름 및 파일 내용이 맞는지 확인하고 보낸다.");
	printf("recv()\n:보낸 파일이 맞는지 확인한다.");
	printf("closesocket():소켓을 닫는다");
	client_function();
	return 0;
}