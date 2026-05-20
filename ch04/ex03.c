#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

/* dup()을 이용한 stdout 리다이렉션
 *
 * dup()은 인자로 받은 fd를 현재 프로세스에서 가장 낮은 번호의 빈 fd로 복제한다.
 * 아래 코드의 흐름:
 *   1) open("duptest") → fd=3 (fd 0/1/2는 이미 사용 중)
 *   2) close(1)        → fd 1 (stdout)을 반납 → fd 1이 빈 슬롯이 됨
 *   3) dup(fd)         → fd 3을 복제. 가장 낮은 빈 fd인 1번으로 복제됨
 *   4) close(fd)       → 원본 fd 3 닫기. 이제 fd 1만 "duptest"를 가리킴
 *   5) write(1, ...)   → fd 1 = "duptest" 파일에 쓰기
 *   6) printf(...)     → 내부적으로 stdout(fd 1)에 쓰므로 역시 "duptest"에 기록됨
 */
int main(void)
{
    int fd;
    char buffer[]="Hello, dup!!\n";

    // "duptest" 파일을 쓰기 전용으로 열기. 없으면 생성, 있으면 내용 비움
    if((fd=open("duptest" ,O_WRONLY|O_CREAT|O_TRUNC,0666))==-1) {
        perror("open");
        exit(1);
    }

    close(1);    // stdout(fd 1) 닫기 → fd 1번 슬롯을 비움
    dup(fd);     // fd(duptest)를 가장 낮은 빈 fd(=1)로 복제 → fd 1이 duptest를 가리킴
    close(fd);   // 이제 fd 1로만 접근 가능하므로 원본 fd는 닫아도 됨

    // fd 1은 이제 duptest 파일 → 화면이 아닌 파일에 기록됨
    write(1, buffer, strlen(buffer));
    printf("This is dup() test file\n");  // printf도 stdout(fd 1) 사용 → 파일에 기록

    return 0 ;
}
