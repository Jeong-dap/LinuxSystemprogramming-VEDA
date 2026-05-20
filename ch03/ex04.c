#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>     // open(), O_RDONLY, O_WRONLY, O_CREAT, O_TRUNC
#include <sys/stat.h>  // fstat(), struct stat
#include <sys/types.h>
#include <unistd.h>    // read(), write(), close()

int main() {
    int rfd, wfd;      // 읽기용 fd, 쓰기용 fd
    struct stat sb;    // 파일 메타데이터 (크기 조회에 사용)
    char *buf;         // 파일 전체를 담을 동적 버퍼
    ssize_t nread;     // 실제로 읽은 바이트 수 (음수면 에러)

    // /etc/hosts 읽기 전용으로 열기
    if ((rfd = open("/etc/hosts", O_RDONLY)) == -1) {
        perror("open1");
        fprintf(stderr, "File Read Fail.....\n");
        return 1;
    }
    printf("File Read Success!! fd = %d\n", rfd);

    // fstat()으로 열린 fd의 파일 크기(sb.st_size)를 조회해 버퍼 크기 결정
    fstat(rfd, &sb);
    buf = malloc(sb.st_size);
    if (buf == NULL) {
        perror("malloc");
        close(rfd);
        return 1;
    }

    // myhosts를 쓰기 전용으로 열기. 없으면 생성, 있으면 내용 비움(O_TRUNC)
    if ((wfd = open("myhosts", O_WRONLY | O_CREAT | O_TRUNC, 0666)) == -1) {
        perror("open2");
        fprintf(stderr, "File Create Fail.....\n");
        free(buf);
        close(rfd);
        return 1;
    }
    printf("File Creat Success!! fd = %d\n", wfd);

    // /etc/hosts 전체를 한 번에 읽어 buf에 저장
    nread = read(rfd, buf, sb.st_size);
    if (nread == -1) {
        perror("read");
    // 읽은 바이트만큼 write — write 반환값이 nread와 다르면 일부만 쓰인 것(에러)
    } else if (write(wfd, buf, nread) != nread) {
        perror("write");
    } else {
        printf("File Copy Success!!\n");
    }

    free(buf);
    close(rfd);
    close(wfd);
    return 0;
}
