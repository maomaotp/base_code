#include "tipsmanager.h"
#include <unistd.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pwd.h>
#include <unistd.h>
#include <zlog.h>

using namespace std;

extern struct status_struct status;

void WtPid()
{
	FILE *fp;
	char id_buf[10];
	pid_t pid;
	memset(id_buf,0,10);
	pid = getpid();
	dzlog_info("manager start now,pid=%d", pid);

	sprintf(id_buf,"%d",pid);
	if(NULL == (fp = fopen(MNGAGENTPID,"w+")))
	{
		perror("fopen");
		return;
	}
	fwrite(id_buf,strlen(id_buf),1,fp);
	fclose(fp);
	return;
}

int main()
{
  #if 0
    if(fork()!=0)return 0;
    for(int i=0;i<256;++i)close(i);
    setsid();
    umask(0);
    chdir(getpwuid(getuid())->pw_dir);
  #endif
	
    //zlog init
    if(dzlog_init("/usr/local/tipschecker/etc/log.conf", "Manager")){
        printf("dzlog init failed\n");
    }
	WtPid();

	TipsManager tipsmanager;
	dzlog_info("Start_Init_Module()!");

	tipsmanager.Init_Module();
	dzlog_info("Exit_Init_Module()!Start_Module");
	tipsmanager.Start_Module();
	for(;;)
	{
		pause();
	}
	return 0;
}
