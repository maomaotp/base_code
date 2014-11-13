#include "accept_policy.h"

int Init_accept_server()
{
	unlink(DOMAIN_CACHE);
	int chksock = 0;
  chksock=socket(PF_UNIX,SOCK_STREAM,0);
  if(chksock<0)
  {   
    close(chksock);//by---yong 20130410
		exit(0);
	}
  struct sockaddr_un saddr;
  saddr.sun_family=AF_UNIX;
  strncpy(saddr.sun_path,DOMAIN_CACHE,sizeof(saddr.sun_path)-1);
  int err=bind(chksock,(struct sockaddr*)&saddr,sizeof(saddr));
  if(err<0)
  {
	    close(chksock);
	    unlink(DOMAIN_CACHE);
	    exit(0);
  }	
  err=listen(chksock,1);
  if(err<0)
  {
	    close(chksock);
	    unlink(DOMAIN_CACHE);
	    exit(0);
  }
	printf("Init_accept_server finished\nserver_sock = %d\n",chksock);
	return chksock;
}

int accept_policy(int server_sock,char* cmdbuff, int* len)
{
//	printf("line=%d accept_policy===================\n", __LINE__);
//	char cmdbuff[1024*1024*11];
  int cmdlen = 0;
  int getlen = 0;
  int cnt = 0;
	int mngsock = 0;
  struct sockaddr_un daddr;
  socklen_t socklen=sizeof(daddr);
//	printf("line=%d\n", __LINE__);
	printf("server_sock:%d\n",server_sock);
  mngsock=accept(server_sock,(struct sockaddr*)&daddr,&socklen);
//	printf("line=%d\n", __LINE__);
  if(mngsock<0)
  {
    close(mngsock); //20130410PM
		return -1;
  }
//	printf("line=%d\n", __LINE__);
  int ct = read (mngsock, (int*)&getlen, sizeof (getlen));
//	printf("line=%d\n", __LINE__);
	printf("getlen=%d\n", getlen);
	
  cmdlen = 0;
  cnt = 0;
  while(cmdlen < getlen)
  {
//		printf("line=%d\n", __LINE__);
  	char buff[1024*200]; 
 		memset (buff, 0, sizeof (buff));
    if ((unsigned int)getlen < sizeof (buff))			//update by hyj
      cnt = read(mngsock, buff, getlen - cmdlen);
    else
      cnt = read(mngsock, buff, sizeof (buff));
//		printf("buff=%s\n", buff);
//		printf("line=%d\n", __LINE__);
		printf("cnt=%d\n", cnt);
		memcpy (cmdbuff+cmdlen, buff, cnt);
//		printf("line=%d\n", __LINE__);
		
//		int i = 0;
//		char * p = cmdbuff+12;
//		printf("startip=%u\n",(unsigned int)(*p));
#if 0
		while(i < 100)
			printf("cmdbuff=%x\n", cmdbuff[i++]);
#endif
    cmdlen += cnt;
		printf("cmdlen=%d\n", cmdlen);
  }
	*len = cmdlen;
//	printf("line=%d\n", __LINE__);
//
	ct = ct; 			//保留变量
	return 0;
}
