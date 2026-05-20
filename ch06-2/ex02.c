#include <signal.h>
#include <unistd.h>
#include <stdio.h>


int main(void) {
   sigset_t set1, set2;

   sigfillset(&set1);           /* set1: 모든 시그널 포함 */
   sigemptyset(&set2);          /* set2: 빈 집합으로 초기화 */
   sigaddset(&set2, SIGINT);    /* set2: SIGINT만 추가 */

   sigprocmask(SIG_BLOCK, &set1, NULL);    /* 모든 시그널 블록: sleep 중 Ctrl+C 등 무시 */

   printf("signal block start \n");
   sleep(5);   /* 5초 동안 어떤 시그널도 처리되지 않음 */

   sigprocmask(SIG_UNBLOCK, &set2, NULL);  /* SIGINT만 해제: 이후 Ctrl+C로 종료 가능 */

   while(1) {
       printf("Hello Signal\n");
       sleep(2);
   }

   return 0;
}
