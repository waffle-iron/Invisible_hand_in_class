#include<stdio.h>
#include "function.h"

#define PORT 22// �ٲ�ߵ�
#define BUFSIZE 1024

int main(){
	
	printf("socket():���� ����(��Ʈ ��)�� ����.");
	printf("connect()\n:������ �����Ѵ�."); 
	printf("send()\n:������ �����̸� �� ���� ������ �´��� Ȯ���ϰ� ������.");
	printf("recv()\n:���� ������ �´��� Ȯ���Ѵ�.");
	printf("closesocket():������ �ݴ´�");
	client_function();
	return 0;
}