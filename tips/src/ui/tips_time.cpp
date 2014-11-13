#include "tips_time.h"
#include <string.h>

time_t CTips_time::CatchData(char *seetime)
{
	regex_t preg;
	int day=0,month=0,year=0,hour=0,min=0,sec=0,sum=0,leap=0,weekid=0;
	char errbuf[256];
	int judge;
	char buf[5];
	struct tm tm;
	if((judge = regcomp(&preg, "([0-9]{4})-((0[1-9]{1})|(1[0-2]{1}))-(([0-2]{1}[0-9]{1})|(3[0-1]{1})) +(([0-1]{1}[0-9]{1})|(2[0-3]{1})):([0-5]{1}[0-9]{1}):([0-5]{1}[0-9]{1})", REG_EXTENDED)) != 0){
		regerror(judge, &preg, errbuf, sizeof(errbuf));
		return 0;
	}
	if((judge = regexec(&preg, seetime, 20, pmatch, 0))!=0){
		regerror(judge, &preg, errbuf, sizeof(errbuf));
		return 0;
	}
	regfree(&preg);
	
	memset(buf, 0, 5);
	memcpy(buf, seetime+pmatch[1].rm_so, pmatch[1].rm_eo-pmatch[1].rm_so);
	year = atoi(buf);
	tm.tm_year = year-1900;
	memset(buf, 0, 5);
	memcpy(buf, seetime+pmatch[2].rm_so, pmatch[2].rm_eo-pmatch[2].rm_so);
	month = atoi(buf);
	tm.tm_mon = month - 1;
	memset(buf, 0, 5);
	memcpy(buf, seetime+pmatch[5].rm_so, pmatch[5].rm_eo-pmatch[5].rm_so);
	tm.tm_mday = atoi(buf);
	day = tm.tm_mday;
	memset(buf, 0, 5);
	memcpy(buf, seetime+pmatch[8].rm_so, pmatch[8].rm_eo-pmatch[8].rm_so);
	tm.tm_hour = atoi(buf);
	hour = tm.tm_hour;
	memset(buf, 0, 5);
	memcpy(buf, seetime+pmatch[11].rm_so, pmatch[11].rm_eo-pmatch[11].rm_so);
	tm.tm_min = atoi(buf);
	min = tm.tm_min;
	memset(buf, 0, 5);
	memcpy(buf, seetime+pmatch[12].rm_so, pmatch[12].rm_eo-pmatch[12].rm_so);
	tm.tm_sec = atoi(buf);
	sec = tm.tm_sec;
	if(year%400==0||(year%4==0&&year%100!=0))
		leap=1;
	else
		leap=0;
	switch(month)
	{
		case 1:sum=0;if(day>31) return 0;break;
		case 2:sum=31;if((day>29) || ((leap == 0) && (day == 29))) return 0;break;
		case 3:sum=59;if(day>31) return 0;break;
		case 4:sum=90;if(day>30) return 0;break;
		case 5:sum=120;if(day>31) return 0;break;
		case 6:sum=151;if(day>30) return 0;break;
		case 7:sum=181;if(day>31) return 0;break;
		case 8:sum=212;if(day>31) return 0;break;
		case 9:sum=243;if(day>30) return 0;break;
		case 10:sum=273;if(day>31) return 0;break;
		case 11:sum=304;if(day>30) return 0;break;
		case 12:sum=334;if(day>31) return 0;break;
		default:return 0;
	}
	sum=sum+day; 
	if(leap==1&&month>2)
	{
		sum++;
	}
	if(month==2)		//update by hyj
	{
		month+=12;
		year--;
	}
	weekid=(day+2*month+3*(month+1)/5+year+year/4-year/100+year/400)%7+1;
	if(weekid==7)
		weekid=0;
	tm.tm_wday=weekid;
	tm.tm_yday=sum-1;
	tm.tm_isdst=0;
	time_t time=mktime(&tm);
	printf("%s\n",ctime(&time));
	return time;
}

int CTips_time::CatchSeeTime(time_t clock, char *clock_str)
{
//hyj	char temp[20];
	char *format=(char *)"%Y-%m-%d %H:%M:%S";
	struct tm *tm;
	tm = localtime(&clock);
	strftime(clock_str,  20, format,tm);
	return 0;
}
