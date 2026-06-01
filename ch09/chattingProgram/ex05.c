#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/ioctl.h>

int main(int argc, char **argv)
{
    struct sockaddr_in serveraddr, clientaddr;
    int sockfd;
    int client_sd;
    int  socklen;
    int n, i;
    int readn;
    struct epoll_event ev,*client_list;
    int efd;
    char buf_in[256];

    client_list = (struct epoll_event *)malloc(sizeof(*client_list) * 20);
    if ((efd = epoll_create(100)) < 0) {
            perror("epoll_create");
            exit(1);
    }
    socklen = sizeof(clientaddr);
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0))==-1) {
        perror("socket error :");
        close(sockfd);
        return 1;
    }
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(60000);
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind (sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) == -1) {
        close(sockfd);
        return 1;
    }
    listen(sockfd, 5);
    ev.events = EPOLLIN;
    ev.data.fd = sockfd;
    epoll_ctl(efd, EPOLL_CTL_ADD, sockfd, &ev);
    while(1) {
        if((n = epoll_wait(efd, client_list, 20, -1))==-1) {
            perror("epoll_wait");
        }
        for (i = 0;i < n; i++){
            if (client_list[i].data.fd == sockfd) {
                printf("Accept\n");
                client_sd = accept(sockfd, (struct sockaddr *)&clientaddr, & socklen);
                ev.events = EPOLLIN;
                ev.data.fd = client_sd;
                epoll_ctl(efd, EPOLL_CTL_ADD, client_sd, &ev);
            } else {
                readn = read(client_list[i].data.fd, buf_in, 255);
                if(readn <= 0){
                    epoll_ctl(efd, EPOLL_CTL_DEL, client_list[i].data.fd, client_list);
                    close(client_list[i].data.fd);
                    printf("Close fd\n");
                } else {
                    // client - server 통신
                    // printf("read data %s\n", buf_in);
                    // write(client_list[i].data.fd, buf_in, readn);
                    // 이 다음꺼는 client_list[i].data.fd로 받은 데이터를 모든 클라이언트에게 전송
                    int j;
                    for (j = 0; j < n; j++) {
                        if (client_list[j].data.fd != sockfd && client_list[j].data.fd != client_list[i].data.fd) {
                            write(client_list[j].data.fd, buf_in, readn);
                        }
                    }
                }
            }   // else-end
         } // for-end
    } // while -end
    return 0;
}