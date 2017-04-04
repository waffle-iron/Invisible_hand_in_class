#include<stdio.h>
#include<stdlib.h>
#include<netdb.h>
#include<string.h>
#include "function.h"

#define PORTNUM 9005
int main(void){
	

	printf("socket():\n");
	printf("connect():\n"); 
	printf("send():\n");
	printf("recv():\n");
	printf("closesocket():");
	client_function();
	int sd, n;
	char buf[256];
	struct sockaddr_in sin;
  
	if ((sd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        	perror("socket");
	        exit(1);
      	}
  
      	memset((char *)&sin, '\0', sizeof(sin));
      	sin.sin_family = AF_INET;
      	sin.sin_port = htons(PORTNUM);
    	sin.sin_addr.s_addr = inet_addr("192.168.162.133");
  
      	strcpy(buf, "I am a client.");
      	if (sendto(sd, buf, strlen(buf)+1, 0,
               (struct sockaddr *)&sin, sizeof(sin)) == -1) {
        	perror("sendto");
        	exit(1);
	}
	n = recvfrom(sd, buf, 255, 0, NULL, NULL);
     	buf[n] = '\0';
      	printf("** From Server : %s\n", buf);
  
      	return 0;

	
}
