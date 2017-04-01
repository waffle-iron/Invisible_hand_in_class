#Makefile

CC=gcc
CFLAGS=-Wall
SERVER_OBJS=server.o server_function.o
CLIENT_OBJS=client.o client_function.o
LIBS=

all: server client

server: $(SERVER_OBJS)
	$(CC) $(CFLAGS) -o server $(SERVER_OBJS) $(LIBS)

server.o: server.c
	$(CC) $(CFLAGS) -c server server.c $(LIBS)

server_function.o: server_function.c
	$(CC) $(CFLAGS) -c server server.c $(LIBS)

client: $(CLIENT_OBJS)
	$(CC) $(CFLAGS) -o client $(SERVER_OBJS) $(LIBS)

client.o: client.c
	$(CC) $(CFLAGS) -c client client.c $(LIBS)

client_function.o: client_function.c
	$(CC) $(CFLAGS) -c server server.c $(LIBS)

clean:
	rm -f server client $(SERVER_OBJS) $(CLIENT_OBJS)
