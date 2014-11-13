#include <pthread.h>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include "cachehandle.h"
#include "publicdata.h"
#include <errno.h>
#include <unistd.h>
#include "zlog.h"

using namespace std;

//sqlite3* db = NULL;
const char * prot_short_str[] = {IPOQUE_PROTOCOL_SHORT_STRING};
#define BUFFSIZE 256
#define UNIX_DOMAIN "/tmp/CACHE_IP.domain"
#define CACHEPID "/usr/local/tipschecker/etc/tipscheck/cachepid"
char LOCALDEVID[11];
//list<struct ip_rules> global_iplist;
#if 0
void* iplist (void* num)
{
	unlink(UNIX_DOMAIN);
	int fd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (fd < 0)
	{
		fprintf(stderr, "%s\n", strerror(errno));
		close(fd);
	}
	struct sockaddr_un saddr, daddr;
	memset(&saddr, 0, sizeof (saddr));
	memset(&daddr, 0, sizeof (daddr));
	saddr.sun_family = AF_UNIX;
	strcpy(saddr.sun_path, UNIX_DOMAIN);
	int err = bind(fd, (struct sockaddr*)&saddr, sizeof (saddr));
	if (err < 0)
	{
		unlink(UNIX_DOMAIN);
		fprintf(stderr, "%s\n", strerror(errno));
		close(fd);
	}
	err = listen(fd, 1);
	{
		unlink(UNIX_DOMAIN);
		fprintf(stderr, "%s\n", strerror(errno));
		close(fd);
	}
	while (1)
	{
		socklen_t len = sizeof (daddr);
		int acfd = accept(fd, (struct sockaddr*)&daddr, &len);
		int ret = 1;
		struct ip_rules ip;
		while (ret > 0)
		{
			memset(&ip, 0, sizeof (ip));
			ret = read(acfd, &ip, sizeof (ip));
			global_iplist.push_back(ip);
		}
		close(acfd);
	}
	return (void*)0;
}
#endif
void WtPid()
{
	FILE *fp;
	char id_buf[10];
	pid_t pid;
	memset(id_buf,0,10);
	pid = getpid();
    dzlog_info("cache:start now,pid==%d", pid);
	sprintf(id_buf,"%d",pid);
	if(NULL == (fp = fopen(CACHEPID,"w+")))
	{
		dzlog_info("fopen");
		return;
	}
	fwrite(id_buf,strlen(id_buf),1,fp);
	fclose(fp);
	return;
}



bool ParBuff(char *buffer)
{
         int i = 0;
         if(buffer[strlen(buffer) - 1] == '\n')
         {
                 buffer[strlen(buffer) - 1]   = '\0';
         }
		   while(buffer[i])
          {
                  if(!isdigit(buffer[i]))
                  {
                          return false;
                  }
                  ++i;
          }
          return true;
}

void WtDevID()
{
	FILE *fd = 0;
	if((0 == (fd = fopen(DEVICEIDFILE,"r"))))
	{
		dzlog_info("fopen Configre File--");	
		fclose(fd);
		return;
	}
	char buffer[BUFFSIZE] = {0};
	char *tmp = 0;
	char *tmpp = 0;
	while(fgets(buffer,sizeof(buffer),fd))		
	{
		if((tmp = strstr(buffer,"DEVICE_ID")))
		{
			strtok(tmp,"=");		
			if((tmpp = strtok(NULL,"=")))
			{
				while(!isdigit(*tmpp))	
				{
					tmpp++;
				}
				if(ParBuff(tmpp))
				{
					memcpy(LOCALDEVID,tmpp,11);		
				}
			}
		}
		memset(buffer,0,sizeof(buffer));
	}
	fclose(fd);
	return ;	
}


int main()
{
/*	int rc = sqlite3_open(DATABASE_PATH, &db);
	if (rc)
	  printf("sqlite3_open faild！");
#if 1
	rc = sqlite3_key(db, "admin", 5);
	if (rc)
	  printf("Key error！");
#endif
*/

    if(dzlog_init("/usr/local/tipschecker/etc/log.conf", "cache")){
        printf("dzlog init failed\n");
    }

	WtPid();
	int data = 0;
	data = data;		//hyj 未使用的变量
	memset(LOCALDEVID, 0, sizeof (LOCALDEVID));
//	strcpy(LOCALDEVID, "1303030001");
	WtDevID();
#if 0
	pthread_t ptd;
	pthread_create(&ptd, NULL, iplist, (void*)&data);
#endif

	CCachehandler cachehandler;
	cachehandler.Init_Server(100, 1024);//初始化服务器程序
	//第一个参数为监听队列(100),第二个参数为缓存池缓存的数据个数;
	cout<<"init_server sucess!"<<endl;
	cachehandler.Start_Server(2);//开启服务，60为定时读取数据时间
	cout<<"start_server sucess!"<<endl;
	cachehandler.Wait_Exit();
	cachehandler.Exit_Server();
}
