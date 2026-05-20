// #include <stdio.h>
// #include <unistd.h>
// #include <signal.h>

// int count1 = 0;
// int count2 = 0;

// void usr1_handler(int sig)
// {
//     count1++;
// }
// void usr2_handler(int sig)
// {
//    count2++;
// }

// int main(void) {
//     struct sigaction usr1, usr2;

//     usr1.sa_handler=usr1_handler;
//     sigfillset(&usr1.sa_mask);
//     usr1.sa_flags=SA_RESTART;

//     usr2.sa_handler=usr2_handler;
//     sigfillset(&usr2.sa_mask);
//     usr2.sa_flags=SA_RESTART;

//     sigset_t set1, set2;

//     sigaction(SIGUSR1, &usr1, NULL);
//     sigaction(SIGUSR2, &usr2, NULL);

//     return 0;
// }

#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int usr1_count = 0;
int usr2_count = 0;

void usr1_handler(int signo) {
    usr1_count++;
}

void usr2_handler(int signo) {
    usr2_count++;
}

void int_handler(int signo) {
    printf("usr1 count: %d\n", usr1_count);
    printf("usr2 count: %d\n", usr2_count);
}

int main() {
    sigset_t sigset;
    
    sigfillset(&sigset);
    sigdelset(&sigset, SIGINT);
    sigdelset(&sigset, SIGUSR1);
    sigdelset(&sigset, SIGUSR2);

    sigprocmask(SIG_BLOCK, &sigset, NULL); 

    struct sigaction act;
    act.sa_handler = int_handler;
	sigfillset(&act.sa_mask);
	act.sa_flags = SA_RESTART;
	sigaction(SIGINT, &act, NULL);
    
    act.sa_handler = usr1_handler;
	sigaction(SIGUSR1, &act, NULL);

    act.sa_handler = usr2_handler;
	sigaction(SIGUSR2, &act, NULL);

    while(1) {
        sleep(1);
    }
}