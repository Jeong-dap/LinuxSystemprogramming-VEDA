/* 공유 메모리 수신자 — ex08-1(송신자)과 쌍을 이루는 예제.
 *
 * "end" 문자열이 공유 메모리에 기록될 때까지 busy-waiting으로 반복 읽기한다.
 * 빈 문자열("")은 아직 새 데이터가 없는 것으로 간주하고 건너뛴다.
 * 수신 완료 후 shmctl(IPC_RMID)로 공유 메모리를 제거한다.
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
	int shmid, i, j;
	char *shmaddr;

	/* 송신자와 동일한 키(0x123400)로 공유 메모리에 접근 */
	if((shmid=shmget(0x123400, 30, 0660|IPC_CREAT|IPC_EXCL))==-1) {
		if((shmid=shmget(0x123400, 30, 0660))==-1){
			perror("shmget");
			exit(1);
		}
	}
 	if((shmaddr= shmat(shmid, (char *)0, 0))== NULL) {
		perror("shmat");
		exit(1);
	}

 	while(1) {
 		if(!strcmp(shmaddr,"end"))   /* 종료 신호 수신 */
 			break;
 		if(!strcmp(shmaddr,""))      /* 아직 데이터 없음 → 계속 대기 */
 			continue;
 		printf("recv : %s\n", shmaddr);
		for(j=0; j<99999999; j++);  /* busy-waiting: 송신자가 다음 메시지 쓸 시간 확보 */
 	}

	if(shmdt(shmaddr) == -1 ) {
		perror("shmdt");
		exit(1);
	}
	if(shmctl(shmid, IPC_RMID, (struct shmid_ds *)0) == -1 ) {  /* 공유 메모리 제거 */
		perror("shmctl");
		exit(1);
	}
	return 0;
}
