/* popen()으로 자식 프로세스(grep)에 데이터를 쓰는 예제.
 * 부모가 파이프를 통해 문자열을 전송하면, grep이 'Hello' 포함 행만 출력한다. */
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    FILE *fp;
    int m;

    /* "grep 'Hello'"를 자식으로 실행하고, 쓰기 파이프("w")를 연다.
     * 이후 fp에 쓴 내용이 grep의 stdin으로 전달된다. */
    if ((fp = popen("grep 'Hello'", "w")) == NULL) {
        fprintf(stderr, "popen failed.\n");
        exit(1);
    }

    /* 10회 반복: 'Hello' 행과 'Goodbye' 행을 교대로 전송.
     * grep은 'Hello' 포함 행만 걸러서 터미널에 출력한다. */
    for (m = 1; m <= 10; m++) {
        fprintf(fp, "Hello, pipe!!\n");
        fprintf(fp, "Goodbye, pipe...\n");
    }

    /* 파이프를 닫으면 grep의 stdin이 EOF가 되어 grep이 종료된다. */
    pclose(fp);
    return 0;
}
