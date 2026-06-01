#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <termios.h>
#include <pthread.h>
#include <wiringPi.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define LED_PIN  18
#define PORT     8000

static int is_run = 1;

int kbhit(void);
void *webserverFunction(void *arg);
static void *clnt_connection(void *arg);

int main(void)
{
    pthread_t ptWebserver;

    wiringPiSetupGpio();
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    pthread_create(&ptWebserver, NULL, webserverFunction, NULL);

    printf("q : Quit\n");
    while(is_run) {
        if(kbhit()) {
            if(getchar() == 'q') {
                pthread_kill(ptWebserver, SIGINT);
                pthread_cancel(ptWebserver);
                is_run = 0;
            }
        }
        delay(100);
    }

    printf("Good Bye!\n");
    return 0;
}

int kbhit(void)
{
    struct termios oldt, newt;
    int ch, oldf;

    tcgetattr(0, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(0, TCSANOW, &newt);
    oldf = fcntl(0, F_GETFL, 0);
    fcntl(0, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(0, TCSANOW, &oldt);
    fcntl(0, F_SETFL, oldf);
    if(ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }
    return 0;
}

void *webserverFunction(void *arg)
{
    int ssock, csock;
    pthread_t thread;
    struct sockaddr_in servaddr, cliaddr;
    unsigned int len;

    ssock = socket(AF_INET, SOCK_STREAM, 0);
    if(ssock == -1) { perror("socket()"); exit(1); }

    int opt = 1;
    setsockopt(ssock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    if(bind(ssock, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
        perror("bind()"); exit(1);
    }
    if(listen(ssock, 10) == -1) {
        perror("listen()"); exit(1);
    }

    while(is_run) {
        len = sizeof(cliaddr);
        csock = accept(ssock, (struct sockaddr *)&cliaddr, &len);
        if(csock < 0) break;
        int *pcsock = malloc(sizeof(int));
        *pcsock = csock;
        pthread_create(&thread, NULL, clnt_connection, pcsock);
        pthread_detach(thread);
    }

    close(ssock);
    return NULL;
}

static void *clnt_connection(void *arg)
{
    int csock = *((int *)arg);
    free(arg);

    FILE *clnt_read  = fdopen(csock, "r");
    FILE *clnt_write = fdopen(dup(csock), "w");

    char req_line[BUFSIZ];
    fgets(req_line, BUFSIZ, clnt_read);

    /* 요청 라인에서 메서드와 경로 파싱 */
    char method[16], path[BUFSIZ];
    sscanf(req_line, "%15s %s", method, path);

    /* 나머지 헤더 소비 */
    char line[BUFSIZ];
    while(fgets(line, BUFSIZ, clnt_read)) {
        if(strncmp(line, "\r\n", 2) == 0) break;
    }

    if(strcmp(method, "GET") != 0) {
        fprintf(clnt_write,
            "HTTP/1.1 405 Method Not Allowed\r\n\r\n");
        goto END;
    }

    /* 쿼리스트링에서 led 파라미터 처리 */
    char *query = strchr(path, '?');
    if(query) {
        query++;
        char key[64], val[64];
        char *tok = strtok(query, "&");
        while(tok) {
            if(sscanf(tok, "%63[^=]=%63s", key, val) == 2) {
                if(strcmp(key, "led") == 0) {
                    if(strcmp(val, "On") == 0)
                        digitalWrite(LED_PIN, HIGH);
                    else if(strcmp(val, "Off") == 0)
                        digitalWrite(LED_PIN, LOW);
                }
            }
            tok = strtok(NULL, "&");
        }
    }

    /* index.html 파일 전송 */
    FILE *fp = fopen("index.html", "r");
    if(!fp) {
        fprintf(clnt_write,
            "HTTP/1.1 404 Not Found\r\n\r\n<h1>404 Not Found</h1>");
        goto END;
    }

    fseek(fp, 0, SEEK_END);
    long fsize = ftell(fp);
    rewind(fp);

    fprintf(clnt_write,
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html; charset=UTF-8\r\n"
        "Content-Length: %ld\r\n"
        "\r\n", fsize);

    char buf[BUFSIZ];
    size_t n;
    while((n = fread(buf, 1, sizeof(buf), fp)) > 0)
        fwrite(buf, 1, n, clnt_write);

    fclose(fp);

END:
    fflush(clnt_write);
    fclose(clnt_read);
    fclose(clnt_write);
    return NULL;
}
