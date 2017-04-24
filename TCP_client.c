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
#include <sys/time.h>
#include "library.h"

#define PORTNUM 9000
#define BUFFERSIZE 256

int main(int argc, char** argv) {
	int sd, fd, n;
	char buf[SIZEBUF];
	char end_buf[SIZEBUF];
	char percent[SIZEBUF+1];
	struct sockaddr_in sin;
	const char* filename = argv[2]; //파일 이름
	printf("%s\n", filename);


	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}
	//e
	memset((char *)&sin, '\0', sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(PORTNUM);
	sin.sin_addr.s_addr = inet_addr(argv[1]);

	if (connect(sd, (struct sockaddr *)&sin, sizeof(sin)) == -1) {
		perror("connect");
		exit(1);
	}
	//file open
	fd = open(filename, O_RDONLY);
	if (fd == -1){
		perror("file open fail");
		exit(1);
	}

	// 파일 이름 전송
	if (send(sd, filename, SIZEBUF, 0) == -1){
		perror("send filename");
		exit(1);
	}


	//파일 명 다시 받기
	int bytes_read = (recv(sd, buf, SIZEBUF, MSG_WAITALL));

	if (bytes_read < 0) {
		perror("recvfrom filename");
		exit(1);
	}

	//내가 보낸 파일이랑 이름이 다른경우
	if (strcmp(filename, buf) != 0) { //buf랑 비교
		printf("%s\n", buf);
		perror("not match filename.");
		exit(1);
	} else printf("** match filename : %s\n", buf);// 파일이름 받고출력


	//file 내용을 전송
	while ((n = read(fd, buf, SIZEBUF)) > 0){ //fd에 있는걸 buf로 저장

		printf("SEND : %d\n", n);
		int a = send(sd, buf, SIZEBUF, 0);
		if (a == -1) {
			perror("sendto");
			exit(1);
		}
        //printf("buf = %s\n",buf); 
        memset(buf,0,256);
	}


	memset(buf, 0, SIZEBUF);
    sprintf(buf,"end of file");
	//end of file 을 전송
	if (send(sd, buf, SIZEBUF, 0) == -1){
		perror("send filena?me");
		exit(1);
	}


	// end of file 확인
	bytes_read = (recv(sd, end_buf, SIZEBUF, MSG_WAITALL));

	if (bytes_read == -1) {
		perror("recv end of file");
		exit(1);
	}

	if (strcmp("end of file", end_buf) == 0) { //buf랑 비교
		printf("%s\n", end_buf);
	} else{
		perror("error : file is not end");
		exit(1);
	}

	close(fd);

	///////////////////////////////////////////////////////////
	//////// 무결성 체크/////////////////////////////////////////////
	printf(" check whether your file is correct send\n");

	sleep(2);
	//scanf("%s", &filename);
	//printf("%s\n", filename);
	//file open
	int fd1;
	fd1 = open(filename, O_RDONLY);
	if (fd1 == -1){
		perror("file open fail");
		exit(1);
	}

	//file 내용을 다시 전송
	while ((n = read(fd1, buf, SIZEBUF)) > 0){ //fd에 있는걸 buf로 저장

		printf("RESEND : %d\n", n);

		if (send(sd, buf, SIZEBUF, 0) == -1) {
			perror("resendto");
			exit(1);
		}
		memset(buf, 0, SIZEBUF);
	}
	memset(buf, 0, SIZEBUF);
    sprintf(buf,"end of file");
	//파일끝이라고 전송
	if (send(sd, buf, SIZEBUF, 0) == -1){
		perror("send filena?me");
		exit(1);
	}
	// end of file 확인
	bytes_read = (recv(sd, end_buf, SIZEBUF, 0));

	if (bytes_read == -1) {
		perror("recv end of file");
		exit(1);
	}

	if (strcmp("end of file", end_buf) == 0) { //buf랑 비교
		//printf("여기디이빈다 : %s\n", end_buf);
	} else{
		perror("error : file is not end");
		exit(1);
	}
	//여긴 머냐
	//만약 일치 불일치 메세지 를 받는다.
	memset(percent, 0, SIZEBUF+1);
//	printf("불일치 사이즈: %d\n", bytes_read);
	bytes_read = recv(sd, percent, SIZEBUF, 0);
	//percent[bytes_read] = '\0';
	//printf("불일치 사이즈: %d\n", bytes_read);
	//printf("%s 퍼센트 가 무엇인지 출력\n", percent);
	//만약 불일치라면 소켓을 닫는다
	if (strcmp("100percent", percent) == 0) { //buf랑 비교
		printf("%s same file\n", percent);
	} else{
		printf("error : file is not same\n");
		exit(1);
	}
	return 0;
}
