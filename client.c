#include "library.h"

int main(int argc, char** argv){

	int sd;
	char buf[SIZEBUF];
	int fileCount = 1;
	const char* filename = argv[2]; //파일 이름
	char choose[20]; //= argv[3]; //TCP/UDP

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

	// 파일 및 디렉토리 담는 변수 선언
	// 5.9일 추가
	idx = 0;
	files_size = SIZEBUF;
	file_info = (file_information*)malloc(sizeof(file_information) * files_size);

	CountFile(filename);
	sprintf(buf, "%d", GetFileCount());
/*	int i;
	for(i =0; i<atoi(buf); ++i){
		printf("path = %s, file? =  %c\n", file_info[i].path, file_info[i].or_file_dir);
	}*/
	//파일 갯수가 몇개인지 전송
	printf("file 갯수 : %s\n", buf);
	if (sendto(sd, buf, 12, 0, (struct sockaddr*)&sin, sizeof(sin)) == -1){
		perror("sendto file Count");
		exit(1);
	}
	// 거기에 대한 답장을 받음
	if (recvfrom(sd, buf, SIZEBUF, 0, (struct sockaddr *)&sin, &add_len) == -1){
		perror("recvfrom ready");
		exit(1);
	}

	//경로 전체배열 전송
	if (sendto(sd, file_info, sizeof(file_information) * idx, 0, (struct sockaddr*)&sin, sizeof(sin)) == -1){
		perror("sendto file and dir path");
		exit(1);
	}
	// 거기에 대한 답장을 받음`
	if (recvfrom(sd, buf, SIZEBUF, 0, (struct sockaddr *)&sin, &add_len) == -1){
		perror("recvfrom file and dir path");
		exit(1);
	}

	double total_size = FileTransferSize(argv[2]);

	if(65535.0 < total_size){
		strcpy(choose,"TCP");

		printf("socket TCP\n");
		fflush(stdout);
		// TCP/UDP인지 전송
		if (sendto(sd, choose, strlen(choose)+1, 0, (struct sockaddr*)&sin, sizeof(sin)) == -1){
			perror("sendto TCP");
			exit(1);
		}
	}
	else if(65535.0 > total_size){
		strcpy(choose,"UDP");

		printf("socket UDP\n");
		fflush(stdout);
		// TCP/UDP인지 전송
		if (sendto(sd, choose, strlen(choose)+1, 0, (struct sockaddr*)&sin, sizeof(sin)) == -1){
			perror("sendto UDP");
			exit(1);
		}
	}



	//준비 되었는지 메세지 받기
	if (recvfrom(sd, buf, SIZEBUF, 0, (struct sockaddr *)&sin, &add_len) == -1){
		perror("recvfrom ready");
		exit(1);
	}

	printf("choose stop %s\n", choose);

	if (strcmp("Start", buf) != 0) { //buf랑 비교
		perror("Server is not ready");
		exit(1);
	}
	printf("choose stop2 %s\n", choose);


	//choose 가 udp or tcp 인지 해서 server의 UdpServer() 또는 TcpServer()
	if(!strcmp(choose, "UDP") || !strcmp(choose, "udp")){
		UdpClient(argc, argv, sd , sin);
	}else if (!strcmp(choose, "TCP") || !strcmp(choose, "tcp")){
		close(sd);

		if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
			perror("socket");
			exit(1);
		}
		// init
		memset((char *)&sin, '\0', sizeof(sin));
		sin.sin_family = AF_INET;
		sin.sin_addr.s_addr = inet_addr(argv[1]);//아이피 주소
		sin.sin_port = htons(PORTNUM);

		TcpClient(argc, argv, sd, sin);
	}

	close(sd);
	free(file_info);
	return 0;
}

	
