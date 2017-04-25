#include "library.h"

void UdpServer(int sd, struct sockaddr_in sin){

	char buf[SIZEBUF];

	socklen_t  clientlen = sizeof(sin);

	// save 디렉토리 안에 저장하기 위해 작성
	mkdir("save", 0744);

	//
	while (1) {
		printf("요청이 들어오기를 대기중입니다.~~\n");

		int bytes_read= 0;
		
		// 파일 혹은 디렉토리 인지 전송 받음
		if ((bytes_read = recvfrom(sd, buf, SIZEBUF, 0, (struct sockaddr *)&sin, &clientlen)) == -1){
			perror("recvfrom file or dir");
			exit(1);
		}
		
		printf("전송하고 싶은 게 어떤 것이니? =  %s\n", buf);

		// 제대로 잘 받았다고 메시지 전송
		if (sendto(sd, "SUCCUSS", SIZEBUF, 0, (struct sockaddr*)&sin, sizeof(sin)) == -1){
			perror("sendto file or dir");
			exit(1);
		}

		// 파일 인경우
		if (strcmp("This is File", buf) == 0){
			int fd;
			
			// 저장해야되는 파일 경로 받음
			if ((bytes_read = recvfrom(sd, buf, SIZEBUF, 0, (struct sockaddr *)&sin, &clientlen)) == -1){
				perror("recvfrom filepath");
				exit(1);
			}
			
			printf("** file path From Client : %s\n", buf);

			// 저장해야되는 파일 경로 답장 
			if (sendto(sd, buf, SIZEBUF, 0, (struct sockaddr*)&sin, sizeof(sin)) == -1){
				perror("sendto filename");
				exit(1);
			}

			// 해당 파일을 연다
			if ((fd = open(buf, O_WRONLY | O_CREAT, 0666)) == -1){
				perror("file open fail");
				exit(1);
			}
			
			// 파일 내용 전송
			while (1) {

				memset(buf, 0, sizeof(buf));

				// 파일 내용 받음
				if ((bytes_read = recvfrom(sd, buf, SIZEBUF, 0, (struct sockaddr *)&sin, &clientlen)) == -1){
					perror("recvfrom file contests");
					exit(1);
				}
				printf("SEND FILE CONTENTS SIZE: %d\n", bytes_read);
				
				// 만약 받은 문자열이 end of file (즉, 파일의 전송이 끝남을 알릴경우)
				if (!strncmp(buf, "end of file", SIZEBUF)) { 
						
					// 거기에 대한 답장 
					if (sendto(sd, "end of file", SIZEBUF, 0, (struct sockaddr *)&sin, sizeof(sin)) == -1){
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
		} else if (strcmp("This is DIR", buf) == 0){
			// 디렉토리 인경우

			// 디렉토리 경로 받음
			if ((bytes_read = recvfrom(sd, buf, SIZEBUF, 0, (struct sockaddr *)&sin, &clientlen)) == -1){
				perror("recvfrom dirname");
				exit(1);
			}

			printf(" 디렉토리 경로 = %s", buf);
			//거기에 대한 답장
			if (sendto(sd, buf, SIZEBUF, 0, (struct sockaddr*)&sin, sizeof(sin)) == -1){
				perror("sendto dirname");
				exit(1);
			}

			// 디렉토리 생성 
			mkdir(buf, 0744);
			
		} else{
			printf("파일과 디렉토리가 아닙니다.\n");
		}

		//printf("** From Client : %s\n", buf);// �����̸� �ް� ����
		////POSIX ǥ�� ������-> ANSI ������
		//FILE *o_fd, *fd;
		////FILE *o_fd2,*fd2;


		//filename = buf;
		//char finalFile[256] = "./temp/";
		//strcat(finalFile, filename);
		//printf("%s", finalFile);
		/////////////////////// ���� �̸� ������///////////////////////////////////
		//if (sendto(sd, buf, strlen(buf) + 10, 0, (struct sockaddr *)&sin, sizeof(sin))
		//	== -1){
		//	perror("sendto filename");
		//	exit(1);
		//}

		///////////////////////////////////////////////////////////////////////
		//fd = fopen("temp.dat", "w");
		//if (fd == NULL){
		//	perror("file fail");
		//	exit(1);
		//}

		//while (1) {

		//	memset(buf, 0, sizeof(buf));
		//	//printf("2while\n");

		//	bytes_read = recvfrom(sd, buf, 255, 0, (struct sockaddr *)&sin, &clientlen);
		//	printf("RECV : %d byte\n", bytes_read);

		//	//printf("3while\n");
		//	if (bytes_read == -1) {
		//		perror("recvfrom date");
		//		exit(1);
		//	}
		//	buf[bytes_read] = '\0';

		//	if (!strncmp(buf, "end of file", 10)) { //������ �޽����� end of file�̸� ����
		//		//�ӽ� ���� ���� -> ���� ���� + ���� ���� ����
		//		fclose(fd);
		//		char writeBuffer[256];
		//		o_fd = fopen(finalFile, "w+");
		//		if (o_fd == NULL)  perror("file fail");

		//		fd = fopen("temp.dat", "r");

		//		memset(writeBuffer, 0, sizeof(writeBuffer));

		//		while (fgets(writeBuffer, sizeof(writeBuffer), fd) != NULL)
		//			fprintf(o_fd, "%s", writeBuffer);


		//		if (chmod(finalFile, 0766) == -1)

		//			printf("���ٱ��� ���濡 ���� �߽��ϴ�. ������ ���� ������ Ȯ���� �ּ���.");
		//		printf("file close\n");
		//		fclose(o_fd);
		//		fclose(fd); //stream �ݱ�

		//		//���� �� �޾Ҵٰ� ����

		//		if (sendto(sd, "end of file", strlen("end of file") + 1, 0, (struct sockaddr *)&sin, sizeof(sin)) == -1){
		//			perror("sendto end of file");
		//			exit(1);
		//		}

		//		break; //while�� ����������
		//	} else {
		//		//printf("%d byte recv: %s\n", bytes_read, buf);
		//		//			    fputs(buf, stream); //���Ϸ� ����
		//		fprintf(fd, "%s", buf);
		//	}
		//}
		//���Ἲ üũ ����
		//	printf("���Ἲ üũ ����");
		//	fd = fopen("temp2.dat", "w+");
		//	o_fd = fopen(finalFile, "w+");
		//	while (1) {

		//		memset(buf, 0, sizeof(buf));
		//		//printf("2while\n");

		//		bytes_read = recvfrom(sd, buf, 255, 0, (struct sockaddr *)&sin, &clientlen);
		//		printf("RECV : %d byte\n", bytes_read);

		//		//printf("3while\n");
		//		if (bytes_read == -1) {
		//			perror("recvfrom date");
		//			exit(1);
		//		}
		//		buf[bytes_read] = '\0';

		//		if (!strncmp(buf, "end of file", 10)) { //������ �޽����� end of file�̸� ����
		//			//�ӽ� ���� ���� -> ���� ���� + ���� ���� ����

		//			if (sendto(sd, "end of file", strlen("end of file") + 1, 0, (struct sockaddr *)&sin, sizeof(sin)) == -1){
		//				perror("sendto end of file");
		//				exit(1);
		//			}

		//			char checkBuffer_1[256];
		//			char checkBuffer_2[256];
		//			memset(checkBuffer_1, 0, sizeof(checkBuffer_1));
		//			memset(checkBuffer_2, 0, sizeof(checkBuffer_2));

		//			while (fgets(checkBuffer_1, sizeof(checkBuffer_1), fd) != NULL){//�ٽ� �޾ƿ� ���� ������
		//				fgets(checkBuffer_2, sizeof(checkBuffer_2), fd);//���� ���� ������
		//				if (!strcmp(checkBuffer_1, checkBuffer_2)){
		//					perror("file check fail");
		//					fclose(fd);
		//					fclose(o_fd);
		//					exit(1);
		//				}
		//			}


		//			//100% ����

		//			if (sendto(sd, "100%%", strlen("100%%") + 1, 0, (struct sockaddr *)&sin, sizeof(sin)) == -1){
		//				perror("sendto 100%%");
		//				exit(1);
		//			}
		//			fclose(fd);
		//			fclose(o_fd);
		//			break; //while�� ����������
		//		} else {
		//			printf("Ingridty : %d byte \n", bytes_read);

		//			fprintf(fd, "%s", buf);
		//		}
		//	}
		//	//�ӽ� ���� ������
		//	int removeTempFile = remove("./temp.dat");
		//	if (removeTempFile == -1) printf("remove fail");
		//	removeTempFile = remove("./temp2.dat");
		//	if (removeTempFile == -1) printf("remove fail");
		//}

	}
}

void TcpServer(){

	char buf[SIZEBUF + 1];
	const char* filename;
	struct sockaddr_in sin, cli;
	int sd = 0, ns = 0;
	socklen_t clientlen = sizeof(cli);

	mkdir("temp", 0777);

	FILE *fd, *o_fd;
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

	if (bind(sd, (struct sockaddr *)&sin, sizeof(sin)) == -1) {
		perror("bind");
		exit(1);
	}

	if (listen(sd, 5) == -1) {
		perror("listen");
		exit(1);
	}
	//	printf("whiel���������");
	while (1) {
		printf("inwhile~~\n");

		if ((ns = accept(sd, (struct sockaddr *)&cli, &clientlen)) == -1){
			perror("accept");
			exit(1);
		}

		if (recv(ns, buf, SIZEBUF, MSG_WAITALL) == -1){
			perror("recv filename");
			exit(1);
		}

		filename = buf;

		if (send(ns, buf, SIZEBUF, 0) == -1) {
			perror("send");
			exit(1);
		}
		fd = fopen("temp.dat", "w");
		if (fd == NULL)
			perror("file fail");


		char finalFile[SIZEBUF] = "./temp/";
		strcat(finalFile, filename);
		printf("%s\n", finalFile);

		// ���� - ���ϳ��� �޴°� ����
		while (1) {
			memset(buf, 0, SIZEBUF);
			//			printf("2while\n");
			int bytes_read = 0;
			bytes_read = recv(ns, buf, SIZEBUF, MSG_WAITALL);
			//			printf("finalerecv���̴� = %s\n", finalFile);
			printf("RECV : %d byte\n", bytes_read);
			//            printf("buf = %s\n",buf);
			//			printf("3while\n");
			if (bytes_read == -1) {
				perror("recv date");
				exit(1);
			}
			buf[bytes_read] = '\0';
			//			printf("finaleFIle2 = %s\n", finalFile);
			if (!strncmp(buf, "end of file", 12)) { //������ �޽����� end of file�̸� ����
				printf("file close\n");
				fclose(fd);

				char writeBuffer[SIZEBUF];
				printf("%s finaleFILE ���� \n", finalFile);
				o_fd = fopen(finalFile, "w+");
				if (o_fd == NULL)  perror("file fail");

				fd = fopen("./temp.dat", "rb");
				if (fd == NULL) perror("temp fail");
				memset(writeBuffer, 0, SIZEBUF);

				while (fgets(writeBuffer, SIZEBUF, fd) != NULL){
					printf("%s \n", writeBuffer);
					fputs(writeBuffer, o_fd);
				}




				if (chmod(finalFile, 0766) == -1)

					printf("���ٱ��� ���濡 ���� �߽��ϴ�. ������ ���� ������ Ȯ���� �ּ���.");
				//printf("file close\n");
				fclose(o_fd);
				fclose(fd); //stream �ݱ�

				//
				//���� �� �޾Ҵٰ� ����

				if (send(ns, "end of file", SIZEBUF, 0) == -1){
					perror("send end of file");
					exit(1);
				}
				break; //while�� ����������
			} else {
				//				printf("%d byte recv\n", bytes_read);
				//			    fputs(buf, stream); //���Ϸ� ����
				fwrite(buf, sizeof(char), SIZEBUF, fd);
			}
		}
		//���Ἲ üũ
		printf("���Ἲ üũ\n");
		fd = fopen("temp2.dat", "w");//fd2 �ٽ� ����
		o_fd = fopen(finalFile, "r");
		while (1) {

			memset(buf, 0, SIZEBUF);
			//			printf("���Ἲ2while\n");
			int bytes_read = recv(ns, buf, SIZEBUF, MSG_WAITALL);
			//			printf("���Ἲ3while\n");
			if (bytes_read == -1) {
				perror("recv date");
				exit(1);
			}
			buf[bytes_read] = '\0';

			if (strncmp(buf, "end of file", 12) == 0) { //������ �޽����� end of file�̸� ����
				if (send(ns, "end of file", SIZEBUF, 0) == -1){
					perror("send end of file");
					exit(1);
				}

				char checkBuffer_1[SIZEBUF];
				char checkBuffer_2[SIZEBUF];
				memset(checkBuffer_1, 0, SIZEBUF);
				memset(checkBuffer_2, 0, SIZEBUF);

				while (fgets(checkBuffer_1, SIZEBUF, fd) != NULL){//�ٽ� �޾ƿ� ���� ������
					fgets(checkBuffer_2, SIZEBUF, o_fd);//���� ���� ������
					if (!strcmp(checkBuffer_1, checkBuffer_2)){
						perror("file check fail");
						exit(1);
					}
				}
				fclose(fd);
				fclose(o_fd);
				//	printf("send���Դϴ�\n");
				if (send(ns, "100percent", SIZEBUF, 0) == -1){
					perror("send 100");
					exit(1);
				}

				//	printf("send��\n");
				break;
			} else {
				printf("Ingridty : %d byte \n", bytes_read);
				//			    fputs(buf, stream); //���Ϸ� ����
				fwrite(buf, sizeof(char), SIZEBUF, fd);
			}
		}
		int removeTempFile = remove("./temp.dat");
		if (removeTempFile == -1) printf("remove fail");
		removeTempFile = remove("./temp2.dat");
		if (removeTempFile == -1) printf("remove fail");
		//printf("close ���̴�\n");
		close(ns);
		//close(sd);
	}


/*
double file_Transfer_Timer(long start_tv_sec,long start_tv_usec,long end_tv_sec,long end_tv_usec){

	return (double)(end_tv_sec)+(double)(end_tv_usec)/1000000.0-(double)(start_tv_sec)-(double)(start_tv_usec)/1000000.0;
}

double File_transfer_speed(double timer){
	struct stat inf_file;
	stat(buf, &inf_file);
	int file_size=inf_file.st_size;
	return (double)file_size/timer;
}

			gettimeofday(&start_point, NULL);

					gettimeofday(&end_point, NULL);


					double timer = timer_cal(start_point.tv_sec,start_point.tv_usec,end_point.tv_sec,end_point.tv_usec);
		gettimeofday(&start_point, NULL);
				gettimeofday(&end_point, NULL);

					double timer = timer_cal(start_point.tv_sec,start_point.tv_usec,end_point.tv_sec,end_point.tv_usec);					printf("%f Byte",File_transfer_speed(timer));
*/
}
long long folder_Size(char *Dir_name,long long total_size){
    DIR *dp;
    struct dirent *dent;
    int index = 0;
    int files_number=2000;
    char buf[SIZEBUF];
    char temp_dirent_name[2000] = {0, };
    char temp_Dir_name[SIZEBUF];
    char *cwd;
    struct stat sbuf;

    memset(buf, 0,sizeof(buf));
    sprintf(temp_Dir_name, temp_Dir_name, Dir_name);

    if((dp = opendir(Dir_name)) == NULL){
        perror("opendir");
        exit(1);

    }
    memset(buf,0,SIZEBUF);
    file_information* files = (file_information*)malloc(sizeof(file_information) * files_number);
    while((dent = readdir(dp))){
        if(index >= files_number -2){
            files_number *=2;
            files = (file_information *)realloc((void *)files, sizeof(file_information) * files_number);
        }
        if(strcmp(dent->d_name, ".") == 0){
            continue;
        }
        if(strcmp(dent->d_name, ".") == 0){
            continue;
        }

        sprintf(temp_Dir_name, "%s/%s",Dir_name,dent->d_name);
        if(stat(temp_Dir_name, & sbuf) == -1){
            perror("stat");
            exit(1);
        }
        files[index].dent = *dent;
        if (S_ISDIR(sbuf.st_mode)){
            files[index].or_file_dir = 'd';
        } else if (S_ISREG(sbuf.st_mode)){
            files[index].or_file_dir = 'f';
        }
        index++;
    }

    int i = 0;
    for (i = 0; i < index; ++i){
        printf("temp = %s\n",temp_Dir_name);
        sprintf(temp_Dir_name, "%s/%s",Dir_name ,files[i].dent.d_name);
        if (files[i].or_file_dir == 'f'){
            total_size += file_Size(temp_Dir_name);//
        } else if (files[i].or_file_dir == 'd'){
        char temp[2] = { '/', '\0' };
        sprintf(temp_dirent_name, "%s%s%s", Dir_name, temp, files[i].dent.d_name);
        cwd = getcwd(NULL, SIZEBUF);
        chdir(files[i].dent.d_name);
        total_size += folder_Size(temp_dirent_name,total_size);
        chdir(cwd);
        
        }
    }
    return total_size;
}

long long file_Size(char * file_name){
    struct stat sbuf;
    
    stat(file_name, &sbuf);
    long long fsize = (long long)sbuf.st_size;
    return fsize;
}

double File_transfer_speed(double total_time){
    long long total_size=0;
    char root_folder[SIZEBUF] = "./save";
    total_size += folder_Size (root_folder,total_size);

    return (double)total_size/total_time;
}
