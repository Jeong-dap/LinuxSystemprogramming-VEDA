/* SIGUSR1/SIGUSR2 수신 횟수를 카운트하고, SIGINT(Ctrl+C) 시 결과를 출력하는 예제.
 *
 * 시그널 마스크 설정:
 *   SIGINT, SIGUSR1, SIGUSR2 만 허용하고 나머지는 차단한다.
 *
 * 테스트 방법:
 *   kill -USR1 <pid>  또는  kill -USR2 <pid>  로 시그널 전송
 *   Ctrl+C로 카운트 확인 후 종료
 */

// 아래는 이전 버전 코드 (주석 처리됨)
// #include <stdio.h>
// #include <unistd.h>
// #include <signal.h>

// int count1 = 0;
// int count2 = 0;

// void usr1_handler(int sig)
// {
//     count1++;
// }
// void usr2_handler(int sig)
// {
//    count2++;
// }

// int main(void) {
//     struct sigaction usr1, usr2;

//     usr1.sa_handler=usr1_handler;
//     sigfillset(&usr1.sa_mask);
//     usr1.sa_flags=SA_RESTART;

//     usr2.sa_handler=usr2_handler;
//     sigfillset(&usr2.sa_mask);
//     usr2.sa_flags=SA_RESTART;

//     sigset_t set1, set2;

//     sigaction(SIGUSR1, &usr1, NULL);
//     sigaction(SIGUSR2, &usr2, NULL);

//     return 0;
// }

#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int usr1_count = 0;  /* SIGUSR1 수신 횟수 */
int usr2_count = 0;  /* SIGUSR2 수신 횟수 */

/* SIGUSR1 핸들러 */
void usr1_handler(int signo) {
    usr1_count++;
}

/* SIGUSR2 핸들러 */
void usr2_handler(int signo) {
    usr2_count++;
}

/* SIGINT 핸들러: Ctrl+C 시 카운트 출력 */
void int_handler(int signo) {
    printf("usr1 count: %d\n", usr1_count);
    printf("usr2 count: %d\n", usr2_count);
}

int main() {
    sigset_t sigset;

    /* SIGINT, SIGUSR1, SIGUSR2를 제외한 모든 시그널 차단 */
    sigfillset(&sigset);
    sigdelset(&sigset, SIGINT);
    sigdelset(&sigset, SIGUSR1);
    sigdelset(&sigset, SIGUSR2);
    sigprocmask(SIG_BLOCK, &sigset, NULL);

    struct sigaction act;
    act.sa_handler = int_handler;
	sigfillset(&act.sa_mask);
	act.sa_flags = SA_RESTART;
	sigaction(SIGINT, &act, NULL);

    act.sa_handler = usr1_handler;
	sigaction(SIGUSR1, &act, NULL);

    act.sa_handler = usr2_handler;
	sigaction(SIGUSR2, &act, NULL);

    while(1) {
        sleep(1);
    }
}
