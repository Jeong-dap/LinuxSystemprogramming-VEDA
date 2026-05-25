/* 외부 명령어 실행 처리: 백그라운드(&), 파이프(|), 일반 실행을 모두 담당한다. */
#include <stdlib.h>
#include <stddef.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include "minishell.h"

#define MAXSIZE 4096

/* 외부 명령어를 fork()로 실행한다.
 * 파이프(|)가 있으면 두 명령어를 파이프로 연결하고,
 * 백그라운드(&)이면 부모가 waitpid 없이 즉시 반환한다. */
void is_executable(char **arglist) {

    /* 인자 목록 끝에 '&'가 있으면 제거 후 background=1 */
    int background = is_background(arglist);

    /* 인자 목록에 '|'가 있으면 pipe_check=1 */
    int pipe_check = is_pipe(arglist);

    pid_t pid = fork();
    switch (pid) {
    case -1:
        perror("fork");
        exit(-1);
        break;

    case 0:
        /* 자식: Ctrl+C(SIGINT)를 기본 동작(종료)으로 복원한다.
         * 부모 셸은 SIGINT를 무시하므로, exec 후에도 자식이 영향을 받도록 복원이 필요하다. */
        signal(SIGINT, SIG_DFL);

        if (pipe_check) {
            /* ── 파이프 처리 ──
             * '|' 위치를 찾아 arglist를 left / right 두 부분으로 분리한다. */
            int pd[2];
            pid_t pid;

            int pipe_index = -1;
            for (int i = 0; arglist[i] != NULL; i++) {
                if (strcmp(arglist[i], "|") == 0) {
                    pipe_index = i;
                    break;
                }
            }
            arglist[pipe_index] = NULL;         /* '|' 자리를 NULL로 덮어 left 배열 종료 */
            char **left  = arglist;
            char **right = &arglist[pipe_index + 1];

            if (pipe(pd) == -1) {
                perror("pipe");
                exit(1);
            }

            switch (pid = fork()) {
            case -1:
                perror("fork");
                exit(1);
                break;

            case 0:
                /* 손자(left 명령어): stdout을 파이프 쓰기 끝으로 리다이렉트 */
                close(pd[0]);
                dup2(pd[1], 1);     /* pd[1] → stdout */
                close(pd[1]);
                execvp(left[0], left);
                perror("execvp");
                exit(1);
                break;

            default:
                /* 자식(right 명령어): stdin을 파이프 읽기 끝으로 리다이렉트 */
                close(pd[1]);
                dup2(pd[0], 0);     /* pd[0] → stdin */
                close(pd[0]);
                execvp(right[0], right);
                perror("execvp");
                exit(1);
                break;
            }
        }

        /* 파이프 없는 일반 명령어 실행 */
        execvp(arglist[0], arglist);
        perror("execvp");
        exit(1);
        break;

    default:
        /* 부모: 포그라운드이면 자식 종료를 기다리고, 백그라운드이면 즉시 반환한다.
         * 백그라운드 자식의 회수는 SIGCHLD 핸들러(child_handler)가 담당한다. */
        if (!background)
            waitpid(pid, NULL, 0);
        break;
    }
}

/* arglist 끝이 '&'이면 해당 원소를 NULL로 지우고 1 반환, 아니면 0 반환 */
int is_background(char **arglist) {
    for (int i = 0; arglist[i] != NULL; i++) {
        if (arglist[i + 1] == NULL && strcmp(arglist[i], "&") == 0) {
            arglist[i] = NULL;  /* '&'를 배열에서 제거 */
            return 1;
        }
    }
    return 0;
}

/* arglist에 '|'가 하나라도 있으면 1 반환, 아니면 0 반환 */
int is_pipe(char **arglist) {
    for (int i = 0; arglist[i] != NULL; i++) {
        if (strcmp(arglist[i], "|") == 0)
            return 1;
    }
    return 0;
}

int is_redirect(char **arglist) {
    for (int i = 0; arglist[i] != NULL; i++) {
        if (strcmp(arglist[i], ">") == 0)
            return 1;
    }
    return 0;
}
