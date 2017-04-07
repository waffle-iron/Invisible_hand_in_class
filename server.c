#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>

#include "function.h"

#define PROTUM 9001

int main()
{
	server_function();
	printf("socket() : 소켓을 연다\n");
	printf("connect() : 클라이언트와 연결을 시킨다\n");
	printf("send() : 클라이언트에게 값을 보낸다\n");
	printf("recv() : 파일을 받는다.\n");
	printf("closesocket() : 소켓을 닫는다. \n");

	
	char buf[256];
	struct sockaddr_in sin, cli;
	int sd, clientlen = sizeof(cli);

	if ((sd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		perror("socket");
		exit(1);
		}

	memset((char *)&sin, '\0', sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(PORTNUM);
	sin.sin_addr.s_addr = inet_addr("192.168.162.133");

	if (bind(sd, (struct sockaddr *)&sin, sizeof(sin))) {
		perror("bind");
		exit(1);
	}


	///어디로 넣을지
	////클라이언트: 파일 이름 크기 체크 하고 보낸다 - 파일 크면 나눠서보낸다
	//서버 : 내가 가지고 있는 하드용량 -  
	
	while (1) {
		if ((recvfrom(sd, buf, 255, 0,
			(struct sockaddr *)&cli, &clientlen)) == -1) {
			perror("recvfrom");
			exit(1);
		}

		printf("** From Client : %s\n", buf);
		strcpy(buf, "Hello Client");
		
		if ((sendto(sd, buf, strlen(buf)+1, 0,
			(struct sockaddr *)&cli, sizeof(cli))) == -1) {
			perror("sendto");
			exit(1);
		}

//받는 응용프로그램 만들기 - 자식을 처리할수있는 알고리즘?
		switch (fork()) {
			case 0 :
				prinf("** Fork Client \n");
				close(sd);
				dup2(ns, STDIN_FILENO);
				dup2(ns, STDOUT_FILENO);
				close(ns);
				execl("./han", "han", (char *)0);
		}
		close(ns);


	}

//파일 이름만 아니면 경로만 보낼 것인지
//클라이언트 - 이름과 용량과 아이피
//서버 - 3개를 받아서 ip와 소켓을 열고 하드웨어 용량을 체크
//1인자 ip(서버 ip)  2번자인자 파일이름 3번제인자 파일 용량

	return 0;
}
