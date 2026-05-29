/* Named FIFO(이름 있는 파이프) 송신자 — ex04-2와 쌍을 이루는 예제.
 *
 * FIFO는 파일 시스템에 경로명을 가진 파이프로, 혈연관계 없는 프로세스 간 통신에 사용된다.
 * open()이 상대방(읽기 쪽)이 열기 전까지 블로킹되므로
 * ex04-2(수신자)를 먼저 실행하거나 동시에 실행해야 한다.
 *
 * 흐름: mkfifo → open(O_WRONLY) → 루프(사용자 입력 → write) → close
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

int main(void) {
   int pd, n;
   char msg[1024];

   /* FIFO 파일 생성. 이미 존재(EEXIST)하면 무시하고 계속 진행 */
   if(mkfifo("./myfifo", 0600)==-1) {
	   if(errno!=EEXIST) {
		   perror("mkfifo");
		   exit(1);
	   }
   }

   /* O_WRONLY: 읽기 측이 열기 전까지 이 open()은 블로킹됨 */
   if((pd=open("./myfifo", O_WRONLY)) == -1) {
       perror("open");
       exit(1);
   }

   while(1) {
       printf("namedpipe1> ");
       fgets(msg, 1023, stdin);
       if(strncmp(msg,"quit",4)==0)  /* "quit" 입력 시 루프 탈출 */
           break;
       if((n=write(pd, msg, strlen(msg)))==-1) {
           perror("write");
           exit(1);
       }
   }
   close(pd);
   return(0);
}
