#include <signal.h>
#include <unistd.h>
#include <stdio.h>

void handler(int signo)
{
    write(1, "Control_C\n", 10);
}

int main() {
    signal(SIGINT, handler);
    while(1)
    {
        printf("signal test\n");
        sleep(2);
    }
    return 0;
}