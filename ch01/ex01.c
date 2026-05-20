#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <stdlib.h>

int main(void) {
    // time_t 크기만큼 힙 메모리 할당
    time_t* cur_time = (time_t*)malloc(sizeof(time_t));

    time(cur_time);  // 현재 시각을 유닉스 타임스탬프(초)로 가져와 저장
    printf("Current Time = %d\n", (int) *cur_time);  // 타임스탬프 정수값 출력

    free(cur_time);  // 힙 메모리 해제
    return 0;
}
