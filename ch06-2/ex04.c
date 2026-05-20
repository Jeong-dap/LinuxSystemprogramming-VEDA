#include <stdio.h>
#include <unistd.h>
#include <signal.h>

int main( void)
{
   sigset_t sigset;
   sigset_t oldset;

   /* 모든 시그널을 블록하고, 현재 마스크를 oldset에 저장 */
   sigfillset( &sigset);
   sigprocmask(SIG_BLOCK, &sigset, &oldset);

   /* sigsuspend용 마스크: 모든 시그널을 블록하되 SIGINT만 허용 */
   sigfillset( &sigset);
   sigdelset( &sigset, SIGINT);
   /* sigsuspend: sigset을 임시 마스크로 교체 후 시그널이 올 때까지 대기
      SIGINT가 도착하면 깨어나고, 반환 시 이전 마스크(oldset)가 자동 복원 */
   sigsuspend( &sigset);

   return 0;
}
