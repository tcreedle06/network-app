CC = gcc
CFLAGS = -Wall
LDFLAGS = -lncurses

all: echo-client echo-server
	@echo Compilation complete

echo-client: echo-client.o
	$(CC) $(CFLAGS) -c echo-client.c $(LDFLAGS)

echo-server: echo-server.o
	$(CC) $(CFLAGS) -c echo-server.c

clean:
	rm *.o
	@echo Clean complete

