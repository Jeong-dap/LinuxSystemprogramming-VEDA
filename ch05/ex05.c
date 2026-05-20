#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>  // wait(), WIFEXITED(), WEXITSTATUS()
#include <unistd.h>

/* 자식 프로세스의 종료 코드(exit code) 부모에서 읽기
 *
 * WEXITSTATUS(status): WIFEXITED()가 참일 때만 유효
 *                      exit()에 전달된 인자값(0~255)을 추출한다.
 *
 * 현재 자식은 exit(1)로 종료하므로 부모는 exit code = 1을 출력한다.
 * exit(0) 줄을 주석 해제하고 exit(1)을 주석 처리하면 exit code = 0이 출력된다.
 */
int main(void)
{
   int status;

   switch(fork())
   {
       case -1:
               perror("fork");
               exit(1);
               break;

       case 0:  // 자식 프로세스
               sleep(3);
               //exit(0);   // 성공 종료 코드 (0)
               exit(1);     // 실패 종료 코드 (1) — 현재 활성화
               break;

       default:  // 부모 프로세스
               wait(&status);  // 자식이 종료될 때까지 블로킹

               // WIFEXITED: 자식이 exit()로 정상 종료했는지 확인
               if(WIFEXITED(status)) {
                   // WEXITSTATUS: status에서 exit() 인자값(종료 코드)만 추출
                   printf("exit code of child process : %d\n", WEXITSTATUS(status));
               }
   }
}
