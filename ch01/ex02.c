#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <stdlib.h>
int main(void) {
    time_t* cur_time=(time_t*) malloc(sizeof(time_t));  // 힙에 time_t 크기만큼 할당
    time(cur_time);                                      // 현재 시각을 유닉스 타임스탬프(초)로 저장
    printf("Current Time = %d\n", (int) *cur_time);     // 타임스탬프 정수값 출력

    char* cur_string;
    // ctime(): time_t를 "Www Mmm DD HH:MM:SS YYYY\n" 형식의 문자열로 변환
    // 반환 포인터는 내부 정적 버퍼를 가리키므로 별도 free 불필요
    cur_string = ctime(cur_time);
    printf("Current Time String = %s\n", cur_string);   // 사람이 읽기 쉬운 날짜/시각 문자열 출력

    return 0;
}