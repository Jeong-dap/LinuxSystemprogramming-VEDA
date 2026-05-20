#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

/* SIGCHLD 핸들러: 종료된 자식 프로세스를 모두 회수하여 좀비 방지 */
void child_handler(int signo)
{
	pid_t  pid;

  	for (;;) {  /* 대기 중인 자식이 없을 때까지 반복 회수 */
		/* WNOHANG: 종료된 자식이 없으면 블로킹하지 않고 즉시 반환 */
		pid = waitpid(-1, NULL, WNOHANG);
		if (pid == 0) {          /* 회수할 자식이 더 없음 */
      			break;
		} else if (pid == -1 && errno == ECHILD) {  /* 자식 프로세스가 하나도 없음 */
      			break;

   		} else if (pid == -1) {  /* 그 외 waitpid 오류 */
      			perror("waitpid");
      			abort();
    		}
		printf("PID of the dead child = %d\n", pid);  /* 종료된 자식 PID 출력 */

  	}
  	return;
}