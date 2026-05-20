#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

/* SIGUSR1 핸들러: 시그널을 받은 프로세스가 PID를 출력하고 정상 종료 */
void usr1_handler(int signo)
{
	printf("pid=%d received SIGUSR1, exiting.\n", getpid());
	exit(EXIT_SUCCESS);
}

/* 현재 프로세스의 PID, PPID, PGID, SID를 msg 접두사와 함께 출력 */
void processInfo(char *msg)
{
	printf("%s: my pid = %d, ppid = %d, pgid = %d, sid = %d\n",
	       msg, getpid(), getppid(), getpgid(0), getsid(0));
}


int main(void)
{
	struct sigaction act;

	/* 모든 프로세스에 SIGUSR1 핸들러 등록 */
	act.sa_handler=usr1_handler;
	sigfillset(&act.sa_mask);
	act.sa_flags=SA_RESTART;
	sigaction(SIGUSR1, &act, NULL);
	processInfo("main");

	pid_t cpid1, cpid2, cpid3;
	switch(cpid1=fork()) {
	case -1:
		perror("fork");
		break;
	case 0:
		/* child1: 부모와 같은 프로세스 그룹에 속함 */
		processInfo("child1");
		sleep(5);
		printf("child1 exiting\n");
		exit(EXIT_SUCCESS);
		break;
	default:
		switch(cpid2=fork()) {
		case -1:
			perror("fork2");
			break;
		case 0:
			/* child2: 새 프로세스 그룹 리더가 되어 독립 그룹 생성 */
			if(setpgid(0, 0) == -1)
				perror("setpgid error:");
			processInfo("child2");
			switch(cpid3=fork()) {
			case -1:
				perror("fork3");
				break;
			case 0:
				/* child3: child2와 같은 프로세스 그룹에 속함 */
				processInfo("child3");
				sleep(3);
				// kill(getpid(), SIGUSR1);  /* 자기 자신에게만 보내는 방식 (비활성) */
				/* 자신이 속한 프로세스 그룹 전체(child2+child3)에 SIGUSR1 전송 */
				kill(-getpgid(getpid()), SIGUSR1);
				printf("child3 exiting\n");
				exit(EXIT_SUCCESS);
				break;
			default:
				/* child2: child3 종료 대기 후 종료 */
				wait(NULL);
				printf("child2 exiting\n");
				exit(EXIT_SUCCESS);
				break;
			}
		}
	}
	sleep(2);
	//kill(0, SIGUSR1);  /* 현재 프로세스 그룹 전체에 보내는 방식 (비활성) */
	/* 부모: child2, child1 순서로 회수 */
	waitpid(cpid2,NULL, 0);
	waitpid(cpid1,NULL, 0);
	printf("main exiting\n");
	return 0;
}