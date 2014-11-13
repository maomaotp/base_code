#ifndef MNGCHKPOSTMAN_H
#define MNGCHKPOSTMAN_H
#include<sys/socket.h>
#include<sys/un.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <errno.h>
#include "rulesdata.h"
#include <string.h>
#include <errno.h>
#include <unistd.h>
//#define UNIX_DOMAIN				"/tmp/UNIX.domain"
#define DOMAIN_DAEMON			"/tmp/domain.daemon"
#define DOMAIN_ALARM			"/tmp/domain.alarm"
#define DOMAIN_NETLOG			"/tmp/domain.netlog"
#define DOMAIN_SENSITIVE	"/tmp/domain.sensitive"
#define DOMAIN_BLOCK	 		"/tmp/domain.block"
#define DOMAIN_CACHE 			"/tmp/domain.cache"
//#define DOMAIN_DAEMON			"/tmp/UNIX.domain"
//#define DOMAIN_ALARM			"/tmp/UNIX.domain"
//#define DOMAIN_NETLOG			"/tmp/UNIX.domain"
//#define DOMAIN_SENSITIVE	"/tmp/UNIX.domain"
//#define DOMAIN_BLOCK	 		"/tmp/UNIX.domain"
//#define DOMAIN_CACHE 			"/tmp/UNIX.domain"
class MngChkPostman{
	int chksock;
	struct sockaddr_un saddr;
	public:
	void Init_Postman(const char* domain);
	int Send_Msg(char* buff,int len,char* rbuf,int rlen);
	int Send_Msg2(const char* domain, char* buff,int len,char* rbuf,int rlen, int flag);
};

#endif
