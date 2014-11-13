#include "audittask.h"
#include <sys/types.h>
#include <signal.h>
#include "data.h"
extern int startrun;
extern struct status_struct status;
void AuditTask::Start_Task()
{
	char buffer[1024];
	memset (buffer, 0, sizeof (buffer));
	FILE* fp = fopen (RUNCONGFILE, "r");
	if (NULL == fp)
		cerr <<strerror (errno) <<endl;
	while(fgets (buffer, sizeof (buffer), fp))
	{
		Deal_Task (buffer, (char*)"CPU");
		Deal_Task (buffer, (char*)"MEM");
		Deal_Task (buffer, (char*)"DISK");
	}
  fclose (fp);
}

void AuditTask::Deal_Task(char* buffer, char* str)
{
	if (!strncmp (buffer, str, strlen (str)))
	{
		buffer[strlen(buffer) - 1] = '\0';
		int n = atoi(strchr(buffer, '=') + 1);
		if(!strcmp(str, "CPU"))
		{
			if (status.cpu < n)
				return;
			else
			{
				Deal_CPU();
			}
		}
		else if(!strcmp(str, "MEM"))
		{
			if (status.mem < n)
				return;
			else
			{
				Deal_MEM ();
			}
		}
		else if(!strcmp(str, "DISK"))
		{
			cout <<" n =========================" << n <<endl;
			cout <<" disk =========================" << status.disk <<endl;
  #if 1
			if (status.disk > n)
				return;
			else
			{
				//return;
				Deal_DISK ();
			}
#endif
		}
	}
}

void AuditTask::Deal_CPU()
{		
	 struct cache_opt_t opt_stut;
	 INIT_OPT_HEAD(&opt_stut);
         strcpy(opt_stut.username,"system");
         time_t t;
         time(&t);
         funhandler->Turntime(t,opt_stut.opttime,24);
         strcpy(opt_stut.describe,"CPU利用率过大");
         int send_err=cacheclient->Send_Msg((char*)&opt_stut,sizeof(opt_stut));
		 send_err = send_err;	//hyj 未使用的变量

}

void AuditTask::Deal_MEM()
{
	 struct cache_opt_t opt_stut;
	 INIT_OPT_HEAD(&opt_stut);
         strcpy(opt_stut.username,"system");
         time_t t;
         time(&t);
         funhandler->Turntime(t,opt_stut.opttime,24);
         strcpy(opt_stut.describe,"内存利用率过大");
         int send_err=cacheclient->Send_Msg((char*)&opt_stut,sizeof(opt_stut));
		 send_err = send_err;	//hyj 未使用的变量
}
void AuditTask::Deal_DISK()
{
	 struct cache_opt_t opt_stut;
	 INIT_OPT_HEAD(&opt_stut);
         strcpy(opt_stut.username,"system");
         time_t t;
         time(&t);
         funhandler->Turntime(t,opt_stut.opttime,24);
         strcpy(opt_stut.describe,"磁盘剩余空间不足");
         int send_err=cacheclient->Send_Msg((char*)&opt_stut,sizeof(opt_stut));
		 send_err = send_err;	//hyj 未使用的变量
	 char sql[1024];
	 sprintf(sql,"delete from tips_netlog order by Start_time limit %d;",DELETECOUNT);
	 int err=sqlhandler->Insert_Sql_Exec(sql);
	 cout<<"磁盘剩余空间不足，删除数据条数:"<<err<<endl;
}

void AuditTask::Init_Task()
{
	//cout<<"上报报警信息初始化"<<endl;
	Second_Time= 24*3600*7;
	//Second_Time= 5;
	//Second_Time=10;
}
