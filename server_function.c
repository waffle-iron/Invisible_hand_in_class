#include "library.h" 


double File_transfer_speed(long start_tv_sec,long start_tv_usec,long end_tv_sec,long end_tv_usec){

	return (double)(end_tv_sec)+(double)(end_tv_usec)/1000000.0-(double)(start_tv_sec)-(double)(start_tv_usec)/1000000.0;
}

double File_transfer_speed(double timer){
	struct stat inf_file;
	stat(buf, &inf_file);
	int file_size=inf_file.st_size;
	return (double)file_size/timer;
}

void UdpServer(){

	int sd;
	char buf[SIZEBUF];

	struct sockaddr_in sin;
	socklen_t  clientlen = sizeof(sin);;
	struct timeval start_point, end_point;
	
	//저장할 장소 생성
	mkdir("save", 0644);

	// 소켓 오픈
	if ((sd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}
	
	//init 
	memset((char *)&sin, '\0', sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(PORTNUM);
	sin.sin_addr.s_addr = htonl(INADDR_ANY);

	// 클라이언트와 바인드 
	if (bind(sd, (struct sockaddr *)&sin, sizeof(sin)) == -1) {
		perror("bind");
		exit(1);
	}

	//
	while (1) {
		printf("inwhile~~\n");
		int bytes_read;
		// 디렉토리인지 파일인지 받는 부분
		if ((bytes_read = recvfrom(sd, buf, SIZEBUF, 0, (struct sockaddr *)&sin, &clientlen)) == -1){
			perror("recvfrom isDIR");
			exit(1);
		}

		// 파일인지 아닌지 알았다는 메시지 출력
		if (sendto(sd, "SUCCUSS", SIZEBUF, 0, (struct sockaddr*)&sin, sizeof(sin)) == -1){
			perror("파일인지 아닌지 알았다는 메세지");
			exit(1);
		}

		memset(buf, 0, SIZEBUF);

		//파일 인경우
		if (strcmp("This is File", buf) == 0){
			int fd;
			//파일 이름 받기
			if ((bytes_read = recvfrom(sd, buf, SIZEBUF, 0, (struct sockaddr *)&sin, &clientlen)) == -1){
				perror("recvfrom filename");
				exit(1);
			}
			// 파일이름 받고 출력
			printf("** From Client : %s\n", buf);

			//파일이름 재전송
			if (sendto(sd, buf, SIZEBUF, 0, (struct sockaddr*)&sin, sizeof(sin)) == -1){
				perror("sendto filename");
				exit(1);
			}

			//그리고 파일 받기 
			// 파일열기
			if ((fd = open(buf, O_RDONLY)) == -1){
				perror("file open fail");
				exit(1);
			}
			
			gettimeofday(&start_point, NULL);
			//file 내용을 전송
			while (1) {

				memset(buf, 0, sizeof(buf));

				if ((bytes_read = recvfrom(sd, buf, SIZEBUF, 0, (struct sockaddr *)&sin, &clientlen)) == -1){
					perror("recvfrom end of file");
					exit(1);
				}
				printf("RECV : %d byte\n", bytes_read);

				//buf[bytes_read] = '\0';

				if (!strncmp(buf, "end of file", 10)) { //마지막 메시지가 end of file이면 종료
					//파일 끝 받았다고 전송

					gettimeofday(&end_point, NULL);
		
					
					double timer = timer_cal(start_point.tv_sec,start_point.tv_usec,end_point.tv_sec,end_point.tv_usec);
					printf("%f Byte",File_transfer_speed(timer));
					if (sendto(sd, "end of file", SIZEBUF, 0, (struct sockaddr *)&sin, sizeof(sin)) == -1){
						perror("sendto end of file");
						exit(1);
					}

					close(fd);
					printf("file close\n");
					break; //while문 빠져나가기
				} else {
					//printf("%d byte recv: %s\n", bytes_read, buf);
					//			    fputs(buf, stream); //파일로 저장
					fprintf(fd, "%s", buf);
				}
			}
		} else if (strcmp("This is DIR", buf) == 0){
			//디렉토리 인 경우 디렉토리 만들기 그리고 그 디렉토리로 이동

			//디렉토리 이름 받기
			if ((bytes_read = recvfrom(sd, buf, SIZEBUF, 0, (struct sockaddr *)&sin, &clientlen)) == -1){
				perror("recvfrom dirname");
				exit(1);
			}
			// 디렉토리 이름 전송
			if (sendto(sd, buf, SIZEBUF, 0, (struct sockaddr*)&sin, sizeof(sin)) == -1){
				perror("sendto dirname");
				exit(1);
			}
			mkdir(buf, 0644);
			//		chdir(buf);
		}

		//printf("** From Client : %s\n", buf);// 파일이름 받고 출력
		////POSIX 표준 입출력-> ANSI 입출력
		//FILE *o_fd, *fd;
		////FILE *o_fd2,*fd2;


		//filename = buf;
		//char finalFile[256] = "./temp/";
		//strcat(finalFile, filename);
		//printf("%s", finalFile);
		/////////////////////// 파일 이름 보내기///////////////////////////////////
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

		//	if (!strncmp(buf, "end of file", 10)) { //마지막 메시지가 end of file이면 종료
		//		//임시 파일 내용 -> 파일 내용 + 파일 권한 수정
		//		fclose(fd);
		//		char writeBuffer[256];
		//		o_fd = fopen(finalFile, "w+");
		//		if (o_fd == NULL)  perror("file fail");

		//		fd = fopen("temp.dat", "r");

		//		memset(writeBuffer, 0, sizeof(writeBuffer));

		//		while (fgets(writeBuffer, sizeof(writeBuffer), fd) != NULL)
		//			fprintf(o_fd, "%s", writeBuffer);


		//		if (chmod(finalFile, 0766) == -1)

		//			printf("접근권한 변경에 실패 했습니다. 파일의 접근 권한을 확인해 주세요.");
		//		printf("file close\n");
		//		fclose(o_fd);
		//		fclose(fd); //stream 닫기

		//		//파일 끝 받았다고 전송

		//		if (sendto(sd, "end of file", strlen("end of file") + 1, 0, (struct sockaddr *)&sin, sizeof(sin)) == -1){
		//			perror("sendto end of file");
		//			exit(1);
		//		}

		//		break; //while문 빠져나가기
		//	} else {
		//		//printf("%d byte recv: %s\n", bytes_read, buf);
		//		//			    fputs(buf, stream); //파일로 저장
		//		fprintf(fd, "%s", buf);
		//	}
		//}
		//무결성 체크 시작
		//	printf("무결성 체크 시작");
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

		//		if (!strncmp(buf, "end of file", 10)) { //마지막 메시지가 end of file이면 종료
		//			//임시 파일 내용 -> 파일 내용 + 파일 권한 수정

		//			if (sendto(sd, "end of file", strlen("end of file") + 1, 0, (struct sockaddr *)&sin, sizeof(sin)) == -1){
		//				perror("sendto end of file");
		//				exit(1);
		//			}

		//			char checkBuffer_1[256];
		//			char checkBuffer_2[256];
		//			memset(checkBuffer_1, 0, sizeof(checkBuffer_1));
		//			memset(checkBuffer_2, 0, sizeof(checkBuffer_2));

		//			while (fgets(checkBuffer_1, sizeof(checkBuffer_1), fd) != NULL){//다시 받아온 파일 데이터
		//				fgets(checkBuffer_2, sizeof(checkBuffer_2), fd);//원본 파일 데이터
		//				if (!strcmp(checkBuffer_1, checkBuffer_2)){
		//					perror("file check fail");
		//					fclose(fd);
		//					fclose(o_fd);
		//					exit(1);
		//				}
		//			}


		//			//100% 전송

		//			if (sendto(sd, "100%%", strlen("100%%") + 1, 0, (struct sockaddr *)&sin, sizeof(sin)) == -1){
		//				perror("sendto 100%%");
		//				exit(1);
		//			}
		//			fclose(fd);
		//			fclose(o_fd);
		//			break; //while문 빠져나가기
		//		} else {
		//			printf("Ingridty : %d byte \n", bytes_read);

		//			fprintf(fd, "%s", buf);
		//		}
		//	}
		//	//임시 파일 지우기
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
     struct timeval start_point, end_point;


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
	//	printf("whiel들어가기전에");
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

		// 정현 - 파일내용 받는곳 같음
		gettimeofday(&start_point, NULL);
		while (1) {
			memset(buf, 0, SIZEBUF);
			//			printf("2while\n");
			int bytes_read = 0;
			bytes_read = recv(ns, buf, SIZEBUF, MSG_WAITALL);
			//			printf("finalerecv밑이다 = %s\n", finalFile);
			printf("RECV : %d byte\n", bytes_read);
			//            printf("buf = %s\n",buf);
			//			printf("3while\n");
			if (bytes_read == -1) {
				perror("recv date");
				exit(1);
			}
			buf[bytes_read] = '\0';
			//			printf("finaleFIle2 = %s\n", finalFile);
			if (!strncmp(buf, "end of file", 12)) { //마지막 메시지가 end of file이면 종료
				gettimeofday(&end_point, NULL);

					double timer = timer_cal(start_point.tv_sec,start_point.tv_usec,end_point.tv_sec,end_point.tv_usec);
					printf("%f Byte",File_transfer_speed(timer));
				printf("file close\n");
				fclose(fd);

				char writeBuffer[SIZEBUF];
				printf("%s finaleFILE 경로 \n", finalFile);
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

					printf("접근권한 변경에 실패 했습니다. 파일의 접근 권한을 확인해 주세요.");
				//printf("file close\n");
				fclose(o_fd);
				fclose(fd); //stream 닫기

				//
				//파일 끝 받았다고 전송

				if (send(ns, "end of file", SIZEBUF, 0) == -1){
					perror("send end of file");
					exit(1);
				}
				break; //while문 빠져나가기
			} else {
				//				printf("%d byte recv\n", bytes_read);
				//			    fputs(buf, stream); //파일로 저장
				fwrite(buf, sizeof(char), SIZEBUF, fd);
			}
		}
		//무결성 체크
		printf("무결성 체크\n");
		fd = fopen("temp2.dat", "w");//fd2 다시 연다
		o_fd = fopen(finalFile, "r");
		while (1) {

			memset(buf, 0, SIZEBUF);
			//			printf("무결성2while\n");
			int bytes_read = recv(ns, buf, SIZEBUF, MSG_WAITALL);
			//			printf("무결성3while\n");
			if (bytes_read == -1) {
				perror("recv date");
				exit(1);
			}
			buf[bytes_read] = '\0';

			if (strncmp(buf, "end of file", 12) == 0) { //마지막 메시지가 end of file이면 종료
				if (send(ns, "end of file", SIZEBUF, 0) == -1){
					perror("send end of file");
					exit(1);
				}

				char checkBuffer_1[SIZEBUF];
				char checkBuffer_2[SIZEBUF];
				memset(checkBuffer_1, 0, SIZEBUF);
				memset(checkBuffer_2, 0, SIZEBUF);

				while (fgets(checkBuffer_1, SIZEBUF, fd) != NULL){//다시 받아온 파일 데이터
					fgets(checkBuffer_2, SIZEBUF, o_fd);//원본 파일 데이터
					if (!strcmp(checkBuffer_1, checkBuffer_2)){
						perror("file check fail");
						exit(1);
					}
				}
				fclose(fd);
				fclose(o_fd);
				//	printf("send전입니다\n");
				if (send(ns, "100percent", SIZEBUF, 0) == -1){
					perror("send 100");
					exit(1);
				}

				//	printf("send후\n");
				break;
			} else {
				printf("Ingridty : %d byte \n", bytes_read);
				//			    fputs(buf, stream); //파일로 저장
				fwrite(buf, sizeof(char), SIZEBUF, fd);
			}
		}
		int removeTempFile = remove("./temp.dat");
		if (removeTempFile == -1) printf("remove fail");
		removeTempFile = remove("./temp2.dat");
		if (removeTempFile == -1) printf("remove fail");
		//printf("close 전이다\n");
		close(ns);
		//close(sd);
	}


}
