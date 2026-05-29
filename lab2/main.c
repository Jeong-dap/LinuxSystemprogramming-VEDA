#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <bits/types/sigset_t.h>
#include <bits/sigaction.h>
#include <asm-generic/signal-defs.h>

#include "parsing.h"
#include "minishell.h"
#include "is_executable.h"
#include "child_handler.h"

#define MAXLINE 255

int main(void) {
    char line[MAXLINE];
    char **args;

    struct sigaction act;

    act.sa_handler = child_handler;
    sigfillset(&act.sa_mask);
    act.sa_flags = SA_RESTART;
    sigaction(SIGCHLD, &act, NULL);

    signal(SIGINT, SIG_IGN);
    
    print_cwd();

    while (fgets(line, MAXLINE, stdin)) {
        if(!strncmp(line, "exit", 4)) {
            exit(0);
        }

        args = command_parse(line);

        if (args != NULL) {
            if (!__has_builtin__(args)) {
                is_executable(args);
            }
        }

        command_freelist(args);

        print_cwd();
    }
    return 0;
}