/* 익명 파이프(pipe)로 자식→부모 단방향 통신 예제.
 * 자식이 문자열을 쓰고, 부모가 읽어서 출력한다. */
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#define MAXSIZE 4096

int main(void) {

    int pd[2];   /* pd[0]: 읽기 끝, pd[1]: 쓰기 끝 */
    pid_t pid;
    int n;
    char buf[MAXSIZE];

    /* 파이프 생성 — pd[0](읽기)과 pd[1](쓰기) fd 쌍이 만들어진다. */
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
        /* 자식: 읽기 끝은 사용하지 않으므로 닫는다. */
        close(pd[0]);
        strcpy(buf, "Hello, Parent!!");
        /* 쓰기 끝으로 문자열을 부모에게 전송한다. */
        write(pd[1], buf, strlen(buf));
        exit(1);
        break;

    default:
        /* 부모: 쓰기 끝은 사용하지 않으므로 닫는다. */
        close(pd[1]);
        /* 자식이 보낸 데이터를 읽는다. */
        n = read(pd[0], buf, MAXSIZE);
        buf[n] = '\0';
        close(pd[0]);
        printf("PARENT : %s from CHILD\n", buf);
        /* 자식이 종료될 때까지 기다려 좀비 프로세스를 방지한다. */
        if (waitpid(pid, NULL, 0) == -1) {
            perror("waitpid");
            exit(1);
        }
        break;
    }
    return 0;
}
