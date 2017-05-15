#include "library.h"

int fileCount = 1;
int count_Dir = 0;
int count_File = 0;
int indexA,offset;


void initGrobal(){
	fileCount = 1;
	count_Dir = 0;
	count_File = 0;
}
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
		return ;
		//perror("opendir");
	};

	while ((dent = readdir(dp)) != NULL) {

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

		if (S_ISDIR(buf.st_mode)){
			CountFile(temp_dir_name);
			count_Dir++;
		}
	}
}
// save 패스를 붙이고, 
// 인덱스와 오프셋를 구함 
void FilePathCheck(int file_size){
	//클라언트 array 받고 , 내꺼 파일이랑 비교
	struct stat sbuf;
	char temp_file_name[SIZEBUF];
	//socklen_t clientlen = sizeof(cli);
	int i;
	
   
    for(i=0; i<file_size; i++){
		//printf("file_size i : %d", i);
       // fflush(stdout);
        sprintf(temp_file_name,"./save/%s",file_info[i].path);
        printf("temp file name : %s\n", temp_file_name);
		printf("\nfile size = %s \n", temp_file_name);
		
        if(lstat(temp_file_name, &sbuf) >= 0){
			//파일 읽는것 파일이 있을경우
			indexA = i;
			offset = FileLocatePointer(temp_file_name);
		} else {
			//파일이 없을경우 // 그전 파일의 offset 을 구한다.
			if( i == 0 ){
				//첫번쨰 인경우
				indexA=0;
				offset=0;
				break;
			}else{
				//첫번째가 아닌경우
				sprintf(temp_file_name,"./save/%s",file_info[i-1].path);
				indexA = i-1;
				offset = FileLocatePointer(temp_file_name);
				break;
			}
		}
	}

	for (i = 0; i < fileCount; ++i){
		sprintf(temp_file_name,"./save/%s",file_info[i].path);
		strcpy(file_info[i].final_path,temp_file_name);
	}
	printf("index: %d  offset : %d\n", indexA, offset);
}

// 파일 offset 구하기
int FileLocatePointer(char *file){
	int fileEnd=0;

	int fd = open(file, O_RDONLY | O_CREAT | O_APPEND);
	fileEnd = lseek(fd, (off_t)0, SEEK_END);
	close(fd);

	return fileEnd ==0? fileEnd: fileEnd-1;
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
	printf("PAHT %s\n", path);
	CountFile(path);
	CountDir(path);

	// 파일 개수 전송
	if ((bytes_read = recvfrom(sd, buf, SIZEBUF, 0, (struct sockaddr *)&cli, &clientlen)) == -1){
		perror("file cnt recieve");
		exit(1);
	}
	sprintf(check,"file_cnt = %d", count_File);
	if (sendto(sd, buf, SIZEBUF, 0, (struct sockaddr*)&cli, sizeof(cli)) == -1){
		perror("file cnt");
		exit(1);
	}
	if (strcmp(buf, check) != 0){
		flag = 0;
	}
	//리시브
	if ((bytes_read = recvfrom(sd, buf, SIZEBUF, 0, (struct sockaddr *)&cli, &clientlen)) == -1){
		perror("file cnt recieve");
		exit(1);
	}

	// 폴더 개수 전송
	sprintf(check,"dir_cnt = %d", count_Dir);
	if( strcmp(buf,check) != 0){
		flag = 0;
	}
	if (sendto(sd, buf, SIZEBUF, 0, (struct sockaddr*)&cli, sizeof(cli)) == -1){
		perror("file cnt");
		exit(1);
	}

	flag? sprintf(buf,"True"): sprintf(buf,"False");

	//확인
	if (sendto(sd, buf, SIZEBUF, 0, (struct sockaddr*)&cli, sizeof(cli)) == -1){
		perror("무결성 체크");
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
	//int file_flag = 1; // 1 파일 0  폴
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

	
    FilePathCheck(files_size);

	sprintf(buf, "%d", offset);
    printf("%s\n",buf);

    if (send(ns, buf, SIZEBUF, 0) == -1){
		perror("sendto offset");
		exit(1);
	}

    if(recv(ns, buf, SIZEBUF, MSG_WAITALL) == -1){
		perror("recv offset");
		exit(1);
	}

	sprintf(buf, "%d", indexA);

    printf("%s\n",buf);
    if (send(ns, buf, SIZEBUF, 0) == -1){
		perror("sendto index");
		exit(1);
	}

    if(recv(ns, buf, SIZEBUF, MSG_WAITALL) == -1){
		perror("recv index");
		exit(1);
	}

	for (i = 0; i < fileCount; ++i){
		printf("index =  %s   ,file or dir=   %c\n", file_info[i].final_path,file_info[i].or_file_dir);

	}
	printf("\n");
    for (i = indexA; i < fileCount; ++i){
		printf("i = %d \n",i);
		printf("요청이 들어오기를 대기중입니다.~~\n");
		printf("name %s   ,file or dir=   %c\n", file_info[i].final_path,file_info[i].or_file_dir);
		int bytes_read = 0;
		if(file_info[i].or_file_dir == 'd'){
			mkdir(file_info[i].final_path, 0744);
			continue;
		
		} else if(file_info[i].or_file_dir == 'f'){
			int fd;

			// 해당 파일을 연다
			if ((fd = open(file_info[i].final_path, O_WRONLY | O_CREAT | O_APPEND, 0666)) == -1){
				perror("file open fail");
				exit(1);
			}

			// 파일 내용 받음
			while (1) {
				int n=0;
				int bytes_read=0;
				char tmp[10];
				memset(buf, 0x00, SIZEBUF);
				// 파일 내용 받음
				
				if ( (bytes_read = recv(ns, buf, SIZEBUF, MSG_WAITALL)) == -1){
					perror("recv file contests");
					exit(1);
				}
					
				//printf("SEND FILE CONTENTS : %s\n", buf);
				// 만약 받은 문자열이 end of file (즉, 파일의 전송이 끝남을 알릴경우)
				if (!strncmp(buf, "end of file", SIZEBUF)){

					// 거기에 대한 답장

					if (send(ns, "end of file", SIZEBUF, 0) == -1){
						perror("send end of file");
						exit(1);
					}

					printf(" *****  END OF TRANSFER FILE  *****  \n");
					close(fd);

					break; // while문 탈출
				}
				
				else {
					
					// 끝이 아닌 경우 파일 내용 파일에 작성
					
					if ( recv(ns, tmp,10, 0 ) == -1){
					perror("recv file contests");
					exit(1);
					}
					n = atoi(tmp);
					
					printf("SEND FILE CONTENTS SIZE: %d\n", n);
					printf("파일내용 적어요\n");
                    write(fd, buf, n);
				}
			}
		}
    }
    CountFile(file_info[0].path);
    CountDir(file_info[0].path);

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
    close(ns);
}

