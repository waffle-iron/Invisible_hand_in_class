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
#include "library.h"

#define SIZEBUF 1024
#define PORTNUM 9000
void UdpClient(int argc, char** argv);
void UdpServer();
void TcpClient(int argc, char** argv);
void TcpServer();

#endif
