#include "ui_manage_client.h"
#include "gsad_base.h"
#include <errno.h>
#include <stdio.h>

int CUI_Manage_cli::cli_connect(const char *name, const char *sname)
{
/*保留参数?*/if(name){};
	struct sockaddr_un un;
	int unsize = 0;
	int rval = 0;
	int err = 0;
	int sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
	if(sockfd < 0){
		return -1;
	}
	printf("in cli_connect ing !\n");
#if 0 
	unlink(name);
	memset(&un, 0, sizeof(un));
	un.sun_family = AF_UNIX;
	strcpy(un.sun_path, name);
	unsize = offsetof(struct sockaddr_un, sun_path) + strlen(name);

	if(bind(sockfd, (struct sockaddr *)(&un), unsize) < 0){
		rval = -1;
		goto errout;
	}
#endif
	memset(&un, 0, sizeof(un));
	un.sun_family = AF_UNIX;
	strcpy(un.sun_path, sname);
	unsize = offsetof(struct sockaddr_un, sun_path) + strlen(sname);
	
	if((connect(sockfd, (struct sockaddr*)(&un), unsize)) < 0){
		rval = -2;
		goto errout;
	}
	printf("in cli_connect ing !\n");
	return sockfd;
errout:
	err = errno;
	close(sockfd);
	errno = err;
	return rval;
}

int CUI_Manage_cli::cli_write(int fd, int cmd)
{
	printf("in cli_write\n");
	int n;
	printf("c===%d\n",cmd);
	n = write(fd, &cmd, sizeof(cmd));
	printf("n===%d\n",n);
	return n;
}


int CUI_Manage_cli::cli_read_string(int fd, void* devicesta, int len)
{
	int n;
	n = read(fd, devicesta, len);
	return n;
}
