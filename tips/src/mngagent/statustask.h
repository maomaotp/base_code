#ifndef STATUSTASK_H
#define STATUSTASK_H
#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <sys/vfs.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include "itimertask.h"
#include "rulesdata.h"
#include "data.h"
#include "reporttask.h"
#include "net_guard_final/client.h"
#include "netlog.h"
#include "reporttask.h"
#define DAEMON_PID "/usr/local/tipschecker/etc/tipscheck/daemonpid"
#define ALARM_PID "/usr/local/tipschecker/etc/tipscheck/trojanpid"
#define CHECK_PID "/usr/local/tipschecker/etc/tipscheck/checkerpid"
#define AUDIT_PID "/usr/local/tipschecker/etc/tipscheck/auditpid"
#define BLOCK_PID "/usr/local/tipschecker/etc/tipscheck/blockpid"
#define SENST_PID "/usr/local/tipschecker/etc/tipscheck/senstpid"
#define UI_PID "/usr/local/tipschecker/etc/tipscheck/userpid"
#define CACHE_PID "/usr/local/tipschecker/etc/tipscheck/cachepid"
#define CONFIG_MAXLINE 4098
#define CONFIG_FILE "/usr/local/tipschecker/etc/tipscheck/config"
#include "tipsmanager.h"
class StatusTask:public ITimerTask
{
	ReportTask report;
//	struct config_struct config;
	double total, cached, buffers, memfree;
	char* frules;
	char* srules;
	char* Memtotal;
	char* Cached;
	char* Memfree;
	char* Buffers;
	void Parse_Cmd(char* cmd);
	int Get_Param(char* cmd,char* param);
	int Get_Mem();
	int Get_Cpu();

	/*add by lipeiting 2014--6-18*/
	int init_one_config(const char *source, const char *descript, FILE *fp);
	int get_config_manage_xml(void *config);

	double Get_Disk();
	int Get_Format(char*, int, char*, void**, int*);
	int Add_Str(int type, char* buf, int count);
	int Add_Str2(int type, char* buf, int count);
	void signal_usr1(char* str, char*);
 	int Getpid(pid_t *pid, const char* process_id);
	void status_Init(struct status_part* status);
	public:
	double disk;
	int mem, cpu;
	StatusTask();
	virtual void Start_Task();
	virtual void Init_Task();
};
#endif

