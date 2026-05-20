#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include "minishell.h"

#define MAXLINE 255

int main(void) {
	char line[MAXLINE];
	char **arglist;

	signal(SIGINT, SIG_IGN);  /* 셸 자신은 Ctrl+C로 종료되지 않도록 SIGINT 무시 */

	/* 백그라운드 자식 종료 시 좀비 방지: SIGCHLD 핸들러 등록 */
	struct sigaction act;
	act.sa_handler = child_handler;
	sigfillset(&act.sa_mask);
	act.sa_flags = SA_RESTART;
	sigaction(SIGCHLD, &act, NULL);
	
	fputs("minishell> ", stdout);

	while (fgets(line, MAXLINE, stdin)) {
		/* "exit" 입력 시 셸 종료 */
		if (!strncmp(line, "exit", 4))
			exit(0);

		if ((arglist = command_parse(line)) != NULL) {
			if (!check_builtin(arglist)) {
				/* 내장 명령어가 아니면 외부 명령어로 실행 */
				is_executable(arglist);
			}
		}
		fputs("minishell> ", stdout);
		command_freelist(arglist);  /* command_parse에서 malloc한 메모리 해제 */
	}
	return 0;
}