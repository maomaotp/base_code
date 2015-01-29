#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

int strtonum(const char *string)
{
	int number = 0;
	//assert(!string);
	
	if (!string) {
		printf("opint is NULL\n");
		return -1;
	}

	while(*string != 0) {
		if (*string <= '0' || *string >='9') {
			++string; 
			continue;
		}
		number = number*10 + *string - '0'; 
		++string;
	}

	return number;
}

int main()
{
	int res;
	char *str = "0123";

	res = strtonum(str);
	printf("res=%d\n", res);
}
