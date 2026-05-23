/* sigaction()으로 SIGINT 핸들러를 등록하고, 수신 횟수를 카운트하는 예제.
 *
 * sigaction()의 주요 필드:
 *   sa_handler : 시그널 핸들러 함수 포인터
 *   sa_mask    : 핸들러 실행 중 추가로 차단할 시그널 집합
 *   sa_flags   : SA_RESTART — 시그널로 인해 중단된 시스템 콜을 자동 재시작
 *
 * 10번 반복 후 원래 핸들러(oldact)로 복원하고 count를 출력한다. */
#include <signal.h>
#include <unistd.h>
#include <stdio.h>

int count = 0;  /* SIGINT 수신 횟수 */

/* SIGINT 핸들러: 호출될 때마다 count 증가 */
void handler(int signo)
{
    count++;
}

int main() {
    int i = 0;
    struct sigaction act, oldact;

    act.sa_handler = handler;
    sigemptyset(&act.sa_mask);
    sigaddset(&act.sa_mask, SIGINT);   /* 핸들러 실행 중 SIGINT 추가 차단 */
    sigaddset(&act.sa_mask, SIGQUIT);  /* 핸들러 실행 중 SIGQUIT 추가 차단 */
    act.sa_flags=SA_RESTART;
    sigaction(SIGINT, &act, &oldact);  /* SIGINT 핸들러 등록, 이전 핸들러를 oldact에 저장 */

    while(i<10)
    {
        printf("signal test\n");
        sleep(2);
        i++;
    }

    sigaction(SIGINT, &oldact, NULL);  /* 10회 반복 후 원래 핸들러 복원 */
    printf("count=%d\n", count);       /* SIGINT를 받은 총 횟수 출력 */
    while(1);  /* 복원된 핸들러(기본: 종료)로 Ctrl+C 시 종료 */
}
