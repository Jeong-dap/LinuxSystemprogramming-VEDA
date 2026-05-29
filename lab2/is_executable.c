#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include "child_handler.h"

#define MAX_SIZE 4096

int is_background(char **args) {
    int index = 0;
    for (index = 0; args[index] != NULL; index++);

    if (index == 0) {
        return 1;
    }
    else {
        if (!strcmp(args[index - 1], "&")) {
            args[index - 1] = NULL;
            return 0;
        }
    }
    return 1;
}

void is_executable(char **args) {
    pid_t pid;
    int is_foreground = is_background(args);

    pid = fork();

    switch (pid) {
    case -1:
        perror("fork");
        exit(-1);
        break;
    case 0:
        signal(SIGINT, SIG_DFL);

        char **args1 = args, **args2;
        int is_pipe = 0;
        int is_redirection = 0;

        for (int i = 0; args[i] != NULL; i++) {
            if (!is_pipe && !strcmp(args[i], "|")) {
                is_pipe = 1;
                args[i] = NULL;
                args2 = args + i + 1;
                break;
            }

            if (!is_redirection && !strcmp(args[i], ">")) {
                is_redirection = 1;
                args[i] = NULL;
                args2 = args + i + 1;
                break;
            }
        }

        if (!is_pipe && !is_redirection) {
            execvp(args[0], args);
            perror("execvp");
        }
        else if (is_pipe) {
            int pd[2];
            pid_t pid;

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
                close(pd[0]);
                dup2(pd[1], STDOUT_FILENO);
                execvp(args1[0], args1);
                perror("execvp");
                exit(1);
                break;
            default:
                close(pd[1]);
                dup2(pd[0], STDIN_FILENO);
                execvp(args2[0], args2);
                perror("execvp");
                close(pd[0]);
                if (waitpid(pid, NULL, 0) == -1) {
                    perror("waitpid");
                    exit(1);
                }
                break;
            }
        }
        else if (is_redirection) {
            int fd;
            fd = open(args2[0], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            dup2(fd, STDOUT_FILENO);
            close(fd);

            execvp(args1[0], args1);
        }

        exit(-1);
        break;
    default:
        if (is_foreground == 1) {
            waitpid(pid, NULL, 0);
        }
        break;
    }
}