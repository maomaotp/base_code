#include "ui_manage_server.h"
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/un.h>
#include <stddef.h>
#include <errno.h>
#include <stdio.h>

int CUI_Manage_ser::serv_listen(const char *name)
{
	struct sockaddr_un un;
	int unsize = 0;
	int rval = 0;
	int err = 0;

	int listenfd = socket(AF_UNIX, SOCK_STREAM, 0);
	if(listenfd < 0){
		return -1;
	}

	unlink(name);

	memset(&un, 0, sizeof(un));
	un.sun_family = AF_UNIX;
	strcpy(un.sun_path, name);
	unsize = offsetof(struct sockaddr_un, sun_path) + strlen(name);

	if(bind(listenfd, (struct sockaddr *)(&un), unsize) < 0){
		rval = -2;
		goto errout;
	}

	if(listen(listenfd, 10) < 0){
		rval = -3;
		goto errout;
	}
	return listenfd;

errout:
	err = errno;
	close(listenfd);
	errno = err;
	return rval;
}

int CUI_Manage_ser::serv_accept(int listenfd)
{
	int accfd = 0;
	int err = 0;
	int rval = 0;
	struct sockaddr_un un;
	socklen_t len = sizeof (un);
	if((accfd = accept(listenfd, (struct sockaddr *)(&un), &len)) < 0){
		rval = -1;
		goto errout;
	}
	return accfd;
errout:
	err = errno;
	close(listenfd);
	errno = err;
	return rval;
}

int CUI_Manage_ser::serv_read(int fd)
{
	int cmd = 0;
	int rval = 0;
	if((rval = read(fd, &cmd, 4)) < 0)
		return -1;
	return cmd;
}

char* CUI_Manage_ser::serv_read_string(int fd)
{
	char* cmd = (char*)calloc(1, 41);
	int rval = 0;
	if((rval = read(fd, cmd, 40)) < 0)
		return NULL;
	return cmd;
}

int CUI_Manage_ser::serv_write_string(int fd, void *des, int len)
{
	int rval = 0;
	if((rval = write(fd, des, len)) < 0)
		return -1;
	return rval;
}
