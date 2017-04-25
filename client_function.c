#include "library.h"

int fileCount = 1;
int count_Dir = 0;
int count_File = 0;

int compare_file_name(const void* f_a, const void* f_b){

	int result = 0;
	char* temp1 = (char*)(((file_information*)f_a)->dent.d_name);
	char* temp2 = (char*)(((file_information*)f_b)->dent.d_name);

	return result;
}

int GetCountFile(){
	return count_File;
}
int GetCountDir(){
	return count_Dir;
}
int GetFileCount(){
	return fileCount;
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
	
	if ((dp = opendir(name)) == NULL){
		perror("opendir");
	};
	char temp_dir_name[SIZEBUF];
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
		fileCount++;

		if (S_ISDIR(buf.st_mode)){
			CountFile(temp_dir_name);
			count_Dir++;
		}
	}
}
// UDP 클라이언트 부분
void UdpClient(int argc, char** argv, int sd, struct sockaddr_in sin){
	char buf[SIZEBUF];
	struct stat sbuf;
	struct timeval start_point, end_point;
	int bytes_read;	

	socklen_t add_len = sizeof(struct sockaddr);
	stat(argv[2], &sbuf);
	
	gettimeofday(&start_point, NULL);

	// is directory
	if (S_ISDIR(sbuf.st_mode)){
		printf("THIS IS DIRECTORY.\n");
		UdpDirTrans(sd, sin, add_len, argv[2]);
	} else if (S_ISREG(sbuf.st_mode)){
		// is file
		printf("THIS IS FILE.\n");
		UdpFileTrans(sd, sin, add_len, argv[2]);
	} else{
		// ERROR
		printf("access %s: No such file or directory\n", argv[2]);
		exit(1);
	}

	gettimeofday(&end_point, NULL);

	double total_timer = FileTransferTimer(start_point.tv_sec, start_point.tv_usec, end_point.tv_sec, end_point.tv_usec);
	printf("총 시간 = %g\n", total_timer);
	double total_speed = FileTransferSpeed(total_timer, argv[2]);
	printf("평균 속도 = %g\n", total_speed);
	
	//무결성 검사를ㄹ 하라능 ㅋㅋㅋㅋㅋㅋㅋ

	CountDir(argv[2]);

	sprintf(buf,"file_cnt = %d", count_File);
	//sendto
	if (sendto(sd, buf, SIZEBUF, 0, (struct sockaddr*)&sin, sizeof(sin)) == -1){
		perror("file cnt");
		exit(1);
	}
	//리시브
	
	if ((bytes_read = recvfrom(sd, buf, SIZEBUF, 0, (struct sockaddr *)&sin, &add_len)) == -1){
		perror("file cnt recieve");
		exit(1);
	}

	sprintf(buf,"dir_cnt = %d", count_Dir);

	//sendto
	if (sendto(sd, buf, SIZEBUF, 0, (struct sockaddr*)&sin, sizeof(sin)) == -1){
		perror("file cnt");
		exit(1);
	}
	//리시브
	
	if ((bytes_read = recvfrom(sd, buf, SIZEBUF, 0, (struct sockaddr *)&sin, &add_len)) == -1){
		perror("file cnt recieve");
		exit(1);
	}

	if ((bytes_read = recvfrom(sd, buf, SIZEBUF, 0, (struct sockaddr *)&sin, &add_len)) == -1){
		perror("무결성 받");
		exit(1);
	}
	printf("무결성 =  %s\n",buf);	
}

// UDP 파일 전송하는 함수
void UdpFileTrans(int sd, struct sockaddr_in sin, socklen_t add_len, char* file_name){

	int fd, n;
	int bytes_read;
	char buf[SIZEBUF + 1];
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
	int files_size = SIZEBUF;

	char buf[SIZEBUF + 1];
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

// TCP 클라이언트 부분
void TcpClient(int argc, char** argv, int sd, struct sockaddr_in sin){
 	char buf[SIZEBUF];
	struct stat sbuf;
	struct timeval start_point, end_point;

	if (stat(argv[2], &sbuf) == -1){
		perror("stat");
		exit(1);
	};
	gettimeofday(&start_point, NULL);
 
	// connect 하는 부분
	if (connect(sd, (struct sockaddr *)&sin, sizeof(sin)) == -1) {
		perror("connect");
		exit(1);
	}
	
	// is directory
	if (S_ISDIR(sbuf.st_mode)){
		printf("THIS IS TCP DIRECTORY.\n");
		
		TcpDirTrans(sd, argv[2]);
	} else if (S_ISREG(sbuf.st_mode)){
		// is file
		printf("THIS IS FILE.\n");
		TcpFileTrans(sd, argv[2]);
	} else{
		// ERROR
		printf("access %s: No such file or directory\n", argv[2]);
		exit(1);
	}

	gettimeofday(&end_point, NULL);

	double total_timer = FileTransferTimer(start_point.tv_sec, start_point.tv_usec, end_point.tv_sec, end_point.tv_usec);
	printf("총 시간 = %g\n", total_timer);
	double total_speed = FileTransferSpeed(total_timer, argv[2]);
	printf("평균 속도 = %g\n", total_speed);

	
	//무결성 검사하기 
	//CountFile(argv[2]);
	CountDir(argv[2]);
	
	// 파일 개수 전송
	sprintf(buf,"file_cnt = %d", count_File);
	printf("%s\n", buf);
	if (send(sd, buf, SIZEBUF, 0) == -1){
		perror("file count send");
		exit(1);
	}

	// 파일 개수 전송받음
	if (recv(sd, buf, SIZEBUF, MSG_WAITALL) == -1){
		perror("file count recieve");
		exit(1);
	}

	// 폴더 개수 전송
	sprintf(buf,"dir_cnt = %d", count_Dir);
	printf("%s\n", buf);
	if (send(sd, buf, SIZEBUF, 0) == -1){
		perror("dir count send");
		exit(1);
	}
	if (recv(sd, buf, SIZEBUF, MSG_WAITALL) == -1){
		perror("dir count recieve");
		exit(1);
	}
	
	// 무결성 체크
	if (recv(sd, buf, SIZEBUF, MSG_WAITALL) == -1){
		perror("무결성 받음 ");
		exit(1);
	}
	printf("무결성은 = %s\n", buf);
}

// TCP 파일 전송하는 함수
void TcpFileTrans(int sd, char* file_name){

	int fd, n;
	char buf[SIZEBUF];
	char temp_file_name[SIZEBUF];

	// 파일이라고 서버에게 알려줌
	if (send(sd, "This is File", SIZEBUF, 0) == -1){
		perror("send filename");
		exit(1);
	}

	// 파일에 대한 답장 
	if (recv(sd, buf, SIZEBUF, MSG_WAITALL) == -1){
		perror("recv isfile");
		exit(1);
	}

	memset(buf, 0, SIZEBUF);

	// 저장해야되는 파일 경로 전송
	sprintf(temp_file_name, "./save/%s", file_name);

	if (send(sd, temp_file_name, SIZEBUF, 0) == -1){
		perror("send filename");
		exit(1);
	}

	// 저장해야되는 파일 경로전송에 대한 답장받음
	if (recv(sd, buf, SIZEBUF, MSG_WAITALL) == -1){
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

		if (send(sd, buf, SIZEBUF, 0) == -1) {
			perror("send");
			exit(1);
		}
	}
 
	memset(buf, 0, SIZEBUF);
	
	// 파일전송이 끝났다고 알려줌.
	if (send(sd, "end of file", SIZEBUF, 0) == -1){
		perror("send filename");
		exit(1);
	}

	// 파일전송에 끝났다는 답장 받음
	if (recv(sd, buf, SIZEBUF, MSG_WAITALL) == -1){
		perror("recv end of file");
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

// TCP 디렉토리 전송 하는 함수
void TcpDirTrans(int sd, char* dir_name){

	int index = 0;
	int i = 0;
	int files_size = SIZEBUF;

	char buf[SIZEBUF];
	char temp_dir_name[SIZEBUF];
	char* cwd;
	DIR *dp;
	struct dirent *dent;
	struct stat sbuf;
 
	// 디렉토리라고 처음에 서버에게 알려줌.
	if (send(sd, "This is DIR", SIZEBUF, 0) == -1){
		perror("send DIR");
		exit(1);
	}
	
	//서버가 알겠다고 응답.
	if (recv(sd, buf, SIZEBUF, MSG_WAITALL) == -1){
		perror("recv isDIR");
		exit(1);
	}
	
	printf("현재 디렉토리 이름 = %s\n", dir_name);

	memset(buf, 0, SIZEBUF);

	//서버는 save 디렉토리 밑에 저장하기 때문에 위치를 이렇게 전송함.
	sprintf(temp_dir_name, "./save/%s", dir_name);
	printf("경로 = %s\n", temp_dir_name);

	// 서버에게 디렉토리 이름 전송
	if (send(sd, temp_dir_name, SIZEBUF, 0) == -1){
		perror("dir name send error");
		exit(1);
	}

	// 보낸디렉토리 이름에 대한 응답 받음
	if (recv(sd, buf, SIZEBUF, MSG_WAITALL) == -1){
		perror("recv DIRname");
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
			TcpFileTrans(sd, temp_dir_name);
		} else if (files[i].or_file_dir == 'd'){
			//디렉토리 인경우 

			//현재 경로 저장
			cwd = getcwd(NULL, SIZEBUF);
			//디렉토리 이동
			chdir(files[i].dent.d_name);
			//디렉토리 함수 이동
			TcpDirTrans(sd, temp_dir_name);
			//원래 저장경로로 돌아옴
			chdir(cwd);
		}
	}

}

double FileTransferTimer(long start_tv_sec, long start_tv_usec, long end_tv_sec, long end_tv_usec){

	return (double)(end_tv_sec)+(double)(end_tv_usec) / 1000000.0 - (double)(start_tv_sec)-(double)(start_tv_usec) / 1000000.0;
}

double FileTransferSpeed(double total_time, char *F){
	long long total_size = 0;

	struct stat buf;
	stat(F, &buf);

	if (S_ISDIR(buf.st_mode)){
		printf("THIS IS DIRECTORY.\n");
		total_size += FolderSize(F, total_size);
	} else if (S_ISREG(buf.st_mode)){
		// is file
		printf("THIS IS FILE.\n");
		total_size += FileSize(F);
	} else{
		printf("access %s: No such file or directory\n", F);
		exit(1);
	}

	return (double)total_size / total_time;
}

long long FileSize(char* file_name){
	struct stat sbuf;

	stat(file_name, &sbuf);
	long long fsize = sbuf.st_size;
	return fsize;
}

long long FolderSize(char *dir_name, long long total_size){
	DIR *dp;
	struct dirent *dent;
	int index = 0;
	int i = 0;
	int files_number = SIZEBUF;
	char buf[SIZEBUF];
	char temp_dir_name[SIZEBUF];
	char *cwd;
	struct stat sbuf;

	memset(buf, 0, sizeof(buf));
	if ((dp = opendir(dir_name)) == NULL){
		perror("opendir");
		exit(1);
	}

	memset(buf, 0, SIZEBUF);
	file_information* files = (file_information*)malloc(sizeof(file_information) * files_number);

	while ((dent = readdir(dp))){
		if (index >= files_number - 2){
			files_number *= 2;
			files = (file_information *)realloc((void *)files, sizeof(file_information) * files_number);
		}
		if (strcmp(dent->d_name, ".") == 0){
			continue;
		}
		if (strcmp(dent->d_name, "..") == 0){
			continue;
		}

		sprintf(temp_dir_name, "%s/%s", dir_name, dent->d_name);
		if (stat(temp_dir_name, &sbuf) == -1){
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

	for (i = 0; i < index; ++i){
		printf("temp = %s\n", temp_dir_name);
		sprintf(temp_dir_name, "%s/%s", dir_name, files[i].dent.d_name);
		if (files[i].or_file_dir == 'f'){
			total_size += FileSize(temp_dir_name);
		} else if (files[i].or_file_dir == 'd'){
			cwd = getcwd(NULL, SIZEBUF);
			chdir(files[i].dent.d_name);
			total_size = total_size + FolderSize(temp_dir_name, total_size);
			chdir(cwd);

		}
	}
	return total_size;
}
