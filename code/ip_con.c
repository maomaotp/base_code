#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char* argv[])
{
	char buf[24];
	unsigned int num;
	printf("argv[1] == %s\n", argv[1]);
	inet_ntop(AF_INET, argv[1], buf, 24);

	printf("IP == %s\n", buf);

	return 0;
}
