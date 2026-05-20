#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>   // tcgetattr(), tcsetattr(), struct termios

#define CR '\n'  // Enter 키 = Carriage Return(줄바꿈 문자)

/* 터미널 raw 모드로 비밀번호 입력 흉내내기
 * 입력한 문자는 화면에 표시되지 않고 '*'로 대체 출력됨
 * Enter 키 입력 시 종료하고 터미널 설정을 원래대로 복구
 */
int main() {
    struct termios tbuf, oldtbuf;
    char ch;

    // 현재 터미널(fd=0, stdin) 설정을 tbuf에 저장
    // 이후 원복을 위해 oldtbuf에도 백업
    if (tcgetattr(0, &tbuf) == -1) {
        perror("tcgetattr");
        exit(1);
    }
    oldtbuf = tbuf;  // 원래 설정 백업 (종료 시 복구용)

    // ICANON 해제: 기본적으로 터미널은 Enter 전까지 입력을 버퍼링(canonical 모드)하지만,
    // 이를 끄면 입력한 문자를 즉시 read/getchar로 받을 수 있음
    tbuf.c_lflag &= ~ICANON;

    // ECHO 해제: 입력한 문자가 터미널에 그대로 출력되는 것을 막음
    tbuf.c_lflag &= ~ECHO;

    // VMIN=1: 최소 1바이트가 들어올 때까지 read() 블로킹
    // VTIME=0: 타임아웃 없음 (무한 대기)
    tbuf.c_cc[VMIN] = 1;
    tbuf.c_cc[VTIME] = 0;

    // 변경된 설정 적용. TCSAFLUSH: 현재 버퍼의 입출력을 모두 비운 뒤 설정 적용
    if (tcsetattr(0, TCSAFLUSH, &tbuf) == -1) {
        perror("tcsetattr");
        exit(1);
    }

    while (1)
    {
        ch = getchar();  // echo 없이 문자 1개 즉시 수신

        if (ch == CR)    // Enter 입력 시 루프 탈출
            break;

        printf("*");     // 실제 문자 대신 '*' 출력 (비밀번호 마스킹 효과)
        fflush(stdout);  // 버퍼링 없이 즉시 화면에 반영
    }

    // 터미널 설정을 원래 상태(canonical + echo)로 복구
    if (tcsetattr(0, TCSAFLUSH, &oldtbuf) == -1) {
        perror("tcsetattr");
        exit(1);
    }

    return 0;
}
