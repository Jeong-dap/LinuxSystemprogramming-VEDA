#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
    int msgid1, msgid2, n;
    char buf[1024];
    struct mymsgbuf {
        long m_type;
        char m_str[1024];
    } sendbuf, recvbuf;

    if((msgid1=msgget(0x123401,IPC_CREAT|0666))==-1)
    {
        perror("msgget");
        exit(1);
    }

    if((msgid2=msgget(0x123402,IPC_CREAT|0666))==-1)
    {
        perror("msgget");
        exit(1);
    }

    while (1)
    {
        n=msgrcv(msgid1,&recvbuf,1024, 1, 0);
        recvbuf.m_str[n]='\0';
        printf(">> %s\n",recvbuf.m_str);
        if(!strncmp(recvbuf.m_str,"end", 3))
            break;
        printf(" Input --> ");
        fgets(buf, 1023, stdin);
        sendbuf.m_type=2;
        strcpy(sendbuf.m_str, buf);
        msgsnd(msgid2, &sendbuf, strlen(sendbuf.m_str), 0);
        if(!strncmp(buf,"end",3))
            break;
    }
    msgctl(msgid1, IPC_RMID, 0);
    msgctl(msgid2, IPC_RMID, 0);
    return 0;
}
