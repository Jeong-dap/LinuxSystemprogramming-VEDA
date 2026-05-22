/* minishell 진입점.
 * 프롬프트를 반복 출력하며 사용자 입력을 받고, 내장 명령어 또는
 * 외부 실행 파일을 분기하여 처리한다. */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include "minishell.h"

#define MAXLINE 255

int main(void) {
    char line[MAXLINE];
    char **arglist;

    /* Ctrl+C(SIGINT)를 셸 자신은 무시한다.
     * 자식 프로세스는 기본 동작(종료)으로 복원되어 Ctrl+C에 반응한다. */
    signal(SIGINT, SIG_IGN);

    /* 자식이 종료될 때 발생하는 SIGCHLD를 child_handler로 처리한다.
     * SA_RESTART: 시그널 처리 후 블로킹 syscall을 자동 재시작.
     * sigfillset: 핸들러 실행 중 다른 시그널을 모두 차단해 재진입 방지. */
    struct sigaction act;
    act.sa_handler = child_handler;
    sigfillset(&act.sa_mask);
    act.sa_flags = SA_RESTART;
    sigaction(SIGCHLD, &act, NULL);

    fputs("minishell> ", stdout);

    while (fgets(line, MAXLINE, stdin)) {

        /* "exit"로 시작하면 셸을 종료한다. */
        if (!strncmp(line, "exit", 4))
            exit(0);

        /* 입력 라인을 토큰 배열로 파싱한다. */
        arglist = command_parse(line);

        /* 빈 입력이면 프롬프트만 다시 출력하고 계속한다. */
        if (arglist == NULL) {
            fputs("minishell> ", stdout);
            continue;
        }

        /* 내장 명령어가 아니면 외부 실행 파일로 처리한다. */
        if (!check_builtin(arglist))
            is_executable(arglist);

        fputs("minishell> ", stdout);
        command_freelist(arglist);
    }
    return 0;
}
