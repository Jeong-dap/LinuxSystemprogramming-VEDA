/* System V 공유 메모리(Shared Memory) 기본 생명 주기 데모.
 *
 * 단계별 ipcs -m 출력으로 공유 메모리의 상태 변화(생성 → 연결 → 해제 → 제거)를 확인한다.
 * 각 단계마다 getchar()로 일시 정지하여 사용자가 직접 확인할 수 있다.
 *
 * shmget → shmat → (사용) → shmdt → shmctl(IPC_RMID)
 */
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


int main() {
	int shmid;
	char *shmaddr;

	/* 키 0x123400, 크기 30바이트 공유 메모리 생성 (없으면 생성, 권한 0660) */
	if((shmid= shmget(0x123400, 30, 0660|IPC_CREAT)) == -1) {
		perror("shmget");
		exit(1);
	}
	printf("\nAfter shmget...\n");
	system("ipcs -m | grep 123400 ");  /* 생성 직후 상태 확인 */
	getchar();

	/* shmat: 공유 메모리를 현재 프로세스의 주소 공간에 연결(attach)
	 * 두 번째 인자 NULL: 커널이 적절한 주소를 자동 선택 */
	if((shmaddr= shmat(shmid, (char *)0, 0)) == NULL) {
		perror("shmat");
		exit(1);
	}
	printf("\nAfter shmat...\n");
	system("ipcs -m | grep 123400  ");  /* 연결 후 nattch(연결 수) 증가 확인 */
	getchar();

	strcpy(shmaddr, "shared memory test");  /* 공유 메모리에 문자열 기록 */
	printf("shmaddr = %p : %s\n", shmaddr, shmaddr);

	sleep(1);

	/* shmdt: 현재 프로세스의 주소 공간에서 공유 메모리를 분리(detach)
	 * 분리 후에도 다른 프로세스가 사용 중이면 메모리는 유지된다 */
	if(shmdt(shmaddr) == -1 ) {
		perror("shmdt");
		exit(1);
	}
	printf("\nAfter shmdt...\n");
	system("ipcs -m  | grep 123400 ");  /* 분리 후 nattch 감소 확인 */
	getchar();

	/* shmctl(IPC_RMID): 공유 메모리 세그먼트 제거 (커널 자원 반납) */
	if(shmctl(shmid, IPC_RMID, (struct shmid_ds *)0) == -1 ) {
		perror("shmctl");
		exit(1);
	}
	printf("\nAfter shmctl(IPC_RMID)...\n");
	system("ipcs -m | grep 123400");  /* 제거 후 목록에서 사라진 것 확인 */
	return 0;
}
