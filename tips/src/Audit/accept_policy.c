#define UNIX_DOMAIN "/tmp/domain.block"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>

int Init_accept_server()
{
	unlink(UNIX_DOMAIN);
	int chksock = 0;
  chksock=socket(PF_UNIX,SOCK_STREAM,0);
  if(chksock<0)
  {   
    close(chksock);//by---yong 20130410
		exit(0);
	}
  struct sockaddr_un saddr;
  saddr.sun_family=AF_UNIX;
  strncpy(saddr.sun_path,UNIX_DOMAIN,sizeof(saddr.sun_path)-1);
  int err=bind(chksock,(struct sockaddr*)&saddr,sizeof(saddr));
  if(err<0)
  {
	    close(chksock);
	    unlink(UNIX_DOMAIN);
	    exit(0);
  }	
  err=listen(chksock,1);
  if(err<0)
  {
	    close(chksock);
	    unlink(UNIX_DOMAIN);
	    exit(0);
  }
	printf("Init_accept_server finished\nserver_sock = %d\n",chksock);
	return chksock;
}

int accept_policy(int server_sock,char ** cmdbuff, int* len)
{
//	char cmdbuff[1024*1024*11];
  int cmdlen = 0;
  int getlen = 0;
  int cnt = 0;
	int mngsock = 0;
  struct sockaddr_un daddr;
  socklen_t socklen=sizeof(daddr);
  mngsock=accept(server_sock,(struct sockaddr*)&daddr,&socklen);
  if(mngsock<0)
  {
    close(mngsock); //20130410PM
		return -1;
  }
  int ct = read (mngsock, (int*)&getlen, sizeof (getlen));
  cmdlen = 0;
  cnt = 0;
  while(cmdlen < getlen)
  {
#if 1
  	char buff[1024*200]; 
 		memset (buff, 0, sizeof (buff));
#endif 
    if (getlen < sizeof (buff))
      cnt = read(mngsock, buff, getlen - cmdlen);
    else
      cnt = read(mngsock, buff, sizeof (buff));
		memcpy ((char*)(*cmdbuff)+cmdlen, buff, cnt);
    cmdlen += cnt;
  }
	*len = cmdlen;
	return 0;
}
