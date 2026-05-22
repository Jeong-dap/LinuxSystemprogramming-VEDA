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

 	if((shmaddr= shmat(shmid, (char *)0, 0))== NULL) {
		perror("shmat");
		exit(1);
	}
	
	struct sembuf sp = {1, -1, SEM_UNDO};
	struct sembuf sv = {0, 1, SEM_UNDO};
 	while(1) {
		semop(semid, &sp, 1);
 		if(!strcmp(shmaddr,"end"))
 			break;
 		if(!strcmp(shmaddr,""))
 			continue;
 		printf("recv : %s\n", shmaddr);
		semop(semid, &sv, 1);
		for(j=0; j<99999999; j++);
 	}

	if(shmdt(shmaddr) == -1 ) {
		perror("shmdt");
		exit(1);
	}
	if(shmctl(shmid, IPC_RMID, (struct shmid_ds *)0) == -1 ) {
		perror("shmctl");
		exit(1);
	}
	semctl(semid, 0, IPC_RMID);
	return 0;
}