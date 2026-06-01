#include <pthread.h>
#include <stdio.h>
#include <sys/timeb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_LINE 10

void nonblock(int sockfd)
{
	int opts;

    if((opts=fcntl(sockfd, F_GETFL, 0))==-1) {
        perror("fcntl");
        exit(1);
    }
    if(fcntl(sockfd, F_SETFL, opts | O_NONBLOCK) ==-1) {
        perror("fcntl");
        exit(1);
    }
}

int main(int argc, char **argv)
{
    int efd;
    int listensd;
    struct epoll_event ev, *client_list;
    struct sockaddr_in serveraddr,clientaddr;
    int clientsd;
    int i, readn, res;
    char buffer[MAX_LINE];
    socklen_t size;

    if((listensd = socket(AF_INET, SOCK_STREAM, 0)) ==-1) {
        perror("sockfd");
        exit(1);
    }
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(60000);
    if( bind(listensd, (struct sockaddr *) &serveraddr, sizeof(serveraddr))==-1) {
        perror("bind");
        exit(1);
    }
    listen(listensd, 10);
    client_list = malloc(sizeof(struct epoll_event) * 50);
    if((efd = epoll_create(50))==-1) {
        perror("epoll_create");
        exit(1);
    }
    ev.events = EPOLLIN;
    ev.data.fd = listensd;
    if(epoll_ctl(efd, EPOLL_CTL_ADD, listensd, &ev)==-1)  {
        perror("epoll_ctl");
        exit(1);
    }
    while(1)   {
        res = epoll_wait(efd, client_list, 50, -1);
        for(i = 0; i < res; i++) {
        	size=sizeof(struct sockaddr);
            if(client_list[i].data.fd == listensd)  {
                clientsd = accept(listensd,(struct sockaddr*) &clientaddr, &size);
                printf("server : got connection from %s \n", inet_ntoa(clientaddr.sin_addr));
                if(clientsd > 0) {
                    nonblock(clientsd);
                    ev.events = EPOLLIN | EPOLLET;
                    ev.data.fd = clientsd;
                    if(epoll_ctl(efd, EPOLL_CTL_ADD, clientsd, &ev)==-1) {
                        perror("epoll_ctl");
                        exit(1);
                    }
                }
            } else {
                int done = 0;
                while(1) {
                    readn = read (client_list[i].data.fd, buffer, sizeof(buffer));
                    if(readn ==-1) {
                        if(errno != EAGAIN) {   // non-block모드에서  버퍼에 있는 데이터를 모두 읽은 경우
                            perror ("read");    // read 함수의 수행결과가 -1 이고, errno 가 EAGAIN 이 됨,
                            done = 1;
                        }
                        break;
                     } else if(readn == 0) {	// 상대방이 먼저 연결을 끊은 경우
                           done = 1;
                           break;
                     }
                     if(strncmp(buffer, "...",3)==0) {
                    	 done=1;
                    	 break;
                     }
                     if(write(client_list[i].data.fd, buffer, readn)==-1) {
                           perror ("write");
                           exit(1);
                     }
                 } // while-end
                 if(done) {
                     printf ("Closed connection on descriptor %d\n", client_list[i].data.fd);
                     epoll_ctl(efd, EPOLL_CTL_DEL, client_list[i].data.fd, client_list);
                     close (client_list[i].data.fd);
                 }
            } //else-end
        } //for-end
    } //while-end
    return 0;
}