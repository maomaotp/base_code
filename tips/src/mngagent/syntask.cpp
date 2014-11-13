#include "syntask.h"
#include "tipsalarm.h"
#include <iostream>
extern int startrun;
extern TipsAlarm thealarm;
void SynTask::Start_Task()
{
//	return ;
	printf("*************************************SynTask::Start_Task*******************************\n");
	fflush(stdout);
	time_t old_time = 0, new_time = 0;
	int ret = 0;
	ret = GetTime(NULL,0,&old_time,&new_time);
	if(-1 == ret)
	{
		struct cache_opt_t opt_stut;
		INIT_OPT_HEAD(&opt_stut);
    strcpy(opt_stut.username,"system");      
		time_t t;
    time(&t);
    funhandler->Turntime(t,opt_stut.opttime,24);
    strcpy(opt_stut.describe,"时间同步失败");
    int send_err=cacheclient->Send_Msg((char*)&opt_stut,sizeof(opt_stut));
	send_err = send_err;		//hyj 未使用的变量
		return;
	}
	thealarm.Get_Systime_Gap(old_time, new_time);
	thealarm.Modify_Task_Alarm();
	thealarm.Set_An_Alarm();
/*
	struct cache_opt_t opt_stut;
	INIT_OPT_HEAD(&opt_stut);
     	strcpy(opt_stut.username,"system");      
	time_t t;
      	time(&t);
      	funhandler->Turntime(t,opt_stut.opttime,24);
      	strcpy(opt_stut.describe,"时间同步成功");
      	int send_err=cacheclient->Send_Msg((char*)&opt_stut,sizeof(opt_stut));
				*/
}
void SynTask::Init_Task()
{
	char cmd[1024];
	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd), "cat %s | grep SYNTIME | cut -d '=' -f2", RUNCONGFILE);
	FILE * fp = popen(cmd, "r");
	memset(cmd, 0, sizeof(cmd));
	if( NULL ==	fgets(cmd, sizeof(cmd), fp))
	{
		printf("SYNTIME not config, use default syntime 60sec\n");
		Second_Time=60;
	}
	else
	{
		Second_Time = atoi(cmd);
		printf("Syntask time is %dsec\n", Second_Time);
	}
}
