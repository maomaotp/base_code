#include <stdio.h>
#include <string.h>

int main()
{
	char buf[24] = "hello,world";
	printf("strlen = %d\n", strlen(buf));
	printf("sizeof = %d\n", sizeof(buf));
	printf("sizeof str = %d\n", sizeof("hello,world"));

	return 0;
}
