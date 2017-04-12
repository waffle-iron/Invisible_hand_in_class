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
//서버로 보낼때 체크 할거: 파일 용량, 파일 이름 
//서버로 할때 할거: 파일 나누기, 파일 시리얼 넘버.
//서버로 보내고 나서: 재전송 준비.
int main(int argc, char** argv){
	
//	client_function();

	int sd, fd, n;
	char buf[256];
	const char* filename = argv[2];
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
    sin.sin_addr.s_addr = inet_addr("127.0.0.1");
    sin.sin_port = htons(PORTNUM);
	
	//file open 
	fd = open(filename, O_RDONLY);
	if(fd == -1){
		perror("file open fail");
		exit(1);
	}

	if(sendto(sd, filename, strlen(filename)+1, 0, (struct sockaddr*)&sin, sizeof(sin)) == -1){
		perror("sendto filename");
		exit(1);
	}
	//file 내용을 전송
	while((n = read(fd, buf, 255)) > 0){
		printf("%d\n", n);
    	if (sendto(sd, buf, n, 0,
               (struct sockaddr *)&sin, sizeof(sin)) == -1) {
      		perror("sendto");
      		exit(1);
		}
	}
	memset(buf,0, sizeof(buf));
	sprintf(buf, "end of file");
	if(sendto(sd, buf, strlen(buf)+1, 0, (struct sockaddr*)&sin, sizeof(sin)) == -1){
		perror("sendto filename");
		exit(1);
	}

	 return 0;
}
