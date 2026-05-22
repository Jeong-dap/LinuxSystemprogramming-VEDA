/* SIGCHLD 핸들러: 종료된 백그라운드 자식 프로세스를 회수해 좀비를 방지한다. */
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* SIGCHLD 수신 시 호출된다.
 * WNOHANG으로 비블로킹 waitpid를 반복하여 한 번의 시그널로 여러 자식을 처리한다.
 * (시그널은 큐잉되지 않으므로, 루프로 처리하지 않으면 자식이 누락될 수 있다.) */
void child_handler(int signo)
{
    pid_t pid;

    for (;;) {
        pid = waitpid(-1, NULL, WNOHANG);
        if (pid == 0) {
            /* 아직 종료된 자식이 없음 — 루프 종료 */
            break;
        } else if (pid == -1 && errno == ECHILD) {
            /* 대기할 자식이 더 이상 없음 — 루프 종료 */
            break;
        } else if (pid == -1) {
            /* 예기치 않은 오류 */
            perror("waitpid");
            abort();
        }
        /* 회수 성공: 종료된 자식의 PID 출력 */
        printf("PID of the dead child = %d\n", pid);
    }
}
