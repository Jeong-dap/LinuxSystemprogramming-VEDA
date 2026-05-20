#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void int_handler(int signo)
{
   int i;

   printf( "int_handler() start.\nPlease waiting for 5 secs!!\n");
   for (i=0; i<5; i++ ){
      printf( "%d\n", i+1);
      sleep(1);
   }
   printf("int_handler() end\n");
}

int main(void)
{
   struct sigaction act;

   act.sa_handler = int_handler;
   sigfillset(&act.sa_mask);

   sigaction( SIGINT, &act, NULL);
   while(1) {
	  printf("sigaction() : signal mask test\n");
      sleep(1);
   }
}