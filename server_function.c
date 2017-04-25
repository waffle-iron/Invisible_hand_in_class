#include "library.h"

int fileCount = 1;
int count_Dir = 0;
int count_File = 0; 

void SetFileCount(int num){
	fileCount = num;
}

void SetCountDir(int num){
	count_Dir = num;
}
void SetCountFile(int num){
	count_File = num;
}
void CountDir(const char* dir_name){
	//fileCount는 가지고있다고 생각	

	struct stat sbuf;
	stat(dir_name, &sbuf);

	// 디렉토리 인경우
	if (S_ISDIR(sbuf.st_mode)){
		count_Dir++;
	} else if (S_ISREG(sbuf.st_mode)){
	//파일 인경우
		;
	}

	count_File = fileCount - count_Dir;

}
void CountFile(const char* name){

	DIR *dp;
	struct dirent *dent;
	struct stat buf;
	char temp_dir_name[SIZEBUF];

	if ((dp = opendir(name)) == NULL){
		perror("opendir");
	};
	
	while ((dent = readdir(dp)) != NULL) {
		printf("%s\n", dent->d_name);
		if (strcmp(dent->d_name, ".") == 0){
			continue;
		}
		if (strcmp(dent->d_name, "..") == 0){
			continue;
		}

		sprintf(temp_dir_name, "%s/%s", name, dent->d_name);
		if (stat(temp_dir_name, &buf) == -1){
			perror("stat");
			exit(1);
		}
		//fileCount++;
		printf("%d\n", fileCount);
		if (S_ISDIR(buf.st_mode)){
			CountFile(temp_dir_name);
			count_Dir++;
		}
	}
}

////UDP 서버부분
void UdpServer(int sd, struct sockaddr_in cli){

	int i = 0;
	char buf[SIZEBUF];
	socklen_t clientlen = sizeof(cli);
	char path[SIZEBUF];
	char check[SIZEBUF];
	int flag = 1;
	int bytes_read= 0;
	int file_flag = 1; // 1 파일 0  폴
	// save 디렉토리 안에 저장하기 위해 작성
	mkdir("save", 0744);

	//while (1) {
	for (i = 0; i < fileCount; ++i){
		printf("요청이 들어오기를 대기중입더니다.~~\n");


		// 파일 혹은 디렉토리 인지 전송 받음
		if ((bytes_read = recvfrom(sd, buf, SIZEBUF, 0, (struct sockaddr *)&cli, &clientlen)) == -1){
			perror("recvfrom file or dir");
			exit(1);
		}
		
		// 제대로 잘 받았다고 메시지 전송
		if (sendto(sd, "SUCCUSS", SIZEBUF, 0, (struct sockaddr*)&cli, sizeof(cli)) == -1){
			perror("sendto file or dir");
			exit(1);
		}
		printf("전송하고 싶은 게 어떤 것이니? =  %s\n", buf);
		if (strcmp("This is File", buf) == 0){
			file_flag = 1;		
		}
		if(strcmp("This is DIR", buf) == 0){
			file_flag = 0;		
		}

		// 디렉토리 경로 받음
		if ((bytes_read = recvfrom(sd, buf, SIZEBUF, 0, (struct sockaddr *)&cli, &clientlen)) == -1){
			perror("recvfrom dirname");
			exit(1);
		}
		if( i == 0 ){
			sprintf(path,"%s",buf);
			printf("%s\n",path);
		}
		printf("디렉토리 경로 = %s\n", buf);
		//거기에 대한 답장
		if (sendto(sd, buf, SIZEBUF, 0, (struct sockaddr*)&cli, sizeof(cli)) == -1){
			perror("sendto dirname");
			exit(1);
		}

		// 파일 인경우
		if (file_flag == 1){
			int fd;

			// 해당 파일을 연다
			if ((fd = open(buf, O_WRONLY | O_CREAT, 0766)) == -1){
				perror("file open fail");
				exit(1);
			}
			
			// 파일 내용 전송
			while (1) {

				memset(buf, 0, sizeof(buf));

				// 파일 내용 받음
				if ((bytes_read = recvfrom(sd, buf, SIZEBUF, 0, (struct sockaddr *)&cli, &clientlen)) == -1){
					perror("recvfrom file contests");
					exit(1);
				}
				printf("SEND FILE CONTENTS SIZE: %d\n", bytes_read);
				
				// 만약 받은 문자열이 end of file (즉, 파일의 전송이 끝남을 알릴경우)
				if (!strncmp(buf, "end of file", SIZEBUF)) { 
						
					// 거기에 대한 답장 
					if (sendto(sd, "end of file", SIZEBUF, 0, (struct sockaddr *)&cli, sizeof(cli)) == -1){
						perror("sendto end of file");
						exit(1);
					}

					printf(" *****  END OF TRANSFER FILE\n  *****  ");
					close(fd);
					
					break; // while문 탈출
				} else {
					// 끝이 아닌 경우 파일 내용 파일에 작성 
					write(fd, buf, SIZEBUF);
				}
			}
		} else if (file_flag == 0){
			// 디렉토리 인경우
			// 디렉토리 생성 
			mkdir(buf, 0744);
		} else{
			printf("파일과 디렉토리가 아닙니다.\n");
		}
	}
	//무결성
	
	//리시브
	if ((bytes_read = recvfrom(sd, buf, SIZEBUF, 0, (struct sockaddr *)&cli, &clientlen)) == -1){
		perror("file cnt recieve");
		exit(1);
	}
	
	sprintf(check,"file_cnt = %d", count_File);
	if( strcmp(buf,check) != 0){
		flag = 0;	
	}
	
	//sendto
	if (sendto(sd, buf, SIZEBUF, 0, (struct sockaddr*)&cli, sizeof(cli)) == -1){
		perror("file cnt");
		exit(1);
	}
	//리시브
	if ((bytes_read = recvfrom(sd, buf, SIZEBUF, 0, (struct sockaddr *)&cli, &clientlen)) == -1){
		perror("file cnt recieve");
		exit(1);
	}

	sprintf(check,"dir_cnt = %d", count_Dir);
	if( strcmp(buf,check) != 0){
		flag = 0;	
	}
	
	//sendto
	if (sendto(sd, buf, SIZEBUF, 0, (struct sockaddr*)&cli, sizeof(cli)) == -1){
		perror("file cnt");
		exit(1);
	}
	flag? sprintf(buf,"True"): sprintf(buf,"False");

	//확인
	if (sendto(sd, buf, SIZEBUF, 0, (struct sockaddr*)&cli, sizeof(cli)) == -1){
		perror("무결성 체");
		exit(1);
	}
}

void TcpServer(int sd, struct sockaddr_in cli){
	int ns;
	int i = 0;
	char buf[SIZEBUF];
	socklen_t clientlen = sizeof(cli);
	char path[SIZEBUF];
	char check[SIZEBUF];
	int flag = 1;
	int file_flag = 1; // 1 파일 0  폴
	// save 디렉토리 안에 저장하기 위해 작성
	mkdir("save", 0744);

	if (listen(sd, 5)) {
		perror("listen");
		exit(1);
	}
	//요청을 받음 
	if ((ns = accept(sd, (struct sockaddr *)&cli, &clientlen)) == -1){
		perror("accept");
		exit(1);
	}
	for (i = 0; i < fileCount; ++i){
		printf("요청이 들어오기를 대기중입니다.~~\n");
		printf("for = %d\n", fileCount);
		int bytes_read = 0;

		// 파일 혹은 디렉토리 인지 전송 받음
		if (recv(ns, buf, SIZEBUF, MSG_WAITALL) == -1){
			perror("recvfrom file or dir");
			exit(1);
		}
		printf("전송하고 싶은 게 어떤 것이니? =  %s\n", buf);
		if (strcmp("This is File", buf) == 0){
			file_flag = 1;		
		}
		if(strcmp("This is DIR", buf) == 0){
			file_flag = 0;		
		}
		// 제대로 잘 받았다고 메시지 전송
		if (send(ns, "SUCCUSS", SIZEBUF, 0) == -1) {
			perror("send");
			exit(1);
		}

		// 디렉토리 경로 받음
		if (recv(ns, buf, SIZEBUF, MSG_WAITALL) == -1){
			perror("recv dirname");
			exit(1);
		}
		if( i == 0 ){
			sprintf(path,"%s",buf);
			printf("첫번쨰 경로 = %s\n",path);
		}
		printf(" 디렉토리 경로 = %s\n", buf);
		//거기에 대한 답장
		if (send(ns, buf, SIZEBUF, 0) == -1){
			perror("send dirname");
			exit(1);
		}
		// 파일 인경우
		if (file_flag == 1){
			int fd;

			// 해당 파일을 연다
			if ((fd = open(buf, O_WRONLY | O_CREAT, 0666)) == -1){
				perror("file open fail");
				exit(1);
			}

			// 파일 내용 전송
			while (1) {

				memset(buf, 0, sizeof(buf));

				// 파일 내용 받음
				if (recv(ns, buf, SIZEBUF, MSG_WAITALL) == -1){
					perror("recv file contests");
					exit(1);
				}
				printf("SEND FILE CONTENTS SIZE: %d\n", bytes_read);

				// 만약 받은 문자열이 end of file (즉, 파일의 전송이 끝남을 알릴경우)
				if (!strncmp(buf, "end of file", SIZEBUF)) {

					// 거기에 대한 답장 
					if (send(ns, "end of file", SIZEBUF, 0) == -1){
						perror("send end of file");
						exit(1);
					}

					printf(" *****  END OF TRANSFER FILE  *****  \n");
					close(fd);

					break; // while문 탈출
				} else {
					// 끝이 아닌 경우 파일 내용 파일에 작성 
					write(fd, buf, SIZEBUF);
				}
			}
		} else if (file_flag == 0){
			// 디렉토리 인 경우  생성 
			mkdir(buf, 0744);
		} else{
			printf("파일과 디렉토리가 아닙니다.\n");
		}
	}
	printf("PAHT %s\n", path);
	CountFile(path);
	CountDir(path);
	
	// 파일 개수 전송
	if (recv(ns, buf, SIZEBUF, MSG_WAITALL) == -1){
		perror("file count recieve");
		exit(1);
	}
	sprintf(check, "file_cnt = %d", count_File);
	if (send(ns, buf, SIZEBUF, 0) == -1){
		perror("file count send");
		exit(1);
	}
	printf("check = %s\n", check);
	if (strcmp(buf, check) != 0){
		flag = 0;
	}

	// 폴더 개수 전송
	if (recv(ns, buf, SIZEBUF, MSG_WAITALL) == -1){
		perror("dir count recieve");
		exit(1);
	}
	sprintf(check,"dir_cnt = %d", count_Dir);
	if (send(ns, buf, SIZEBUF, 0) == -1){
		perror("dir count send");
		exit(1);
	}
	printf("check = %s\n", check);
	if (strcmp(buf, check) != 0){
		flag = 0;
	}

	flag? sprintf(buf,"True"): sprintf(buf,"False");
	
	//확인
	if (send(ns, buf, SIZEBUF, 0) == -1){
		perror("무결성 체크");
		exit(1);
	}
	
}
