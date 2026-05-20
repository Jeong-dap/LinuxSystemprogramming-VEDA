#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(void) {
    char *homedir, filename[80]; // homedir: 환경변수 값 포인터, filename: 전체 파일 경로 저장 버퍼
    FILE *fp;                    // 파일 스트림 포인터

    homedir = getenv("HOME");    // HOME 환경변수에서 홈 디렉토리 경로를 가져옴 (예: /home/user). HOME 미설정 시 NULL 반환 → strcpy 크래시

    strcpy(filename, homedir);   // filename에 홈 디렉토리 경로 복사
    strcat(filename, "/test.log"); // 뒤에 "/test.log" 붙여서 전체 경로 완성 (예: /home/user/test.log)

    // 쓰기 모드("w")로 파일 열기. 실패하면 에러 출력 후 종료
    if((fp=fopen(filename, "w"))==NULL) {
        perror("fopen");
        exit(1);
    }

    fwrite("getenv test\n", 12, 1, fp); // 12바이트짜리 문자열 1개를 파일에 씀
    fclose(fp);                          // 파일 닫기
    return 0;
}