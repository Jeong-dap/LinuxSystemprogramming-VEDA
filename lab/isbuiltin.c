#include <stdio.h>
#include <string.h>
#include <pwd.h>
#include <sys/utsname.h>
#include <unistd.h>
#include <stdlib.h>
#include "minishell.h"

/* 내장 명령어 테이블: 명령어 문자열과 처리 함수를 쌍으로 등록, NULL로 끝을 표시 */
BT_TYPE bt_array[] = {
	"hostname", hostname_func,
	"cd",       cd_func,
	"pwd",      pwd_func,
	NULL,       NULL
};

/* arglist[0]이 내장 명령어이면 해당 함수를 호출하고 1 반환, 아니면 0 반환 */
int check_builtin(char** arglist)
{
	int i;
	for (i = 0; bt_array[i].cmd; i++)
		if (!strcmp(bt_array[i].cmd, arglist[0])) {
			bt_array[i].bt_func(arglist);
			return 1;
		}
	return 0;
}

/* 현재 호스트명을 uname(2)으로 조회하여 출력 */
void hostname_func(char** arg)
{
	struct utsname hostinfo;

	if (uname(&hostinfo) == -1) {
		perror("uname");
		return;
	}
	printf("hostname: %s \n", hostinfo.nodename);
}

/* cd [dir]: 인자가 없으면 HOME으로, 있으면 지정 디렉터리로 이동 */
void cd_func(char** arg)
{
	if (arg[1] == NULL) {
		/* arg[0]: 명령어, arg[1]: 디렉터리명 */
		char *homedir = getenv("HOME");
		chdir(homedir);
	} else {
		if (chdir(arg[1]) == -1) {
			perror("chdir");
			return;
		}
		printf("Success change directory: %s\n", arg[1]);
	}
}

/* 현재 작업 디렉터리를 getcwd(3)으로 조회하여 출력 */
void pwd_func(char** arg)
{
	char path[1024];
	getcwd(path, 1023);
	printf("Pwd: %s", path);
}
