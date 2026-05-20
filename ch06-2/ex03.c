#include <stdio.h>
#include <unistd.h>
#include <signal.h>

int main( void)
{
     sigset_t sigset;
     sigset_t pendingset;
     int i;

     /* 모든 시그널을 블록: 10초 동안 수신된 시그널은 pending 상태로 누적 */
     sigfillset( &sigset);
     sigprocmask( SIG_SETMASK, &sigset, NULL);

     printf("waiting for signal...(10 sec.)\n");
     for(i=0; i<10; i++ ) {
          printf( "카운트: %d\n", i);
          sleep( 1);
     }

     /* sigpending: 블록된 상태에서 수신된(pending) 시그널 집합을 조회 */
     if(sigpending(&pendingset)==0)  {
          printf("I got : \n");
          /* sigismember: 해당 시그널이 pending 집합에 있는지 확인 */
          if (sigismember( &pendingset, SIGINT))
               printf( "SIGINT\n");
          if (sigismember( &pendingset, SIGQUIT))
               printf( "SIGQUIT\n");
          if (sigismember( &pendingset, SIGUSR1))
               printf( "SIGUSR1.\n");
          if (sigismember( &pendingset, SIGUSR2))
              printf( "SIGUSR2.\n");
     }
     return 0;
}
