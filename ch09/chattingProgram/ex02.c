// client
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
// #include <sys/types.h>
// #include <sys/socket.h>
// #include <netinet/in.h>
#include <arpa/inet.h>      // struct sockaddr_in, inet_ntoa
#include <unistd.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>

#define MAXDATASIZE 100

void chatting(int sd) {
    fd_set fdset, fdset1;
    FD_ZERO(&fdset);
    FD_SET(0, &fdset);
    FD_SET(sd, &fdset);
    char buf[MAXDATASIZE];
    while(1) {
        fdset = fdset1;
        select(sd+1, &fdset, NULL, NULL, NULL);
        if (FD_ISSET(0, &fdset)) {      // send()
            fgets(buf, MAXDATASIZE, stdin);
            send(sd, buf, strlen(buf), 0);
        } else if (FD_ISSET(sd, &fdset)) {      // recv()
            recv(sd, buf, MAXDATASIZE-1, 0);
            printf("%s\n", buf);
        } else {        // error
            perror("select");
            break;
        }
    }
}

int main(int argc, char *argv[])        // 서버에 접속해서 메시지를 받는 클라이언트 프로그램
{
   int sockfd, numbytes;            // 소켓 파일 디스크립터와 수신한 바이트 수를 저장하는 변수
   socklen_t addr_len;              // 주소 구조체의 크기를 저장하는 변수
   char buf[MAXDATASIZE];           // 수신한 메시지를 저장하는 버퍼
   struct hostent *he;              // 호스트 정보를 저장하는 구조체 포인터
   struct sockaddr_in server_addr;  // 서버 주소 구조체

   if(argc != 2) {      // 명령행 인자가 2개가 아니면에러 메시지 출력하고 종료
       fprintf(stderr, "usage : client hostname \n");
       exit(1);
   }
   if((he = gethostbyname(argv[1])) == NULL) {  // gethostbyname() 함수는 호스트 이름을 IP 주소로
    //변환하는 함수
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
    // connect() 함수는 서버에 연결하는 함수
       perror("connect");
       exit(1);
   }
   if((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
    // recv() 함수는 서버로부터 메시지를 수신하는 함수
       perror("recv");
       exit(1);
   }
   buf[numbytes] = '\0';        // 수신한 메시지의 끝에 널 문자 추가해서 문자열로 만듦
   chatting(sockfd);  // 클라이언트와의 통신을 처리하는 함수 호출
   close(sockfd);
   return 0;
}