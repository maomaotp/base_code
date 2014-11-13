#ifndef ITIMERTASK_H
#define ITIMERTASK_H
#include<iostream>
#include "publicdata.h"
#include "mngchkpostman.h"
#include "ointerface.h"
#include "sqlhandler.h"
#include "clienthandler.h"
#include "funhandler.h"
#include "cachestruct.h"
using namespace std;
class ITimerTask{
	
	protected:
	int Second_Time;
	//定义通信接口对象，等待调用
	MngChkPostman cpostman;
	OInterface ointerface;
	Sqlhandler* sqlhandler;
	CCacheclient* cacheclient;
	Funhandler* funhandler;
	public:
	int flag;
	ITimerTask();
virtual void Start_Task()=0;
virtual void Init_Task()=0;
virtual int Get_Time();
virtual void Set_Flag(int f);
virtual void Set_Sqlhandler(Sqlhandler* sql);
virtual void Set_Cacheclient(CCacheclient* cacheclient);
virtual void Set_Funhandler(Funhandler* fun);
};
#endif
