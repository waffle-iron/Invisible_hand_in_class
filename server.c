#include "library.h"

int main(int argc, char** argv){
	char buf[256];
	struct sockaddr_in sin, cli;
	int sd;
	socklen_t clientlen = sizeof(cli);

	if ((sd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}
	memset((char *)&sin, '\0', sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(PORTNUM);
	sin.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(sd, (struct sockaddr *)&sin, sizeof(sin)) == -1) {
		perror("bind");
		exit(1);
	}

	while (1) {
		printf("server FIRST ~\n");
		//파일 갯수 전송 받기 
		if ((recvfrom(sd, buf, SIZEBUF, 0, (struct sockaddr *)&cli, &clientlen)) == -1) {
			perror("recvfrom filecount");
			exit(1);
		}
		
		SetFileCount(atoi(buf));
		//printf("** From Client : %s\n", buf);
		if ((sendto(sd, "File count", SIZEBUF, 0, (struct sockaddr *)&cli, sizeof(cli))) == -1) {
			perror("sendto file count");
			exit(1);
		}

		// TCP/UDP인지 전송
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
		}

		if ( !strcmp(buf, "TCP") || !strcmp(buf, "tcp")){
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

			if (bind(sd, (struct sockaddr *)&sin, sizeof(sin)) == -1) {
				perror("bind");
				exit(1);
			}

			TcpServer(sd, cli);
		}

	}

	return 0;
}
