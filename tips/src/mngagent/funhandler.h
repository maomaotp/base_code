#ifndef FUNHANDLER_H
#define FUNHANDLER_H
#include <sys/types.h>
class Funhandler{
	public:
	int Turntime(time_t clock,char* tips_clock,int clock_size);
	//函数功能将time_t类型的clock时间转换成|2013-12-31 12:00:00|格式的tips_clock时间
	//成功返回1，失败返回-1--->clock_size小于转换后的长度
};
#endif
