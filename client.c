#include<stdio.h>
#include<stdlib.h>
#include<netdb.h>
#include<string.h>
#include "function.h"

int main(void){
	

	client_function();

	int sd, n,i;
	char buf[256];
	struct sockaddr_in sin;
	FILE *fp;
	
	char filename[50];
	gets(filename);
	printf("%s \n",filename);

	
	//������ ������ üũ �Ұ�: ���� �뷮, ���� �̸� 
	//������ �Ҷ� �Ұ�: ���� ������, ���� �ø��� �ѹ�.
	//������ ������ ����: ������ �غ�.
	if ((sd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        	perror("socket");
	        exit(1);
      	}
  
      	memset((char *)&sin, '\0', sizeof(sin));
      	sin.sin_family = AF_INET;
      	sin.sin_port = htons(PORTNUM);
    	sin.sin_addr.s_addr = inet_addr("127.0.0.1");
		

      	if (sendto(sd, buf, strlen(buf)+1, 0,
               (struct sockaddr *)&sin, sizeof(sin)) == -1) {
        	perror("sendto");
        	exit(1);
	}
	n = recvfrom(sd, buf, 255, 0, NULL, NULL);
     	
  
      	return 0;

	
}
