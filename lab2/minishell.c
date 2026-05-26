#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/utsname.h>
#include <unistd.h>
#include "minishell.h"

BTYPE builtin_list[] = {
    "hostname", func_hostname,
    "cd", func_cd,
    "pwd", func_pwd,
    NULL, NULL
};

int __has_builtin__(char **args) {
    for (int i = 0; builtin_list[i].cmd != NULL; i++) {
        if (strcmp(builtin_list[i].cmd, args[0]) == 0) {
            builtin_list[i].bfunc(args);
            return 1;
        }
    }
    return 0;
}

void func_hostname(char **args) {
    struct utsname hostinfo;

    if (uname(&hostinfo) == -1) {
        perror("uname");
        return;
    }
    
    printf("hostname: %s\n", hostinfo.nodename);
}

/**
 * cd dir_name
 * cd
 * chdir()
 */
void func_cd(char **args) {
    if (args[2] != NULL) {
        fprintf(stderr, "cd: too many arguments\n");
        return;
    }

    if (args[1] == NULL) {
        chdir(getenv("HOME"));
    }
    else {
        if (chdir(args[1]) == -1) {
            perror("chdir");
        }
    }
}

/**
 * pwd
 * getcwd()
 */
void func_pwd(char **args) {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("getcwd");
    }
    else {
        fputs(cwd, stdout);
    }
}

void print_cwd() {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("getcwd");
    }
    else {
        strcat(cwd, "> ");
        fputs(cwd, stdout);
    }
}