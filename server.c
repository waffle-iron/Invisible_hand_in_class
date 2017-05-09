#include "library.h"

int main(int argc, char** argv){
	char buf[256];
	struct sockaddr_in sin, cli;
	int sd;
	int reuse = 1;
	socklen_t clientlen = sizeof(cli);

	while (1) {
		printf("server FIRST ~\n");
		if ((sd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
			perror("socket");
			exit(1);
		}

		memset((char *)&sin, '\0', sizeof(sin));
		sin.sin_family = AF_INET;
		sin.sin_port = htons(PORTNUM);
		sin.sin_addr.s_addr = htonl(INADDR_ANY);

		if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0){
			perror("setsockopt(SO_REUSEADDR) failed");
		}

		if (bind(sd, (struct sockaddr *)&sin, sizeof(sin)) == -1) {
			perror("bind");
			exit(1);
		}


		initGrobal();
		//���� ���� ���� �ޱ�
		if ((recvfrom(sd, buf, SIZEBUF, 0, (struct sockaddr *)&cli, &clientlen)) == -1) {
			perror("recvfrom filecount");
			exit(1);
		}
		int file_size = (int)buf;

		SetFileCount(atoi(buf));
		//printf("** From Client : %s\n", buf);
		if ((sendto(sd, "File count", SIZEBUF, 0, (struct sockaddr *)&cli, sizeof(cli))) == -1) {
			perror("sendto file count");
			exit(1);
		}

		//client 로 부터 file info array을 한다
		file_info = (file_information*)malloc(sizeof(file_information) * file_size);//동적할당

		if ((recvfrom(sd, file_info, SIZEBUF, 0, (struct sockaddr *)&cli, &clientlen)) == -1) {
			perror("recvfrom filecount");
			exit(1);
		}

		if ((sendto(sd, "path array", SIZEBUF, 0, (struct sockaddr *)&cli, sizeof(cli))) == -1) {
			perror("sendto path array");
			exit(1);
		}

		// ////서버가 가지고 있는 array 만들기
		// file_info = (file_information*)malloc(sizeof(file_information) * file_size);//동적할당
		//
		FilePathCheck(file_size, sd , cli); //클라이언트 어레이
		free(file_info);


		// TCP/UDP���� ����
		if ((recvfrom(sd, buf, SIZEBUF, 0, (struct sockaddr *)&cli, &clientlen)) == -1) {
			perror("recvfrom");
			exit(1);
		}
		//printf("** From Client : %s\n", buf);
		if ((sendto(sd, "Start", SIZEBUF, 0, (struct sockaddr *)&cli, sizeof(cli))) == -1) {
			perror("sendto");
			exit(1);
		}

		if (strcmp(buf, "UDP") != 0 && strcmp(buf, "udp") != 0&&
			strcmp(buf, "TCP") != 0 && strcmp(buf, "tcp") != 0){
			return 0;
		}

		if ( !strcmp(buf, "UDP") || !strcmp(buf, "udp")){
			printf("udp");
			UdpServer(sd, cli);

		}else if ( !strcmp(buf, "TCP") || !strcmp(buf, "tcp")){
			close(sd);
			printf("tcp");

			if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
				perror("socket");
				exit(1);
			}

			memset((char *)&sin, '\0', sizeof(sin));
			sin.sin_family = AF_INET;
			sin.sin_port = htons(PORTNUM);
			sin.sin_addr.s_addr = htonl(INADDR_ANY);

			if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0){
				perror("setsockopt(SO_REUSEADDR) failed");
			}

			if (bind(sd, (struct sockaddr *)&sin, sizeof(sin)) == -1) {
				perror("bind");
				exit(1);
			}
			TcpServer(sd, cli);
		}
		close(sd);
	}



	return 0;
}
