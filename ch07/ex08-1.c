/* 공유 메모리 송신자 — ex08-2(수신자)와 쌍을 이루는 예제.
 *
 * IPC_EXCL: 이미 존재하면 실패 → 실패 시 IPC_EXCL 없이 재시도해 기존 세그먼트에 접근.
 * 20번 반복하며 메시지를 기록하고, 마지막에 "end" 문자열로 종료를 알린다.
 * 동기화 없이 busy-waiting(j 루프)으로 타이밍을 맞추므로 race condition이 발생할 수 있다.
 * (동기화는 exercise-3/4의 세마포어 버전 참조)
 */
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


int main()
{
	int shmid, i,j ;
	char *shmaddr;

	/* IPC_EXCL: 처음 생성 시도. 이미 있으면 EEXIST로 실패 → IPC_EXCL 없이 재접근 */
	if((shmid=shmget(0x123400, 30, 0660|IPC_CREAT|IPC_EXCL))==-1) {
		if((shmid=shmget(0x123400, 30, 0660))==-1){
			perror("shmget");
			exit(1);
		}
	}
 	if((shmaddr=shmat(shmid, (char *)0, 0))== NULL) {
		perror("shmat");
		exit(1);
	}

 	for(i=0; i<20; i++) {
 		sprintf(shmaddr, "shared memory test %d", i+1);  /* 공유 메모리에 메시지 기록 */
 		printf("send : %s\n", shmaddr);
 		for(j=0; j<100000000; j++);  /* busy-waiting: 수신자가 읽을 시간 확보 (비동기화) */
 	}
 	sprintf(shmaddr, "end");  /* 종료 신호 기록 */

	if(shmdt(shmaddr)==-1 ) {
		perror("shmdt");
		exit(1);
	}

	return 0;
}
