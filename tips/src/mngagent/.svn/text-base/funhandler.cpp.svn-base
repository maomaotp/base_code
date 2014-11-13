#include "funhandler.h"
#include <time.h>
int Funhandler::Turntime(time_t clock,char* tips_clock,int clock_size)
{
	if(clock_size<20)return -1;
	struct tm* tm;
	char* format=(char*)"%Y-%m-%d %H:%M:%S";
	tm=localtime(&clock);
	strftime(tips_clock,clock_size,format,tm);
	return 1;
}




