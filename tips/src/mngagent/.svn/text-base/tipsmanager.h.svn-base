#ifndef TIPSMANAGER_H
#define TIPSMANAGER_H
#include "data.h"
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <linux/if.h>
#include <linux/if_arp.h>
#include <arpa/inet.h>
#include <errno.h>
#include "itimertask.h"
#include "objectcreator.h"
#include "sqlhandler.h"
#include "ui_manage_server.h"
#define UI_UNIX_DOMAIN "/tmp/UNIXUI.domain"
#define BuffSize 256
#define MANAGE_IP (char *)"ADMINSTRATION_IP"
#define MANAGE_IP1 (char *)"ADMINSTRATION_IP1"
//#define MANAGE_IP (char *)"MANAGE_IP"
#define MANAGE_PORT (char *)"MANAGE_PORT"
#define MANAGE_PORT1 (char *)"MANAGE_PORT1"
using namespace std;
#include "reporttask.h"
#include "statustask.h"
#include "mngchkpostman.h"
#include "mngchkdata.h"
#include "clienthandler.h"
#include "funhandler.h"
#include "./sharemem/sharemem.h"

class TipsManager{
	int classid[CLASSCOUNT];
	ITimerTask* timetask[CLASSCOUNT];
	void ConfigInit();
//	CGetMac getmac;
	CUI_Manage_ser manageser;
	private:
//定义共用资源
	Sqlhandler sqlhandler;
	CCacheclient ccacheclient;
	Funhandler funhandler;
	int sockfd;
	int sign;
	bool ParseBuff(char *tmp,char *Buff);
	void Get_ManageIP(char* ip);
	void Get_ManageIP1(char* ip);
	void WtDevID();
	bool ParBuff(char *buffer);
	void status_Init(struct status_part* status);
	int Get_Mem();
	int Get_Cpu();
	double Get_Disk();
	char* Memtotal;
	char* Cached;
	char* Memfree;
	char* Buffers;
	double total, cached, buffers, memfree;
	public:
	double disk;
	int mem, cpu;
	TipsManager();
	int Init_Module();
	int Exit_Module();
	int Start_Module();
	int Restart_Module();
};
#endif
