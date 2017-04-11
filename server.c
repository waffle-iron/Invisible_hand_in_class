#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "function.h"

#define PORTNUM 9005

int main()
{
	server_function();
	printf("socket() : 소켓을 연다\n");
	printf("connect() : 클라이언트와 연결을 시킨다\n");
	printf("send() : 클라이언트에게 값을 보낸다\n");
	printf("recv() : 파일을 받는다.\n");
	printf("closesocket() : 소켓을 닫는다. \n");

	char buf[256];
	struct sockaddr_in sin;//, cli;
	int sd;
	socklen_t  clientlen;


	if ((sd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}

	memset((char *)&sin, '\0', sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(PORTNUM);
	//sin.sin_addr.s_addr = inet_addr("192.168.162.133");
	sin.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(sd, (struct sockaddr *)&sin, sizeof(sin))) {
		perror("bind");
		exit(1);
	}

/*
while (1) {
    printf("\nUDP Server: waiting for connection...");
    bytes_read = recvfrom(socket_fd, recv_data, 1023, 0,
            (struct sockaddr *) &client_addr, &sin_size);

    if (bytes_read > 0) {
                // a connection has been established
            recv_data[bytes_read] = '\0';
                printf("\nUDP Server: received -> %s", recv_data);
            pid = fork();

            if (pid < 0) {
                perror("UDP Server: ERROR while forking new process.\n");
                exit(1);
            }
            // check if the process ID is zero
            if (pid == 0) {
                // we are now inside the new forked process
                char result[50];
                int len = sprintf(result, "%d", server_parse_command(recv_data));
                len = sendto(socket_fd, result, len, 0,
                (struct sockaddr *) &client_addr, sin_size);
                close(socket_fd);
                exit(0);
            }
    }
}
*/
	int pid;
	while (1) {
		int bytes_read = (recvfrom(sd, buf, 255, 0,(struct sockaddr *)&sin, &clientlen));
		if (bytes_read == -1) {
			perror("recvfrom");
			exit(1);
		}

		printf("** From Client : %s\n", buf);
		strcpy(buf, "Hello Client");

		if (bytes_read > 0) {
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
			// we are now inside the new forked process
			//	char result[50];
			//    int len = sprintf(result, "%d", server_parse_command(buf));
			//	len = sendto(sd, buf, strlen(buf)+1, 0,(struct sockaddr *)&cli, sizeof(cli));
			    //len = sendto(socket_fd, result, len, 0,(struct sockaddr *) &client_addr, sin_size);
			//	if (len == -1) {
			//		perror("sendto");
			//		exit(1);
			//	}
				close(sd);
			    exit(0);
			}
		}
	}


//파일 이름만 아니면 경로만 보낼 것인지
//클라이언트 - 이름과 용량과 아이피
//서버 - 3개를 받아서 ip와 소켓을 열고 하드웨어 용량을 체크
//1인자 ip(서버 ip)  2번자인자 파일이름 3번제인자 파일 용량

	return 0;
}
