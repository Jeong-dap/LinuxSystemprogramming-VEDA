/* System V 메시지 큐(Message Queue) 송신자 — ex05-2와 쌍을 이루는 예제.
 *
 * 메시지 큐는 커널이 관리하는 IPC 자원으로, ipcs -q 명령으로 확인할 수 있다.
 * 프로세스가 종료되어도 큐는 남아있으므로 수신자(ex05-2)가 msgctl(IPC_RMID)로 제거해야 한다.
 *
 * 흐름: msgget → 루프(사용자 입력 → msgsnd) → "end" 전송 후 종료
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
   int msgid;
   char buf[1024];
   /* 메시지 구조체: 첫 필드 m_type(long)은 반드시 양수여야 함 */
   struct mymsgbuf {
       long    m_type;
       char m_str[1024];
   } sendbuf;

   /* 0x123400: IPC 키. IPC_CREAT: 없으면 생성, 0666: 접근 권한 */
   if((msgid=msgget(0x123400,IPC_CREAT|0666))==-1)
   {
       perror("msgget");
       exit(1);
   }

   while (1)
   {
       printf(" Input --> ");
       fgets(buf, 1023, stdin);
       sendbuf.m_type=1;              /* 메시지 타입 1로 설정 (수신자가 msgrcv에서 타입 지정) */
       strcpy(sendbuf.m_str,buf);
       msgsnd(msgid,&sendbuf, strlen(sendbuf.m_str),0);  /* 블로킹 전송 (마지막 인자 0) */
       if(!strncmp(buf,"end",3))      /* "end" 전송 후 루프 탈출 */
               break;
   }
   sleep(1);  /* 수신자가 "end"를 처리할 시간 확보 */
   return 0;
}
