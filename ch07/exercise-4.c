/* 공유 메모리 + 세마포어(Semaphore) 수신자 — exercise-3(송신자)과 쌍을 이루는 예제.
 *
 * 세마포어 인덱스 규칙 (수신자 기준):
 *   sp: sem[1] -= 1 (P 연산) — 송신자가 V 신호를 보낼 때까지 대기
 *   sv: sem[0] += 1 (V 연산) — 읽기 완료 후 송신자에게 쓰기 허가 반환
 *
 * 수신자가 공유 메모리를 읽은 뒤 shmctl(IPC_RMID)로 공유 메모리를 제거한다.
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
	/* 세마포어 접근 */
	if((semid=semget(0x123400, 2, 0660|IPC_CREAT|IPC_EXCL))==-1) {
		if((semid=semget(0x123400, 2, 0660))==-1){
			perror("semget");
			exit(1);
		}
	}

 	if((shmaddr= shmat(shmid, (char *)0, 0))== NULL) {
		perror("shmat");
		exit(1);
	}

	/* sp: sem[1] -= 1 (P 연산) — 송신자 V 신호 대기 */
	struct sembuf sp = {1, -1, SEM_UNDO};
	/* sv: sem[0] += 1 (V 연산) — 송신자에게 쓰기 허가 반환 */
	struct sembuf sv = {0, 1, SEM_UNDO};

 	while(1) {
		semop(semid, &sp, 1);          /* 송신자가 쓰기를 완료할 때까지 대기 */
 		if(!strcmp(shmaddr,"end"))     /* 종료 신호 확인 */
 			break;
 		if(!strcmp(shmaddr,""))        /* 빈 문자열: 건너뜀 */
 			continue;
 		printf("recv : %s\n", shmaddr);
		semop(semid, &sv, 1);          /* 읽기 완료: 송신자에게 쓰기 허가 반환 */
		for(j=0; j<99999999; j++);
 	}

	if(shmdt(shmaddr) == -1 ) {
		perror("shmdt");
		exit(1);
	}
	if(shmctl(shmid, IPC_RMID, (struct shmid_ds *)0) == -1 ) {  /* 공유 메모리 제거 */
		perror("shmctl");
		exit(1);
	}
	semctl(semid, 0, IPC_RMID);  /* 세마포어 제거 */
	return 0;
}
