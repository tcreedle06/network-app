#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<signal.h>

#define MAXLINE 1024
#define SERV_PORT 49049
#define bool int
#define true 1
#define false 0

#define SA struct sockaddr

void str_cli(FILE *fp, int sockfd);
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

   str_cli(stdin, sockfd);
   exit(0);
}

void str_cli(FILE *fp, int sockfd)
{
   char sendline[MAXLINE];
   char recvline[MAXLINE];
   bool done = false;
   
   sendline[0] = '\0';
   while(!done && fgets(sendline, MAXLINE, fp) != NULL) {
      //remove the newline...
      sendline[strlen(sendline)-1] = '\0';
      printf("Client (from stdin): >%s<\n",sendline);
      write(sockfd, sendline, strlen(sendline)+1);
      if(strcmp(sendline,"exit") == 0)
         done = true;
      else {
         if(read(sockfd, recvline, MAXLINE)==0)
            printf("str_cli: server terminated prematurely\n");
      }
      //fputs(recvline, stdout);  use this for exact output to screen
      printf("Client (from server): %s\n",recvline);  //use this if readable and want to end
                                   //the line with \n
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