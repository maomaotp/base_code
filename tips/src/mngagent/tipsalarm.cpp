#include "tipsalarm.h"
#include <string.h>
#include <signal.h>
#include <stdio.h>
/*void alarm_hander(int param)
{
	thealarm.Run_Alarm_Task();
	thealarm.Set_An_Alarm();
}
*/
int TipsAlarm::Init_Alarm()
{
	alarmhead=NULL;
	regnum=1;//定时任务个数
	//start_alarms=0;//标志是否启动倒计时
	//
	
	return 0;		//hyj 返回值?
}
int TipsAlarm::Start_Alarm()
{
	return 0;		//hyj 返回值?
}





int TipsAlarm::UnRegister_Alarm(int clientreg)
{
	struct alarm_struct *ptr,**prevnext=&alarmhead;
	for(ptr=alarmhead;ptr!=NULL&&ptr->clientreg!=(unsigned int)clientreg;ptr=ptr->next)		//update by hyj
	{
		prevnext=&(ptr->next);
	}
	if(ptr!=NULL)
	{
		struct alarm_struct* p=ptr->next;
		while(p)
		{
			p->clientreg--;
			p=p->next;
		}
		*prevnext=ptr->next;
		delete ptr;
		regnum--;
	}
	return 1;		//hyj 返回值?
}
void TipsAlarm::Set_Update_Entry(struct alarm_struct* a)
{
	if(a->t_last.tv_sec==0&&a->t_last.tv_usec==0)
	{//这是新建的定时器，上次到时时间为0
		struct timeval t_now;
		gettimeofday(&t_now,NULL);
		a->t_last.tv_sec=t_now.tv_sec;//设置上次到时时间为当前时间
		a->t_last.tv_usec=t_now.tv_usec;		
		a->t_next.tv_sec=t_now.tv_sec+a->t.tv_sec;//设置下次到时时间
		a->t_next.tv_usec=t_now.tv_usec+a->t.tv_usec;
		
		while(a->t_next.tv_usec>=1000000)
		{
			a->t_next.tv_usec-=1000000;
			a->t_next.tv_sec+=1;
		}
	}
	else if(a->t_next.tv_sec==0&&a->t_next.tv_usec==0)
	{//这是非新创建的的定时器
		if(a->t.tv_sec==0&&a->t.tv_usec==0)
		{//没有设置定时时间
			UnRegister_Alarm(a->clientreg);	
			return;
		}
		a->t_next.tv_sec=a->t_last.tv_sec+a->t.tv_sec;
		a->t_next.tv_usec=a->t_last.tv_usec+a->t.tv_usec;
		while(a->t_next.tv_usec>=1000000)
		{
			a->t_next.tv_usec-=1000000;
			a->t_next.tv_sec+=1;
		}
	}
}
int TipsAlarm::Register_Alarm(unsigned int when,ITimerTask* task,void* clientarg)
{
	struct alarm_struct* current=new alarm_struct;
	memset(current,0,sizeof(alarm_struct));
//	memcpy(current,&alarm,sizeof(alarm_struct));
	if(when==0)
	{
		current->t.tv_sec=ALARMTIME;
		current->t.tv_usec=1;
	}
	else
	{
		current->t.tv_sec=when;
		current->t.tv_usec=0;
	}
	current->clientarg=clientarg;
	current->timetask=task;
	current->clientreg=regnum++;	
	current->timetask->flag=current->clientreg;
	if(alarmhead==NULL)
	{
		alarmhead=current;//如果当前没有定时任务
		Set_Update_Entry(current);//设置该定时器下次超时时间
		return 1;
	}
	struct alarm_struct *pnext=alarmhead;
	struct alarm_struct* ptr;
	while(pnext)
	{
		ptr=pnext;
		pnext=pnext->next;
	}
	ptr->next=current;
	Set_Update_Entry(current);//设置该定时器下次超时时间
	return 1;
}






struct TipsAlarm::alarm_struct* TipsAlarm::Find_Next_Alarm()
{
	struct alarm_struct* a,*lowest=NULL;
	for(a=alarmhead;a!=NULL;a=a->next)
	{
		if(lowest==NULL)lowest=a;
		else if(a->t_next.tv_sec==lowest->t_next.tv_sec)
		{
			if(a->t_next.tv_usec<lowest->t_next.tv_usec)
			{
				lowest=a;
			}
		}
		else if(a->t_next.tv_sec<lowest->t_next.tv_sec)
		{
			lowest=a;
		}
	}
	return lowest;
}
int TipsAlarm::Get_Next_Alarm_Delay_Time(struct timeval* delta)
{//获得最早超时时间
	struct alarm_struct* ptr;
	struct timeval t_diff,t_now;
	ptr=Find_Next_Alarm();
	if(ptr)
	{
		gettimeofday(&t_now,0);
		if((t_now.tv_sec>ptr->t_next.tv_sec)||((t_now.tv_sec==ptr->t_next.tv_sec)&&(t_now.tv_usec>ptr->t_next.tv_usec)))
		{//已经超时
			delta->tv_sec=0;
			delta->tv_usec=1;
			return ptr->clientreg;
		}
		else
		{//尚未超时
			t_diff.tv_sec=ptr->t_next.tv_sec-t_now.tv_sec;
			t_diff.tv_usec=ptr->t_next.tv_usec-t_now.tv_usec;
			while(t_diff.tv_usec<0)
			{
				t_diff.tv_sec-=1;
				t_diff.tv_usec+=1000000;
			}
			delta->tv_sec=t_diff.tv_sec;
			delta->tv_usec=t_diff.tv_usec;
			return ptr->clientreg;
		}
	}
	return 0;
}
void TipsAlarm::Set_An_Alarm()
{
	//设置下一个最近到时定时任务的信号
	struct timeval delta;
	int nextalarm=Get_Next_Alarm_Delay_Time(&delta);
	if(nextalarm)
	{
		struct itimerval it;
		it.it_value.tv_sec=delta.tv_sec;
		it.it_value.tv_usec=delta.tv_usec;
		it.it_interval.tv_sec=0;//只设置value那么只定时一次
		it.it_interval.tv_usec=0;
		signal(SIGALRM,alarm_hander);
#if 0 
		struct sigaction act, oact;
		act.sa_handler = alarm_hander;
		sigemptyset (&act.sa_mask);
		act.sa_flags = 0;
#ifndef SA_RESTART
			act.sa_flags |= SA_RESTART;
#endif
		if (sigaction (SIGALRM, &act, &oact) < 0)
			cerr << "sigaction error!" <<endl;;
#endif		
		printf("next SIGALRM sec=%ld\n", it.it_value.tv_sec);
	//	cout<<"next SIGALRM sec="<<it.it_value.tv_sec<<endl;
		setitimer(ITIMER_REAL,&it,NULL);
	}
}










struct TipsAlarm::alarm_struct* TipsAlarm::Find_Specific_Alarm(int clientreg)
{
	struct alarm_struct* ptr;
	for(ptr=alarmhead;ptr!=NULL;ptr=ptr->next)
	{
		if(ptr->clientreg==(unsigned int)clientreg)return ptr;
	}
	return NULL;
}
int TipsAlarm::Run_Alarm_Task()
{
	//运行所有到时任务
	int done=0;
	struct alarm_struct* a=NULL;
	unsigned int clientreg;
	struct timeval t_now;
//hyj	int start_i=0;
	while(!done)
	{
		if((a=Find_Next_Alarm())==NULL)
		{//没有将要超时的定时任务
			return 0;
		}
		gettimeofday(&t_now,NULL);
		if((a->t_next.tv_sec<t_now.tv_sec)||((a->t_next.tv_sec==t_now.tv_sec)&&(a->t_next.tv_usec<t_now.tv_usec)))
		{
			clientreg=a->clientreg;
			a->timetask->Start_Task();//运行注册的定时任务
			if((a=Find_Specific_Alarm(clientreg))!=NULL)
			{
				a->t_last.tv_sec=t_now.tv_sec;
				a->t_last.tv_usec=t_now.tv_usec;
				a->t_next.tv_sec=0;
				a->t_next.tv_usec=0;
				Set_Update_Entry(a);
			}
			
		}
		else
		{
			done=1;
		}
	}
	return 1;		//hyj 返回值？
}

int TipsAlarm::Modify_Task_Alarm()
{
        struct alarm_struct *ptr,**prevnext=&alarmhead;
	if(systime_gap.sign == 1)
	{
       		for(ptr=alarmhead;ptr!=NULL;ptr=ptr->next)
		{
			ptr->t_next.tv_sec += systime_gap.timegap;
			ptr->t_last.tv_sec += systime_gap.timegap;
        	}
	}
	else if(systime_gap.sign == -1)
	{
       		for(ptr=alarmhead;ptr!=NULL;ptr=ptr->next)
		{
			ptr->t_next.tv_sec -= systime_gap.timegap;
			ptr->t_last.tv_sec -= systime_gap.timegap;
        	}
	}
	prevnext = prevnext;		//hyj 未使用的变量
	return 0;
}

int TipsAlarm::Get_Systime_Gap(time_t old_time, time_t new_time)
{
	if(old_time <= new_time)
	{
		systime_gap.sign = 1;
		systime_gap.timegap = new_time - old_time;
	}
	else if(old_time > new_time)
	{
		systime_gap.sign = -1;
		systime_gap.timegap = old_time - new_time;	
	}
	return 0;
}
