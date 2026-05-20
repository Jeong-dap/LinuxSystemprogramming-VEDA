#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>

/* fork()로 부모/자식 프로세스 생성 후 각자 동작 확인
 *
 * fork() 반환값:
 *   -1 : 실패
 *    0 : 자식 프로세스 (자신의 PID는 getpid()로 확인)
 *   >0 : 부모 프로세스 (반환값 = 자식의 PID)
 *
 * 주의: 이 예제는 wait()를 호출하지 않아 자식이 먼저 종료되면
 *       부모가 회수하기 전까지 zombie 상태로 남을 수 있다.
 */
int main(void) {
    pid_t pid;

    switch(pid=fork()) {
        case -1:
            perror("fork");
            break;

        case 0:  // 자식 프로세스: fork()가 0을 반환한 쪽
            printf("CHILD PROCESS : %d\n", getpid());
            sleep(3);               // 3초 대기 (이 시점에 ps로 두 프로세스를 확인할 수 있음)
            system("echo CHILD");
            system("ps -l");        // 자식 종료 직전 프로세스 목록 출력
            exit(0);
            break;

        default:  // 부모 프로세스: fork()가 자식 PID를 반환한 쪽
            printf("PARENT PROCESS : %d\n", getpid());
            printf("Return Value of fork : %d\n\n", pid);  // pid = 자식의 PID
            sleep(7);               // 자식(3초)보다 오래 대기해 자식 종료 후 상태 확인
            system("echo PARENT");
            system("ps -l");        // 부모 종료 직전 프로세스 목록 출력
            break;
    }
    return 0;
}
