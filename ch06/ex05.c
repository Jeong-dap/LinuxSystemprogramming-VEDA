/* kill()로 특정 PID에 SIGTERM 시그널을 전송하는 예제.
 *
 * kill() 에러 코드:
 *   EINVAL : 시그널 번호가 유효하지 않음
 *   EPERM  : 대상 프로세스에 신호를 보낼 권한 없음 (다른 사용자의 프로세스 등)
 *   ESRCH  : 해당 PID의 프로세스가 존재하지 않음
 *
 * 사용법: ./ex05 <pid>
 */
#include <signal.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
int main(int argc, char *argv[]) {
    if(argc<2) {
        fprintf(stderr, "Usage : ex05 pid\n");
        exit(1);
    }

    /* atoi()로 문자열 PID를 정수로 변환한 뒤 SIGTERM 전송 */
    if(kill(atoi(argv[1]), SIGTERM)==-1) {
        if(errno==EINVAL)
            fprintf(stderr, "Invalid Signal\n");
        else if(errno==EPERM)
            fprintf(stderr, "No Permission\n");
        else if(errno==ESRCH)
            fprintf(stderr, "no search process\n");
    }
    return 0;
}
