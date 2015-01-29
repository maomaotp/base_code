#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
	char *a;
	char *b;
	a = malloc(120);
	strncpy(a, "hello,world",11);
	printf("a = %s\n", a);
	b = a;
	a = malloc(12);
	strcpy(a, "hello,liuq");
	printf("a = %s\nb = %s\n", a, b);

	free(a);
}
