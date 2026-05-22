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

	if((shmid= shmget(0x123400, 30, 0660|IPC_CREAT)) == -1) {
		perror("shmget");
		exit(1);
	}
	printf("\nAfter shmget...\n");
	system("ipcs -m | grep 123400 ");
	getchar();

	if((shmaddr= shmat(shmid, (char *)0, 0)) == NULL) {
		perror("shmat");
		exit(1);
	}
	printf("\nAfter shmat...\n");
	system("ipcs -m | grep 123400  ");
	getchar();

	strcpy(shmaddr, "shared memory test");
	printf("shmaddr = %p : %s\n", shmaddr, shmaddr);

	sleep(1);

	if(shmdt(shmaddr) == -1 ) {
		perror("shmdt");
		exit(1);
	}
	printf("\nAfter shmdt...\n");
	system("ipcs -m  | grep 123400 ");
	getchar();

	if(shmctl(shmid, IPC_RMID, (struct shmid_ds *)0) == -1 ) {
		perror("shmctl");
		exit(1);
	}
	printf("\nAfter shmctl(IPC_RMID)...\n");
	system("ipcs -m | grep 123400");
	return 0;
}