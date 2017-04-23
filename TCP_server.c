#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include "library.h"

#define PORTNUM 9000
#define BUFFERSIZE 256

int main() {
  char buf[256];
  const char* filename;
  struct sockaddr_in sin, cli;
  int sd = 0, ns = 0, clientlen = sizeof(cli);

  mkdir("temp",0777);

  FILE *fd,*o_fd;
  close(sd);
  close(ns);
  if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
      perror("socket");
      exit(1);
  }

  memset((char *)&sin, '\0', sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_port = htons(PORTNUM);
  sin.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(sd, (struct sockaddr *)&sin, sizeof(sin))) {
      perror("bind");
      exit(1);
  }

  while (1) {
		printf("inwhile~~\n");

        if (listen(sd, 5)) {
            perror("listen");
            exit(1);
        }

        if ((ns = accept(sd, (struct sockaddr *)&cli, &clientlen))==-1){
            perror("accept");
            exit(1);
        }

		if (recv(ns, buf, 256, 0) == -1){
            perror("recv filename");
            exit(1);
        }

        filename = buf;

        if (send(ns, buf, strlen(buf) + 1, 0) == -1) {
            perror("send");
            exit(1);
        }
        fd = fopen("temp.dat", "w");
        if(fd == NULL)
        perror("file fail");


        char finalFile[248]="./temp/";
        strcat(finalFile,filename);
        printf("%s",finalFile);


		while(1) {

			memset(buf,0,sizeof(buf));
			printf("2while\n");
			int bytes_read = recv(ns, buf, sizeof(buf), 0);
			printf("3while\n");
			if (bytes_read == -1) {
				perror("recv date");
				exit(1);
			}
            buf[bytes_read] = '\0';

			if(!strncmp(buf, "end of file", 12)) { //마지막 메시지가 end of file이면 종료
				printf("file close\n");
			    fclose(fd);
                char writeBuffer[256];
                o_fd = fopen(finalFile, "w+");
                if(o_fd == NULL)  perror("file fail");

                fd = fopen("temp.dat", "r");

                memset(writeBuffer,0,sizeof(writeBuffer));

                while(fgets(writeBuffer,sizeof(writeBuffer), fd)!=NULL)
                fprintf(o_fd,"%s",writeBuffer);



                if(chmod(finalFile,0766) == -1)

                printf("접근권한 변경에 실패 했습니다. 파일의 접근 권한을 확인해 주세요.");
                printf("file close\n");
                fclose(o_fd);
                fclose(fd); //stream 닫기

                //
                //파일 끝 받았다고 전송

                if(send(ns, "end of file", strlen("end of file")+1, 0) == -1){
                perror("send end of file");
                exit(1);
                }
			    break; //while문 빠져나가기
			  } else {
			   	  printf("%d byte recv\n", bytes_read);
//			    fputs(buf, stream); //파일로 저장
				    fwrite(buf , sizeof(char), 255, fd);
          }
      }

      printf("무결성 체크");//무결성 체크
      fd = fopen("temp2.dat", "w+");//fd2 다시 연다
      o_fd = fopen(finalFile, "w+");
      while(1) {

			  memset(buf,0,sizeof(buf));
			  printf("2while\n");
			  int bytes_read = recv(ns, buf, sizeof(buf), 0);
			  printf("3while\n");
			  if (bytes_read == -1) {
				  perror("recv date");
				  exit(1);
			  }
            buf[bytes_read] = '\0';

			  if(!strncmp(buf, "end of file", 12)) { //마지막 메시지가 end of file이면 종료
          if(send(ns, "end of file", strlen("end of file")+1, 0) == -1){
            perror("send end of file");
            exit(1);
          }

          char checkBuffer_1[256];
          char checkBuffer_2[256];
          memset(checkBuffer_1,0,sizeof(checkBuffer_1));
          memset(checkBuffer_2,0,sizeof(checkBuffer_2));

          while(fgets(checkBuffer_1,sizeof(checkBuffer_1), fd)!=NULL){//다시 받아온 파일 데이터
            fgets(checkBuffer_2,sizeof(checkBuffer_2), fd);//원본 파일 데이터
            if(!strcmp(checkBuffer_1,checkBuffer_2)){
              perror("file check fail");
              exit(1);
            }
          }
          fclose(fd);
          fclose(o_fd);
          if(send(ns, "100%%", strlen("100%%")+1, 0) == -1){
            perror("send 100%%");
            exit(1);
          }

          break;
        }
        else {
			   	  printf("%d byte recv\n", bytes_read);
//			    fputs(buf, stream); //파일로 저장
				    fwrite(buf , sizeof(char), 255, fd);
          }
      }
      int removeTempFile = remove("./temp.dat");
      if(removeTempFile == -1) printf("remove fail");
      removeTempFile = remove("./temp2.dat");
      if(removeTempFile == -1) printf("remove fail");
      close(ns);
    }

    close(sd);
    return 0;
}
