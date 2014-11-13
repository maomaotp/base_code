#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sds.h"
typedef char *sds;

sds sdsnewlen(const void *init, size_t initlen)
{
	struct sdshdr *sh;
	sh = malloc(sizeof(struct sdshdr) + initlen+1);

	if(sh == NULL) {
		return NULL;
	}
	sh->len = initlen;
	sh->free = 0;
	if(initlen && init) {
		memcpy(sh->buf, init, initlen);
	}
	sh->buf[initlen] = '\0';
	return (char*)sh->buf;
}

sds sdsnew(const char* init)
{
	size_t initlen = (init == NULL) ? 0:strlen(init);
	return sdsnewlen(init, initlen);
}

int main()
{
	char initbuf[20] = "hello";
	sdsnew(initbuf);

	return 0;
}
