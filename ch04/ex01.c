#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

/* 디바이스 파일에 문자열 쓰기
 * 유닉스에서 디바이스 파일도 일반 파일과 동일하게 open/write/close로 접근 가능함을 보여줌
 * 사용 예: ./ex01 /dev/pts/1  (다른 터미널 세션의 pts 번호를 인자로 전달)
 */
int main(int argc, char *argv[])
{
   char buffer[1024];
   int fd, n;

   if(argc!=2) {
       fprintf(stderr, "Usage : ex01 device_filename\n");
       exit(1);
   }

   // 디바이스 파일을 읽기/쓰기(O_RDWR) 모드로 열기
   // /dev/pts/N 같은 pseudo-terminal 디바이스 파일을 지정하면 해당 터미널에 출력됨
   if((fd=open(argv[1],O_RDWR))==-1) {
       perror("open");
       exit(1);
   }

   strcpy(buffer, "Hello, Terminal\n");
   n=strlen(buffer);

   // 일반 파일 쓰기와 동일한 write() 시스템 콜로 디바이스에 데이터 전송
   write(fd,buffer,n);

   close(fd);
   return 0;
}
