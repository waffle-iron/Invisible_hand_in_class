#include "library.h"

int main(int argc, char** argv){

	char buf[256];
	struct sockaddr_in sin, cli;
	int sd;
	socklen_t  clientlen;

	if ((sd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}

	memset((char *)&sin, '\0', sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(PORTNUM);
	sin.sin_addr.s_addr = htonl(INADDR_ANY);

	//printf("connect() : 클라이언트와 연결을 시킨다\n");
	if (bind(sd, (struct sockaddr *)&sin, sizeof(sin)) == -1) {

		perror("bind");
		exit(1);
	}

	while (1) {
		if ((recvfrom(sd, buf, SIZEBUF, 0,
			(struct sockaddr *)&cli, &clientlen)) == -1) {
				perror("recvfrom");
				exit(1);
			}

			if(!strcmp(buf, "UDP") || !strcmp(buf, "udp") ||
				!strcmp(buf, "TCP") || !!strcmp(buf, "tcp")){
					return 0;
				}
			if(strcmp(buf, "UDP") || strcmp(buf, "udp") ){
				UdpServer();
			}
			if(strcmp(buf, "TCP") || strcmp(buf, "tcp") ){
				TcpServer();
			}
			//printf("** From Client : %s\n", buf);
			strcpy(buf, "Start");

			if ((sendto(sd, buf, SIZEBUF, 0,
			(struct sockaddr *)&cli, sizeof(cli))) == -1) {
				perror("sendto");
				exit(1);
			}
		}



		return 0;
	}
