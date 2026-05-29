#pragma once
char** command_parse(char*);
void command_freelist(char**);

void hostname_func(char**);
void cd_func(char**);
void pwd_func(char**);
int check_builtin(char**);

void is_executable(char**);
int is_pipe(char**);
int is_background(char**);

void child_handler(int);

int is_redirect(char **arglist);

typedef struct bt_type {
	char* cmd;
	void(*bt_func)(char**);
} BT_TYPE;
