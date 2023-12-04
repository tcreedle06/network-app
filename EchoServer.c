#include<sys/types.h>
#include<sys/wait.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<signal.h>

#define MAXLINE 1024
#define LISTENQ 1024
#define SERV_PORT 49049
#define bool int
#define true 1
#define false 0

#define SA struct sockaddr

void str_echo(int sockfd);
void sig_child(int signc);

int main(int argc, char **argv)
{
   int listenfd, connfd;
   pid_t childpid;
   //cli = client info
   //serv = server info
   socklen_t cli_len;
   struct sockaddr_in cli_addr;
   struct sockaddr_in serv_addr;
   char buff[MAXLINE];

   listenfd = socket(AF_INET, SOCK_STREAM, 0);

   bzero(&serv_addr, sizeof(serv_addr));
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
   serv_addr.sin_port = htons(SERV_PORT);  


   bind(listenfd, (SA *) &serv_addr, sizeof(serv_addr));
   listen(listenfd, LISTENQ);

   signal(SIGCHLD, sig_child);  //must call waitpid
   for(;;) {
      
      cli_len = sizeof(cli_addr);
      connfd = accept(listenfd, (SA *) &cli_addr, &cli_len);

      inet_ntop(AF_INET, &cli_addr.sin_addr, buff, sizeof(buff));
      printf("S: Connection from %s, port %d\n",buff, ntohs(cli_addr.sin_port));

      if((childpid = fork())==0) {
         //child process
         close(listenfd);  // child does not need to listen on the socket
         str_echo(connfd);  //process the request
         exit(0); 
      }
      //parent will end up here, the child will exit somewhere else.
      close(connfd);

   }
   
   return 0; 
}


void str_echo(int sockfd)
{
   ssize_t n;
   char buff[MAXLINE];

   bool done = false;


   while(!done) {
      while(!done && (n = read(sockfd, buff, MAXLINE) > 0)) {
          printf("Server read: %s size:%ld\n",buff,strlen(buff));
          if(strcmp(buff,"exit") == 0)
             done = true;
          else
             write(sockfd,buff,strlen(buff)+1);
      }
      if(n<0 && errno != EINTR) {
         printf("str_echo: read error\n");
         done = true;
      }

   }
}

void sig_child(int signo)
{
   pid_t pid;
   int stat;

   while((pid = waitpid(-1, &stat, WNOHANG))>0)
      printf("Child %d terminated\n",pid);
}
