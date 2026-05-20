#include <stdio.h>
#include <string.h>
#include <sys/time.h>  // gettimeofday(), struct timeval
#include <unistd.h>
#include <time.h>      // time(), time_t
int main(void)
{
    time_t cur_time;                  // 1970-01-01 00:00:00(UTC) 기준 경과 초(seconds)
    struct timeval cur_gettimeofday;  // tv_sec(초) + tv_usec(마이크로초) 두 필드를 가진 구조체

    time(&cur_time);  // 현재 시각을 초 단위로 가져와 cur_time에 저장
    printf("time : current seconds = %ld\n", cur_time);

    // 현재 시각을 초 + 마이크로초 단위로 가져옴. 두 번째 인자(timezone)는 사용 안 해서 NULL
    gettimeofday(&cur_gettimeofday, NULL);
    printf("current seconds = %ld, micro seconds = %ld\n",
           cur_gettimeofday.tv_sec,   // 초 단위 타임스탬프 (time()과 동일)
           cur_gettimeofday.tv_usec); // 마이크로초 단위 나머지 (0 ~ 999999)
    return 0;
}