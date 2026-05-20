#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int main(void)
{
    // fd 0=stdin, 1=stdout, 2=stderr 만 열려 있고 fd 3은 존재하지 않음
    int fd = 3;
    // 열지 않은 fd를 close() 하면 EBADF 에러 발생 → "close: Bad file descriptor" 출력
    if(close(fd)==-1) {
        perror("close");
        exit(1);
    }
    return 0;
}