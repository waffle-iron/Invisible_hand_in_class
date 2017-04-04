#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "function.h"

#define PORT 22//¼öÁ¤ÇØ¾ßµÊ.
#define BUFSIZE 1024
#define IP "127.0.0.1"

int main(){
	//SOCKADDR_IN addr;
	//unsigned int count;
	

	printf("socket():\n");
	printf("connect():\n"); 
	printf("send():\n");
	printf("recv():\n");
	printf("closesocket():");
	client_function();
	return 0;	
}
