#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define MAX_STRING_SIZE 80

int main(void)
{
	char *homedir, filename[MAX_STRING_SIZE];
	char date[MAX_STRING_SIZE], input[MAX_STRING_SIZE];
	time_t cur_time;
	struct tm* cur_localtime;
	FILE *fp;

	/* 현재 시각을 가져와 로컬 시간 구조체로 변환 */
	time(&cur_time);
	cur_localtime = localtime(&cur_time);

	/* 오늘 날짜를 YYYY+MM+D 형식 문자열로 생성
	   월은 %02d로 0 패딩, 일은 %d로 패딩 없음 (예: 5월 9일 → "2026059", 5월 19일 → "20260519") */
	sprintf(date, "%d%02d%d", cur_localtime->tm_year + 1900, cur_localtime->tm_mon + 1, cur_localtime->tm_mday);

	/* HOME 환경변수로 홈 디렉토리 경로를 얻고, 날짜를 파일명으로 조합
	   HOME이 설정되지 않으면 getenv가 NULL을 반환해 strcpy에서 크래시됨 */
	homedir = getenv("HOME");
	strcpy(filename, homedir);
	strcat(filename, "/");
	strcat(filename, date);

	/* 홈 디렉토리에 날짜 이름의 파일을 쓰기 모드로 생성 */
	if ((fp = fopen(filename, "w")) == NULL) {
		perror("fopen");
		exit(1);
	}

	/* 사용자 입력을 받아 파일에 기록 */
	printf("User Input: ");
	fgets(input, MAX_STRING_SIZE, stdin);
	fputs(input, fp);

	if (fclose(fp) < 0) {
		perror("fclose");
		exit(1);
	}

	return 0;
}