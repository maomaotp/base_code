#define UNIX_DOMAIN "/tmp/domain.block"
#include "accept_policy.h"
#include "zlog.h"
#include <error.h>

int Init_accept_server()
{
	unlink(UNIX_DOMAIN);
	int clientfd, chksock;

  chksock=socket(PF_UNIX,SOCK_STREAM,0);
  if(chksock<0){   
        dzlog_info("socket fail!!!");
        close(chksock);//by---yong 20130410
		exit(0);
    }
  struct sockaddr_un saddr;
  saddr.sun_family=AF_UNIX;
  strncpy(saddr.sun_path,UNIX_DOMAIN,sizeof(saddr.sun_path)-1);
  int err=bind(chksock,(struct sockaddr*)&saddr,sizeof(saddr));
  if(err<0)
  {
        dzlog_info("bind error");
	    close(chksock);
	    unlink(UNIX_DOMAIN);
	    exit(0);
  }	
  
  listen(chksock,1);

	return chksock;
}
