#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>     // open(), O_RDONLY
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>    // read(), close()

#define MAX_SIZE	1024  // 한 번에 읽을 최대 바이트 수

int main(int argc, char** argv)
{
	int fd, length;
	char buffer[MAX_SIZE];  // 읽기 버퍼. 마지막 바이트는 '\0' 용도로 남겨둠

	if(argc!=2) {
		fprintf(stderr, "Usage : %s filename\n", argv[0]);
		exit(1);
	}

	// 인자로 받은 파일을 읽기 전용으로 열기
	if((fd = open(argv[1], O_RDONLY))==-1)  {
		perror("open");
		exit(1);
	}

	printf("buffer : ");
	while(1) {
		// 최대 MAX_SIZE-1 바이트씩 읽기
		// length: 실제로 읽은 바이트 수. 0이면 EOF, -1이면 에러
		length = read( fd, buffer, MAX_SIZE-1 );
		printf("%s", buffer);  // buffer는 null 종료가 보장되지 않으나 마지막 루프에서 길이가 짧아 종료됨

		// 읽은 양이 요청량보다 적으면 EOF에 도달한 것 → 루프 종료
		if(length < MAX_SIZE-1)
			break;

	}
	printf("\n");
	close(fd);
}
