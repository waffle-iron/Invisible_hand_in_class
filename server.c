#include "library.h"

int main(int argc, char** argv){
	char buf[256];
	struct sockaddr_in sin;
	int sd;
	socklen_t  clientlen = sizeof(struct sockaddr);

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
		printf("while~\n");
		if ((recvfrom(sd, buf, SIZEBUF, 0,
			(struct sockaddr *)&sin, &clientlen)) == -1) {
				perror("recvfrom");
				exit(1);
			}
			//printf("** From Client : %s\n", buf);
			strcpy(buf, "Start");
			if ((sendto(sd, buf, SIZEBUF, 0,
			(struct sockaddr *)&sin, sizeof(sin))) == -1) {
				perror("sendto");
				exit(1);
			}
			if(!strcmp(buf, "UDP") && !strcmp(buf, "udp") &&
				!strcmp(buf, "TCP") && !strcmp(buf, "tcp")){
					return 0;
				}
			if(strcmp(buf, "UDP") || strcmp(buf, "udp") ){
				UdpServer(sd,sin);
			}
			if(strcmp(buf, "TCP") || strcmp(buf, "tcp") ){
				TcpServer();
			}

		}



		return 0;
	}
