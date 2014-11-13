#include "connectlimit.h"
#include <stdio.h>
#include <stdlib.h>
int main()
{
	long limit_num = 123;
	set_limit(limit_num);
	long num = get_limit();
	if(num != -1)
		printf("limit num=%ld\n", num);
	return 0;
}
