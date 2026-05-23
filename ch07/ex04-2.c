/* Named FIFO(이름 있는 파이프) 수신자 — ex04-1과 쌍을 이루는 예제.
 *
 * open(O_RDONLY)는 쓰기 측이 열기 전까지 블로킹된다.
 * 쓰기 측이 close()하면 read()가 0을 반환(EOF)하여 루프가 종료된다.
 *
 * 흐름: mkfifo → open(O_RDONLY) → 루프(read → write to stdout) → close
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
   int pd, n = 0;
   char rcvmsg[1024];

   /* FIFO 파일 생성. 이미 존재(EEXIST)하면 무시 */
   if(mkfifo("./myfifo", 0600)==-1) {
	   if(errno!=EEXIST) {
		   perror("mkfifo");
		   exit(1);
	   }
   }

   /* O_RDONLY: 쓰기 측이 열기 전까지 블로킹 */
   if((pd=open("./myfifo", O_RDONLY)) == -1 ) {
       perror("open");
       exit(1);
   }

   /* read() 반환값: 양수=읽은 바이트, 0=EOF(쓰기 측 종료), -1=에러 */
   while((n=read(pd, rcvmsg, 1023))>0) {
       rcvmsg[n]='\0';
       write(1,"namedpipe2> ", 12);  /* 프롬프트 출력 */
       write(1, rcvmsg, n );         /* 수신 메시지 출력 */
   }

   if(n==-1) {
       perror("read");
       exit(1);
   }

   write(1,"\n", 1 );
   close(pd);
   return(0);
}
