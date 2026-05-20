#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* system()으로 쉘 명령어 실행
 * system(cmd)는 내부적으로 fork() → execl("/bin/sh", "sh", "-c", cmd) → wait() 순으로 동작한다.
 * 즉 자식 프로세스에서 쉘을 띄워 명령을 실행하고, 부모는 그 종료를 기다린다.
 * 따라서 system() 호출 중에는 부모 프로세스가 블로킹된다.
 */
int main(void) {
   char cmd_buf[256];

   printf("---------- system() start -------------\n\n");

   strcpy(cmd_buf, "sleep 3");
   system(cmd_buf);  // 쉘에서 sleep 3 실행 → 3초간 블로킹

   strcpy(cmd_buf, "ls");
   system(cmd_buf);  // 쉘에서 ls 실행 → 현재 디렉토리 목록 출력 후 반환

   printf("\n----------  system() end  -------------\n");
   return 0;
}
