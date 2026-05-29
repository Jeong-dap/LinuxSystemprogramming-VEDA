#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>

#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    int server_sockfd, client_sockfd;
    int state, client_len;

    struct sockaddr_in clientaddr, serveraddr, myaddr;


    // internet 기반의 스트림 소켓을 만들도록 한다.
    if ((server_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket error : ");
        exit(0);
    }
    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(50000);

    state = bind(server_sockfd , (struct sockaddr *)&serveraddr,
            sizeof(serveraddr));
    if (state == -1)
    {
        perror("bind error : ");
        exit(0);
    }

    state = listen(server_sockfd, 5);
    if (state == -1)
    {
        perror("listen error : ");
        exit(0);
    }


    client_sockfd = accept(server_sockfd, (struct sockaddr *)&clientaddr,
                           &client_len);
    if (client_sockfd == -1)
    {
        perror("Accept error : ");
        exit(0);
    }


    close(client_sockfd);
    return 1;
}
