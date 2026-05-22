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

	if((shmid=shmget(0x123400, 30, 0660|IPC_CREAT|IPC_EXCL))==-1) {
		if((shmid=shmget(0x123400, 30, 0660))==-1){
			perror("shmget");
			exit(1);
		}
	}
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
	semctl(semid, 0, SETVAL, semctrl);
	semctrl.val = 0;
	semctl(semid, 1, SETVAL, semctrl);

 	if((shmaddr=shmat(shmid, (char *)0, 0))== NULL) {
		perror("shmat");
		exit(1);
	}

	struct sembuf sp = {0, -1, SEM_UNDO};
	struct sembuf sv = {1, 1, SEM_UNDO};
 	for(i=0; i<20; i++) {
		semop(semid, &sp, 1);
 		sprintf(shmaddr, "shared memory test %d", i+1);
 		printf("send : %s\n", shmaddr);
		semop(semid, &sv, 1);
 		// for(j=0; j<100000000; j++);
		// sleep(1);
 	}
	// end도 op 걸어줘야 함
	semop(semid, &sp, 1);
 	sprintf(shmaddr, "end");

	if(shmdt(shmaddr)==-1 ) {
		perror("shmdt");
		exit(1);
	}

	semctl(semid, 0, IPC_RMID);
	semctl(semid, 1, IPC_RMID);
	return 0;
}