// server
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>    // struct sockaddr_in, inet_ntoa
//#include <sys/socket.h>
//#include <sys/wait.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>

#define BACKLOG 10
#define MAXDATASIZE 100

void *handle_client(void *arg) {        // 이 함수는 클라이언트와의 통신을 처리하는 스레드 함수
    int new_fd = *((int *)arg);         // 클라이언트 소켓 파일 디스크립터를 가져옴
    free(arg);                      // 동적으로 할당된 메모리를 해제
    if(send(new_fd, "Hello, client!\n", 14, 0) == -1)       // 클라이언트에게 메시지 보내기
        perror("send");     // 에러 처리
    close(new_fd);
    return NULL;
}

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

int main(void)
{
    int sockfd, new_fd;                 // 서버 소켓 클라이언트 소켓 파일 디스크립터
    struct sockaddr_in server_addr;     // 서버 주소 구조체
    struct sockaddr_in client_addr;     // 클라이언트 주소 구조체
    int sin_size;          // 클라이언트 주소 구조체 크기

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {      // TCP 소켓 생성하는 함수
        perror("socket");
        exit(1);
    }
    server_addr.sin_family = AF_INET;       // 주소 체계 설정
    server_addr.sin_port = htons(60000);    // 포트 번호 설정
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);    // 모든 인터페이스에서 연결 허용
    memset(&(server_addr.sin_zero), '\0', 8); // sockaddr_in 구조체에는 패딩용 sin_zero[8] 필드가 있는데,
    // 이걸 안 비워두면 bind() 할 때 쓰레기 값이 들어가서 문제가 생길 수 있어서 0으로 초기화하는 것
    if(bind(sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr))==-1) {
        // bind() 함수는 소켓에 주소를 할당하는 함수인데, 이 과정에서 문제가 생기면 -1을 반환하므로
        // 에러 처리를 해주는 것
        perror("bind");
        exit(1);
    }

    if(listen(sockfd, BACKLOG) == -1) { // listen() 함수는 소켓을 수신 대기 상태로 만드는 함수인데,
        // 이 과정에서 문제가 생기면 -1을 반환하므로
        perror("listen");
        exit(1);
    }
    
    while(1) {
        sin_size = sizeof(struct sockaddr_in);
        if((new_fd = accept(sockfd, (struct sockaddr *)&client_addr, &sin_size))== -1) {
            perror("accept");
            continue;
        }
        printf("server : got connection from %s \n", inet_ntoa(client_addr.sin_addr));
        chatting(new_fd);  // 클라이언트와의 통신을 처리하는 함수 호출
        // if(!fork()) {
        //     close(sockfd);
        //     if(send(new_fd, "Hello, client!\n", 14, 0) == -1)
        //         perror("send");
        //         close(new_fd);
        //         exit(0);
        // }
        // close(new_fd);
        // while(waitpid(-1, NULL, WNOHANG) > 0);
        pthread_t tid;  // 스레드 ID 저장하는 변수
        if (pthread_create(&tid, NULL, handle_client, &new_fd) != 0) {
            // pthread_create() 함수는 새로운 스레드를 생성하는 함수
            perror("pthread_create");
            exit(1);
        }
        pthread_detach(tid);  // pthread_detach() 함수는 스레드를 분리하는 함수로
        // 이 함수를 호출하면 스레드가 종료될 때 자동으로 자원을 해제하도록 하는 것
    }
    return 0;
}