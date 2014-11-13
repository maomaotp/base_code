#include "tips_netlog.h"
#include "tips_xml_part.h"

extern CUser_sqlite_handle tips_sqlite_handle;

xml_part* xml_part_netlog[] = {xml_Dip,
			       xml_Sip,
             xml_Sport,
             xml_Protocol,
			       xml_Start_time,
			       xml_End_time,
			       xml_Load,
			       xml_Upload,
             xml_Dport};

int CTips_netlog::Netlog_one_init(char **sqltemp, char *name, char *value, char* match)
{
	if((value) && (strlen(value))){
		sql = g_strdup_printf("%s %s %s '%s' and",*sqltemp, name, match,value);
		g_free(*sqltemp);
		*sqltemp = sql;
	}
/*返回值？*/return 0;
}

int CTips_netlog::Netlog_init_sql_interface(netlog_stut_t *snetlog, char *sqlpart,int how)
{
	sql = NULL;
	char *sqltemp = g_strdup_printf("%s where", sqlpart);
	char *Dip = NULL;
	char *Sip = NULL;
	char *Dport = NULL;
	char *Sport = NULL;
	char *Protocol = NULL;
	char *Start_time = NULL;
	char *End_time = NULL;
	char *Load = NULL;
	char *Upload = NULL;
	int protocol_flag = 0;

	tips_sqlite_handle.init_sql_num(snetlog->Dip, &Dip);
	tips_sqlite_handle.init_sql_num(snetlog->Sip, &Sip);
	tips_sqlite_handle.init_sql_num(snetlog->Sport, &Sport);
	tips_sqlite_handle.init_sql_num(snetlog->Dport, &Dport);
	tips_sqlite_handle.init_sql_num(snetlog->Protocol, &Protocol);
	tips_sqlite_handle.init_sql_num(snetlog->Start_time, &Start_time);
	tips_sqlite_handle.init_sql_num(snetlog->End_time, &End_time);
	tips_sqlite_handle.init_sql_num(snetlog->Load, &Load);
	tips_sqlite_handle.init_sql_num(snetlog->Upload, &Upload);

	char *SSip = g_strdup_printf("%c%s%c",'%', Sip, '%');
	char *DDip = g_strdup_printf("%c%s%c",'%', Dip, '%');
	char *SStart_time = g_strdup_printf("%c%s%c",'%', Start_time, '%');
	char *EEnd_time = g_strdup_printf("%c%s%c",'%', End_time, '%');
	protocol_flag = atoi(Protocol);
	printf("Protocol is %d\n",protocol_flag);
	if(protocol_flag == 0)
		Protocol[0] = '\0';
#if 0
	if(!strcmp(Protocol, "TCP")){
			strcpy(Protocol, "1");
	}
	else if(!strcmp(Protocol, "UDP")){
			strcpy(Protocol, "6");
	}
	else if(!strcmp(Protocol, "HTTP")){
			strcpy(Protocol, "80");
	}
	else if(!strcmp(Protocol, "FTP")){
			strcpy(Protocol, "21");
	}
	else{
		Protocol[0] = '\0';
	}
#endif

	Netlog_one_init(&sqltemp, (char *)"Dip", DDip, (char *)"like");
	Netlog_one_init(&sqltemp, (char *)"Sip", SSip, (char *)"like");
	Netlog_one_init(&sqltemp, (char *)"Sport", Sport, (char *)"=");
	Netlog_one_init(&sqltemp, (char *)"Dport", Dport, (char *)"=");
	Netlog_one_init(&sqltemp, (char *)"Protocol", Protocol, (char *)"=");
	Netlog_one_init(&sqltemp, (char *)"Start_time", SStart_time, (char *)"like");
	Netlog_one_init(&sqltemp, (char *)"End_time", EEnd_time, (char *)"like");
	Netlog_one_init(&sqltemp, (char *)"`Load`", Upload, (char *)"=");
	Netlog_one_init(&sqltemp, (char *)"`Upload`", Load, (char *)"=");

	g_free(SSip);
	g_free(DDip);
	g_free(SStart_time);
	g_free(EEnd_time);

	free(Dip);
	free(Sip);
	free(Sport);
	free(Protocol);
	free(Start_time);
	free(End_time);
	free(Load);
	free(Upload);
	if(sql == NULL){
		g_free(sqltemp);
		sql = g_strdup("%s");
		//return 1;
	}
	else{
		sql[strlen(sql)-3]='\0';
	}
	if(1==how){
  	sqltemp = sql;
  	//sql = g_strdup_printf("%s group by Protocol,Start_time,End_time,Sip,Sport,Dip,Load,Upload order by Start_time desc", sqltemp);
  	sql = g_strdup_printf("%s order by Start_time desc", sqltemp);
  	g_free(sqltemp);
	}
	else if(2==how){
  	sqltemp = sql;
  //	sql = g_strdup_printf("%s group by Protocol,Start_time,End_time,Sip,Sport,Dip,Load,Upload order by Start_time asc", sqltemp);
  	sql = g_strdup_printf("%s order by Start_time asc", sqltemp);
  	g_free(sqltemp);
	}
	return 0;
}

int CTips_netlog::Netlog_init_sql(netlog_stut_t *snetlog, uint32_t pagenum,int sort)
{
/*
	sql = NULL;
	char *sqltemp = g_strdup("select  Dip,Sip,Sport,Protocol,Start_time,End_time,Load,Upload from tips_netlog where");
	char *Dip = NULL;
	char *Sip = NULL;
	char *Sport = NULL;
	char *Protocol = NULL;
	char *Start_time = NULL;
	char *End_time = NULL;
	char *Load = NULL;
	char *Upload = NULL;

	tips_sqlite_handle.init_sql_num(snetlog->Dip, &Dip);
	tips_sqlite_handle.init_sql_num(snetlog->Sip, &Sip);
	tips_sqlite_handle.init_sql_num(snetlog->Sport, &Sport);
	tips_sqlite_handle.init_sql_num(snetlog->Protocol, &Protocol);
	tips_sqlite_handle.init_sql_num(snetlog->Start_time, &Start_time);
	tips_sqlite_handle.init_sql_num(snetlog->End_time, &End_time);
	tips_sqlite_handle.init_sql_num(snetlog->Load, &Load);
	tips_sqlite_handle.init_sql_num(snetlog->Upload, &Upload);

	char *SSip = g_strdup_printf("%c%s%c",'%', Sip, '%');
	char *DDip = g_strdup_printf("%c%s%c",'%', Dip, '%');
	char *SStart_time = g_strdup_printf("%c%s%c",'%', Start_time, '%');
	char *EEnd_time = g_strdup_printf("%c%s%c",'%', End_time, '%');

	Netlog_one_init(&sqltemp, (char *)"Dip", DDip, (char *)"like");
	Netlog_one_init(&sqltemp, (char *)"Sip", SSip, (char *)"like");
	Netlog_one_init(&sqltemp, (char *)"Sport", Sport, (char *)"=");
	Netlog_one_init(&sqltemp, (char *)"Protocol", Protocol, (char *)"=");
	Netlog_one_init(&sqltemp, (char *)"Start_time", SStart_time, (char *)"like");
	Netlog_one_init(&sqltemp, (char *)"End_time", EEnd_time, (char *)"like");
	Netlog_one_init(&sqltemp, (char *)"Load", Load, (char *)"=");
	Netlog_one_init(&sqltemp, (char *)"Upload", Upload, (char *)"=");

	g_free(SSip);
	g_free(DDip);
	g_free(SStart_time);
	g_free(EEnd_time);

	free(Dip);
	free(Sip);
	free(Sport);
	free(Protocol);
	free(Start_time);
	free(End_time);
	free(Load);
	free(Upload);
	if(sql == NULL){
		g_free(sqltemp);
		sql = g_strdup("select  Dip,Sip,Sport,Protocol,Start_time,End_time,Load,Upload from tips_netlog");
		//return 1;
	}
	else{
		sql[strlen(sql)-3]='\0';
	}
*/
	netlog_pagenum = pagenum;
	Netlog_init_sql_interface(snetlog, (char *)"select  Dip,Sip,Sport,Protocol,Start_time,End_time,`Load`,`Upload`,Dport from tips_netlog",sort);
	return 0;
}

char* CTips_netlog::Netlog_exec_sql(int all)
{
	char *sqltemp = sql;
	int every_pagenum =0;
	FILE *fp;
	char id_buf[10];
	memset(id_buf,0,sizeof(id_buf));
	fp = fopen((char *)"/usr/local/tipschecker/etc/tipscheck/every_pagenum","r+");
	fread(id_buf,sizeof(id_buf),1,fp);
	if(strlen(id_buf)==0)
		/*hyj int*/ every_pagenum = 50;
	else 
		every_pagenum = atoi(id_buf);
	fclose(fp);
	int len = every_pagenum;
	//int len = ONCE_MAXDATA;
	if(!all)
		sql = g_strdup_printf("%s limit %d offset %d", sqltemp, len, len*(netlog_pagenum-1));
	else
		sql = g_strdup(sqltemp);
	g_free(sqltemp);
	//char *test = "select * from tips_netlog";
	//char *xml = tips_sqlite_handle.catch_xml_interface(test, xml_part_netlog);
	char *xml = tips_sqlite_handle.catch_xml_interface(sql, xml_part_netlog);
	g_free(sql);
	printf("netlog sql is ==============%s\n",xml);
//	exit(0);
	sql=NULL;
	return xml;
}

int CTips_netlog::Copy_init_sql(netlog_stut_t *snetlog, uint32_t pagenum)
{
/*
	sql = NULL;
	char *sqltemp = g_strdup("select  Dip,Dport,Sip,Sport,Protocol,Start_time,End_time,Load,Upload from tips_netlog where");
	char *Dip = NULL;
	char *Sip = NULL;
	char *Sport = NULL;
	char *Protocol = NULL;
	char *Start_time = NULL;
	char *End_time = NULL;
	char *Load = NULL;
	char *Upload = NULL;

	tips_sqlite_handle.init_sql_num(snetlog->Dip, &Dip);
	tips_sqlite_handle.init_sql_num(snetlog->Sip, &Sip);
	tips_sqlite_handle.init_sql_num(snetlog->Sport, &Sport);
	tips_sqlite_handle.init_sql_num(snetlog->Protocol, &Protocol);
	tips_sqlite_handle.init_sql_num(snetlog->Start_time, &Start_time);
	tips_sqlite_handle.init_sql_num(snetlog->End_time, &End_time);
	tips_sqlite_handle.init_sql_num(snetlog->Load, &Load);
	tips_sqlite_handle.init_sql_num(snetlog->Upload, &Upload);

	char *SSip = g_strdup_printf("%c%s%c",'%', Sip, '%');
	char *DDip = g_strdup_printf("%c%s%c",'%', Dip, '%');
	char *SStart_time = g_strdup_printf("%c%s%c",'%', Start_time, '%');
	char *EEnd_time = g_strdup_printf("%c%s%c",'%', End_time, '%');

	Netlog_one_init(&sqltemp, (char *)"Dip", DDip, (char *)"like");
	Netlog_one_init(&sqltemp, (char *)"Sip", SSip, (char *)"like");
	Netlog_one_init(&sqltemp, (char *)"Sport", Sport, (char *)"=");
	Netlog_one_init(&sqltemp, (char *)"Protocol", Protocol, (char *)"=");
	Netlog_one_init(&sqltemp, (char *)"Start_time", SStart_time, (char *)"like");
	Netlog_one_init(&sqltemp, (char *)"End_time", EEnd_time, (char *)"like");
	Netlog_one_init(&sqltemp, (char *)"Load", Load, (char *)"=");
	Netlog_one_init(&sqltemp, (char *)"Upload", Upload, (char *)"=");

	g_free(SSip);
	g_free(DDip);
	g_free(SStart_time);
	g_free(EEnd_time);

	free(Dip);
	free(Sip);
	free(Sport);
	free(Protocol);
	free(Start_time);
	free(End_time);
	free(Load);
	free(Upload);
	if(sql == NULL){
		g_free(sqltemp);
		sql = g_strdup("select  Dip,Dport,Sip,Sport,Protocol,Start_time,End_time,Load,Upload from tips_netlog");
		//return 1;
	}
	else{
		sql[strlen(sql)-3]='\0';
	}
*/
	netlog_pagenum = pagenum;
	Netlog_init_sql_interface(snetlog, (char *)"select  Dip,Dport,Sip,Sport,Protocol,Start_time,End_time,Load,Upload from tips_netlog");
	return 0;
}

int CTips_netlog::Copy_exec_sql(int all)
{
	char *sqltemp = sql;
	int every_pagenum =0;
	FILE *fp;
	char id_buf[10];
	memset(id_buf,0,sizeof(id_buf));
	fp = fopen((char *)"/usr/local/tipschecker/etc/tipscheck/every_pagenum","r+");
	fread(id_buf,sizeof(id_buf),1,fp);
	if(strlen(id_buf)==0)
		/*hyj int*/ every_pagenum = 50;
	else 
		every_pagenum = atoi(id_buf);
	fclose(fp);
	int len = every_pagenum;
	//int len = ONCE_MAXDATA;
	if(!all)
		sql = g_strdup_printf("%s limit %d offset %d", sqltemp, len, len*(netlog_pagenum-1));
	else
		sql = g_strdup(sqltemp);
	g_free(sqltemp);
	int ret = tips_sqlite_handle.tips_Copy(sql, callback_netlog, (char *)"通信会话审计",2);
	g_free(sql);
	sql=NULL;
	return ret;
}

int CTips_netlog::Delete_init_sql(netlog_stut_t *snetlog, uint32_t pagenum)
{
/*
	sql = NULL;
	char *sqltemp = g_strdup("delete from tips_netlog where");
	char *Dip = NULL;
	char *Sip = NULL;
	char *Sport = NULL;
	char *Protocol = NULL;
	char *Start_time = NULL;
	char *End_time = NULL;
	char *Load = NULL;
	char *Upload = NULL;

	tips_sqlite_handle.init_sql_num(snetlog->Dip, &Dip);
	tips_sqlite_handle.init_sql_num(snetlog->Sip, &Sip);
	tips_sqlite_handle.init_sql_num(snetlog->Sport, &Sport);
	tips_sqlite_handle.init_sql_num(snetlog->Protocol, &Protocol);
	tips_sqlite_handle.init_sql_num(snetlog->Start_time, &Start_time);
	tips_sqlite_handle.init_sql_num(snetlog->End_time, &End_time);
	tips_sqlite_handle.init_sql_num(snetlog->Load, &Load);
	tips_sqlite_handle.init_sql_num(snetlog->Upload, &Upload);

	char *SSip = g_strdup_printf("%c%s%c",'%', Sip, '%');
	char *DDip = g_strdup_printf("%c%s%c",'%', Dip, '%');
	char *SStart_time = g_strdup_printf("%c%s%c",'%', Start_time, '%');
	char *EEnd_time = g_strdup_printf("%c%s%c",'%', End_time, '%');

	Netlog_one_init(&sqltemp, (char *)"Dip", DDip, (char *)"like");
	Netlog_one_init(&sqltemp, (char *)"Sip", SSip, (char *)"like");
	Netlog_one_init(&sqltemp, (char *)"Sport", Sport, (char *)"=");
	Netlog_one_init(&sqltemp, (char *)"Protocol", Protocol, (char *)"=");
	Netlog_one_init(&sqltemp, (char *)"Start_time", SStart_time, (char *)"like");
	Netlog_one_init(&sqltemp, (char *)"End_time", EEnd_time, (char *)"like");
	Netlog_one_init(&sqltemp, (char *)"Load", Load, (char *)"=");
	Netlog_one_init(&sqltemp, (char *)"Upload", Upload, (char *)"=");

	g_free(SSip);
	g_free(DDip);
	g_free(SStart_time);
	g_free(EEnd_time);

	free(Dip);
	free(Sip);
	free(Sport);
	free(Protocol);
	free(Start_time);
	free(End_time);
	free(Load);
	free(Upload);
	if(sql == NULL){
		g_free(sqltemp);
		sql = g_strdup("delete from tips_netlog");
		//return 1;
	}
	else{
		sql[strlen(sql)-3]='\0';
	}
*/
	netlog_pagenum = pagenum;
	Netlog_init_sql_interface(snetlog, (char *)"select rowid from tips_netlog");
	return 0;
}

int CTips_netlog::Delete_exec_sql(int all)
{
	char *sqltemp = sql;
	int every_pagenum =0;
	FILE *fp;
	char id_buf[10];
	memset(id_buf,0,sizeof(id_buf));
	fp = fopen((char *)"/usr/local/tipschecker/etc/tipscheck/every_pagenum","r+");
	fread(id_buf,sizeof(id_buf),1,fp);
	if(strlen(id_buf)==0)
		/*hyj int*/ every_pagenum = 50;
	else 
		every_pagenum = atoi(id_buf);
	fclose(fp);
	int len = every_pagenum;
	//int len = ONCE_MAXDATA;
	if(!all)
		sql = g_strdup_printf("delete from tips_netlog where rowid in(%s limit %d offset %d)", sqltemp, len, len*(netlog_pagenum-1));
	else
		sql = g_strdup_printf("delete from tips_netlog where rowid in(%s)", sqltemp);
	g_free(sqltemp);
	int ret = tips_sqlite_handle.Data_Delete_obj(sql);
	g_free(sql);
	sql=NULL;
	return ret;
}

int CTips_netlog::Count_init_sql(netlog_stut_t* snetlog, uint32_t pagenum)
{
/*保留参数？*/pagenum = pagenum;

	Netlog_init_sql_interface(snetlog, (char *)"select count(*) from tips_netlog");
	return 0;
}

int CTips_netlog::Count_exec_sql(uint32_t *total_number)
{	
	//printf("sql:::::::::::::::::%s\n",sql);
	//char *wo = "select count(*) from tips_netlog";
	//puts(wo);
	int ret = tips_sqlite_handle.last_page(sql,0,total_number);
	//int ret = tips_sqlite_handle.last_page(wo);
	g_free(sql);
	sql=NULL;
	printf("ret is %d\n",ret);
	//exit(0);
	return ret;
}


int CTips_netlog::Count_picture_sql(int *count, const char *search_type)
{                                                
        sql=NULL;                                
        //Alarm_init_sql_ilnterface(salarm, pagenum, (char *)"select count(*) from tips_Alarm");
        sql = g_strdup_printf("select count(*) from tips_netlog where  Start_time like '%%%s%%' and End_time like '%%'",search_type);
        printf("sql======%s\n",sql);
        printf("I going ************************\n");
        *count = tips_sqlite_handle.picture_count(sql);
        printf("myself_sql*************is %d\n",*count);
        g_free(sql);
        return 0;
}

#if 0 
int CTips_netlog::Count_risk_sql(int *count,int search_type,const char *start_time,const char *end_time)
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
#endif
