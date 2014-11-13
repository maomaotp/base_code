#include <stdio.h>
#include <time.h>
#include <string.h>
int get_day(char date_time[][11])
{
	int year,month,day,last_year,last_month,year_flag = 0;
	int i = 1,j = 0;
	char dalte_buf[10];
	int day_temp = 0;
//	char date_time[5][11];
	struct tm *p;
	time_t date;
	char time_buff[30];
	char tim[20];
	time(&date);
	memset(time_buff,0,sizeof(time_buff));
	memset(tim,0,sizeof(tim));
//	for(j=0;j<5;j++)
//		memset(date_time[j],0,sizeof(date_time[j]));
	p = localtime(&date);
	sprintf(time_buff,"%d-%02d-%02d-%02d-%02d-%02d",p->tm_year+1900,p->tm_mon+1,p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);
	strncpy(tim,time_buff,10);
	puts(tim);
	puts(time_buff);
	sscanf(tim,"%d-%d-%d%s",&year,&month,&day,dalte_buf);
	printf("%d:::%d::::%d\n",year,month,day);
	day_temp = day;
	last_year = year;
	last_month= month;
	if(year%4==0 && year%100 !=0 || year%400 ==0)
		year_flag = 1;	
	printf("year_flag=================%d\n",year_flag);
//	day = 7;
	for(i=1;i<6;i++)	
	{
		if(day==1)
			break;
		day = day -1;
//		printf("day================%d\n",day);
	}
	printf("i=============%d\n",i);
	if(i <  5)
	{
		if(i==1)	
		{
			if(month==1)
				{
					last_year=year-1;
					last_month= 12;
					sprintf(date_time[0],"%d-%02d-%02d",last_year,last_month,28);
					sprintf(date_time[1],"%d-%02d-%02d",last_year,last_month,29);
					sprintf(date_time[2],"%d-%02d-%02d",last_year,last_month,30);
					sprintf(date_time[3],"%d-%02d-%02d",last_year,last_month,31);
					sprintf(date_time[4],"%d-%02d-%02d",year,month,day_temp);
				}
			else if(month==2 || month==4 || month==6 || month == 9 || month==11)
				{
					last_month = month-1;		
					sprintf(date_time[0],"%d-%02d-%02d",year,last_month,28);
					sprintf(date_time[1],"%d-%02d-%02d",year,last_month,29);
					sprintf(date_time[2],"%d-%02d-%02d",year,last_month,30);
					sprintf(date_time[3],"%d-%02d-%02d",year,last_month,31);
					sprintf(date_time[4],"%d-%02d-%02d",year,month,day_temp);
				}
			else if(month==3)
				{
					last_month = month-1;	
					if(year_flag)
					{
						sprintf(date_time[0],"%d-%02d-%02d",year,last_month,26);
						sprintf(date_time[1],"%d-%02d-%02d",year,last_month,27);
						sprintf(date_time[2],"%d-%02d-%02d",year,last_month,28);
						sprintf(date_time[3],"%d-%02d-%02d",year,last_month,29);
						sprintf(date_time[4],"%d-%02d-%02d",year,month,day_temp);
					}
					else
					{
						sprintf(date_time[0],"%d-%02d-%02d",year,last_month,25);
						sprintf(date_time[1],"%d-%02d-%02d",year,last_month,26);
						sprintf(date_time[2],"%d-%02d-%02d",year,last_month,27);
						sprintf(date_time[3],"%d-%02d-%02d",year,last_month,28);
						sprintf(date_time[4],"%d-%02d-%02d",year,month,day_temp);
					}
				}
			else 
				{
						last_month = month-1;
						sprintf(date_time[0],"%d-%02d-%02d",year,last_month,27);
						sprintf(date_time[1],"%d-%02d-%02d",year,last_month,28);
						sprintf(date_time[2],"%d-%02d-%02d",year,last_month,29);
						sprintf(date_time[3],"%d-%02d-%02d",year,last_month,30);
						sprintf(date_time[4],"%d-%02d-%02d",year,month,day_temp);
				}
		}
		else if(i==2)	
		{
				if(month==1)
				{
					last_year=year-1;
					last_month= 12;
					sprintf(date_time[0],"%d-%02d-%02d",last_year,last_month,29);
					sprintf(date_time[1],"%d-%02d-%02d",last_year,last_month,30);
					sprintf(date_time[2],"%d-%02d-%02d",last_year,last_month,31);
					sprintf(date_time[3],"%d-%02d-%02d",year,month,day_temp-1);
					sprintf(date_time[4],"%d-%02d-%02d",year,month,day_temp);
				}
			else if(month==2 || month==4 ||month==6 || month == 9 ||month==11)
				{
					last_month = month-1;		
					sprintf(date_time[0],"%d-%02d-%02d",year,last_month,29);
					sprintf(date_time[1],"%d-%02d-%02d",year,last_month,30);
					sprintf(date_time[2],"%d-%02d-%02d",year,last_month,31);
					sprintf(date_time[3],"%d-%02d-%02d",year,month,day_temp-1);
					sprintf(date_time[4],"%d-%02d-%02d",year,month,day_temp);
				}
			else if(month==3)
				{
					last_month = month-1;	
					if(year_flag)
					{
						sprintf(date_time[0],"%d-%02d-%02d",year,last_month,27);
						sprintf(date_time[1],"%d-%02d-%02d",year,last_month,28);
						sprintf(date_time[2],"%d-%02d-%02d",year,last_month,29);
						sprintf(date_time[3],"%d-%02d-%02d",year,month,day_temp-1);
						sprintf(date_time[4],"%d-%02d-%02d",year,month,day_temp);
					}
					else
					{
						sprintf(date_time[0],"%d-%02d-%02d",year,last_month,26);
						sprintf(date_time[1],"%d-%02d-%02d",year,last_month,27);
						sprintf(date_time[2],"%d-%02d-%02d",year,last_month,28);
						sprintf(date_time[3],"%d-%02d-%02d",year,month,day_temp-1);
						sprintf(date_time[4],"%d-%02d-%02d",year,month,day_temp);
					}
				}
			else 
				{
						last_month = month-1;
						sprintf(date_time[0],"%d-%02d-%02d",year,last_month,28);
						sprintf(date_time[1],"%d-%02d-%02d",year,last_month,29);
						sprintf(date_time[2],"%d-%02d-%02d",year,last_month,30);
						sprintf(date_time[3],"%d-%02d-%02d",year,month,day_temp-1);
						sprintf(date_time[4],"%d-%02d-%02d",year,month,day_temp);
				}
		}
		else if(i==3)	
		{
			if(month==1)
				{
					last_year=year-1;
					last_month= 12;
					sprintf(date_time[0],"%d-%02d-%02d",last_year,last_month,30);
					sprintf(date_time[1],"%d-%02d-%02d",last_year,last_month,31);
					sprintf(date_time[2],"%d-%02d-%02d",year,month,day_temp-2);
					sprintf(date_time[3],"%d-%02d-%02d",year,month,day_temp-1);
					sprintf(date_time[4],"%d-%02d-%02d",year,month,day_temp);
				}
			else if(month==2 || month==4 ||month==6 || month == 9 ||month==11)
				{
					last_month = month-1;		
					sprintf(date_time[0],"%d-%02d-%02d",year,last_month,30);
					sprintf(date_time[1],"%d-%02d-%02d",year,last_month,31);
					sprintf(date_time[2],"%d-%02d-%02d",year,month,day_temp-2);
					sprintf(date_time[3],"%d-%02d-%02d",year,month,day_temp-1);
					sprintf(date_time[4],"%d-%02d-%02d",year,month,day_temp);
				}
			else if(month==3)
				{
					last_month = month-1;	
					if(year_flag)
					{
						sprintf(date_time[0],"%d-%02d-%02d",year,last_month,28);
						sprintf(date_time[1],"%d-%02d-%02d",year,last_month,29);
						sprintf(date_time[2],"%d-%02d-%02d",year,month,day_temp-2);
						sprintf(date_time[3],"%d-%02d-%02d",year,month,day_temp-1);
						sprintf(date_time[4],"%d-%02d-%02d",year,month,day_temp);
					}
					else
					{
						sprintf(date_time[0],"%d-%02d-%02d",year,last_month,27);
						sprintf(date_time[1],"%d-%02d-%02d",year,last_month,28);
						sprintf(date_time[2],"%d-%02d-%02d",year,month,day_temp-2);
						sprintf(date_time[3],"%d-%02d-%02d",year,month,day_temp-1);
						sprintf(date_time[4],"%d-%02d-%02d",year,month,day_temp);
					}
				}
			else 
				{
						last_month = month-1;
						sprintf(date_time[0],"%d-%02d-%02d",year,last_month,29);
						sprintf(date_time[1],"%d-%02d-%02d",year,last_month,30);
						sprintf(date_time[2],"%d-%02d-%02d",year,month,day_temp-2);
						sprintf(date_time[3],"%d-%02d-%02d",year,month,day_temp-1);
						sprintf(date_time[4],"%d-%02d-%02d",year,month,day_temp);
				}

		}
		else if(i==4)
		{
				if(month==1)
				{
					last_year=year-1;
					last_month= 12;
					sprintf(date_time[0],"%d-%02d-%02d",last_year,last_month,31);
					sprintf(date_time[1],"%d-%02d-%02d",year,month,day_temp-3);
					sprintf(date_time[2],"%d-%02d-%02d",year,month,day_temp-2);
					sprintf(date_time[3],"%d-%02d-%02d",year,month,day_temp-1);
					sprintf(date_time[4],"%d-%02d-%02d",year,month,day_temp);
				}
			else if(month==2 || month==4 ||month==6 || month == 9 ||month==11)
				{
					last_month = month-1;		
					sprintf(date_time[0],"%d-%02d-%02d",year,last_month,31);
					sprintf(date_time[1],"%d-%02d-%02d",year,month,day_temp-3);
					sprintf(date_time[2],"%d-%02d-%02d",year,month,day_temp-2);
					sprintf(date_time[3],"%d-%02d-%02d",year,month,day_temp-1);
					sprintf(date_time[4],"%d-%02d-%02d",year,month,day_temp);
				}
			else if(month==3)
				{
					last_month = month-1;	
					if(year_flag)
					{
						sprintf(date_time[0],"%d-%02d-%02d",year,last_month,29);
						sprintf(date_time[1],"%d-%02d-%02d",year,month,day_temp-3);
						sprintf(date_time[2],"%d-%02d-%02d",year,month,day_temp-2);
						sprintf(date_time[3],"%d-%02d-%02d",year,month,day_temp-1);
						sprintf(date_time[4],"%d-%02d-%02d",year,month,day_temp);
					}
					else
					{
						sprintf(date_time[0],"%d-%02d-%02d",year,last_month,28);
						sprintf(date_time[1],"%d-%02d-%02d",year,month,day_temp-3);
						sprintf(date_time[2],"%d-%02d-%02d",year,month,day_temp-2);
						sprintf(date_time[3],"%d-%02d-%02d",year,month,day_temp-1);
						sprintf(date_time[4],"%d-%02d-%02d",year,month,day_temp);
					}
				}
			else 
				{
						last_month = month-1;
						sprintf(date_time[0],"%d-%02d-%02d",year,last_month,30);
						sprintf(date_time[1],"%d-%02d-%02d",year,month,day_temp-3);
						sprintf(date_time[2],"%d-%02d-%02d",year,month,day_temp-2);
						sprintf(date_time[3],"%d-%02d-%02d",year,month,day_temp-1);
						sprintf(date_time[4],"%d-%02d-%02d",year,month,day_temp);
				}

		}
	}
	else 
	{
		printf("dat_temp===========%d\n",day_temp);
		printf("dat_temp===========%d\n",year);
		printf("dat_temp===========%d\n",month);
		sprintf(date_time[0],"%d-%02d-%02d",year,month,day_temp-4);
		sprintf(date_time[1],"%d-%02d-%02d",year,month,day_temp-3);
		sprintf(date_time[2],"%d-%02d-%02d",year,month,day_temp-2);
		sprintf(date_time[3],"%d-%02d-%02d",year,month,day_temp-1);
		sprintf(date_time[4],"%d-%02d-%02d",year,month,day_temp);
	}
	for (j=0;j<5;j++)
		printf("%s\n",date_time[j]);
	return 0;
}

int main()
{
#if 0
	int n = 0;
	char buf[5][11];
	for(n=0;n<5;n++)
		memset(buf[n],0,sizeof(buf[n]));
	get_day(buf);
#endif 
	char a[10];	
	char c[10];
	char b;
	memset(a,0,sizeof(a));	
	memset(c,0,sizeof(c));	
	if(!strlen(a))
		printf("%s\n",a);
	else 
		puts("fjiejfie");
	if(b=='\0')	
	printf("b==	==============%cjfiejifjeijfie\n",b);
	sprintf(c,"%s",a);
	puts(c);
	return 0;
}
