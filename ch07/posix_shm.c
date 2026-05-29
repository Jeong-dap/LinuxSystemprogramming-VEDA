/* POSIX 공유 메모리(shm_open + mmap) 인터랙티브 데모.
 *
 * System V shmget/shmat 대신 POSIX API를 사용한다.
 *   shm_open : /dev/shm/<name> 아래 파일처럼 관리되는 공유 메모리 객체 생성/열기
 *   ftruncate: 공유 메모리 객체의 크기 설정 (최초 생성 시 반드시 필요)
 *   mmap     : 공유 메모리 객체를 현재 프로세스 주소 공간에 매핑
 *   munmap   : 주소 공간에서 분리
 *   shm_unlink: 공유 메모리 객체 이름 제거 (커널에서 완전 삭제는 마지막 매핑 해제 후)
 *
 * 빌드: gcc -o posix_shm posix_shm.c -lrt
 * 다른 터미널에서 같은 프로그램을 실행하면 공유 메모리를 공유할 수 있다.
 *
 * 사용법:
 *   '*'  : 현재 공유 메모리 내용 출력
 *   '.'  : 종료
 *   기타 : 입력 문자열을 공유 메모리에 기록
 */
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define NAME_POSIX_SHM  "/mytest"      /* POSIX 공유 메모리 이름 (/dev/shm/mytest) */
#define SZ_SHM_SEGMENT  1024           /* 공유 메모리 크기 (바이트) */

int shm_fd;
char *shm_ptr;

int main()
{
	char input[256];

	printf("* SHM Name : %s\n", NAME_POSIX_SHM);

	/* O_EXCL: 이미 존재하면 실패(EEXIST) → 새로 생성한 쪽만 ftruncate로 크기 설정 */
	if ((shm_fd=shm_open(NAME_POSIX_SHM, O_RDWR|O_CREAT|O_EXCL, 0660)) > 0) {
		printf("* Create SHM\n");
		if (ftruncate(shm_fd, SZ_SHM_SEGMENT) == -1)  /* 공유 메모리 크기 설정 */
			exit(1);
	} else {
		if (errno != EEXIST) {  /* EEXIST가 아닌 다른 에러면 실패 */
			perror("shm_open");
			exit(1);
		}
		/* 이미 존재하는 공유 메모리에 읽기/쓰기 모드로 접근 */
		shm_fd = shm_open(NAME_POSIX_SHM, O_RDWR, 0);
		if (shm_fd == -1) {
			perror("shm_open()");
			exit(1);
		}
	}

	/* MAP_SHARED: 다른 프로세스와 실제로 메모리를 공유 (MAP_PRIVATE는 copy-on-write) */
	shm_ptr = (char *)mmap(NULL, SZ_SHM_SEGMENT, PROT_READ|PROT_WRITE, MAP_SHARED, shm_fd, 0);
	if (shm_ptr == MAP_FAILED) {
		perror("mmap");
		exit(1);
	}

	printf("'*' - 현재 공유 메모리 출력\n");
	printf("'.' - 종료.\n");
	printf("키보드입력 - 공유메모리로 문자 입력\n");

	while (1) {
		printf("\n>>");
		if (fgets(input, sizeof(input), stdin) == NULL) {
				break;
	 	}
		if (input[0] == '.') {
	  		break;
	 	} else if (input[0] == '*') {
			printf("shm -> '%.*s'\n", SZ_SHM_SEGMENT, shm_ptr);  /* 현재 내용 출력 */
	 	} else {
			input[strlen(input)-1]='\0';         /* 개행 문자 제거 */
	  		memcpy(shm_ptr, input, strlen(input)); /* 공유 메모리에 기록 */
	 	}
	}
	munmap(shm_ptr, SZ_SHM_SEGMENT);  /* 주소 공간에서 매핑 해제 */

	printf("* 공유메모리(%s) 삭제? (y/n)", NAME_POSIX_SHM);
	fgets(input, sizeof(input), stdin);
	if (input[0] == 'y') {
		shm_unlink(NAME_POSIX_SHM);  /* 공유 메모리 이름 제거 */
	}

	return 0;
}
