#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    struct in_addr addr;

    if (argc != 2) {
        fprintf(stderr, "%s <IP Address>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    printf("inet_addr(\"%s\") : %08x\n", argv[1], inet_addr(argv[1]));
    if (inet_aton(argv[1], &addr) == 0) {
        fprintf(stderr, "Invalid address\n");
        exit(EXIT_FAILURE);
    }
    printf("inet_atona(\"%s\") : %08x\n",argv[1],  addr);
    printf("\ninet_ntoa(%08x) : %s\n",  addr, inet_ntoa(addr));
    return 0;
}

/*
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    struct in_addr addr;

    if (argc != 2) {
        fprintf(stderr, "%s <IP Address>\n", argv[0]);
        exit(EXIT_FAILURE);
    }


    printf("inet_addr(\"%s\") : %08x\n", argv[1], (unsigned int)inet_addr(argv[1]));
    if (inet_aton(argv[1], &addr) == 0) {
        fprintf(stderr, "Invalid address\n");
        exit(EXIT_FAILURE);
    }
    printf("inet_atona(\"%s\") : %08x\n",argv[1], addr.s_addr);
    printf("\ninet_ntoa(%08x) : %s\n",  addr.s_addr, inet_ntoa(addr));

    char npaddr[17];
    struct sockaddr_in paddr;

    inet_pton(AF_INET, argv[1], &paddr.sin_addr.s_addr);
    inet_ntop(AF_INET, &paddr.sin_addr.s_addr, npaddr, 17);
    printf("\ninet_ptoa(%08x) : %s\n",  paddr.sin_addr.s_addr, npaddr);
    return 0;
}

*/