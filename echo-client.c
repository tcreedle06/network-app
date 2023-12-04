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
/*#define bool int
#define true 1
#define false 0*/

#define SA struct sockaddr

void str_cli(WINDOW *win, int sockfd); // cli now uses WINDOW from ncurses
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
   echo(); // Don't echo while we do getch
   keypad(stdscr, TRUE); // Enable special keys

   WINDOW *win = newwin(10, 50, 1, 1); // Example window size
    
   str_cli(win, sockfd); // Pass the window to your function
   
   endwin(); // End NCurses
   exit(0);
}

void str_cli(WINDOW *win, int sockfd) {
   char sendline[MAXLINE];
   char recvline[MAXLINE];
   bool done = false;

   while (!done) {
      sendline[0] = '\0';
      if (wgetnstr(win, sendline, MAXLINE) == ERR)
         break;

      wprintw(win, "Client (from stdin): >%s<\n", sendline);
      wrefresh(win);
      write(sockfd, sendline, strlen(sendline) + 1);

      if (strcmp(sendline, "exit") == 0)
         break;

      if (read(sockfd, recvline, MAXLINE) == 0) {
         wprintw(win, "str_cli: server terminated prematurely\n");
         break;
      }

      wprintw(win, "Client (from server): %s\n", recvline);
      wrefresh(win);
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