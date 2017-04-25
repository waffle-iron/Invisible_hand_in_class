#ifndef _FUNCTION_H_
#define _FUNCTION_H_

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <dirent.h>

#include "library.h"

#define SIZEBUF 1024
#define PORTNUM 9000
<<<<<<< HEAD

void UdpClient(int argc, char** argv, int sd, struct sockaddr_in sin);
void UdpServer(int sd, struct sockaddr_in cli);
void UdpFileTrans(int sd, struct sockaddr_in sin, socklen_t add_len, char* file_name);
void UdpDirTrans(int sd, struct sockaddr_in sin, socklen_t add_len, char* dir_name);

void TcpClient(int argc, char** argv, int sd, struct sockaddr_in sin);
void TcpServer(int sd, struct sockaddr_in cli);
void TcpFileTrans(int sd, char* file_name);
void TcpDirTrans(int sd, char* dir_name);


typedef struct _file_information{	// 파일 디렉토리 저장하는 구조체
=======
void UdpClient(int argc, char** argv, int sd,	struct sockaddr_in sin);
void UdpServer();
void TcpClient(int argc, char** argv);
void TcpServer();
long long Folder_Size(char *Dir_name,long long total_size);
long long File_Size(char *file_name);
double File_Transfer_Speed(double total_time,char *F);
double File_Transfer_Timer(long start_tv_sec,long start_tv_usec,long end_tv_sec,long end_tv_usec);
typedef struct _file_information{	//����ü
>>>>>>> Timer
	struct dirent dent;
	char or_file_dir;
}file_information;

#endif
