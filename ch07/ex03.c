/* dup()으로 자식의 stdout을 파이프 쓰기 끝으로 리다이렉트한 뒤
 * exec으로 ls를 실행하고, 부모가 그 출력을 파이프로 읽는 예제. */
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

    /* 파이프 생성 */
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

        /* stdout(fd 1)을 닫은 뒤 dup(pd[1])을 호출하면
         * 커널이 가장 낮은 빈 fd(=1)에 pd[1]을 복제한다.
         * 결과적으로 fd 1(stdout)이 파이프 쓰기 끝을 가리키게 된다. */
        close(1);
        dup(pd[1]);   /* 새 fd = 1 (stdout이 파이프로 리다이렉트됨) */
        close(pd[1]); /* 원본 pd[1]은 더 이상 필요 없으므로 닫는다. */

        /* ls의 stdout이 파이프로 연결되어, 출력이 부모에게 전달된다. */
        execlp("ls", "ls", NULL);
        perror("execlp"); /* exec 실패 시에만 도달 */
        exit(1);
        break;

    default:
        /* 부모: 쓰기 끝은 사용하지 않으므로 닫는다. */
        close(pd[1]);
        /* ls 출력을 파이프에서 읽는다. */
        n = read(pd[0], buf, MAXSIZE);
        buf[n] = '\0';
        close(pd[0]);
        printf("PARENT : %s from CHILD\n", buf);
        /* 자식 종료 대기 — 좀비 방지 */
        if (waitpid(pid, (int *)NULL, 0) == -1) {
            perror("exit(1); waitpid");
        }
        break;
    }
}
