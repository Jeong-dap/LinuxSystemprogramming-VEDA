#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void child_handler(int signo);

int main(void)
{
	struct sigaction act;

	/* SIGCHLD 시그널 핸들러 등록 */
	act.sa_handler=child_handler;
	sigfillset(&act.sa_mask);   /* 핸들러 실행 중 모든 시그널 차단 */
	act.sa_flags=SA_RESTART;    /* 시스템 콜이 시그널에 의해 중단되면 자동 재시작 */
	sigaction(SIGCHLD, &act, NULL);

	switch(fork()) {
		case -1:
			perror("fork");
			exit(1);
			break;
		case 0:
			/* 첫 번째 자식: sleep 20 실행 */
			printf("CHILD 1 : %d\n", getpid());
			execlp("sleep", "sleep", "20", NULL);
			perror("child1");
			exit(1);
			break;
		default:
			switch(fork()) {
				case -1:
					perror("fork");
					exit(1);
					break;
				case 0:
					/* 두 번째 자식: sleep 30 실행 */
					printf("CHILD 2 : %d\n", getpid());
					execlp("sleep", "sleep", "30", NULL);
					perror("child2");
					exit(1);
					break;
				default:
					/* 부모 프로세스: 자식들이 종료될 때까지 대기 */
					printf("PARENT : %d\n", getpid());
					break;
			}
	}
	while(1);  /* SIGCHLD 시그널을 받을 때까지 무한 대기 */
	return 0;
}

/* SIGCHLD 핸들러: 종료된 자식 프로세스를 모두 회수하여 좀비 방지 */
void child_handler(int signo)
{
	pid_t  pid;

  	for (;;) {
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
