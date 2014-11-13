#include "tips_event.h"
#include "tips_xml_part.h"

extern CUser_sqlite_handle tips_sqlite_handle;

xml_part* xml_part_event[] =  {xml_Event_class,
			       xml_Event_time,
                               xml_Sip,
                               xml_Sport,
			       xml_Dip,
			       xml_Dmac,
			       xml_Sender,
			       xml_Receiver,
			       xml_File_name,
			       xml_Sen_msg};

int CTips_Event::Event_one_init(char **sqltemp, char *name, char *value, char* match)
{
	if((value) && (strlen(value))){
		sql = g_strdup_printf("%s %s %s '%s' and",*sqltemp, name, match,value);
		g_free(*sqltemp);
		*sqltemp = sql;
	}
/*返回值？*/return 0;
}

int CTips_Event::Event_init_sql_interface(event_stut_t *sevent, char* sqlpart, int how)
{
	sql = NULL;
	char *Event_class = NULL;
	char *Event_time = NULL;
	char *Sip = NULL;
	char *Sport = NULL;
	char *Dip = NULL;
	char *Sender = NULL;
	char *Receiver = NULL;
	char *Sen_msg = NULL;

	tips_sqlite_handle.init_sql_num(sevent->Event_class, &Event_class);
	tips_sqlite_handle.init_sql_num(sevent->Event_time, &Event_time);
	tips_sqlite_handle.init_sql_num(sevent->Sip, &Sip);
	tips_sqlite_handle.init_sql_num(sevent->Sport, &Sport);
	tips_sqlite_handle.init_sql_num(sevent->Dip, &Dip);
	tips_sqlite_handle.init_sql_num(sevent->Sender, &Sender);
	tips_sqlite_handle.init_sql_num(sevent->Receiver, &Receiver);
	tips_sqlite_handle.init_sql_num(sevent->Sen_msg, &Sen_msg);
	char *sqltemp = g_strdup_printf("%s where", sqlpart);

	char *SSip = g_strdup_printf("%c%s%c",'%', Sip, '%');
	char *DDip = g_strdup_printf("%c%s%c",'%', Dip, '%');
	char *EEvent_time = g_strdup_printf("%c%s%c",'%', Event_time, '%');

	Event_one_init(&sqltemp, (char *)"Event_class", Event_class, (char *)"=");
	Event_one_init(&sqltemp, (char *)"Event_time", EEvent_time, (char *)"like");
	Event_one_init(&sqltemp, (char *)"Sip", SSip, (char *)"like");
	Event_one_init(&sqltemp, (char *)"Sport", Sport, (char *)"=");
	Event_one_init(&sqltemp, (char *)"Dip", DDip, (char *)"like");
	Event_one_init(&sqltemp, (char *)"Sender", Sender, (char *)"=");
	Event_one_init(&sqltemp, (char *)"Receiver", Receiver, (char *)"=");
	Event_one_init(&sqltemp, (char *)"Sen_msg", Sen_msg, (char *)"=");
	
	g_free(SSip);
	g_free(DDip);
	g_free(EEvent_time);

	free(Event_class);
	free(Event_time);
	free(Sip);
	free(Sport);
	free(Dip);
	free(Sender);
	free(Receiver);
	free(Sen_msg);
	if(sql == NULL){
		g_free(sqltemp);
		sql = g_strdup(sqlpart);
		//return 1;
	}
	else{
		sql[strlen(sql)-3]='\0';
	}
	if(1==how){
  	sqltemp = sql;
  	sql = g_strdup_printf("%s group by Event_class,Event_type,Sip,Sport,Dip,Dport,Dmac,Sender,Receiver,Title,File_name,Descr,Sen_msg,Device_id,Event_judgetime order by Event_judgetime desc", sqltemp);
  	g_free(sqltemp);
	}
	else if(2==how){
  	sqltemp = sql;
  	sql = g_strdup_printf("%s group by Event_class,Event_type,Sip,Sport,Dip,Dport,Dmac,Sender,Receiver,Title,File_name,Descr,Sen_msg,Device_id,Event_judgetime order by Event_judgetime asc", sqltemp);
  	g_free(sqltemp);
	}
	return 0;
}

int CTips_Event::Copy_init_sql(event_stut_t *sevent, uint32_t pagenum)
{/*
	sql = NULL;
	char *Event_class = NULL;
	char *Event_time = NULL;
	char *Sip = NULL;
	char *Sport = NULL;
	char *Dip = NULL;
	char *Sender = NULL;
	char *Receiver = NULL;
	char *Sen_msg = NULL;

	tips_sqlite_handle.init_sql_num(sevent->Event_class, &Event_class);
	tips_sqlite_handle.init_sql_num(sevent->Event_time, &Event_time);
	tips_sqlite_handle.init_sql_num(sevent->Sip, &Sip);
	tips_sqlite_handle.init_sql_num(sevent->Sport, &Sport);
	tips_sqlite_handle.init_sql_num(sevent->Dip, &Dip);
	tips_sqlite_handle.init_sql_num(sevent->Sender, &Sender);
	tips_sqlite_handle.init_sql_num(sevent->Receiver, &Receiver);
	tips_sqlite_handle.init_sql_num(sevent->Sen_msg, &Sen_msg);
	char *sqltemp = g_strdup("select Event_class,Event_type,Event_time,Sip,Sport,Dip,Dport,Dmac,Sender,Receiver,Title,File_name,Descr,Sen_msg,Event_iid,Device_id from tips_Event where");

	char *SSip = g_strdup_printf("%c%s%c",'%', Sip, '%');
	char *DDip = g_strdup_printf("%c%s%c",'%', Dip, '%');
	char *EEvent_time = g_strdup_printf("%c%s%c",'%', Event_time, '%');

	Event_one_init(&sqltemp, (char *)"Event_class", Event_class, (char *)"=");
	Event_one_init(&sqltemp, (char *)"Event_time", EEvent_time, (char *)"like");
	Event_one_init(&sqltemp, (char *)"Sip", SSip, (char *)"like");
	Event_one_init(&sqltemp, (char *)"Sport", Sport, (char *)"=");
	Event_one_init(&sqltemp, (char *)"Dip", DDip, (char *)"like");
	Event_one_init(&sqltemp, (char *)"Sender", Sender, (char *)"=");
	Event_one_init(&sqltemp, (char *)"Receiver", Receiver, (char *)"=");
	Event_one_init(&sqltemp, (char *)"Sen_msg", Sen_msg, (char *)"=");
	
	g_free(SSip);
	g_free(DDip);
	g_free(EEvent_time);

	free(Event_class);
	free(Event_time);
	free(Sip);
	free(Sport);
	free(Dip);
	free(Sender);
	free(Receiver);
	free(Sen_msg);
	if(sql == NULL){
		g_free(sqltemp);
		char *sqltemp = g_strdup("select Event_class,Event_type,Event_time,Sip,Sport,Dip,Dport,Dmac,Sender,Receiver,Title,File_name,Descr,Sen_msg,Event_iid,Device_id from tips_Event");
		//return 1;
	}
	else{
		sql[strlen(sql)-3]='\0';
	}
*/
	event_pagenum = pagenum;
	Event_init_sql_interface(sevent, (char*)"select Event_class,Event_type,Event_time,Sip,Sport,Dip,Dport,Dmac,Sender,Receiver,Title,File_name,Descr,Sen_msg,Event_iid,Device_id,Event_judgetime,flag from tips_Event");
	return 0;
}

int CTips_Event::Copy_exec_sql(int all)
{
	char *sqltemp = sql;
	int len = ONCE_MAXDATA;
	if(!all)
		sql = g_strdup_printf("%s limit %d offset %d", sqltemp, len, len*(event_pagenum-1));
	else
		sql = g_strdup(sqltemp);
	g_free(sqltemp);
	int ret = tips_sqlite_handle.tips_Copy(sql, callback_event, (char *)"涉密标志时间",1);
	g_free(sql);
	sql=NULL;
	return ret;
}

int CTips_Event::Event_init_sql(event_stut_t *sevent, uint32_t pagenum,int sort)
{
/*
	sql = NULL;
	char *Event_class = NULL;
	char *Event_time = NULL;
	char *Sip = NULL;
	char *Sport = NULL;
	char *Dip = NULL;
	char *Sender = NULL;
	char *Receiver = NULL;
	char *Sen_msg = NULL;

	tips_sqlite_handle.init_sql_num(sevent->Event_class, &Event_class);
	tips_sqlite_handle.init_sql_num(sevent->Event_time, &Event_time);
	tips_sqlite_handle.init_sql_num(sevent->Sip, &Sip);
	tips_sqlite_handle.init_sql_num(sevent->Sport, &Sport);
	tips_sqlite_handle.init_sql_num(sevent->Dip, &Dip);
	tips_sqlite_handle.init_sql_num(sevent->Sender, &Sender);
	tips_sqlite_handle.init_sql_num(sevent->Receiver, &Receiver);
	tips_sqlite_handle.init_sql_num(sevent->Sen_msg, &Sen_msg);
	char *sqltemp = g_strdup("select Event_class,Event_time,Sip,Sport,Dip,Dmac,Sender,Receiver,File_name,Sen_msg from tips_Event where");

	char *SSip = g_strdup_printf("%c%s%c",'%', Sip, '%');
	char *DDip = g_strdup_printf("%c%s%c",'%', Dip, '%');
	char *EEvent_time = g_strdup_printf("%c%s%c",'%', Event_time, '%');

	Event_one_init(&sqltemp, (char *)"Event_class", Event_class, (char *)"=");
	Event_one_init(&sqltemp, (char *)"Event_time", EEvent_time, (char *)"like");
	Event_one_init(&sqltemp, (char *)"Sip", SSip, (char *)"like");
	Event_one_init(&sqltemp, (char *)"Sport", Sport, (char *)"=");
	Event_one_init(&sqltemp, (char *)"Dip", DDip, (char *)"like");
	Event_one_init(&sqltemp, (char *)"Sender", Sender, (char *)"=");
	Event_one_init(&sqltemp, (char *)"Receiver", Receiver, (char *)"=");
	Event_one_init(&sqltemp, (char *)"Sen_msg", Sen_msg, (char *)"=");
	
	g_free(SSip);
	g_free(DDip);
	g_free(EEvent_time);

	free(Event_class);
	free(Event_time);
	free(Sip);
	free(Sport);
	free(Dip);
	free(Sender);
	free(Receiver);
	free(Sen_msg);
	if(sql == NULL){
		g_free(sqltemp);
		sql = g_strdup("select Event_class,Event_time,Sip,Sport,Dip,Dmac,Sender,Receive,File_name,Sen_msg from tips_Event");
		//return 1;
	}
	else{
		sql[strlen(sql)-3]='\0';
	}
*/
	event_pagenum = pagenum;
	Event_init_sql_interface(sevent, (char*)"select Event_class,Event_time,Sip,Sport,Dip,Dmac,Sender,Receiver,File_name,Sen_msg from tips_Event",sort);
	return 0;
}

char* CTips_Event::Event_exec_sql(int all)
{
	char *sqltemp = sql;
	int len = ONCE_MAXDATA;
	if(!all)
		sql = g_strdup_printf("%s limit %d offset %d", sqltemp, len, len*(event_pagenum-1));
	else
		sql = g_strdup(sqltemp);
	g_free(sqltemp);
	char *xml = tips_sqlite_handle.catch_xml_interface(sql, xml_part_event);
	g_free(sql);
	sql=NULL;
	return xml;
}

int CTips_Event::Delete_init_sql(event_stut_t *sevent, uint32_t pagenum)
{
/*
	sql = NULL;
	char *Event_class = NULL;
	char *Event_time = NULL;
	char *Sip = NULL;
	char *Sport = NULL;
	char *Dip = NULL;
	char *Sender = NULL;
	char *Receiver = NULL;
	char *Sen_msg = NULL;

	tips_sqlite_handle.init_sql_num(sevent->Event_class, &Event_class);
	tips_sqlite_handle.init_sql_num(sevent->Event_time, &Event_time);
	tips_sqlite_handle.init_sql_num(sevent->Sip, &Sip);
	tips_sqlite_handle.init_sql_num(sevent->Sport, &Sport);
	tips_sqlite_handle.init_sql_num(sevent->Dip, &Dip);
	tips_sqlite_handle.init_sql_num(sevent->Sender, &Sender);
	tips_sqlite_handle.init_sql_num(sevent->Receiver, &Receiver);
	tips_sqlite_handle.init_sql_num(sevent->Sen_msg, &Sen_msg);
	char *sqltemp = g_strdup("delete from tips_Event where");

	char *SSip = g_strdup_printf("%c%s%c",'%', Sip, '%');
	char *DDip = g_strdup_printf("%c%s%c",'%', Dip, '%');
	char *EEvent_time = g_strdup_printf("%c%s%c",'%', Event_time, '%');

	Event_one_init(&sqltemp, (char *)"Event_class", Event_class, (char *)"=");
	Event_one_init(&sqltemp, (char *)"Event_time", EEvent_time, (char *)"like");
	Event_one_init(&sqltemp, (char *)"Sip", SSip, (char *)"like");
	Event_one_init(&sqltemp, (char *)"Sport", Sport, (char *)"=");
	Event_one_init(&sqltemp, (char *)"Dip", DDip, (char *)"like");
	Event_one_init(&sqltemp, (char *)"Sender", Sender, (char *)"=");
	Event_one_init(&sqltemp, (char *)"Receiver", Receiver, (char *)"=");
	Event_one_init(&sqltemp, (char *)"Sen_msg", Sen_msg, (char *)"=");
	
	g_free(SSip);
	g_free(DDip);
	g_free(EEvent_time);

	free(Event_class);
	free(Event_time);
	free(Sip);
	free(Sport);
	free(Dip);
	free(Sender);
	free(Receiver);
	free(Sen_msg);
	if(sql == NULL){
		g_free(sqltemp);
		char *sqltemp = g_strdup("delete from tips_Event");
		//return 1;
	}
	else{
		sql[strlen(sql)-3]='\0';
	}
*/
	event_pagenum = pagenum;
	Event_init_sql_interface(sevent, (char*)"select rowid from tips_Event", 1);
	return 0;
}

int CTips_Event::Delete_exec_sql(int all)
{
	char *sqltemp = sql;
	int len = ONCE_MAXDATA;
	if(!all)
		sql = g_strdup_printf("delete from tips_Event where rowid in(%s limit %d offset %d)", sqltemp, len, len*(event_pagenum-1));
	else
		sql = g_strdup_printf("delete from tips_Event where rowid in(%s)", sqltemp);
	g_free(sqltemp);
	int ret = tips_sqlite_handle.Data_Delete_obj(sql);
	g_free(sql);
	sql=NULL;
	return ret;
}

int CTips_Event::Count_init_sql(event_stut_t *sevent, uint32_t pagenum)
{
/*保留参数？*/pagenum = pagenum;
	//Event_init_sql_interface(sevent, (char *)"select count(*) from tips_Event");
	char *temp = NULL;
	Event_init_sql_interface(sevent, (char *)"select Event_id from tips_Event");
  temp = sql;
  sql = g_strdup_printf("select count(*) from tips_Event where Event_id in (%s)", temp);
  g_free(temp);
	return 0;
}

int CTips_Event::Count_exec_sql()
{	
	int ret = tips_sqlite_handle.last_page(sql);
	g_free(sql);
	sql=NULL;
	return ret;
}


int CTips_Event::Count_picture_sql(int *count, const char *search_type)
{                                                
        sql=NULL;                                
        //Alarm_init_sql_ilnterface(salarm, pagenum, (char *)"select count(*) from tips_Alarm");
        sql = g_strdup_printf("select count(*) from tips_Event where Event_id in (select Event_id from tips_Event where Event_time like '%s%%' group by Event_class,Event_type,Sip,Sport,Dip,Dport,Dmac,Sender,Receiver,Title,File_name,Descr,Sen_msg,Device_id,Event_judgetime)",search_type);
        printf("sql======%s\n",sql);
        printf("I going ************************\n");
        *count = tips_sqlite_handle.picture_count(sql);
        printf("myself_sql*************is %d\n",*count);
        g_free(sql);
        return 0;
}


int CTips_Event::Count_risk_sql(int *count,int search_type,const char *start_time,const char *end_time)
{                                                
        sql=NULL;                                
        //Alarm_init_sql_ilnterface(salarm, pagenum, (char *)"select count(*) from tips_Alarm");
        sql = g_strdup_printf("select count(*) from tips_Event where Event_id in (select Event_id from tips_Event where Event_time between '%s' and '%s' and Event_class='%d' group by Event_class,Event_type,Sip,Sport,Dip,Dport,Dmac,Sender,Receiver,Title,File_name,Descr,Sen_msg,Device_id,Event_judgetime)",start_time,end_time,search_type);
        printf("sql======%s\n",sql);
        printf("I going ************************\n");
        *count = tips_sqlite_handle.picture_count(sql);
        printf("myself_sql*************is %d\n",*count);
        g_free(sql);
        return 0;
}
