#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>
#include <time.h>

/* SIGALRM 핸들러: 인터벌 타이머가 만료될 때마다 현재 시각 출력 */
void interval_handler(int sig)
{
   printf("<interval handler> called : %ld\n", time(NULL));
}

int main(void)
{
	struct sigaction act;
	struct itimerval itimer;

	/* SIGALRM 핸들러 등록 */
	act.sa_handler=interval_handler;
	sigfillset(&act.sa_mask);
	act.sa_flags=SA_RESTART;
	sigaction(SIGALRM, &act, NULL);

	printf("<main> Current Time : %ld\n", time(NULL));

	memset(&itimer, 0, sizeof(itimer));
	itimer.it_value.tv_sec=5;       /* 최초 알람: 5초 후 SIGALRM 발생 */
	itimer.it_interval.tv_sec=2;    /* 이후 반복 주기: 2초마다 SIGALRM 발생 */
	/* ITIMER_REAL: 실제 경과 시간 기준 타이머 (벽시계 시간) */
	setitimer(ITIMER_REAL, &itimer, NULL);

	while(1)
		pause();    /* 시그널마다 핸들러 실행 후 다시 대기 */
	return 0;
}


