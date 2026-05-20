#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>  // waitpid(), WIFEXITED(), WIFSIGNALED()

/* waitpid()로 자식 종료를 기다리고 종료 원인 판별
 *
 * wait()와 waitpid() 차이:
 *   wait(&status)        : 임의의 자식 하나가 종료될 때까지 블로킹
 *   waitpid(pid, &s, 0)  : 특정 pid의 자식이 종료될 때까지 블로킹
 *                          세 번째 인자 0 = 블로킹 모드
 *
 * status 분석 매크로:
 *   WIFEXITED(status)    : exit()로 정상 종료했으면 참
 *   WIFSIGNALED(status)  : 시그널에 의해 강제 종료됐으면 참
 */
int main(void) {
    pid_t pid;
    int status;

    switch(pid=fork()) {
        case-1: perror("fork"); break;

        case 0:  // 자식 프로세스
            printf("CHILD PROCESS : %d\n", getpid());
            sleep(3);
            system("echo CHILD");
            system("ps -l");
            exit(0);  // 정상 종료 → 부모에서 WIFEXITED(status) 가 참이 됨
            break;

        default:  // 부모 프로세스
            printf("PARENT PROCESS : %d\n", getpid());
            printf("Return Value of fork : %d\n\n", pid);

            // wait(&status);  // 임의의 자식 대기 (주석 처리)
            waitpid(pid, &status, 0);  // 특정 자식(pid)이 종료될 때까지 블로킹 대기

            // 자식의 종료 원인 판별
            if(WIFEXITED(status)) {
                printf("Child Process is deaded by exit()\n");   // exit() 정상 종료
            } else if(WIFSIGNALED(status)) {
                printf("Child Process is deaded by Signal\n");   // 시그널(kill 등)에 의한 종료
            }

            system("echo PARENT");
            system("ps -l"); break;
    }
    return 0;
}
