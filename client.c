#include "library.h"

int main(int argc, char** argv){

	int sd;
	char* choose;
	char buf[SIZEBUF];

	const char* filename = argv[2]; //파일 이름
	choose = argv[3]; //TCP/UDP


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
	sin.sin_addr.s_addr = inet_addr(argv[1]);//아이피 주소
	sin.sin_port = htons(PORTNUM);

	//printf("choose : %s\n", choose);
	// TCP/UDP인지 전송
	if (sendto(sd, choose, strlen(choose)+1, 0, (struct sockaddr*)&sin, sizeof(sin)) == -1){
		perror("sendto TCP/UDP");
		exit(1);
	}


	//준비 되었는지 메세지 받기
	int n = (recvfrom(sd, buf, SIZEBUF, 0, (struct sockaddr *)&sin, &add_len));

	if (n == -1) {
		perror("recvfrom ready");
		exit(1);
	}

	if (strcmp("Start", buf) != 0) { //buf랑 비교

		perror("Server is not ready");
		exit(1);
	}

//choose 가 udp or tcp 인지 해서 server의 UdpServer() 또는 TcpServer()

	if(strcmp(choose, "UDP") || strcmp(choose, "udp")){
		UdpClient(argc, argv,sd , sin);
	}
	else if (strcmp(choose, "TCP") || strcmp(choose, "tcp")){
		TcpClient(argc, argv);
	}


	return 0;
}
