#include "itimertask.h"
int ITimerTask::Get_Time()
{
	return Second_Time;
}
ITimerTask::ITimerTask()
{
//	cpostman.Init_Postman();
}
void ITimerTask::Set_Flag(int f)
{
	flag=f;
}
void ITimerTask::Set_Funhandler(Funhandler* fun)
{
	funhandler=fun;
}
void ITimerTask::Set_Cacheclient(CCacheclient* cache)
{
	cacheclient=cache;
}
void ITimerTask::Set_Sqlhandler(Sqlhandler* sql)
{
	sqlhandler=sql;
}
