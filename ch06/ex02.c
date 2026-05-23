/* signal()로 SIGINT(Ctrl+C) 핸들러를 등록하는 기본 예제.
 *
 * signal()은 구현이 간단하지만 이식성이 낮고 재진입 문제가 있어
 * 실제 코드에서는 sigaction()을 권장한다.
 * 핸들러 내 write()는 async-signal-safe 함수라 안전하다. */
#include <signal.h>
#include <unistd.h>
#include <stdio.h>

/* SIGINT 핸들러: Ctrl+C 입력 시 호출되어 메시지를 출력 */
void handler(int signo)
{
    write(1, "Control_C\n", 10);  /* printf 대신 async-signal-safe 함수 사용 */
}

int main() {
    signal(SIGINT, handler);  /* SIGINT를 handler 함수로 처리 */
    while(1)
    {
        printf("signal test\n");
        sleep(2);
    }
    return 0;
}
