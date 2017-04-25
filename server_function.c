#include "library.h"

extern int fileCount;

////UDP 서버부분
void UdpServer(int sd, struct sockaddr_in cli){
	printf("tjlqkflajksdl");

	int i = 0;
	char buf[SIZEBUF];
	socklen_t clientlen = sizeof(cli);

	// save 디렉토리 안에 저장하기 위해 작성
	mkdir("save", 0744);

	//while (1) {
	for (i = 0; i < fileCount; ++i){
		printf("요청이 들어오기를 대기중입니다.~~\n");

		int bytes_read= 0;
		
		// 파일 혹은 디렉토리 인지 전송 받음
		if ((bytes_read = recvfrom(sd, buf, SIZEBUF, 0, (struct sockaddr *)&cli, &clientlen)) == -1){
			perror("recvfrom file or dir");
			exit(1);
		}
		
		printf("전송하고 싶은 게 어떤 것이니? =  %s\n", buf);

		// 제대로 잘 받았다고 메시지 전송
		if (sendto(sd, "SUCCUSS", SIZEBUF, 0, (struct sockaddr*)&cli, sizeof(cli)) == -1){
			perror("sendto file or dir");
			exit(1);
		}

		// 파일 인경우
		if (strcmp("This is File", buf) == 0){
			int fd;
			
			// 저장해야되는 파일 경로 받음
			if ((bytes_read = recvfrom(sd, buf, SIZEBUF, 0, (struct sockaddr *)&cli, &clientlen)) == -1){
				perror("recvfrom filepath");
				exit(1);
			}
			
			printf("** file path From Client : %s\n", buf);

			// 저장해야되는 파일 경로 답장 
			if (sendto(sd, buf, SIZEBUF, 0, (struct sockaddr*)&cli, sizeof(cli)) == -1){
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
		} else if (strcmp("This is DIR", buf) == 0){
			// 디렉토리 인경우

			// 디렉토리 경로 받음
			if ((bytes_read = recvfrom(sd, buf, SIZEBUF, 0, (struct sockaddr *)&cli, &clientlen)) == -1){
				perror("recvfrom dirname");
				exit(1);
			}

			printf("디렉토리 경로 = %s\n", buf);
			//거기에 대한 답장
			if (sendto(sd, buf, SIZEBUF, 0, (struct sockaddr*)&cli, sizeof(cli)) == -1){
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

void TcpServer(int sd, struct sockaddr_in cli){
	int ns;
	int i = 0;
	char buf[SIZEBUF];
	socklen_t clientlen = sizeof(cli);

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
	//while (1) {
	for (i = 0; i < fileCount; ++i){
		printf("요청이 들어오기를 대기중입니다.~~\n");

		int bytes_read = 0;

		// 파일 혹은 디렉토리 인지 전송 받음
		if (recv(ns, buf, SIZEBUF, MSG_WAITALL) == -1){
			perror("recvfrom file or dir");
			exit(1);
		}

		printf("전송하고 싶은 게 어떤 것이니? =  %s\n", buf);

		// 제대로 잘 받았다고 메시지 전송
		if (send(ns, "SUCCUSS", SIZEBUF, 0) == -1) {
			perror("send");
			exit(1);
		}

		// 파일 인경우
		if (strcmp("This is File", buf) == 0){
			int fd;

			// 저장해야되는 파일 경로 받음
			if (recv(ns, buf, SIZEBUF, MSG_WAITALL) == -1){
				perror("recv filepath");
				exit(1);
			}

			printf("** file path From Client : %s\n", buf);

			// 저장해야되는 파일 경로 답장 
			if (send(ns, buf, SIZEBUF, 0) == -1){
				perror("send filename");
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
				if ((bytes_read = recv(ns, buf, SIZEBUF, MSG_WAITALL)) == -1){
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
			if (recv(ns, buf, SIZEBUF, MSG_WAITALL) == -1){
				perror("recv dirname");
				exit(1);
			}

			printf(" 디렉토리 경로 = %s", buf);
			//거기에 대한 답장
			if (send(ns, buf, SIZEBUF, 0) == -1){
				perror("send dirname");
				exit(1);
			}

			// 디렉토리 생성 
			mkdir(buf, 0744);

		} else{
			printf("파일과 디렉토리가 아닙니다.\n");
		}
		//char buf[SIZEBUF + 1];
		//const char* filename;
		//struct sockaddr_in sin, cli;
		//int sd = 0, ns = 0;

		//close(sd);

		//			if (chmod(finalFile, 0766) == -1)

		//				printf("���ٱ��� ���濡 ���� �߽��ϴ�. ������ ���� ������ Ȯ���� �ּ���.");
		//			printf("file close\n");
		//			fclose(o_fd);
		//			fclose(fd); //stream �ݱ�

		//			
		//			���� �� �޾Ҵٰ� ����

		//			if (send(ns, "end of file", SIZEBUF, 0) == -1){
		//				perror("send end of file");
		//				exit(1);
		//			}
		//			break; //while�� ����������
		//		} else {
		//							printf("%d byte recv\n", bytes_read);
		//						    fputs(buf, stream); //���Ϸ� ����
		//			fwrite(buf, sizeof(char), SIZEBUF, fd);
		//		}
		//	}
		//	���Ἲ üũ
		//	printf("���Ἲ üũ\n");
		//	fd = fopen("temp2.dat", "w");//fd2 �ٽ� ����
		//	o_fd = fopen(finalFile, "r");
		//	while (1) {

		//		memset(buf, 0, SIZEBUF);
		//					printf("���Ἲ2while\n");
		//		int bytes_read = recv(ns, buf, SIZEBUF, MSG_WAITALL);
		//					printf("���Ἲ3while\n");
		//		if (bytes_read == -1) {
		//			perror("recv date");
		//			exit(1);
		//		}
		//		buf[bytes_read] = '\0';

		//		if (strncmp(buf, "end of file", 12) == 0) { //������ �޽����� end of file�̸� ����
		//			if (send(ns, "end of file", SIZEBUF, 0) == -1){
		//				perror("send end of file");
		//				exit(1);
		//			}

		//			char checkBuffer_1[SIZEBUF];
		//			char checkBuffer_2[SIZEBUF];
		//			memset(checkBuffer_1, 0, SIZEBUF);
		//			memset(checkBuffer_2, 0, SIZEBUF);

		//			while (fgets(checkBuffer_1, SIZEBUF, fd) != NULL){//�ٽ� �޾ƿ� ���� ������
		//				fgets(checkBuffer_2, SIZEBUF, o_fd);//���� ���� ������
		//				if (!strcmp(checkBuffer_1, checkBuffer_2)){
		//					perror("file check fail");
		//					exit(1);
		//				}
		//			}
		//			fclose(fd);
		//			fclose(o_fd);
		//				printf("send���Դϴ�\n");
		//			if (send(ns, "100percent", SIZEBUF, 0) == -1){
		//				perror("send 100");
		//				exit(1);
		//			}

		//				printf("send��\n");
		//			break;
		//		} else {
		//			printf("Ingridty : %d byte \n", bytes_read);
		//						    fputs(buf, stream); //���Ϸ� ����
		//			fwrite(buf, sizeof(char), SIZEBUF, fd);
		//		}
		//	}
		//	int removeTempFile = remove("./temp.dat");
		//	if (removeTempFile == -1) printf("remove fail");
		//	removeTempFile = remove("./temp2.dat");
		//	if (removeTempFile == -1) printf("remove fail");
		//	printf("close ���̴�\n");
		//	close(ns);
		//close(sd);
	}
}