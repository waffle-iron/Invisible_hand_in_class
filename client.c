#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "function.h"

#define PORTNUM 9005

//clinet.exe ip file
//������ ������ üũ �Ұ�: ���� �뷮, ���� �̸� 
//������ �Ҷ� �Ұ�: ���� ������, ���� �ø��� �ѹ�.
//������ ������ ����: ������ �غ�.
int main(int argc, char** argv){
	
//	client_function();

	int sd, fd, n;
	char buf[256];
	struct sockaddr_in sin;
	socklen_t add_len = sizeof(struct sockaddr);
	
	// socket open
	if ((sd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
    	perror("socket");
	    exit(1);
	}
  	
	// init 
	memset((char *)&sin, '\0', sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = inet_addr(argv[1]);
    sin.sin_port = htons(PORTNUM);
	
	if(bind(sd, (struct sockaddr*)&sin, add_len) < 0){
		perror("bind fail");
		exit(1);
	}
	//file open 
	fd = open(argv[2], O_RDONLY);
	if(fd == -1){
		perror("file open fail");
		exit(1);
	}
	//file ������ ����
	while((n = read(fd, buf, 255)) > 0){	
    	if (sendto(sd, buf, n, 0,
               (struct sockaddr *)&sin, sizeof(sin)) == -1) {
      		perror("sendto");
      		exit(1);
		}
	}
	//	n = recvfrom(sd, buf, 255, 0, NULL, NULL);

	 return 0;
}
