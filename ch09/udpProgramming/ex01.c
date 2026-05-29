#include<stdio.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/wait.h>
#include<errno.h>

#define MAXDATASIZE 100
#define BACKLOG 10

int main()
{
	int sockfd, new_fd;
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	int sin_size;

	if((sockfd = socket(AF_INET,SOCK_DGRAM,0)) == -1){
		perror("socket");
		exit(1);
	}
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(60000);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	memset(&(server_addr.sin_zero), '\0', 8);
	if(bind(sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr))==-1) {
		perror("bind");
		exit(1);
	}
	
	while(1) {
		char buf[MAXDATASIZE];
		memset(buf, 0, MAXDATASIZE);
		sin_size = sizeof(struct sockaddr_in);
		if((new_fd = recvfrom(sockfd, buf, MAXDATASIZE-1, 0, (struct sockaddr *)&client_addr, &sin_size))== -1) {
			perror("recvfrom");
			continue;
		}
		if(!strcmp(buf,"quit\n")){
			printf("Server quit!!\n");
			break;
		}
		printf("[ %s ] %s\n",inet_ntoa(client_addr.sin_addr), buf);
	}
	return 0;
}
