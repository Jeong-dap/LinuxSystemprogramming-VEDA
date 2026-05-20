#include <sys/types.h>
#include <sys/stat.h>  // stat(), struct stat, S_IS* 매크로
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
   struct stat fstatbuf;  // 파일 메타데이터(inode, 권한, 크기 등)를 담는 구조체

   if(argc!=2) {
	   fprintf(stderr, "Usage : ex01 filename\n");
	   exit(1);
   }

   // stat()으로 argv[1] 경로의 파일 메타데이터를 fstatbuf에 저장
   // 실패(파일 없음, 권한 없음 등) 시 -1 반환 → errno 설정
   if(stat(argv[1],&fstatbuf)==-1) {
	   perror(argv[1]);
	   exit(1);
   }

   // st_mode 필드에 파일 종류 비트가 함께 저장됨
   // S_ISDIR / S_ISREG / S_ISCHR / S_ISBLK 매크로로 종류 판별
   if(S_ISDIR(fstatbuf.st_mode))
       printf("%s is Directory\n", argv[1]);
   else if(S_ISREG(fstatbuf.st_mode))
       printf("%s is Regular File\n", argv[1]);
   else if(S_ISCHR(fstatbuf.st_mode))
       printf("%s is Character Device File\n", argv[1]);
   else if(S_ISBLK(fstatbuf.st_mode))
       printf("%s is Block Device File\n", argv[1]);

   // st_mode와 권한 비트 마스크를 AND 연산해 소유자(USR) 권한만 출력
   // S_IRUSR=0400, S_IWUSR=0200, S_IXUSR=0100
   printf("Permission :");
   if(fstatbuf.st_mode & S_IRUSR)
       putchar('r');
   if(fstatbuf.st_mode & S_IWUSR)
       putchar('w');
   if(fstatbuf.st_mode & S_IXUSR)
       putchar('x');
   putchar('\n');

   // st_ino: inode 번호, st_uid: 소유자 UID, st_size: 파일 크기(바이트)
   printf("inode = %d, uid = %d, size = %d\n",\
       fstatbuf.st_ino, fstatbuf.st_uid, fstatbuf.st_size);
   return 0;
}
