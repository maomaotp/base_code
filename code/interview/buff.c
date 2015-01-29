#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void test(char* buf)
{
	char temp[24] = "hello world!";
	strcpy(buf, temp);
	return;
}

int main()
{
	char buf[24];

	test(buf);
	printf("buf==%s\n", buf);
}
