#include <string.h>

#include "init.h"

#define PIDFILE  (char *)"/usr/local/tipschecker/etc/tipscheck/alarmid"

extern struct func *head;


void handle_func(int sig)
{
	dzlog_info("tipsALARM: i received the signal and i will sleep 30 ");
    sleep(30);
    dzlog_info("i wake up begin start work");
	if(sig==SIGSTOP) return;

	return;
}

void WtPid()
{
	FILE *fp=NULL;
	char id_buf[10];
	pid_t pid;

	memset(id_buf,0,10);
	pid = getpid();
	dzlog_info("alarm start now, pid==%d", pid);
	printf("alarm start now, pid==%d", pid);
	sprintf(id_buf,"%d",pid);
	if(NULL == (fp = fopen(PIDFILE,"w+"))){
		dzlog_info("open PIDFILE failed!!!");	
		return;
	}
	fwrite(id_buf,strlen(id_buf),1,fp);
	fclose(fp);
	return;
}

void init()
{
    if(dzlog_init("/usr/local/tipschecker/etc/log.conf", "alarm")){
        printf("alarm:zlog init failed\n");
    }
    printf(">>>>>>>>>>>>>zlog init success<<<<<<<<<<<<<<<<<<<\n");

	readrule();
	printf(">>>>>>>>>>>>>>parse rule complite<<<<<<<<<<<<<<<<\n");
	
	readIPfile();
	printf(">>>>>>>>>>>>>>parse IP rule complite<<<<<<<<<<<<<<<<\n");
	
	WtPid();

	char localIP[] = "127.0.0.1";
	if( Init_Client(localIP) < 0 ) {
        dzlog_info("init client failed!!!");
        printf("init client failed!!!\n");
		return;
	}
	return;
}
