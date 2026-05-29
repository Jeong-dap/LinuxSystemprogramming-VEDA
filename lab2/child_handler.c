#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <bits/sigaction.h>
#include <bits/types/sigset_t.h>
#include <asm-generic/signal-defs.h>
#include <bits/waitflags.h>
#include <errno.h>

void child_handler(int signo) 
{
	pid_t pid;

  	for (;;) { 
		pid = waitpid(-1, NULL, WNOHANG);
		if (pid == 0) { 		 
			break;
		} 
		else if (pid == -1 && errno == ECHILD) {  
			break;

   		} 
		else if (pid == -1) {	 
			perror("waitpid");
			abort();
		}
		printf("PID of the dead child = %d\n", pid);

  	}
  	return;
}
