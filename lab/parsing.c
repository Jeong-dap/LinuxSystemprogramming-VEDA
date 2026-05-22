/* 입력 라인을 토큰 배열로 분리하고, 사용 후 메모리를 해제하는 유틸리티. */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* 입력 line을 공백/탭/개행으로 토큰화하여 NULL 종료 문자열 배열로 반환.
 * 빈 입력이면 NULL을 반환한다. 반환된 배열은 command_freelist()로 해제해야 한다. */
char **command_parse(char *line) {
    int argcount = 0;
    char *token;
    char **arglist;

    if (line == NULL)
        return NULL;

    /* 첫 토큰이 없으면 빈 입력이므로 NULL 반환 */
    if ((token = strtok(line, " \t\n")) == NULL)
        return NULL;

    arglist = (char **)malloc(sizeof(char *));

    while (token != NULL) {
        arglist[argcount++] = strdup(token);    /* 토큰 문자열 복사 후 저장 */
        token = strtok(NULL, " \t\n");
        /* 다음 토큰 자리 + NULL 종료 자리를 확보하기 위해 재할당 */
        arglist = (char **)realloc(arglist, (argcount + 1) * sizeof(char *));
    }

    arglist[argcount] = NULL;   /* execvp 등이 요구하는 NULL 종료 */
    return arglist;
}

/* command_parse가 반환한 arglist의 각 문자열과 배열 자체를 해제한다. */
void command_freelist(char **arglist) {
    int i;
    for (i = 0; arglist[i] != NULL; i++)
        free(arglist[i]);
    free(arglist);
}
