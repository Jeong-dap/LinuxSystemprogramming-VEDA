#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

void *thread_function(void *arg);

int main(void) {
   int status;
   pthread_t a_thread;
   void *thread_result;

   status = pthread_create(&a_thread, NULL, thread_function, (void *)NULL);
   if (status != 0) {
       perror("pthread_create");
       exit(1);
   }
   void* ret_value;
   pthread_join(a_thread, &ret_value);
   printf("return from thread : %s\n", (char*) ret_value);
   return 0;
}

void *thread_function(void *arg) {
    printf("this is thread_function\n");
    sleep(10);
    printf("thread_function is end\n");
    pthread_exit("Hello, Thread");
}
