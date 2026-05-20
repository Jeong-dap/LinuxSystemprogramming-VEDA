#include <sys/types.h>
#include <sys/resource.h>  // getrlimit(), setrlimit(), struct rlimit
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

void openfile_test(void);

int main(void)
{
   struct rlimit mylimit;  // rlim_cur(소프트 한도), rlim_max(하드 한도) 필드를 가진 구조체

   // 현재 프로세스가 생성할 수 있는 최대 프로세스 수 조회
   getrlimit(RLIMIT_NPROC, &mylimit);
   printf("Current Number of Process : softlimit=%d, hardlimit=%d\n",
           mylimit.rlim_cur, mylimit.rlim_max);

   // 현재 프로세스가 열 수 있는 최대 파일 디스크립터 수 조회
   getrlimit(RLIMIT_NOFILE, &mylimit);
   printf("\nCurrent Number of File : softlimit=%d, hardlimit=%d\n",
           mylimit.rlim_cur, mylimit.rlim_max);

   // 소프트 한도를 5로 낮춤. 프로세스 시작 시 fd 0(stdin)/1(stdout)/2(stderr) 3개가 이미 열려 있으므로
   // 실제로 추가로 열 수 있는 파일은 2개(fd 3, fd 4)뿐
   mylimit.rlim_cur = 5;
   setrlimit(RLIMIT_NOFILE, &mylimit);
   openfile_test();  // 한도 초과 시 fopen 실패 확인
   return 0;
}

// 파일 6개를 순서대로 열어 한도 초과 시점에 에러를 발생시키는 테스트 함수
void openfile_test(void)
{
   char *filename[6] = { "test1", "test2", "test3", "test4", "test5", "test6" };
   FILE *fp;
   int i;

   for(i=0; i<6; i++)
   {
       // 소프트 한도(5) 도달 후 fopen 실패 → "testN: Too many open files" 출력 후 종료
       if((fp=fopen(filename[i], "w"))==NULL) {
           perror(filename[i]);
           exit(1);
       }
   }
   return;
}
