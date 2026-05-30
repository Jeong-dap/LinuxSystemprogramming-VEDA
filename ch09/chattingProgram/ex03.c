// client
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

#define MAXDATASIZE 100
#define BUFF_SIZE 1024

void chatting(int sd);

int main(int argc, char *argv[])
{
   int sockfd;
   socklen_t addr_len;
   char buf[MAXDATASIZE];
   struct hostent *he;
   struct sockaddr_in server_addr;

   if(argc != 2) {
       fprintf(stderr, "usage : client hostname \n");
       exit(1);
   }
   if((he = gethostbyname(argv[1])) == NULL) {
       perror("gethostbyname");
       exit(1);
   }
   if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
       perror("socket");
       exit(1);
   }
   server_addr.sin_family = AF_INET;
   server_addr.sin_port = htons(60000);
   server_addr.sin_addr = *((struct in_addr *)he->h_addr);
   printf("[ %s ]\n",(char*) inet_ntoa(server_addr.sin_addr));
   memset(&(server_addr.sin_zero), '\0',8);
   if(connect(sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr))== -1) {
       perror("connect");
       exit(1);
   }
    chatting(sockfd);
   return 0;
}

void chatting(int sd) {
    struct pollfd fds[2];
    fds[0].fd = 0;       // stdin
    fds[0].events = POLLIN;
    fds[1].fd = sd;      // socket
    fds[1].events = POLLIN;

    char buffer[BUFF_SIZE];
    while(1){
        poll(fds, 2, -1);
        if(fds[0].revents & POLLIN){
            fgets(buffer, BUFF_SIZE, stdin);
            if(send(sd, buffer, strlen(buffer), 0) == -1){
                perror("send");
                goto EXIT;
            }
            if(strncmp(buffer, "exit", 4) == 0){
                printf("BYE\n");
                goto EXIT;
            }
        }
        else if(fds[1].revents & POLLIN){
            int numbytes = recv(sd, buffer, BUFF_SIZE-1, 0);
            if(numbytes == -1){
                perror("recv");
                goto EXIT;
            }
            else if(numbytes == 0){
                printf("CUT.\n");
                goto EXIT;
            }
            buffer[numbytes] = '\0';
            printf("[rpi] < %s", buffer);
        }
        else{
            printf("오류발생.\n");
            goto EXIT;
        }
    }
EXIT:
    close(sd);
    return;
}
