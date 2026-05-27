#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#define NUMTHREADS 3

pthread_mutex_t mutexid = PTHREAD_MUTEX_INITIALIZER;
int count=0;

void cleanup_handler(void* arg)
{
    pthread_mutex_unlock(&mutexid);
}
void* thread_function(void* arg)
{
    int i;
    pthread_cleanup_push( &cleanup_handler, NULL );
    for(i=0; i<5; i++) {
        pthread_mutex_lock(&mutexid);
        count++;
        printf("count = %d\n", count);
        sleep(1);
        pthread_mutex_unlock(&mutexid);
    }
    pthread_cleanup_pop (0);
}

int main(void){
    pthread_t threads[NUMTHREADS];
    int i;
    for (i=0; i < NUMTHREADS; i++) {
        pthread_create(&threads[i],NULL,thread_function,NULL);
    }
    for (i=0; i < NUMTHREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    return 0;
}
