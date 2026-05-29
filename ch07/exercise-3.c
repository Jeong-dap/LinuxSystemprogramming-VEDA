/* 공유 메모리 + 세마포어(Semaphore) 송신자 — exercise-4(수신자)와 쌍을 이루는 예제.
 *
 * ex08-1/2의 busy-waiting 방식을 세마포어로 개선하여 race condition을 제거한다.
 *
 * 세마포어 인덱스 규칙 (송신자 기준):
 *   sem[0] : 쓰기 허가 (초기값 1 → 송신자가 먼저 씀)
 *   sem[1] : 읽기 허가 (초기값 0 → 수신자는 sem[1]이 1이 될 때까지 대기)
 *
 * sp(P 연산): semval 감소 → 0이면 블로킹 (임계 구역 진입)
 * sv(V 연산): semval 증가 → 상대방 깨우기 (임계 구역 퇴출)
 */
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main()
{
	int shmid, semid, i, j;
	char *shmaddr;

	/* 공유 메모리 접근 */
	if((shmid=shmget(0x123400, 30, 0660|IPC_CREAT|IPC_EXCL))==-1) {
		if((shmid=shmget(0x123400, 30, 0660))==-1){
			perror("shmget");
			exit(1);
		}
	}
	/* 세마포어 2개 생성 (sem[0]: 쓰기 락, sem[1]: 읽기 신호) */
	if((semid=semget(0x123400, 2, 0660|IPC_CREAT|IPC_EXCL))==-1) {
		if((semid=semget(0x123400, 2, 0660))==-1){
			perror("semget");
			exit(1);
		}
	}

	union semnum {
		int val;
		struct semid_ds* buf;
		unsigned short* arr;
		struct seminfo *_buf;
	} semctrl;
	semctrl.val = 1;
	semctl(semid, 0, SETVAL, semctrl);  /* sem[0] = 1: 송신자가 먼저 쓸 수 있음 */
	semctrl.val = 0;
	semctl(semid, 1, SETVAL, semctrl);  /* sem[1] = 0: 수신자는 초기에 대기 */

 	if((shmaddr=shmat(shmid, (char *)0, 0))== NULL) {
		perror("shmat");
		exit(1);
	}

	/* sp: sem[0] -= 1 (P 연산, 쓰기 허가 획득) */
	struct sembuf sp = {0, -1, SEM_UNDO};
	/* sv: sem[1] += 1 (V 연산, 수신자에게 읽기 신호) */
	struct sembuf sv = {1, 1, SEM_UNDO};

 	for(i=0; i<20; i++) {
		semop(semid, &sp, 1);                          /* 쓰기 허가 획득 (sem[0] P) */
 		sprintf(shmaddr, "shared memory test %d", i+1);
 		printf("send : %s\n", shmaddr);
		semop(semid, &sv, 1);                          /* 수신자에게 읽기 신호 (sem[1] V) */
 	}
	/* 마지막 "end"도 세마포어로 보호 */
	semop(semid, &sp, 1);
 	sprintf(shmaddr, "end");

	if(shmdt(shmaddr)==-1 ) {
		perror("shmdt");
		exit(1);
	}

	semctl(semid, 0, IPC_RMID);  /* 세마포어 제거 */
	semctl(semid, 1, IPC_RMID);
	return 0;
}
