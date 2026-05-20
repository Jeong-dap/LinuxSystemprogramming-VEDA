#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <sys/times.h>  // times(), struct tms
#include <unistd.h>     // sysconf()

int main(void) {
    int i;
    time_t c_time;
    clock_t oldtime, newtime;  // times() 반환값: 부팅 후 경과 틱(tick) 수
    struct tms oldtms, newtms; // tms_utime(유저 시간), tms_stime(시스템 시간) 등을 담는 구조체
    long clk_tck;

    // 1초당 틱(tick) 수 조회. 보통 100 (즉 1틱 = 0.01초)
    clk_tck = sysconf(_SC_CLK_TCK);

    // 작업 시작 전 시간 측정. oldtms에 CPU 사용 시간 스냅샷 저장
    if ((oldtime = times(&oldtms)) == -1) {
        perror("old times");
        exit(1);
    }

    // CPU를 많이 쓰는 작업: time()을 9999만 9999번 반복 호출
    for (i = 1; i <= 99999999; i++)
        time(&c_time);

    // 작업 종료 후 시간 측정. newtms에 CPU 사용 시간 스냅샷 저장
    if ((newtime = times(&newtms)) == -1) {
        perror("new times");
        exit(1);
    }

    // (틱 차이 / 초당 틱 수) = 경과 초
    printf("Real Time : %.3f sec\n",        (double)(newtime - oldtime) / clk_tck);             // 실제 경과 시간(벽시계)
    printf("User mode Time : %.3f sec\n",   (double)(newtms.tms_utime - oldtms.tms_utime) / clk_tck); // 유저 공간에서 CPU를 쓴 시간
    printf("System mode Time : %.3f sec\n", (double)(newtms.tms_stime - oldtms.tms_stime) / clk_tck); // 커널(시스템 콜 등)에서 CPU를 쓴 시간

    return 0;
}