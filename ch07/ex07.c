/* System V 공유 메모리 메타데이터(shmid_ds) 조회 예제.
 *
 * shmctl(IPC_STAT)로 공유 메모리 세그먼트의 상태 구조체를 가져와
 * 크기, 최종 attach 시각, 생성 PID, 최근 사용 PID, 현재 attach 수를 출력한다.
 */
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

int main() {
	int shmid;
	char *shmaddr;
	struct shmid_ds shm_stat;  /* 공유 메모리 상태 구조체 */

	/* 공유 메모리 생성 */
	if((shmid= shmget(0x123400,30,0600|IPC_CREAT))==-1) {
		perror("shmget");
		exit(1);
	}
	/* 주소 공간에 연결 */
	if((shmaddr= shmat(shmid,NULL,0))==NULL) {
		perror("shmat");
		exit(1);
	}

	/* IPC_STAT: 공유 메모리 상태를 shm_stat에 복사 */
	if(shmctl(shmid,IPC_STAT,&shm_stat)==-1) {
		perror("shmctl");
		exit(1);
	}

	printf("세그먼트 크기  (bytes) : %d\n", (int) shm_stat.shm_segsz);
	printf("최종  Attach 시간 : %s", ctime(&shm_stat.shm_atime));  /* ctime: time_t → 문자열 */
	printf("공유메모리 생성자(PID)  : %d\n", shm_stat.shm_cpid);
	printf("최근 사용했던 프로세스 PID : %d\n", shm_stat.shm_lpid);
	printf("현재 Attach 되어있는 프로세스 수 : %d\n", (int)shm_stat.shm_nattch);

	if(shmdt(shmaddr)==-1) {  /* 공유 메모리 분리 */
		perror("shmdt");
		exit(1);
	}
	if(shmctl(shmid,IPC_RMID,(struct shmid_ds *)0)==-1) {  /* 공유 메모리 제거 */
		perror("shmctl");
		exit(1);
	}
	return 0;
}
