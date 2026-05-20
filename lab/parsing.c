#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* 입력 line을 공백/탭/개행으로 토큰화하여 NULL 종료 문자열 배열로 반환 */
char** command_parse(char *line) {
	int argcount = 0;
	char *token;
	char **arglist;

	if (line == NULL)
		return NULL;

	/* 첫 토큰이 없으면 빈 입력 */
	if ((token = strtok(line, " \t\n")) == NULL)
		return NULL;

	arglist = (char **)malloc(sizeof(char *));

	while (token != NULL) {
		arglist[argcount++] = strdup(token);        /* 토큰 복사 후 저장 */
		token = strtok(NULL, " \t\n");
		/* 다음 토큰을 위한 공간 확보 (+1은 NULL 종료 자리) */
		arglist = (char **)realloc(arglist, (argcount + 1) * sizeof(char *));
	}

	arglist[argcount] = NULL;   /* 배열 끝 표시 */
	return arglist;
}

/* command_parse가 반환한 arglist의 각 문자열과 배열 자체를 해제 */
void command_freelist(char **arglist) {
	int i;
	for (i = 0; arglist[i] != NULL; i++)
		free(arglist[i]);
	free(arglist);
}
