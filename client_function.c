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
	// 파일이라는 것을 서버에게 알림
	if (sendto(sd, "This is File", SIZEBUF, 0, (struct sockaddr*)&sin, sizeof(sin)) == -1){
		perror("sendto filename");
		exit(1);
	}

	// 파일이라는 것을 서버가 인식하고 보낸 메시지
	if ((bytes_read = recvfrom(sd, buf, SIZEBUF, 0, (struct sockaddr *)&sin, &add_len)) == -1){
		perror("recvfrom isfile");
		exit(1);
	}

	memset(buf, 0, SIZEBUF);
	sprintf(temp_file_name, "./save/%s", file_name);
	// 파일 이름 전송
	if (sendto(sd, temp_file_name, SIZEBUF, 0, (struct sockaddr*)&sin, sizeof(sin)) == -1){
		perror("sendto filename");
		exit(1);
	}

	//파일이름 받기
	//서버에서 파일을 받은후 클라이언트에게 다시 파일 명을 전송함
	//buf로 데이타를 읽어들이게 된다
	if ((bytes_read = recvfrom(sd, buf, SIZEBUF, 0, (struct sockaddr *)&sin, &add_len)) == -1){
		perror("recvfrom filename");
		exit(1);
	}

	//내가 보낸 파일이랑 이름이 다른경우
	//if (strcmp(filename, buf) != 0) { //buf랑 비교
	//	printf("%s\n", buf);
	//	perror("not match filename.");
	//	exit(1);
	//} else {
	//	// 파일이름 받고출력
	//	printf("** match filename : %s\n", buf);
	//}

	// 파일열기
	if ((fd = open(buf, O_RDONLY)) == -1){
		perror("file open fail");
		exit(1);
	}

	//file 내용을 전송
	while ((n = read(fd, buf, SIZEBUF)) > 0){ //fd에 있는걸 buf로 저장

		printf("SEND : %d\n", n);

		if (sendto(sd, buf, n, 0, (struct sockaddr *)&sin, sizeof(sin)) == -1) {
			perror("sendto");
			exit(1);
		}
	}

	memset(buf, 0, SIZEBUF);

	//sprintf(buf, "end of file"); //buf에다가 end of file을 붙이는 것
	//end of file 을 전송
	if (sendto(sd, "end of file", SIZEBUF, 0, (struct sockaddr*)&sin, sizeof(sin)) == -1){
		perror("sendto filename");
		exit(1);
	}


	// end of file 확인
	if ((bytes_read = recvfrom(sd, buf, SIZEBUF, 0, (struct sockaddr *)&sin, &add_len)) == -1){
		perror("recvfrom end of file");
		exit(1);
	}

	if (strcmp("end of file", buf) == 0) { //buf랑 비교
		printf("%s\n", buf);
	} else{
		perror("error : file is not end");
		exit(1);
	}
}

void UdpDirTrans(int sd, struct sockaddr_in sin, socklen_t add_len, char* Dir_name){
	DIR *dp;
	struct dirent *dent;
	//index : 디렉토리안 파일및 디렉토리개수
	int index = 0;
	int bytes_read;
	int	files_number = 2000;
	char buf[SIZEBUF];
	char temp_dirent_name[2000] = { 0, };
	char* cwd; 
	struct stat sbuf;

	// 디렉토리라는 것을 서버에게 알림
	if (sendto(sd, "This is DIR", SIZEBUF, 0, (struct sockaddr*)&sin, sizeof(sin)) == -1){
		perror("sendto DIR");
		exit(1);
	}

	// 디렉토리이라는 것을 서버가 인식하고 보낸 메시지
	if ((bytes_read = recvfrom(sd, buf, SIZEBUF, 0, (struct sockaddr *)&sin, &add_len)) == -1){
		perror("recvfrom isDIR");
		exit(1);
	}

	memset(buf, 0, SIZEBUF);

	// 디렉토리 이름 전송
	if (sendto(sd, Dir_name, SIZEBUF, 0, (struct sockaddr*)&sin, sizeof(sin)) == -1){
		perror("sendto DIRname");
		exit(1);
	}

	// 디렉토리이름 받기
	//서버에서 파일을 받은후 클라이언트에게 다시 파일 명을 전송함
	//buf로 데이타를 읽어들이게 된다
	
	if ((bytes_read = recvfrom(sd, buf, SIZEBUF, 0, (struct sockaddr *)&sin, &add_len)) == -1){
		perror("recvfrom DIRname");
		exit(1);
	}

	if ((dp = opendir(Dir_name)) == NULL) {
		perror("opendir");
		exit(1);
	}
	memset(buf, 0, SIZEBUF);
	file_information* files = (file_information*)malloc(sizeof(file_information) * files_number);
	//읽으면서 files 배열에 저장
	while ((dent = readdir(dp))){
		if (index >= files_number - 2){
			files_number *= 2;
			files = (file_information*)realloc((void*)files, sizeof(file_information) * files_number);
		}
		// 이름시작이 .일경우 계속진행
		if (strcmp(dent->d_name, ".") == 0){
			continue;
		}
		if (strcmp(dent->d_name, "..") == 0){
			continue;
		}
		//정보를 files배열에 저장
		stat(dent->d_name, &sbuf);
		files[index].dent = *dent;
		//디렉토리 인경우
		if (S_ISDIR(sbuf.st_mode)){
			files[index].or_file_dir = 'd';
		} else if (S_ISREG(sbuf.st_mode)){
			//파일인 경우
			files[index].or_file_dir = 'f';
		}
		index++;
	}

	qsort(files, index, sizeof(file_information), compare_file_name);

	int i = 0;
	for (i = 0; i < index; ++i){
		if (files[i].or_file_dir = 'f'){
			UdpFileTrans(sd, sin, add_len, files[i].dent.d_name);
		} else if (files[i].or_file_dir = 'd'){
			char temp[2] = { '/', '\0' };
			//디렉토리 경로 출력하기 위해 변수에 저장
			sprintf(temp_dirent_name, "%s%s%s", Dir_name, temp, files[i].dent.d_name);
			//활동 디렉토리 저장
			cwd = getcwd(NULL, SIZEBUF);
			//하위 디렉토리로 이동후 출력
			chdir(files[i].dent.d_name);
			UdpDirTrans(sd, sin, add_len, temp_dirent_name);
			//다시 활동 디렉토리로 복귀
			chdir(cwd);
		}
	}

}

//첫번째 실행프로그램 / IP / 전송할 디렉토리 or 파일 
void UdpClient(int argc, char** argv){

	int sd;// , fd, n;
		struct stat buf;
		//char end_buf[SIZEBUF];
		//char re_buf[SIZEBUF];

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
		sin.sin_addr.s_addr = inet_addr(argv[1]);
		sin.sin_port = htons(PORTNUM);

		stat(argv[2], &buf);
		// 디렉토리 인경우 
		if (S_ISDIR(buf.st_mode)){
			printf("디렉토리입니다.\n");
			UdpDirTrans(sd, sin, add_len, argv[2]);
		} else if (S_ISREG(buf.st_mode)){
			//입력한게 파일일 경우
			printf("파일입니다.\n");
		UdpFileTrans(sd, sin, add_len, argv[2]);
	} else{
		//파일 및 디렉토리도 아닌 경우 에러출력
		printf("myls: cannot access %s: No such file or directory\n", argv[2]);
		exit(1);
	}



	///////////////////////////////////////////////////////////
	//////// 무결성 체크/////////////////////////////////////////////
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

	////file 내용을 다시 전송
	//while ((n = read(fd1, buf, 255)) > 0){ //fd에 있는걸 buf로 저장

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


	//// end of file 확인
	//bytes_read = (recvfrom(sd, end_buf, 12, 0, (struct sockaddr *)&sin, &add_len));
	//if (bytes_read == -1) {
	//	perror("recvfrom end of file");
	//	exit(1);
	//}

	//if (strcmp("end of file", end_buf) == 0) { //buf랑 비교
	//	printf("%s\n", end_buf);
	//} else{
	//	perror("error : file is not end");
	//	exit(1);
	//}

	////만약 일치 불일치 메세지 를 받는다.
	//bytes_read = (recvfrom(sd, re_buf, 20, 0, (struct sockaddr *)&sin, &add_len));

	////만약 불일치라면 소켓을 닫는다
	//if (strcmp("100%%", re_buf) == 0) { //buf랑 비교
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
	const char* filename = argv[2]; //파일 이름
	printf("%s\n", filename);


	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}
	// 초기화 
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

	// 파일 이름 전송
	if (send(sd, filename, SIZEBUF, 0) == -1){
		perror("send filename");
		exit(1);
	}


	//파일 명 다시 받기
	int bytes_read = (recv(sd, buf, SIZEBUF, MSG_WAITALL));

	if (bytes_read < 0) {
		perror("recvfrom filename");
		exit(1);
	}

	//내가 보낸 파일이랑 이름이 다른경우
	if (strcmp(filename, buf) != 0) { //buf랑 비교
		printf("%s\n", buf);
		perror("not match filename.");
		exit(1);
	} else printf("** match filename : %s\n", buf);// 파일이름 받고출력


	//file 내용을 전송
	while ((n = read(fd, buf, SIZEBUF)) > 0){ //fd에 있는걸 buf로 저장

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
	//end of file 을 전송
	if (send(sd, buf, SIZEBUF, 0) == -1){
		perror("send filena?me");
		exit(1);
	}


	// end of file 확인
	bytes_read = (recv(sd, end_buf, SIZEBUF, MSG_WAITALL));

	if (bytes_read == -1) {
		perror("recv end of file");
		exit(1);
	}

	if (strcmp("end of file", end_buf) == 0) { //buf랑 비교
		printf("%s\n", end_buf);
	} else{
		perror("error : file is not end");
		exit(1);
	}

	close(fd);

	///////////////////////////////////////////////////////////
	//////// 무결성 체크/////////////////////////////////////////////
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

	//file 내용을 다시 전송
	while ((n = read(fd1, buf, SIZEBUF)) > 0){ //fd에 있는걸 buf로 저장

		printf("RESEND : %d\n", n);

		if (send(sd, buf, SIZEBUF, 0) == -1) {
			perror("resendto");
			exit(1);
		}
		memset(buf, 0, SIZEBUF);
	}
	memset(buf, 0, SIZEBUF);
	sprintf(buf, "end of file");
	//파일끝이라고 전송
	if (send(sd, buf, SIZEBUF, 0) == -1){
		perror("send filena?me");
		exit(1);
	}
	// end of file 확인
	bytes_read = (recv(sd, end_buf, SIZEBUF, 0));

	if (bytes_read == -1) {
		perror("recv end of file");
		exit(1);
	}

	if (strcmp("end of file", end_buf) == 0) { //buf랑 비교
		//printf("여기디이빈다 : %s\n", end_buf);
	} else{
		perror("error : file is not end");
		exit(1);
	}
	//여긴 머냐
	//만약 일치 불일치 메세지 를 받는다.
	memset(percent, 0, SIZEBUF + 1);
	bytes_read = recv(sd, percent, SIZEBUF, 0);
	//percent[bytes_read] = '\0';
	//printf("불일치 사이즈: %d\n", bytes_read);
	//printf("%s 퍼센트 가 무엇인지 출력\n", percent);
	//만약 불일치라면 소켓을 닫는다
	if (strcmp("100percent", percent) == 0) { //buf랑 비교
		printf("%s same file\n", percent);
	} else{
		printf("error : file is not same\n");
		exit(1);
	}


}