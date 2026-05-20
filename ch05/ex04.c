#include <sys/types.h>
#include <sys/wait.h>  // waitpid(), WNOHANG
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>     // errno, ECHILD

/* WNOHANG 논블로킹 waitpid()로 자식 2개 종료 수집
 *
 * 자식 구성:
 *   c_pid1 : 10초 대기 후 종료 (long time child)
 *   c_pid2 :  1초 대기 후 종료 (short time child)
 *
 * 부모는 루프를 돌며 다른 작업(카운터 증가 + '.' 출력)을 하면서
 * WNOHANG으로 종료된 자식만 즉시 회수한다.
 * 남은 자식이 없으면 errno가 ECHILD로 설정되어 루프를 탈출한다.
 */
int main(void)
{
   pid_t c_pid1, c_pid2, ret_pid;
   int i=0;

   // 첫 번째 자식 생성 (long time)
   switch(c_pid1=fork())
   {
       case -1:
               perror("fork1");
               exit(1);
               break;
       case 0:  // 자식 1
               printf("%d : long time child process\n", getpid());
               sleep(10);  // 10초 대기
               exit(0);
               break;
       default:
               // 첫 번째 fork의 부모 쪽에서 두 번째 자식 생성
               switch(c_pid2=fork()) {
               case -1:
                   perror("fork2");
                   exit(1);
                   break;
               case 0:  // 자식 2
                   printf("%d : short time child process\n",getpid());
                   sleep(1);   // 1초 대기
                   exit(0);
                   break;
               }

               // 부모: 논블로킹 루프로 자식 종료를 수집
               while(1)
               {
                   // WNOHANG: 종료된 자식이 없으면 즉시 0 반환 (블로킹하지 않음)
                   // 종료된 자식이 있으면 그 PID 반환
                   while((ret_pid=waitpid(-1, NULL, WNOHANG))>0)
                   {
                       // -1 : 임의의 자식을 대상으로 함
                       printf("\nreturn value of waitpid() : %d\n", ret_pid);
                   }

                   // 모든 자식이 종료되어 더 이상 자식이 없으면 errno == ECHILD
                   if(errno==ECHILD) {
                       printf("no child\n");
                       break;
                   }

                   // 자식을 기다리는 동안 부모가 다른 작업을 수행하는 것을 시뮬레이션
                   // 10만 번 반복마다 '.' 출력으로 살아있음을 표시
                   if(i==100000) {
               	   putchar('.');
               	   fflush(stdout);
                       i=0;
                   }
                   i++;
               }
          break;
   }
   return 0;
}
