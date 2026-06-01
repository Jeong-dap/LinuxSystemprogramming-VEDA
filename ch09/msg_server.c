#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>
#include <pthread.h>
#include <time.h>

#define PORT1 60000
#define PORT2 60001
#define MAXDATASIZE 1024
#define QUEUE_SIZE 100

typedef struct {
    char client_ip[INET_ADDRSTRLEN];
    char message[MAXDATASIZE];
} LogEntry;

static LogEntry queue[QUEUE_SIZE];
static int q_head = 0, q_tail = 0, q_count = 0;
static pthread_mutex_t q_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  q_cond  = PTHREAD_COND_INITIALIZER;

static void enqueue(const char *ip, const char *msg) {
    pthread_mutex_lock(&q_mutex);
    if (q_count < QUEUE_SIZE) {
        strncpy(queue[q_tail].client_ip, ip,  INET_ADDRSTRLEN - 1);
        strncpy(queue[q_tail].message,   msg, MAXDATASIZE - 1);
        q_tail = (q_tail + 1) % QUEUE_SIZE;
        q_count++;
        pthread_cond_signal(&q_cond);
    }
    pthread_mutex_unlock(&q_mutex);
}

static void *msg_collect(void *arg) {
    (void)arg;
    FILE *fp = fopen("msg.log", "a");
    if (!fp) { perror("fopen"); return NULL; }

    while (1) {
        pthread_mutex_lock(&q_mutex);
        while (q_count == 0)
            pthread_cond_wait(&q_cond, &q_mutex);

        LogEntry entry = queue[q_head];
        q_head = (q_head + 1) % QUEUE_SIZE;
        q_count--;
        pthread_mutex_unlock(&q_mutex);

        time_t now = time(NULL);
        struct tm *t = localtime(&now);
        char timebuf[64];
        strftime(timebuf, sizeof(timebuf), "%Y년 %m월 %d일 %H:%M:%S", t);

        fprintf(fp, "%s %s %s\n", timebuf, entry.client_ip, entry.message);
        fflush(fp);
        printf("[log] %s %s %s\n", timebuf, entry.client_ip, entry.message);
    }

    fclose(fp);
    return NULL;
}

static void receive_from(int sockfd, int port) {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buf[MAXDATASIZE];

    int nbytes = recvfrom(sockfd, buf, MAXDATASIZE - 1, 0,
                          (struct sockaddr *)&client_addr, &client_len);
    if (nbytes <= 0) return;

    buf[nbytes] = '\0';
    if (buf[nbytes - 1] == '\n')
        buf[--nbytes] = '\0';

    char *ip = inet_ntoa(client_addr.sin_addr);
    printf("[port %d] %s : %s\n", port, ip, buf);
    enqueue(ip, buf);
}

int main(void) {
    int sock1 = socket(AF_INET, SOCK_DGRAM, 0);
    int sock2 = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock1 < 0 || sock2 < 0) { perror("socket"); exit(1); }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    addr.sin_port = htons(PORT1);
    if (bind(sock1, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind port1"); exit(1);
    }

    addr.sin_port = htons(PORT2);
    if (bind(sock2, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind port2"); exit(1);
    }

    pthread_t tid;
    pthread_create(&tid, NULL, msg_collect, NULL);
    pthread_detach(tid);

    struct pollfd fds[2];
    fds[0].fd     = sock1;
    fds[0].events = POLLIN;
    fds[1].fd     = sock2;
    fds[1].events = POLLIN;

    printf("msg_server listening on ports %d and %d ...\n", PORT1, PORT2);

    while (1) {
        if (poll(fds, 2, -1) < 0) {
            perror("poll");
            break;
        }
        if (fds[0].revents & POLLIN) receive_from(sock1, PORT1);
        if (fds[1].revents & POLLIN) receive_from(sock2, PORT2);
    }

    close(sock1);
    close(sock2);
    return 0;
}