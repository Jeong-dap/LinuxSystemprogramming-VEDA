#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>  // chmod(), stat(), struct stat
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    struct stat fstatbuf;  // 파일 메타데이터 구조체

    // 빈 파일 "data" 생성 후 ls -l로 초기 권한 확인
    system("touch data; ls -l data");

    // 현재 "data"의 메타데이터를 fstatbuf에 로드
    stat("data", &fstatbuf);

    printf("\nchmod()\n");
    // 기존 권한에 소유자 실행(S_IXUSR)과 그룹 실행(S_IXGRP) 비트를 추가
    // OR 연산으로 기존 권한은 유지하면서 두 비트만 추가 설정
    chmod("data", fstatbuf.st_mode|S_IXUSR|S_IXGRP);
    system("ls -l data");  // 변경된 권한 확인

    // 하드 링크: data1은 data와 동일한 inode를 가리킴 → 같은 데이터 블록 공유
    link("data", "data1");

    // 심볼릭 링크: data2는 "data"라는 경로 문자열을 저장하는 별도 inode
    symlink("data", "data2");

    printf("\nlink() & symlink()\n");
    // ls -li : inode 번호(-i)와 파일 정보(-l) 함께 출력
    // 하드 링크(data1)는 data와 inode 번호가 같고, 심볼릭 링크(data2)는 다름
    // 주의: "ls-li"는 공백 없이 붙어 있어 실제로는 명령 오류 발생
    system("ls-li data data1 data2");

    return 0;
}
