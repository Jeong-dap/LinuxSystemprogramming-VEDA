#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

int main(void) {
    pid_t pid;

    /* fork()로 자식 프로세스 생성; 부모에게는 자식 PID, 자식에게는 0 반환 */
    pid = fork();

    switch(pid) {
    case -1:
        /* fork 실패 시 오류 출력 후 종료 */
        perror("fork()");
        exit(-1);
        break;

    case 0:
        /* 자식 프로세스: execlp로 현재 디렉터리를 "ls -l"로 대체 실행 */
        printf("CHILD Process\n");
        execlp("ls", "ls", "-l", (char *)0);
        /* execlp가 반환하면 실행 실패 */
        perror("execlp");
        exit(0);
        break;

    default:
        /* 부모 프로세스: 자식이 종료될 때까지 대기 후 메시지 출력 */
        wait(NULL);
        printf("child is dead\n");
        break;
    }
    return 0;
}
