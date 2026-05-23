/* sigaction()의 sa_mask를 이용한 시그널 마스킹 데모.
 *
 * sigfillset(&act.sa_mask): 핸들러 실행 중 모든 시그널을 차단한다.
 * int_handler가 실행되는 5초 동안 Ctrl+C를 눌러도 시그널이 누적(pending)되고,
 * 핸들러가 끝난 뒤 한 번만 처리된다. */
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

/* SIGINT 핸들러: 실행에 5초가 걸리는 긴 핸들러 */
void int_handler(int signo)
{
   int i;

   printf( "int_handler() start.\nPlease waiting for 5 secs!!\n");
   for (i=0; i<5; i++ ){
      printf( "%d\n", i+1);
      sleep(1);
   }
   printf("int_handler() end\n");
}

int main(void)
{
   struct sigaction act;

   act.sa_handler = int_handler;
   sigfillset(&act.sa_mask);  /* 핸들러 실행 중 모든 시그널 차단 */

   sigaction( SIGINT, &act, NULL);
   while(1) {
	  printf("sigaction() : signal mask test\n");
      sleep(1);
   }
}
