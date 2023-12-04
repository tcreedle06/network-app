CC = gcc
CFLAGS = -Wall -Wextra
LDFLAGS = -lncurses

all: echoclient echoserver
	@echo Compilation complete

echoclient: echoclient.o
	$(CC) $(CFLAGS) -o echoclient echoclient.o $(LDFLAGS)

echoserver: echoserver.o
	$(CC) $(CFLAGS) -o echoserver echoserver.o $(LDFLAGS)

echoclient.o: echoclient.c
	$(CC) $(CFLAGS) -c echoclient.c

echoserver.o: echoserver.c
	$(CC) $(CFLAGS) -c echoserver.c

clean:
	rm *.o
	@echo Clean complete

