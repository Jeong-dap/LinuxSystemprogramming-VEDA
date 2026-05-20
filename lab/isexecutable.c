#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include "minishell.h"

int is_background(char** arglist);

/* 외부 명령어를 fork/exec으로 실행하는 함수 */
void is_executable(char** arglist) {
    pid_t pid;
    int backflags = 0;

    backflags = is_background(arglist);

    /* 자식 프로세스 생성 */
    pid = fork();

    switch(pid) {
        case -1:
            /* fork 실패 */
            perror("fork()");
            exit(-1);
            break;

        case 0:
            signal(SIGINT, SIG_DFL);  /* 부모가 무시한 SIGINT를 자식에서는 기본 동작으로 복원 */
            /* 자식: arglist[0] 명령어로 프로세스 이미지 교체 */
            execvp(arglist[0], arglist);
            /* execvp가 반환하면 실행 실패 */
            perror("execvp");
            exit(0);
            break;

        default: /* 부모 */
            if (backflags == 0)
                waitpid(pid, NULL, 0);  /* 포그라운드: 자식 종료까지 블로킹 대기 */
            /* 백그라운드(backflags==1): 대기하지 않고 즉시 복귀, SIGCHLD 핸들러가 회수 */
            break;
    }
}

/* arglist 마지막 토큰이 "&"이면 제거 후 1 반환, 아니면 0 반환 */
int is_background(char** arglist) {
    int i;
    for (i = 0; arglist[i] != NULL; i++);  /* 배열 끝 인덱스 탐색 */
    if (!strcmp(arglist[i-1], "&")) {
        arglist[i-1] = NULL;  /* "&" 제거하여 exec에 넘기지 않도록 */
        return 1;
    }
    return 0;
}