#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include <ncurses.h> // importing ncurses to separate text input from conversation log

#define MAXLINE 1024
#define SERV_PORT 25565

#define SA struct sockaddr

void str_cli(WINDOW *inputWin, WINDOW *outputWin, int sockfd);
void CatchInterrupt (int signum);

int main(int argc, char **argv)
{
   int sockfd;
   struct sockaddr_in servaddr;

   signal(SIGINT, CatchInterrupt);

   if(argc != 2) {
      printf("Usage: a.out <IPaddress>\n");
      exit(1);
   }

   if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      printf("socket error\n");
      exit(2);
   }

   bzero(&servaddr, sizeof(servaddr));
   servaddr.sin_family = AF_INET;
   servaddr.sin_port = htons(SERV_PORT); 
   if(inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0)
      printf("inet_pton error for %s\n", argv[1]);

   if(connect(sockfd, (SA *)&servaddr, sizeof(servaddr)) < 0)
      printf("connect error\n");

   // Initialize NCurses
   initscr();
   cbreak(); // Line buffering disabled
   keypad(stdscr, TRUE); // Enable special keys
   noecho(); // Don't echo while we do getch

   int inputWinHeight = 3; // Height of the input window (including line)
   int outputWinHeight = LINES - inputWinHeight - 1; // Rest of the space for output window

   // Create the output window
   WINDOW *outputWin = newwin(outputWinHeight, COLS, 0, 0);
   box(outputWin, 0, 0);
   scrollok(outputWin, TRUE); // Allow scrolling

   // Create the input window at the bottom
   WINDOW *inputWin = newwin(inputWinHeight, COLS, outputWinHeight, 0);
   box(inputWin, 0, 0);
   mvwhline(inputWin, 1, 1, ACS_HLINE, COLS - 2); // Horizontal line

   wrefresh(outputWin);
   wrefresh(inputWin);

   str_cli(outputWin, inputWin, sockfd); // Pass both windows to your function

   endwin(); // End NCurses
   exit(0);
}

void str_cli(WINDOW *inputWin, WINDOW *outputWin, int sockfd) {
   char sendline[MAXLINE];
   char recvline[MAXLINE];

   while (1) {
      // Get user input
      wmove(inputWin, 1, 1); // Move cursor to input window
      echo();
      wgetnstr(inputWin, sendline, MAXLINE - 1);
      noecho();

      // Display user input in output window
      wprintw(outputWin, "Client: %s\n", sendline);
      wrefresh(outputWin);

      // Send the user input to the server and get the response
      write(sockfd, sendline, strlen(sendline) + 1);
      read(sockfd, recvline, MAXLINE);

      // Display server response in output window below previous messages
      wprintw(outputWin, "Server: %s\n", recvline);
      wrefresh(outputWin);
   }
}


void CatchInterrupt (int signum) 
{
   pid_t my_pid;

   printf("\nReceived an interrupt! About to exit ..%d\n",signum);
   fflush(stdout);
   my_pid = getpid();
   kill(my_pid, SIGKILL);
}