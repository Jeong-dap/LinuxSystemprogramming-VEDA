#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>    // opendir(), readdir(), closedir(), DIR, struct dirent
#include <sys/stat.h>  // stat(), struct stat, S_ISDIR()

/* 디렉토리 엔트리 목록 출력 (간이 ls -i 구현)
 * 인자로 받은 디렉토리의 모든 항목(. 과 .. 포함)의 inode 번호와 파일명 출력
 */
int main(int argc, char *argv[])
{
   DIR *dirp;             // 디렉토리 스트림 포인터 (열린 디렉토리 핸들)
   struct dirent *dirent; // 디렉토리 엔트리 1개를 담는 구조체 (d_ino, d_name 등)
   struct stat fstat;     // 파일 메타데이터 구조체 (디렉토리 여부 확인용)

   if(argc!=2) {
       fprintf(stderr, "Usage: mydir directoryname\n");
       exit(1);
   }

   // stat()으로 인자 경로의 메타데이터 조회 후 디렉토리인지 확인
   stat(argv[1], &fstat);
   if(!S_ISDIR(fstat.st_mode)) {
       fprintf(stderr, "%s is not Directory\n", argv[1]);
       exit(1);
   }

   // 디렉토리 열기. 성공하면 DIR* 스트림 반환, 실패하면 NULL
   if((dirp=opendir(argv[1]))==NULL) {
       perror("opendir");
       exit(1);
   }

   // readdir()은 호출할 때마다 다음 엔트리를 반환하고 끝에 도달하면 NULL 반환
   // d_ino: 해당 파일의 inode 번호, d_name: 파일명 문자열
   while ((dirent=readdir(dirp))!=NULL) {
       printf("inode = %ld, filename = %s\n", dirent->d_ino, dirent->d_name);
   }

   closedir(dirp);  // 디렉토리 스트림 닫기 (내부 자원 해제)
   return 0;
}
