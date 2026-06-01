#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "usage: %s hostname port message\n", argv[0]);
        exit(1);
    }

    struct hostent *he = gethostbyname(argv[1]);
    if (!he) { perror("gethostbyname"); exit(1); }

    int port = atoi(argv[2]);
    char *message = argv[3];

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) { perror("socket"); exit(1); }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port   = htons(port);
    server_addr.sin_addr   = *((struct in_addr *)he->h_addr);

    int nbytes = sendto(sockfd, message, strlen(message), 0,
                        (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (nbytes < 0) { perror("sendto"); exit(1); }

    printf("Sent %d bytes to %s:%d\n", nbytes,
           inet_ntoa(server_addr.sin_addr), port);
    close(sockfd);
    return 0;
}
