#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>    // lseek(), read(), close()
#include <fcntl.h>     // open(), O_RDONLY

int main(void)
{
   int fd;
   char buf[256];

   // myhosts 파일을 읽기 전용으로 열기 (ex04에서 생성한 /etc/hosts 복사본)
   if((fd=open("myhosts",O_RDONLY))==-1)
   {
       perror("open");
       exit(1);
   }

   // SEEK_SET: 파일 시작(offset=0)으로부터 10바이트 뒤로 이동
   // 즉 파일 내 절대 위치 10으로 파일 포지션 설정
   lseek(fd, 10L, SEEK_SET);
   read(fd, buf, 10);   // 현재 위치(10)부터 10바이트 읽기
   buf[10]='\0';        // 문자열 끝 마커 추가
   printf("SEEK_SET : %s\n", buf);
   printf("----------------------------\n\n");

   // SEEK_CUR: 현재 파일 포지션(read 후 20)에서 10바이트 추가로 앞으로 이동 → 위치 30
   lseek(fd, 10L, SEEK_CUR);
   read(fd, buf, 10);   // 위치 30부터 10바이트 읽기
   buf[10]='\0';
   printf("SEEK_CUR : %s\n", buf);
   printf("----------------------------\n\n");

   // SEEK_END: 파일 끝(EOF)으로부터 -10바이트 앞으로 이동 → 마지막 10바이트 위치
   lseek(fd, -10L, SEEK_END);
   read(fd, buf, 10);   // 파일 끝 10바이트 읽기
   buf[10]='\0';
   printf("SEEK_END : %s\n", buf);
   close(fd);

   return 0;
}
