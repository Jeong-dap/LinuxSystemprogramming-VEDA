#pragma once

void func_hostname(char **);
void func_cd(char **);
void func_pwd(char **);
int __has_builtin__(char **);

void print_cwd();

typedef struct btype {
    char *cmd;
    void (*bfunc)(char **);
} BTYPE;