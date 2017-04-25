#include "library.h"

int compare_file_name(const void* f_a, const void* f_b){

	int result = 0;
	char* temp1 = (char*)(((file_information*)f_a)->dent.d_name);
	char* temp2 = (char*)(((file_information*)f_b)->dent.d_name);

	return result;
}

// UDP 파일 전송하는 함수
void UdpFileTrans(int sd, struct sockaddr_in sin, socklen_t add_len, char* file_name){

	int fd, n;
	int bytes_read;
	char buf[SIZEBUF];
	char temp_file_name[SIZEBUF];

	// 파일이라고 서버에게 알려줌
	if (sendto(sd, "This is File", SIZEBUF, 0, (struct sockaddr*)&sin, sizeof(sin)) == -1){
		perror("sendto filename");
		exit(1);
	}

	// 파일에 대한 답장 
	if ((bytes_read = recvfrom(sd, buf, SIZEBUF, 0, (struct sockaddr *)&sin, &add_len)) == -1){
		perror("recvfrom isfile");
		exit(1);
	}
	
	memset(buf, 0, SIZEBUF);
		
	// 저장해야되는 파일 경로 전송
	sprintf(temp_file_name, "./save/%s", file_name);
	if (sendto(sd, temp_file_name, SIZEBUF, 0, (struct sockaddr*)&sin, sizeof(sin)) == -1){
		perror("sendto filename");
		exit(1);
	}

	// 저장해야되는 파일 경로전송에 대한 답장받음
	if ((bytes_read = recvfrom(sd, buf, SIZEBUF, 0, (struct sockaddr *)&sin, &add_len)) == -1){
		perror("recvfrom filename");
		exit(1);
	}

	//파일 열기
	if ((fd = open(file_name, O_RDONLY)) == -1){
		perror("file open fail");
		exit(1);
	}

	// 파일 내용을 전송
	while ((n = read(fd, buf, SIZEBUF)) > 0){ 

		printf("SEND FILE CONTENTS SIZE: %d\n", n);
		
		if (sendto(sd, buf, n, 0, (struct sockaddr *)&sin, sizeof(sin)) == -1) {
			perror("sendto");
			exit(1);
		}
	}

	memset(buf, 0, SIZEBUF);

	// 파일전송이 끝났다고 알려줌.
	if (sendto(sd, "end of file", SIZEBUF, 0, (struct sockaddr*)&sin, sizeof(sin)) == -1){
		perror("sendto filename");
		exit(1);
	}

	// 파일전송에 끝났다는 답장 받음
	if ((bytes_read = recvfrom(sd, buf, SIZEBUF, 0, (struct sockaddr *)&sin, &add_len)) == -1){
		perror("recvfrom end of file");
		exit(1);
	}
	
	// 파일전송 끝을 알리는 부분
	if (strcmp("end of file", buf) == 0) {
		printf("END OF TRANSFER FILE\n");
	} else{
		printf("END OF TRANSFER FILE ERROR\n");
		exit(1);
	}
}

// UDP 디렉토리 전송 하는 함수
void UdpDirTrans(int sd, struct sockaddr_in sin, socklen_t add_len, char* dir_name){

	int index = 0;
	int bytes_read = 0;
	int i = 0;
	int files_size = 1024;

	char buf[SIZEBUF];
	char temp_dir_name[SIZEBUF];
	char* cwd;
	DIR *dp;
	struct dirent *dent;
	struct stat sbuf;

	// 디렉토리라고 처음에 서버에게 알려줌.
	if (sendto(sd, "This is DIR", SIZEBUF, 0, (struct sockaddr*)&sin, sizeof(sin)) == -1){
		perror("sendto DIR");
		exit(1);
	}
	
	//서버가 알겠다고 응답.
	if ((bytes_read = recvfrom(sd, buf, SIZEBUF, 0, (struct sockaddr *)&sin, &add_len)) == -1){
		perror("recvfrom isDIR");
		exit(1);
	}
	
	printf("현재 디렉토리 이름 = %s\n", dir_name);

	memset(buf, 0, SIZEBUF);

	//서버는 save 디렉토리 밑에 저장하기 때문에 위치를 이렇게 전송함.
	sprintf(temp_dir_name, "./save/%s", dir_name);
	printf("경로 = %s\n", temp_dir_name);
	
	// 서버에게 디렉토리 이름 전송
	if (sendto(sd, temp_dir_name, SIZEBUF, 0, (struct sockaddr*)&sin, sizeof(sin)) == -1){
		perror("dir name send error");
		exit(1);
	}

	// 보낸디렉토리 이름에 대한 응답 받음
	if ((bytes_read = recvfrom(sd, buf, SIZEBUF, 0, (struct sockaddr *)&sin, &add_len)) == -1){
		perror("recvfrom DIRname");
		exit(1);
	}
	
	// 해당 디렉토리탐색을 위해 디렉토리 오픈
	if ((dp = opendir(dir_name)) == NULL) {
		perror("opendir");
		exit(1);
	}

	memset(buf, 0, SIZEBUF);
	// 해당 디렉토리 안에 있는 파일 및 디렉토리 담는 함수
	file_information* files = (file_information*)malloc(sizeof(file_information) * files_size);
	
	// 디렉토리 내부를 돌면서 file_information에 저장
	while ((dent = readdir(dp))){
		if (index >= files_size - 2){
			files_size *= 2;
			files = (file_information*)realloc((void*)files, sizeof(file_information) * files_size);
		}
		
		// . 과 ..은 자동으로 생성되므로 패스
		if (strcmp(dent->d_name, ".") == 0){
			continue;
		}
		if (strcmp(dent->d_name, "..") == 0){
			continue;
		}

		// 여기는 sava밑이 아닌 자기가 입력한 디렉토리 밑에서 돌아야되므로
		sprintf(temp_dir_name, "%s/%s", dir_name, dent->d_name);
		if (stat(temp_dir_name, &sbuf) == -1){
			perror("stat");
			exit(1);
		}
		files[index].dent = *dent;
 
		// 디렉토리 인경우
		if (S_ISDIR(sbuf.st_mode)){
			files[index].or_file_dir = 'd';
		} else if (S_ISREG(sbuf.st_mode)){
			//파일 인경우
			files[index].or_file_dir = 'f';
		}
		index++;
	}
	
	// 이름순으로 보기 편하게 하기위해서 하였음 
	// 사실상 할 필요 없음 
	qsort(files, index, sizeof(file_information), compare_file_name);

	// files에 들어가있는 정보를 재귀적으로 돌면서 
	// 파일인 경우 파일 전송하는 부분으로 보내고
	// 디렉토리인 경우는 한번 재귀적으로 돌면서 한다.
	for (i = 0; i < index; ++i){
		printf("파일 순회 = %s\n", temp_dir_name);
		
		sprintf(temp_dir_name, "%s/%s", dir_name, files[i].dent.d_name);
		// 파일인경우 파일출력함수 호출
		if (files[i].or_file_dir == 'f'){
			UdpFileTrans(sd, sin, add_len, temp_dir_name);
		} else if (files[i].or_file_dir == 'd'){
			//디렉토리 인경우 
			
			//현재 경로 저장
			cwd = getcwd(NULL, SIZEBUF);
			//디렉토리 이동
			chdir(files[i].dent.d_name);
			//디렉토리 함수 이동
			UdpDirTrans(sd, sin, add_len, temp_dir_name);
			//원래 저장경로로 돌아옴
			chdir(cwd);
		}
	}

}

// UDP 클라이언트 부분
void UdpClient(int argc, char** argv, int sd, struct sockaddr_in sin){

	struct stat buf;
    struct timeval start_point,end_point;

	socklen_t add_len = sizeof(struct sockaddr);
	stat(argv[2], &buf);
    gettimeofday(&start_point, NULL);

	// is directory
	if (S_ISDIR(buf.st_mode)){
		printf("THIS IS DIRECTORY.\n");
		UdpDirTrans(sd, sin, add_len, argv[2]);
	} else if (S_ISREG(buf.st_mode)){
		// is file
		printf("THIS IS FILE.\n");
		UdpFileTrans(sd, sin, add_len, argv[2]);
	} else{
		// ERROR
		printf("access %s: No such file or directory\n", argv[2]);
		exit(1);
	}
    gettimeofday(&start_point, NULL);
    
    double File_Transfer_Timer(start_point.tv_sec, start_point.tv_usec,end_point.tv_sec,end_point.tv_usec);

	// 무결성
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

long long Folder_Size(char *Dir_name,long long total_size){
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
            total_size += File_Size(temp_Dir_name);//
        } else if (files[i].or_file_dir == 'd'){
        char temp[2] = { '/', '\0' };
        sprintf(temp_dirent_name, "%s%s%s", Dir_name, temp, files[i].dent.d_name);
        cwd = getcwd(NULL, SIZEBUF);
        chdir(files[i].dent.d_name);
        total_size += Folder_Size(temp_dirent_name,total_size);
        chdir(cwd);
        
        }
    }
    return total_size;
}

long long File_Size(char * file_name){
    struct stat sbuf;
    
    stat(file_name, &sbuf);
    long long fsize = (long long)sbuf.st_size;
    return fsize;
}

double File_Transfer_Speed(double total_time){
    long long total_size=0;
    char root_folder[SIZEBUF] = "./save";
    total_size += Folder_Size(root_folder,total_size);

    return (double)total_size/total_time;
}


double File_Transfer_Timer(long start_tv_sec,long start_tv_usec,long end_tv_sec,long end_tv_usec){

	return (double)(end_tv_sec)+(double)(end_tv_usec)/1000000.0-(double)(start_tv_sec)-(double)(start_tv_usec)/1000000.0;
}

/*
			gettimeofday(&start_point, NULL);

					gettimeofday(&end_point, NULL);


					double timer = timer_cal(start_point.tv_sec,start_point.tv_usec,end_point.tv_sec,end_point.tv_usec);
		gettimeofday(&start_point, NULL);
				gettimeofday(&end_point, NULL);

					double timer = timer_cal(start_point.tv_sec,start_point.tv_usec,end_point.tv_sec,end_point.tv_usec);					printf("%f Byte",File_transfer_speed(timer));
*/
