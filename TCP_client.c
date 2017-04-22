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
#include "function.h"

#define PORTNUM 9000
#define BUFFERSIZE 256
 
int main(int argc, char** argv) {
    int sd, fd, n;
	char buf[BUFFERSIZE];
	char end_buf[50];
	char re_buf[50];
    struct sockaddr_in sin;
    const char* filename = argv[2]; //파일 이름
    printf("%s\n", filename);


    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    memset((char *)&sin, '\0', sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(PORTNUM);
    sin.sin_addr.s_addr = inet_addr(argv[1]);
    if (connect(sd, (struct sockaddr *)&sin, sizeof(sin))) {
        perror("connect");
        exit(1);
    }
    //file open
	fd = open(filename, O_RDONLY);
	if(fd == -1){
		perror("file open fail");
		exit(1);
	}

	// 파일 이름 전송
    if(send(sd, filename, strlen(filename)+10, 0) == -1){
		perror("send filename");
		exit(1);
	}
    int bytes_read = (recv(sd, buf, BUFFERSIZE-1, 0));

	if (bytes_read < 0) {
		perror("recvfrom filename");
		exit(1);
	}

	//내가 보낸 파일이랑 이름이 다른경우
	if(strcmp(filename,buf) != 0) { //buf랑 비교
		printf("%s\n", buf);
		perror("not match filename.");
		exit(1);
	}
	else printf("** match filename : %s\n", buf);// 파일이름 받고출력
    //file 내용을 전송
	/*while((n = read(fd, buf, 256)) > 0){ //fd에 있는걸 buf로 저장

		printf("SEND : %d\n", n);

		if (send(sd, buf, n, 0) == -1) {
			perror("send");
			exit(1);
		}
	}*/

	read(fd, buf, 256);
	send(sd, buf, 255, 0);

	memset(buf,0, sizeof(buf));

	//sprintf(buf, "end of file"); //buf에다가 end of file을 붙이는 것
 //end of file 을 전송
	if(send(sd, "end of file", sizeof(buf), 0) == -1){
		perror("send filena?me");
		exit(1);
	}


	// end of file 확인
	bytes_read = (recv(sd, end_buf, 12, 0));
	if (bytes_read == -1) {
		perror("recv end of file");
		exit(1);
	}

	if(strcmp("end of file",end_buf) == 0) { //buf랑 비교
		printf("%s\n", end_buf);
	}else{
		perror("error : file is not end");
		exit(1);
	}
    close(sd);
    close(fd);

  
    return 0;
}


/*
#define PORTNUM 9005
#define BUFFERSIZE 256
//실행 파일 , 아이피 주소, 파일 이름
int main(int argc, char** argv){

	int sd, fd, n;
	char buf[BUFFERSIZE];
	char end_buf[50];
	char re_buf[50];


	const char* filename = argv[2]; //파일 이름
	struct sockaddr_in sin;
	socklen_t add_len = sizeof(struct sockaddr);

	// socket open
	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}

	// init
	memset((char *)&sin, '\0', sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = inet_addr(argv[1]);//아이피 주소
	sin.sin_port = htons(PORTNUM);

	//file open
	fd = open(filename, O_RDONLY);
	if(fd == -1){
		perror("file open fail");
		exit(1);
	}

	// 파일 이름 전송
    if(connect(sd,(struct sockaddr*)&sin,sizeof(sin)) == -1){
        perror("connect error");
		exit(1);
    }
	if(send(sd, filename, strlen(filename)+1, 0) == -1){
		perror("send filename");
		exit(1);
	}

	//파일이름 받기
	//서버에서 파일을 받은후 클라이언트에게 다시 파일 명을 전송함
	//buf로 데이타를 읽어들이게 된다

	int bytes_read = (recv(sd, buf, BUFFERSIZE, 0));

	if (bytes_read < 0) {
		perror("recvfrom filename");
		exit(1);
	}

	//내가 보낸 파일이랑 이름이 다른경우
	if(strcmp(filename,buf) != 0) { //buf랑 비교
		printf("%s\n", buf);
		perror("not match filename.");
		exit(1);
	}
	else printf("** match filename : %s\n", buf);// 파일이름 받고출력


	//file 내용을 전송
	while((n = read(fd, buf, 256)) > 0){ //fd에 있는걸 buf로 저장

		printf("SEND : %d\n", n);

		if (send(sd, buf, n, 0) == -1) {
			perror("send");
			exit(1);
		}
	}

	
	

	memset(buf,0, sizeof(buf));

	//sprintf(buf, "end of file"); //buf에다가 end of file을 붙이는 것
 //end of file 을 전송
	if(send(sd, "end of file", 12, 0)){
		perror("send filename");
		exit(1);
	}


	// end of file 확인
	bytes_read = (recv(sd, end_buf, 12, 0));
	if (bytes_read == -1) {
		perror("recv end of file");
		exit(1);
	}

	if(strcmp("end of file",end_buf) == 0) { //buf랑 비교
		printf("%s\n", end_buf);
	}else{
		perror("error : file is not end");
		exit(1);
	}
	close(fd);
    close(sd);
	return 0;
}*/