char** command_parse(char*);
void command_freelist(char **);
int check_builtin(char**);
void hostname_func(char**);
void cd_func(char**arg);
void pwd_func(char**arg);
void is_executable(char **);
void child_handler(int);

typedef struct{
	char *cmd;
	void (*bt_func)(char**);
}BT_TYPE;

