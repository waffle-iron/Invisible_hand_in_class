#include "library.h"

int compare_file_name(const void* f_a, const void* f_b){

	int result = 0;
	char* temp1 = (char*)(((file_information*)f_a)->dent.d_name);
	char* temp2 = (char*)(((file_information*)f_b)->dent.d_name);

	return result;
}
void UdpFileTrans(int sd, struct sockaddr_in sin, socklen_t add_len, char* file_name){

	int fd, n;
	int bytes_read;
	char buf[SIZEBUF];
	char temp_file_name[SIZEBUF];
	// �����̶��� ���� �������� �˸�

	if (sendto(sd, "This is File", SIZEBUF, 0, (struct sockaddr*)&sin, sizeof(sin)) == -1){
		perror("sendto filename");
		exit(1);
	}

	// �����̶��� ���� ������ �ν��ϰ� ���� �޽���
	if ((bytes_read = recvfrom(sd, buf, SIZEBUF, 0, (struct sockaddr *)&sin, &add_len)) == -1){
		perror("recvfrom isfile");
		exit(1);
	}
	printf("3\n");

	memset(buf, 0, SIZEBUF);
	sprintf(temp_file_name, "./save/%s", file_name);
	// ���� �̸� ����

	if (sendto(sd, temp_file_name, SIZEBUF, 0, (struct sockaddr*)&sin, sizeof(sin)) == -1){
		perror("sendto filename");
		exit(1);
	}


	//�����̸� �ޱ�
	//�������� ������ ������ Ŭ���̾�Ʈ���� �ٽ� ���� ���� ������
	//buf�� ����Ÿ�� �о����̰� �ȴ�
	if ((bytes_read = recvfrom(sd, buf, SIZEBUF, 0, (struct sockaddr *)&sin, &add_len)) == -1){
		perror("recvfrom filename");
		exit(1);
	}
	printf("%s\n", buf);


	//���� ���� �����̶� �̸��� �ٸ�����
	//if (strcmp(filename, buf) != 0) { //buf�� ����
	//	printf("%s\n", buf);
	//	perror("not match filename.");
	//	exit(1);
	//} else {
	//	// �����̸� �ް�����
	//	printf("** match filename : %s\n", buf);
	//}
	// ���Ͽ���
	if ((fd = open(file_name, O_RDONLY)) == -1){
		perror("file open fail");
		exit(1);
	}

	//file ������ ����
	while ((n = read(fd, buf, SIZEBUF)) > 0){ //fd�� �ִ°� buf�� ����

		printf("SEND : %d\n", n);

		if (sendto(sd, buf, n, 0, (struct sockaddr *)&sin, sizeof(sin)) == -1) {
			perror("sendto");
			exit(1);
		}
	}

	memset(buf, 0, SIZEBUF);

	//sprintf(buf, "end of file"); //buf���ٰ� end of file�� ���̴� ��
	//end of file �� ����
	if (sendto(sd, "end of file", SIZEBUF, 0, (struct sockaddr*)&sin, sizeof(sin)) == -1){
		perror("sendto filename");
		exit(1);
	}


	// end of file Ȯ��
	if ((bytes_read = recvfrom(sd, buf, SIZEBUF, 0, (struct sockaddr *)&sin, &add_len)) == -1){
		perror("recvfrom end of file");
		exit(1);
	}

	if (strcmp("end of file", buf) == 0) { //buf�� ����
		printf("%s\n", buf);
	} else{
		perror("error : file is not end");
		exit(1);
	}
}

void UdpDirTrans(int sd, struct sockaddr_in sin, socklen_t add_len, char* Dir_name){
	DIR *dp;
	struct dirent *dent;
	//index : �����丮�� ���Ϲ� �����丮����
	int index = 0;
	int bytes_read;
	int	files_number = 2000;
	char buf[SIZEBUF];
	char temp_dirent_name[2000] = { 0, };
	char temp_Dir_name[SIZEBUF];
	char* cwd;
	struct stat sbuf;

	// �����丮���� ���� �������� �˸�
	if (sendto(sd, "This is DIR", SIZEBUF, 0, (struct sockaddr*)&sin, sizeof(sin)) == -1){
		perror("sendto DIR");
		exit(1);
	}
	// �����丮�̶��� ���� ������ �ν��ϰ� ���� �޽���
	if ((bytes_read = recvfrom(sd, buf, SIZEBUF, 0, (struct sockaddr *)&sin, &add_len)) == -1){
		perror("recvfrom isDIR");
		exit(1);
	}
	printf("%s\n",Dir_name);

	memset(buf, 0, SIZEBUF);

	// �����丮 �̸� ����
	/*if (sendto(sd, Dir_name, SIZEBUF, 0, (struct sockaddr*)&sin, sizeof(sin)) == -1){
		perror("sendto DIRname");
		exit(1);
	}*/
	sprintf(temp_Dir_name, "./save/%s", Dir_name);
	printf("여기요 %s\n",temp_Dir_name);
	if (sendto(sd, temp_Dir_name, SIZEBUF, 0, (struct sockaddr*)&sin, sizeof(sin)) == -1){
		perror("sendto DDDename");
		exit(1);
	}

	// �����丮�̸� �ޱ�
	//�������� ������ ������ Ŭ���̾�Ʈ���� �ٽ� ���� ���� ������
	//buf�� ����Ÿ�� �о����̰� �ȴ�

	if ((bytes_read = recvfrom(sd, buf, SIZEBUF, 0, (struct sockaddr *)&sin, &add_len)) == -1){
		perror("recvfrom DIRname");
		exit(1);
	}
printf("%s  ss  ",buf);
	if ((dp = opendir(Dir_name)) == NULL) {
		perror("opendir");
		exit(1);
	}
	memset(buf, 0, SIZEBUF);
	file_information* files = (file_information*)malloc(sizeof(file_information) * files_number);
	//�����鼭 files �迭�� ����
	while ((dent = readdir(dp))){
		if (index >= files_number - 2){
			files_number *= 2;
			files = (file_information*)realloc((void*)files, sizeof(file_information) * files_number);
		}
		// �̸������� .�ϰ��� ��������
		if (strcmp(dent->d_name, ".") == 0){
			continue;
		}
		if (strcmp(dent->d_name, "..") == 0){
			continue;
		}
		//������ files�迭�� ����
		sprintf(temp_Dir_name, "%s/%s",Dir_name ,dent->d_name);
		if( stat(temp_Dir_name, &sbuf) ==-1 ){
			perror("stat");
			exit;
		}
		files[index].dent = *dent;
		//�����丮 �ΰ���
		//printf("sbuf.st_mode = %x",sbuf.st_mode);

		if (S_ISDIR(sbuf.st_mode)){
			printf("d\n");
			files[index].or_file_dir = 'd';
		} else if (S_ISREG(sbuf.st_mode)){
			//������ ����
			printf("f\n");
			files[index].or_file_dir = 'f';
		}
		index++;
	}
	printf("he\n");
	qsort(files, index, sizeof(file_information), compare_file_name);

	int i = 0;
	for (i = 0; i < index; ++i){
		printf("temp = %s\n",temp_Dir_name);
		sprintf(temp_Dir_name, "%s/%s",Dir_name ,files[i].dent.d_name);
		if (files[i].or_file_dir == 'f'){
			UdpFileTrans(sd, sin, add_len, temp_Dir_name);
		} else if (files[i].or_file_dir == 'd'){
			char temp[2] = { '/', '\0' };
			//�����丮 ���� �����ϱ� ���� ������ ����
			sprintf(temp_dirent_name, "%s%s%s", Dir_name, temp, files[i].dent.d_name);
			//Ȱ�� �����丮 ����

			cwd = getcwd(NULL, SIZEBUF);
			//���� �����丮�� �̵��� ����
			chdir(files[i].dent.d_name);
			UdpDirTrans(sd, sin, add_len, temp_dirent_name);
			//�ٽ� Ȱ�� �����丮�� ����
			chdir(cwd);
		}
	}

}

//ù��° �������α׷� / IP / ������ �����丮 or ����
void UdpClient(int argc, char** argv, int sd,	struct sockaddr_in sin){

	// , fd, n;
		struct stat buf;
		//char end_buf[SIZEBUF];
		//char re_buf[SIZEBUF];

		socklen_t add_len = sizeof(struct sockaddr);


		stat(argv[2], &buf);
		// �����丮 �ΰ���
		if (S_ISDIR(buf.st_mode)){
			printf("�����丮�Դϴ�.\n");
			UdpDirTrans(sd, sin, add_len, argv[2]);
		} else if (S_ISREG(buf.st_mode)){
			//�Է��Ѱ� ������ ����
			printf("�����Դϴ�.\n");
		UdpFileTrans(sd, sin, add_len, argv[2]);
	} else{
		//���� �� �����丮�� �ƴ� ���� ��������
		printf("myls: cannot access %s: No such file or directory\n", argv[2]);
		exit(1);
	}



	///////////////////////////////////////////////////////////
	//////// ���Ἲ üũ/////////////////////////////////////////////
	//printf("check whether your file is correct send\n");
	//sleep(2);
	////scanf("file : %s", &filename);
	//close(fd);
	////file open
	//int fd1;
	//fd1 = open(filename, O_RDONLY);
	//if (fd1 == -1){
	//	perror("file open fail");
	//	exit(1);
	//}

	////file ������ �ٽ� ����
	//while ((n = read(fd1, buf, 255)) > 0){ //fd�� �ִ°� buf�� ����

	//	printf("RESEND : %d\n", n);

	//	if (sendto(sd, buf, n, 0, (struct sockaddr *)&sin, sizeof(sin)) == -1) {
	//		perror("resendto");
	//		exit(1);
	//	}
	//}
	//if (sendto(sd, "end of file", 12, 0, (struct sockaddr*)&sin, sizeof(sin)) == -1){
	//	perror("sendto filename");
	//	exit(1);
	//}


	//// end of file Ȯ��
	//bytes_read = (recvfrom(sd, end_buf, 12, 0, (struct sockaddr *)&sin, &add_len));
	//if (bytes_read == -1) {
	//	perror("recvfrom end of file");
	//	exit(1);
	//}

	//if (strcmp("end of file", end_buf) == 0) { //buf�� ����
	//	printf("%s\n", end_buf);
	//} else{
	//	perror("error : file is not end");
	//	exit(1);
	//}

	////���� ��ġ ����ġ �޼��� �� �޴´�.
	//bytes_read = (recvfrom(sd, re_buf, 20, 0, (struct sockaddr *)&sin, &add_len));

	////���� ����ġ���� ������ �ݴ´�
	//if (strcmp("100%%", re_buf) == 0) { //buf�� ����
	//	printf("%s same file\n", re_buf);
	//} else{
	//	perror("error : file is not same");
	//	exit(1);
	//}
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
	// �ʱ�ȭ
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

	//���� ���� �����̶� �̸��� �ٸ�����
	if (strcmp(filename, buf) != 0) { //buf�� ����
		printf("%s\n", buf);
		perror("not match filename.");
		exit(1);
	} else printf("** match filename : %s\n", buf);// �����̸� �ް�����


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

	if (strcmp("end of file", end_buf) == 0) { //buf�� ����
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
	//���ϳ��̶��� ����
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

	if (strcmp("end of file", end_buf) == 0) { //buf�� ����
		//printf("�������̺��� : %s\n", end_buf);
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
	//printf("%s �ۼ�Ʈ �� �������� ����\n", percent);
	//���� ����ġ���� ������ �ݴ´�
	if (strcmp("100percent", percent) == 0) { //buf�� ����
		printf("%s same file\n", percent);
	} else{
		printf("error : file is not same\n");
		exit(1);
	}


}
