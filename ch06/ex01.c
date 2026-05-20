#include <signal.h>
#include <unistd.h>
#include <stdio.h>

int main(void) {
    sigset_t set;

    /* 모든 시그널을 set에 추가 */
    sigfillset(&set);

    /* set에서 SIGINT(Ctrl+C)만 제거 → SIGINT는 수신 허용 */
    sigdelset(&set, SIGINT);

    /* set에서 SIGQUIT(Ctrl+\)도 제거 → SIGQUIT도 수신 허용 */
    sigdelset(&set, SIGQUIT);
    
    /* set을 현재 프로세스의 시그널 마스크로 적용: SIGINT 외 모든 시그널 차단 */
    sigprocmask(SIG_SETMASK, &set, NULL);

    /* SIGINT로만 종료 가능한 무한 루프 */
    while(1);

    return 0;
}
