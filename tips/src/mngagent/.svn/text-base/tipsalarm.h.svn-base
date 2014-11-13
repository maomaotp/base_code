#ifndef TIPSALARM_H
#define TIPSALARM_H
#include<sys/time.h>
#include<iostream>
#include "itimertask.h"
#define ALARMTIME 3*60
//#define SA_REPEAT 0x01
using namespace std;
void alarm_hander(int param);//定时器到时回调函数
class TipsAlarm{
struct alarm_struct
{
	struct timeval t;//倒计时数
//	int flags;
	unsigned int clientreg;
	struct timeval t_last;//上次超时
	struct timeval t_next;//下次超时
	void* clientarg;
	ITimerTask* timetask;
	struct alarm_struct* next;
};//定时任务结构体
	struct systime_gap_t
{
	int sign;
	time_t timegap;
};
	struct systime_gap_t systime_gap;
	int regnum;//定时任务数量
	int start_alarms;//是否已经设置定时器标志
	struct alarm_struct* alarmhead;//定时任务链表头
	struct alarm_struct* Find_Next_Alarm();//寻找最近超时的定时任务
	struct alarm_struct* Find_Specific_Alarm(int clientreg);//查找clientreg的定时任务
	void Set_Update_Entry(struct alarm_struct* alarm);//设置定时任务下次超时时间
	int Get_Next_Alarm_Delay_Time(struct timeval* delta);//查找最近超时时间
	public:
	int UnRegister_Alarm(int clientreg);//注销一个定时任务
	int Run_Alarm_Task();//运行所有已经超时的定时任务
	int Modify_Task_Alarm();//修改所有定时任务下次超时时间
	int Get_Systime_Gap(time_t old_time, time_t new_time);
	void Set_An_Alarm();//设置下一个将要超时时间定时器
	int Register_Alarm(unsigned int when,ITimerTask* task,void* clientarg);//想定时器中注册定时任务
	
	int Start_Alarm();
	int Init_Alarm();//初始化定时器
	int Stop_Alarm();
	void Destroy_Alarm();
};
#endif
