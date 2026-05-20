#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>

/* alarm_handler가 호출되었는지 기록하는 플래그 */
static int alarm_flag=0;

/* SIGALRM 핸들러: 플래그만 세우고 즉시 반환 (printf 등 비재진입 함수 사용 금지) */
void alarm_handler(int sig)
{
   alarm_flag=1;
}

int main(void)
{
	struct sigaction act;

	/* SIGALRM 핸들러 등록 */
	act.sa_handler=alarm_handler;
	sigfillset(&act.sa_mask);
	act.sa_flags=SA_RESTART;
	sigaction(SIGALRM, &act, NULL);

	alarm(5);       /* 5초 후 SIGALRM 발생 예약 */
	pause();        /* 시그널이 올 때까지 블로킹 대기 */
	if(alarm_flag)  /* SIGALRM에 의해 깨어난 경우 */
		printf("ALARM\n");
	return 0;
}
