#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<netdb.h>
#include<sys/socket.h>
#include<sys/types.h>

#define MAXDATASIZE 100

int main(int argc, char *argv[])
{
	int sockfd, numbytes;
	socklen_t addr_len;
	char buf[MAXDATASIZE];
	struct hostent *he;
	struct sockaddr_in server_addr;
	
	if(argc != 2) {
		fprintf(stderr, "usage : client hostname\n");
		exit(1);
	}
	if((he = gethostbyname(argv[1])) == NULL) {
		perror("gethostbyname");
		exit(1);
	}
	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0))==-1) {
		perror("socket");
		exit(1);
	}
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(60000);
	server_addr.sin_addr = *((struct in_addr *)he->h_addr);

//	printf("[ %s ]\n",(char*)inet_ntoa(server_addr.sin_addr));
	fgets(buf,MAXDATASIZE,stdin);
	memset(&(server_addr.sin_zero),'\0',8);
	if((numbytes = sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr *)&server_addr, sizeof(struct sockaddr))) == -1){
		perror("sendto");
		exit(1);
	}
	printf("Send %ld bytes to %s\n", strlen(buf), (char*)inet_ntoa(server_addr.sin_addr));
	close(sockfd);
	return 0;
}
