#Makefile

CC=gcc
CFLAGS=-Wall
SERVER_OBJS=TCP_server.o
CLIENT_OBJS=TCP_client.o 
LIBS=

all: server client

server: $(SERVER_OBJS)
	$(CC) $(CFLAGS) -o server.exe $(SERVER_OBJS) $(LIBS)

TCP_server.o: TCP_server.c
	$(CC) $(CFLAGS) -c TCP_server.c $(LIBS)

client: $(CLIENT_OBJS)
	$(CC) $(CFLAGS) -o client.exe $(CLIENT_OBJS) $(LIBS)

TCP_client.o: TCP_client.c
	$(CC) $(CFLAGS) -c TCP_client.c $(LIBS)

clean:
	rm -f server.exe client.exe $(SERVER_OBJS) $(CLIENT_OBJS) 
