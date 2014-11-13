#ifndef TIPS_EVENT
#define TIPS_EVENT
#include "tips_sqlite.h"
#include <iostream>

using namespace std;

typedef struct event_stut{
	char *Event_class;		/*事件类型*/
	char *Event_type;		/*事件子类*/
	char *Event_time;		/*事件产生时间*/
	char *Sip;			/*外部主机ip*/
	char *Sport;			/*外部主机端口*/
	char *Dip;			/*内部主机ip*/
	char *Dport;			/*内部端口*/
	char *Dmac;			/*本地主机mac*/
	char *Sender;			/*发送人*/
	char *Receiver;			/*接收人*/
	char *Title;			/*电子邮件标题*/
	char *File_name;		/*文件名*/
	char *Descr;			/*描述*/
	char *Sen_msg;			/*敏感词*/
}event_stut_t;

class CTips_Event{
public:
	char* Event_exec_sql(int all = 0);
	int Event_one_init(char **sqltemp, char *name, char *value, char* match);
	int Event_init_sql(event_stut_t *sblock, uint32_t pagenum,int sort = 1);
	int Copy_init_sql(event_stut_t *sevent, uint32_t pagenum);
	int Copy_exec_sql(int all = 0);
	int Delete_init_sql(event_stut_t *sevent, uint32_t pagenum);
	int Delete_exec_sql(int all = 0);
	int Event_init_sql_interface(event_stut_t *sevent, char* sqlpart,int how = 1);
	int Count_init_sql(event_stut_t *sevent, uint32_t pagenum);
	int Count_exec_sql();
	int Count_picture_sql(int *count, const char *search_type);
	int Count_risk_sql(int *count,int search_type,const char *start_time,const char *end_time);
private:
	uint32_t event_pagenum;
	char *sql;
};

#endif
