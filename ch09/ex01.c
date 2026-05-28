#include <stdio.h>
#include <arpa/inet.h>

int main()
{
	union u_short {
		unsigned short s;
		char s_array[2];
	} port;
	union u_long {
		unsigned long l;
		char l_array[4];
	} addr;
	int i;

	port.s=0x0017;
	addr.l=0x01020304;

	printf("<short> 호스트 바이트 순서 : \n");
	for(i=1; i>=0; i--)
		printf("%p\t%02x\n", &port.s_array[i], port.s_array[i]);
	port.s=htons(port.s);
	printf("<short> 네트워크 바이트 순서 : \n");
	for(i=1; i>=0; i--)
		printf("%p\t%02x\n", &port.s_array[i], port.s_array[i]);

	printf("<long> 호스트 바이트 순서 : \n");
	for(i=3; i>=0; i--)
		printf("%p\t%02x\n", &addr.l_array[i], addr.l_array[i]);
	addr.l=htonl(addr.l);
	printf("<long> 네트워크 바이트 순서 : \n");
	for(i=3; i>=0; i--)
		printf("%p\t%02x\n", &addr.l_array[i], addr.l_array[i]);
	return 0;
}
