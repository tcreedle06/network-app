CC = gcc
CFLAGS = -Wall -Wextra
LDFLAGS = -lncurses

all: echo-client echo-server
	@echo Compilation complete

echo-client: echo-client.o
	$(CC) $(CFLAGS) -o echo-client echo-client.o $(LDFLAGS)

echo-server: echo-server.o
	$(CC) $(CFLAGS) -o echo-server echo-server.o $(LDFLAGS)

echo-client.o: echo-client.c
	$(CC) $(CFLAGS) -c echo-client.c

echo-server.o: echo-server.c
	$(CC) $(CFLAGS) -c echo-server.c

clean:
	rm *.o
	@echo Clean complete

