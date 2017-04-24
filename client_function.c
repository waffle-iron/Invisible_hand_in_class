#include "library.h"

void UdpClient(int argc, char** argv){

	int sd, fd, n;
	char buf[256];
	char end_buf[50];
	char re_buf[50];


	const char* filename = argv[2]; //���� �̸�
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
	sin.sin_addr.s_addr = inet_addr(argv[1]);//������ �ּ�
	sin.sin_port = htons(PORTNUM);

	//file open
	fd = open(filename, O_RDONLY);
	if (fd == -1){
		perror("file open fail");
		exit(1);
	}

	// ���� �̸� ����
	if (sendto(sd, filename, strlen(filename) + 1, 0, (struct sockaddr*)&sin, sizeof(sin)) == -1){
		perror("sendto filename");
		exit(1);
	}

	//�����̸� �ޱ�
	//�������� ������ ������ Ŭ���̾�Ʈ���� �ٽ� ���� ���� ������
	//buf�� ����Ÿ�� �о���̰� �ȴ�
	int bytes_read = (recvfrom(sd, buf, 255, 0, (struct sockaddr *)&sin, &add_len));

	if (bytes_read == -1) {
		perror("recvfrom filename");
		exit(1);
	}

	//���� ���� �����̶� �̸��� �ٸ����
	if (strcmp(filename, buf) != 0) { //buf�� ��
		printf("%s\n", buf);
		perror("not match filename.");
		exit(1);
	} else printf("** match filename : %s\n", buf);// �����̸� �ް����


	//file ������ ����
	while ((n = read(fd, buf, 255)) > 0){ //fd�� �ִ°� buf�� ����

		printf("SEND : %d\n", n);

		if (sendto(sd, buf, n, 0, (struct sockaddr *)&sin, sizeof(sin)) == -1) {
			perror("sendto");
			exit(1);
		}
	}

	memset(buf, 0, sizeof(buf));

	//sprintf(buf, "end of file"); //buf���ٰ� end of file�� ���̴� ��
	//end of file �� ����
	if (sendto(sd, "end of file", 12, 0, (struct sockaddr*)&sin, sizeof(sin)) == -1){
		perror("sendto filename");
		exit(1);
	}


	// end of file Ȯ��
	bytes_read = (recvfrom(sd, end_buf, 12, 0, (struct sockaddr *)&sin, &add_len));
	if (bytes_read == -1) {
		perror("recvfrom end of file");
		exit(1);
	}

	if (strcmp("end of file", end_buf) == 0) { //buf�� ��
		printf("%s\n", end_buf);
	} else{
		perror("error : file is not end");
		exit(1);
	}

	///////////////////////////////////////////////////////////
	//////// ���Ἲ üũ/////////////////////////////////////////////
	printf("check whether your file is correct send\n");
	sleep(2);
	//scanf("file : %s", &filename);
	close(fd);
	//file open
	int fd1;
	fd1 = open(filename, O_RDONLY);
	if (fd1 == -1){
		perror("file open fail");
		exit(1);
	}

	//file ������ �ٽ� ����
	while ((n = read(fd1, buf, 255)) > 0){ //fd�� �ִ°� buf�� ����

		printf("RESEND : %d\n", n);

		if (sendto(sd, buf, n, 0, (struct sockaddr *)&sin, sizeof(sin)) == -1) {
			perror("resendto");
			exit(1);
		}
	}
	if (sendto(sd, "end of file", 12, 0, (struct sockaddr*)&sin, sizeof(sin)) == -1){
		perror("sendto filename");
		exit(1);
	}


	// end of file Ȯ��
	bytes_read = (recvfrom(sd, end_buf, 12, 0, (struct sockaddr *)&sin, &add_len));
	if (bytes_read == -1) {
		perror("recvfrom end of file");
		exit(1);
	}

	if (strcmp("end of file", end_buf) == 0) { //buf�� ��
		printf("%s\n", end_buf);
	} else{
		perror("error : file is not end");
		exit(1);
	}

	//���� ��ġ ����ġ �޼��� �� �޴´�.
	bytes_read = (recvfrom(sd, re_buf, 20, 0, (struct sockaddr *)&sin, &add_len));

	//���� ����ġ��� ������ �ݴ´�
	if (strcmp("100%%", re_buf) == 0) { //buf�� ��
		printf("%s same file\n", re_buf);
	} else{
		perror("error : file is not same");
		exit(1);
	}

}

void TcpClient(int argc, char** argv){

	int sd, fd, n;
	char buf[SIZEBUF];
	char end_buf[SIZEBUF];
	char percent[SIZEBUF + 1];
	struct sockaddr_in sin;
	const char* filename = argv[2]; //���� �̸�
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

	// ���� �̸� ����
	if (send(sd, filename, SIZEBUF, 0) == -1){
		perror("send filename");
		exit(1);
	}


	//���� �� �ٽ� �ޱ�
	int bytes_read = (recv(sd, buf, SIZEBUF, MSG_WAITALL));

	if (bytes_read < 0) {
		perror("recvfrom filename");
		exit(1);
	}

	//���� ���� �����̶� �̸��� �ٸ����
	if (strcmp(filename, buf) != 0) { //buf�� ��
		printf("%s\n", buf);
		perror("not match filename.");
		exit(1);
	} else printf("** match filename : %s\n", buf);// �����̸� �ް����


	//file ������ ����
	while ((n = read(fd, buf, SIZEBUF)) > 0){ //fd�� �ִ°� buf�� ����

		printf("SEND : %d\n", n);
		int a = send(sd, buf, SIZEBUF, 0);
		if (a == -1) {
			perror("sendto");
			exit(1);
		}
		//printf("buf = %s\n",buf);
		memset(buf, 0, 256);
	}


	memset(buf, 0, SIZEBUF);
	sprintf(buf, "end of file");
	//end of file �� ����
	if (send(sd, buf, SIZEBUF, 0) == -1){
		perror("send filena?me");
		exit(1);
	}


	// end of file Ȯ��
	bytes_read = (recv(sd, end_buf, SIZEBUF, MSG_WAITALL));

	if (bytes_read == -1) {
		perror("recv end of file");
		exit(1);
	}

	if (strcmp("end of file", end_buf) == 0) { //buf�� ��
		printf("%s\n", end_buf);
	} else{
		perror("error : file is not end");
		exit(1);
	}

	close(fd);

	///////////////////////////////////////////////////////////
	//////// ���Ἲ üũ/////////////////////////////////////////////
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

	//file ������ �ٽ� ����
	while ((n = read(fd1, buf, SIZEBUF)) > 0){ //fd�� �ִ°� buf�� ����

		printf("RESEND : %d\n", n);

		if (send(sd, buf, SIZEBUF, 0) == -1) {
			perror("resendto");
			exit(1);
		}
		memset(buf, 0, SIZEBUF);
	}
	memset(buf, 0, SIZEBUF);
	sprintf(buf, "end of file");
	//���ϳ��̶�� ����
	if (send(sd, buf, SIZEBUF, 0) == -1){
		perror("send filena?me");
		exit(1);
	}
	// end of file Ȯ��
	bytes_read = (recv(sd, end_buf, SIZEBUF, 0));

	if (bytes_read == -1) {
		perror("recv end of file");
		exit(1);
	}

	if (strcmp("end of file", end_buf) == 0) { //buf�� ��
		//printf("������̺�� : %s\n", end_buf);
	} else{
		perror("error : file is not end");
		exit(1);
	}
	//���� �ӳ�
	//���� ��ġ ����ġ �޼��� �� �޴´�.
	memset(percent, 0, SIZEBUF + 1);
	bytes_read = recv(sd, percent, SIZEBUF, 0);
	//percent[bytes_read] = '\0';
	//printf("����ġ ������: %d\n", bytes_read);
	//printf("%s �ۼ�Ʈ �� �������� ���\n", percent);
	//���� ����ġ��� ������ �ݴ´�
	if (strcmp("100percent", percent) == 0) { //buf�� ��
		printf("%s same file\n", percent);
	} else{
		printf("error : file is not same\n");
		exit(1);
	}


}