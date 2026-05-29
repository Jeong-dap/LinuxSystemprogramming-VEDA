#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int main(int argc, char *argv[]) {
   struct hostent *host;
   struct in_addr inaddr;
   int i;
   char *buf;

   if(argc<2) {
        printf("Usage: %s  hostname\n",argv[0]);
        exit(1);
    }
    if((host=gethostbyname(argv[1]))==NULL) {
         printf("gethostbyname fail\n");
         exit(0);
    }
    printf("호스트 이름: %s\n", host->h_name);
    printf("호스트 주소타입 번호: %d\n", host->h_addrtype);
    printf("호스트 주소의 길이  : %d 바이트\n", host->h_length);
    for(i=0; host->h_addr_list[i]; i++)  {
         memcpy(&inaddr.s_addr, host->h_addr_list[i],sizeof(inaddr.s_addr));
         buf = inet_ntoa(inaddr);
         printf("IP 주소(%d 번째)     : %s\n",i+1,buf);
    }

    for( i=0; host->h_aliases[i]; i++)
        printf("호스트 별명(%d 번째) : %s \n",i+1, host->h_aliases[i]);

/*
    char addr[sizeof(struct sockaddr) +1];
    struct sockaddr_in* sin;
    struct sockaddr_in6* sin6;

    for(struct addrinfo* rp=result; rp!=NULL; rp=rp->ai_next) {
        if(rp->ai_family == AF_INET) {
                sin = (void *)rp->ai_addr;
            inet_ntop(rp->ai_family, &sin->sin_addr, buf, sizeof(buf));
            printf(" protocol : %d  socket type : %d  IP 주소 :  %s\n", rp->ai_protocol, rp->ai_socktype, buf);
        }
                else if(rp->ai_family == AF_INET6) {
            sin6 = (void *)rp->ai_addr;
            inet_ntop(rp->ai_family, &sin6->sin6_addr, buf, sizeof(buf));
            printf(" protocol : %d  socket type : %d  IP 주소 :  %s\n", rp->ai_protocol, rp->ai_socktype, buf);
        }
    }
    freeaddrinfo(result);
*/
    return 0;
}


