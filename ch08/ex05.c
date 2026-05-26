#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>

#define LOOP_MAX 10
pthread_mutex_t mutexid;  // mutex 변수
sem_t semid;           // semaphore 변수

int commonCounter = 0;

void *inc_thread(void *);

int main(void)
{
    pthread_t tid1;
    pthread_t tid2;

    //pthread_mutex_init(&mutexid, NULL);  // mutex 초기화
    sem_init(&semid, 0, 1);             // semaphore 초기화
    if ((pthread_create(&tid1, NULL, inc_thread, "thread1")) ||
        (pthread_create(&tid2, NULL, inc_thread, "thread2"))) {
        perror("pthread_create");
        exit(errno);
    }

    sem_wait(&semid);
    sem_post(&semid);
    pthread_join(tid1, (void **)NULL);
    pthread_join(tid2, (void **)NULL);

    // mutex 해제
    //pthread_mutex_destroy(&mutexid);
    sem_destroy(&semid);                // semaphore 해제

    return 0;
}

void *inc_thread(void *arg)
{
    int loopCount;
    int temp;
    char buffer[80];
    int i;

    for (loopCount = 0; loopCount < LOOP_MAX; loopCount++) {
        //pthread_mutex_lock(&mutexid);   // & + 전역변수명
        sem_wait(&semid);            // semaphore wait

        sprintf(buffer, "<%s> Common counter : from %d to ", (char *)arg, commonCounter);
        write(1, buffer, strlen(buffer));

        temp = commonCounter;
        for (i = 0; i < 900000; i++);   // delay
        commonCounter = temp + 1;

        sprintf(buffer, "%d\n", commonCounter);
        write(1, buffer, strlen(buffer));
        for (i = 0; i < 500000; i++);   // delay

        //pthread_mutex_unlock(&mutexid); // & + 전역변수명
        sem_post(&semid);             // semaphore post
    }
}