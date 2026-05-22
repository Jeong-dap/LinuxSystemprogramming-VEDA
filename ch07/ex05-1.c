#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
   int msgid;
   char buf[1024];
   struct mymsgbuf {
       long    m_type;
       char m_str[1024];
   } sendbuf;

   if((msgid=msgget(0x123400,IPC_CREAT|0666))==-1)
   {
       perror("msgget");
       exit(1);
   }
   while (1)
   {
       printf(" Input --> ");
       fgets(buf, 1023, stdin);
       sendbuf.m_type=1;
       strcpy(sendbuf.m_str,buf);
       msgsnd(msgid,&sendbuf, strlen(sendbuf.m_str),0);
       if(!strncmp(buf,"end",3))
               break;
   }
   sleep(1);
   return 0;
}
