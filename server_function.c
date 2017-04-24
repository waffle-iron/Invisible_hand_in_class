#include "library.h"

void UdpServer(int sd, struct sockaddr_in sin){

	char buf[SIZEBUF];

	socklen_t  clientlen = sizeof(sin);

	//������ ���� ����
	mkdir("save", 0744);

	//
	while (1) {
		printf("inwhile~~\n");

		int bytes_read= 0;
		// �����丮���� �������� �޴� �κ�

		if ((bytes_read = recvfrom(sd, buf, SIZEBUF, 0, (struct sockaddr *)&sin, &clientlen)) == -1){
			perror("recvfrom isDIR");
			exit(1);
		}
		printf("recvfrom %s\n", buf);

		// �������� �ƴ��� �˾Ҵٴ� �޽��� ����
		if (sendto(sd, "SUCCUSS", SIZEBUF, 0, (struct sockaddr*)&sin, sizeof(sin)) == -1){
			perror("�������� �ƴ��� �˾Ҵٴ� �޼���");
			exit(1);
		}


		//���� �ΰ���
		if (strcmp("This is File", buf) == 0){
			int fd;
			//���� �̸� �ޱ�
			if ((bytes_read = recvfrom(sd, buf, SIZEBUF, 0, (struct sockaddr *)&sin, &clientlen)) == -1){
				perror("recvfrom filename");
				exit(1);
			}
			// �����̸� �ް� ����
			printf("** From Client : %s\n", buf);

			//�����̸� ������
			if (sendto(sd, buf, SIZEBUF, 0, (struct sockaddr*)&sin, sizeof(sin)) == -1){
				perror("sendto filename");
				exit(1);
			}
			//�׸��� ���� �ޱ�
			// ���Ͽ���
			if ((fd = open(buf, O_WRONLY | O_CREAT, 0666)) == -1){
				perror("file open fail");
				exit(1);
			}
			//file ������ ����
			while (1) {

				memset(buf, 0, sizeof(buf));

				if ((bytes_read = recvfrom(sd, buf, SIZEBUF, 0, (struct sockaddr *)&sin, &clientlen)) == -1){
					perror("recvfrom end of file");
					exit(1);
				}
				printf("RECV : %d byte\n", bytes_read);
				printf("buf %s = ",buf);
				//buf[bytes_read] = '\0';

				if (!strncmp(buf, "end of file", SIZEBUF)) { //������ �޽����� end of file�̸� ����
					//���� �� �޾Ҵٰ� ����
					if (sendto(sd, "end of file", SIZEBUF, 0, (struct sockaddr *)&sin, sizeof(sin)) == -1){
						perror("sendto end of file");
						exit(1);
					}

					close(fd);
					printf("file close\n");
					break; //while�� ����������
				} else {
					//			    fputs(buf, stream); //���Ϸ� ����
					write(fd, buf, SIZEBUF);
				}
			}
		} else if (strcmp("This is DIR", buf) == 0){
			//�����丮 �� ���� �����丮 ������ �׸��� �� �����丮�� �̵�

			//�����丮 �̸� �ޱ�
			if ((bytes_read = recvfrom(sd, buf, SIZEBUF, 0, (struct sockaddr *)&sin, &clientlen)) == -1){
				perror("recvfrom dirname");
				exit(1);
			}

			printf("ser@\n");
			// �����丮 �̸� ����
			if (sendto(sd, buf, SIZEBUF, 0, (struct sockaddr*)&sin, sizeof(sin)) == -1){
				perror("sendto dirname");
				exit(1);
			}

			printf("버퍼입니당아ㅏ아  = %s\n",buf);
			mkdir(buf, 0744);
			//		chdir(buf);
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


}
