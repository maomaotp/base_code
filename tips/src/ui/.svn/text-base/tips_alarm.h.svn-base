#ifndef TIPS_ALARM
#define TIPS_ALARM
#include "tips_sqlite.h"
#include <iostream>

using namespace std;

typedef struct alarm_stut{
	char *Trojan_id;
	char *Sip;
	char *Dip;
	char *Dmac;
	char *Alarm_time;
	char *Alarm_end_time;
	char *Type;
	char *Risk;
	char *Scc_flag;

}alarm_stut_t;

class CTips_alarm{
public:
	int Alarm_init_sql_interface(alarm_stut_t *salarm, uint32_t pagenum, char *sqlpart,int how = 1);
	int Copy_init_sql(alarm_stut_t *salarm, uint32_t pagenum);
	int Copy_exec_sql(int all=0);
	char* Alarm_exec_sql(int all = 0);
	int Alarm_one_init(char **sqltemp, char *name, char *value, char* match);
	int Alarm_init_sql(alarm_stut_t *salarm, uint32_t pagenum, int sort = 1);
	int Delete_init_sql(alarm_stut_t *salarm, uint32_t pagenum);
	int Delete_exec_sql(int all = 0);
	int Count_init_sql(alarm_stut_t *salarm, uint32_t pagenum);
	int Count_picture_sql(int *count,const char *search_type);
	int Count_risk_sql(int *count, int search_type,const char *start_time,const char *end_time);
	int Count_exec_sql(uint32_t *total_number=NULL);
private:
	uint32_t alarm_pagenum;
	char *sql;
	int search_flag;
	int the_same_day_judge;
};

#endif
