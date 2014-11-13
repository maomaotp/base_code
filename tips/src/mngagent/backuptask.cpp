#include "backuptask.h"
#include <iostream>

extern int startrun;

void BackupTask::Start_Task()
{
	cout<<"开始执行数据处理定时任务"<<endl;
	//定期清除数据,删除数据库中部分数据
	//打开数据库，删除数据
}

void BackupTask::Init_Task()
{
//	cout<<"数据处理定时任务初始化"<<endl;
	Second_Time = 24 * 60 * 60;
//	Second_Time = 24;
}
