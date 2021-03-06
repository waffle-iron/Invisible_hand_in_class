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

void UdpClient(int argc, char** argv, int sd, struct sockaddr_in sin);
void UdpServer(int sd, struct sockaddr_in cli);
void UdpFileTrans(int sd, struct sockaddr_in sin, socklen_t add_len, char* file_name, int file_offset);
void UdpDirTrans(int sd, struct sockaddr_in sin, socklen_t add_len, char* dir_name);

void TcpClient(int argc, char** argv, int sd, struct sockaddr_in sin);
void TcpServer(int sd, struct sockaddr_in cli);
void TcpFileTrans(int sd, char* file_name, int file_offset);
void TcpDirTrans(int sd, char* dir_name);

void CountFile(const char* name);
double FileTransferTimer(long start_tv_sec, long start_tv_usec, long end_tv_sec, long end_tv_usec);
double FileTransferSize(char *F);
long long FileSize(char* file_name);
long long FolderSize(char *dir_name, long long total_size);

typedef struct _file_information{	//����ü

	char path[SIZEBUF];
	char final_path[SIZEBUF];
	char or_file_dir;
}file_information;

file_information* file_info;
int files_size;
int idx;

#endif
