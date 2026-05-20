#include <signal.h>
#include <unistd.h>
#include <stdio.h>

int count = 0;
void handler(int signo)
{
    count++;   
}

int main() {
    int i = 0;
    struct sigaction act, oldact;

    act.sa_handler = handler;
    sigemptyset(&act.sa_mask);
    sigaddset(&act.sa_mask, SIGINT);
    sigaddset(&act.sa_mask, SIGQUIT);
    act.sa_flags=SA_RESTART;
    sigaction(SIGINT, &act,&oldact);
    while(i<10)
    {
        printf("signal test\n");
        sleep(2);
        i++;
    }
    sigaction(SIGINT,&oldact,NULL);
    printf("count=%d\n", count);
    while(1);
}