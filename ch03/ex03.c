#include <stdio.h>
#include <fcntl.h>     // open(), O_RDONLY, O_WRONLY, O_CREAT, O_TRUNC
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>    // close()

int main() {
    int fd;

    // /etc/hosts를 읽기 전용(O_RDONLY)으로 열기
    // 성공하면 fd는 3 이상의 양수, 실패하면 -1
    if((fd=open("/etc/hosts", O_RDONLY))==-1) {
        perror("open1");
        fprintf(stderr, "File Read Fail.....\n");
    } else {
        printf("File Read Success!! fd = %d\n", fd);
        //close(fd);  // 의도적으로 닫지 않음 — 다음 open()이 같은 fd 번호를 재사용하는지 확인용
    }

    // myhosts를 쓰기 전용으로 열기
    // O_CREAT : 없으면 새로 생성
    // O_TRUNC : 이미 있으면 내용을 0으로 비움
    // 0666     : 생성 시 적용할 기본 권한(umask와 AND 연산 후 실제 권한 결정)
    if((fd=open("myhosts", O_WRONLY|O_CREAT|O_TRUNC, 0666))==-1) {
        perror("open2");
        fprintf(stderr, "File Create Fail.....\n");
    } else {
        printf("File Creat Success!! fd = %d\n", fd);
        close(fd);
    }
    return 0;
}
