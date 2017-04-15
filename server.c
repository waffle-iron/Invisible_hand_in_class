#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include "function.h"

#define PORTNUM 9005
//
//파일 이름만 아니면 경로만 보낼 것인지
//클라이언트 - 이름과 용량과 아이피
//서버 - 3개를 받아서 ip와 소켓을 열고 하드웨어 용량을 체크
//1인자 ip(서버 ip)  2번자인자 파일이름 3번제인자 파일 용량
int main(){

	//server_function();
	//printf("send() : 클라이언트에게 값을 보낸다\n");
	//printf("recv() : 파일을 받는다.\n");
	//printf("closesocket() : 소켓을 닫는다. \n");

	char buf[256];
        const char filename[255];
	struct sockaddr_in sin, cli;
	int sd;
	socklen_t  clientlen;
	
	//printf("socket() : 소켓을 연다\n");
	if ((sd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}

	memset((char *)&sin, '\0', sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(PORTNUM);
//	sin.sin_addr.s_addr = inet_addr("127.0.0.1");
	sin.sin_addr.s_addr = htonl(INADDR_ANY);

	//printf("connect() : 클라이언트와 연결을 시킨다\n");
	if (bind(sd, (struct sockaddr *)&sin, sizeof(sin))) {
		perror("bind");
		exit(1);
	}

	int pid;

	while (1) {
		printf("inwhile~~\n");
		int bytes_read = (recvfrom(sd, buf, 255, 0,(struct sockaddr *)&sin, &clientlen));
		if (bytes_read == -1) {
			perror("recvfrom filename");
			exit(1);
		}

		printf("** From Client : %s\n", buf);// 파일이름 받고 출력
///////////////////// 파일 이름 받기///////////////////////////////////
                usleep(200);
                if(sendto(sd, buf, strlen(buf)+10, 0, (struct sockaddr *)&sin, sizeof(sin))
                    == -1){
                    perror("sendto filename");
                    exit(1);
                } 
/////////////////////////////////////////////////////////////////////
		int fd;
		const char* filename = buf;

		fd = open("./temp.dat", O_WRONLY | O_CREAT | O_EXCL, 0644);

		while(1) {
			
			memset(buf,0,sizeof(buf));
			printf("2while\n");
			bytes_read = recvfrom(sd, buf, 255, 0,(struct sockaddr *)&sin, &clientlen);
			printf("3while\n");
			if (bytes_read == -1) {
				perror("recvfrom date");
				exit(1);
			}
			buf[bytes_read] = '\0';
			
			if(!strncmp(buf, "end of file", 10)) { //마지막 메시지가 end of file이면 종료
/////////////////////////////////////////////////////////////////////
                        
                        
                        
                        
                        
/////////////////////////////////////////////////////////////////////
				printf("file close\n");
			    close(fd); //stream 닫기
			    break; //while문 빠져나가기
			} else {
			   	printf("%d byte recv: %s\n", bytes_read, buf);
//			    fputs(buf, stream); //파일로 저장		
				write(fd, buf, 255);
			}
		
/*		if (bytes_read > 0) {
		 // a connection has been established
			 buf[bytes_read] = '\0';
			 printf("\nUDP Server: received -> %s", buf);

			 pid = fork();

			 if (pid < 0) {
            	perror("UDP Server: ERROR while forking new process.\n");
                exit(1);
    		}
        	// check if the process ID is zero
			if (pid == 0) {
				printf("** From Client : %s\n", buf);
        	  	strcpy(buf, "Hello Client");
       			if ((sendto(sd, buf, strlen(buf)+1, 0,(struct sockaddr *)&cli, sizeof(cli))) == -1) {
            		perror("sendto");
	              	exit(1);
    	      	}
				close(sd);
				exit(0);
			}
		}*/
		}
	}
	return 0;
}
