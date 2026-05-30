// server
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <errno.h>

#define BUFF_SIZE 1024

void chatting(int sd);

int main(void) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd == -1) {
        perror("socket");
        exit(1);
    }
    struct sockaddr_in server_addr;
    socklen_t addr_len = sizeof(struct sockaddr_in);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(60000);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    memset(&(server_addr.sin_zero), 0, 8);
    if(bind(sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) {
        perror("bind");
        exit(1);
    }
    if(listen(sockfd, 10) == -1) {
        perror("listen");
        exit(1);
    }
    struct sockaddr_in client_addr;
    int new_fd;
    if((new_fd = accept(sockfd, (struct sockaddr *)&client_addr, &addr_len)) == -1) {
        perror("accept");
        exit(1);
    }
    printf("연결 완료 %s\n", inet_ntoa(client_addr.sin_addr));
    chatting(new_fd);
    close(sockfd);
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
                printf("BYE!\n");
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
            printf("[usr] < %s", buffer);
        }
        else{
            printf("오류발생.\n");
            goto EXIT;
        }
    }
EXIT:
    close(sd);
    printf("연결 종료!\n");
    return;
}
