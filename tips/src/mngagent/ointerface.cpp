#include "ointerface.h"
#include "rulesdata.h"
#include <iostream>
#include "./net_guard_final/client.h"
using namespace std;
int OInterface::Send_Status(struct status_struct* status,char** cmdbuff)
//int OInterface::Send_Status(struct status_struct* status,char* cmdbuff)
{//如果没有管控命令返回0，否则返回命令长度
//	cout<<"调用上传状态函数!"<<endl;
	int n = SendStatus(status,&cmdbuff,1);
	n = SendStatus(status,&cmdbuff,0);
/*	int i = 0;
	for(i=0;i<n;i++)
	{
	//	cout<<"@@@@@"<<cmdbuff[i]<<endl;
	}
*/
	return n;
}
int OInterface::Send_Config(struct config_struct* config,char* cmdbuff)
{//如果没有管控命令返回0，否则返回命令长度
//	cout<<"调用上传状态函数!"<<endl;
	
	 SendConfig(config,cmdbuff,1);
	return SendConfig(config,cmdbuff,0);
}

int OInterface::Load_Rules(char* filename,void** rule_obj,int type)
{//给*rule_obj分配空间，赋值，返回个数
	int resultnum = 0;
	//resultnum = LoadRules(filename,rule_obj,type,1);
	resultnum = LoadRules(filename,rule_obj,type,0);
#if 0 
	cout <<"resultnum ===="<<resultnum <<endl; 
	for(int i=0;i<resultnum;i++)
	{
		cout<<"-----------ointerface.cpp------------"<<endl;
		cout<<((struct dns_rules*)*rule_obj + i)->type<<endl;
		cout<<((struct dns_rules*)*rule_obj + i)->dns_name<<endl;
		cout<<((struct dns_rules*)*rule_obj + i)->match_mode<<endl;
		cout<<((struct dns_rules*)*rule_obj + i)->dangerous<<endl;
		cout<<"-----------ointerface.cpp-------------"<<endl;
	}
#endif
	return resultnum;
}
int OInterface::Update_Sys(char* filename)
{//下载系统升级包到/etc/tipscheck
    UpdateSys(filename,1);
	return UpdateSys(filename,0);
}


int OInterface::Send_Data (void** alt, int len, int type)
{
	SendData(alt, len, type,1);
	return SendData(alt, len, type,0);
}
