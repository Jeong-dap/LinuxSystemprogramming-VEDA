/* System V 메시지 큐(Message Queue) 수신자 — ex05-1과 쌍을 이루는 예제.
 *
 * msgrcv()는 지정한 타입(1)의 메시지가 올 때까지 블로킹한다.
 * "end"를 받으면 루프를 탈출하고 msgctl(IPC_RMID)로 큐를 제거한다.
 *
 * 흐름: msgget → 루프(msgrcv → 출력) → "end" 수신 후 msgctl(IPC_RMID) → 종료
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
    int msgid,n;
    struct mymsgbuf {
    long m_type;
    char m_str[1024];
    } recvbuf;

    /* 동일한 키(0x123400)로 이미 생성된 큐에 접근 */
    if((msgid=msgget(0x123400,IPC_CREAT|0666))==-1)
    {
        perror("msgget");
        exit(1);
    }

    while (1)
    {
        /* 타입 1 메시지를 최대 1024바이트까지 블로킹 수신 */
        n=msgrcv(msgid,&recvbuf,1024, 1, 0);
        recvbuf.m_str[n]='\0';  /* 문자열 종료 마커 */
        printf(">> %s\n",recvbuf.m_str);
        if(!strncmp(recvbuf.m_str,"end", 3))
        break;
    }
    msgctl(msgid, IPC_RMID, 0);  /* 메시지 큐 제거 (커널 자원 반납) */
    return 0;
}
