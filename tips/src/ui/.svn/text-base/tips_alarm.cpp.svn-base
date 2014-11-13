#include "tips_alarm.h"
#include "tips_xml_part.h"


extern CUser_sqlite_handle tips_sqlite_handle;

xml_part* xml_part_alarm[9] = {xml_Trogan_id,
			       xml_Sip,
			       xml_Dip,
			       xml_Dmac,
             xml_Alarm_time,
             xml_Type,
			       xml_Risk,
			       xml_Scc_flag,
						 xml_Alarm_desc
};

int CTips_alarm::Alarm_one_init(char **sqltemp, char *name, char *value, char* match)
{
	if((value) && (strlen(value))){
		sql = g_strdup_printf("%s %s %s '%s' and",*sqltemp, name, match,value);
		g_free(*sqltemp);
		*sqltemp = sql;
	}
/*返回值？*/return 0;
}

int CTips_alarm::Alarm_init_sql_interface(alarm_stut_t *salarm, uint32_t pagenum, char *sqlpart, int how)
{
	sql = NULL;
	char *Trojan_id = NULL;
	char *Sip = NULL;
	char *Dip = NULL;
	char *Dmac = NULL;
	char *Alarm_time = NULL;
        char *AAlarm_time = NULL;
        char *Alarm_end_time = NULL;
	char *Type = NULL;
	char *Risk = NULL;
	char *Scc_flag = NULL;
	char *sqltemp = g_strdup_printf("%s where", sqlpart);

	tips_sqlite_handle.init_sql_num(salarm->Trojan_id, &Trojan_id);
	tips_sqlite_handle.init_sql_num(salarm->Sip, &Sip);
	tips_sqlite_handle.init_sql_num(salarm->Dip, &Dip);
	tips_sqlite_handle.init_sql_num(salarm->Dmac, &Dmac);
	tips_sqlite_handle.init_sql_num(salarm->Alarm_time, &Alarm_time);
	tips_sqlite_handle.init_sql_num(salarm->Alarm_end_time, &Alarm_end_time);
	tips_sqlite_handle.init_sql_num(salarm->Type, &Type);
	tips_sqlite_handle.init_sql_num(salarm->Risk, &Risk);
	tips_sqlite_handle.init_sql_num(salarm->Scc_flag, &Scc_flag);

	char *SSip = g_strdup_printf("%c%s%c",'%', Sip, '%');
	char *DDip = g_strdup_printf("%c%s%c",'%', Dip, '%');
	char *DDmac = g_strdup_printf("%c%s%c",'%', Dmac, '%');
        if((!strcmp(Alarm_time,"")))//如果开始时间为空则搜索所有
         {
           search_flag = 0;
           AAlarm_time = g_strdup_printf("%c%s%c",'%',Alarm_time,'%');
         }
         else if((strcmp(Alarm_time,""))&&(!strcmp(Alarm_end_time,"")))//从开始设置时间到当前时间的搜索
         {
           search_flag = 1;
           AAlarm_time = g_strdup_printf("'%s' and '%s'",Alarm_time,(char *)"2090");
         }
         else//既有开始时间又有结束时间 
         {
	   if(!strcmp(Alarm_time,Alarm_end_time))
	{
	   	the_same_day_judge = 0;	
                search_flag = 1;
	}
	   else
           {
	   	the_same_day_judge = 1;	
                search_flag = 1;
                AAlarm_time = g_strdup_printf("'%s' and '%s'",Alarm_time,Alarm_end_time);
	    }
         }
         Alarm_one_init(&sqltemp, (char *)"Trojan_id", Trojan_id, (char *)"=");
         Alarm_one_init(&sqltemp, (char *)"Sip", SSip, (char *)"like");
         Alarm_one_init(&sqltemp, (char *)"Dip", DDip, (char *)"like");
         Alarm_one_init(&sqltemp, (char *)"Dmac", DDmac, (char *)"like");
         if(search_flag)
         {
	   if(the_same_day_judge)
		{
        	   sql = g_strdup_printf("%s %s %s %s and",sqltemp,(char *)"Alarm_time",(char *)"between",AAlarm_time);
        	   sqltemp = sql;
		}
	    else
		{
           		AAlarm_time = g_strdup_printf("%c%s%c",'%',Alarm_time,'%');
           		Alarm_one_init(&sqltemp, (char *)"Alarm_time", AAlarm_time, (char *)"like");
           		//Alarm_one_init(&sqltemp, (char *)"Alarm_time", buf_the_sameday, (char *)"like");
		}
           search_flag = 0;
         }
         else
           Alarm_one_init(&sqltemp, (char *)"Alarm_time", AAlarm_time, (char *)"like");
	Alarm_one_init(&sqltemp, (char *)"Type", Type, (char *)"=");
	Alarm_one_init(&sqltemp, (char *)"Risk", salarm->Risk, (char *)"=");
	Alarm_one_init(&sqltemp, (char *)"Scc_flag", salarm->Scc_flag, (char *)"=");
	g_free(SSip);
	g_free(DDip);
	g_free(DDmac);
	g_free(AAlarm_time);

	free(Trojan_id);
	free(Sip);
	free(Dip);
	free(Dmac);
	free(Alarm_time);
	free(Type);
	free(Risk);
	free(Scc_flag);
	if(sql == NULL){
		g_free(sqltemp);
		sql = g_strdup(sqlpart);
		//return 1;
	}
	else{
		sql[strlen(sql)-3]='\0';
	}
	if(how == 1){
  	sqltemp = sql;
  	sql = g_strdup_printf("%s group by Alarm_judgetime,Trojan_id,Sip,Dip order by Alarm_time desc", sqltemp);
  	g_free(sqltemp);
	}
	if(how == 2){
  	sqltemp = sql;
  	sql = g_strdup_printf("%s group by Alarm_judgetime,Trojan_id,Sip,Dip order by Alarm_time asc", sqltemp);
  	g_free(sqltemp);
	}
	alarm_pagenum = pagenum;
	return 0;
}

char* CTips_alarm::Alarm_exec_sql(int all)
{
	char *sqltemp = sql;
	int every_pagenum = 0;
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
		sql = g_strdup_printf("%s limit %d offset %d", sqltemp, len, len*(alarm_pagenum-1));
	else
		sql = g_strdup(sqltemp);
	g_free(sqltemp);
	char *xml = tips_sqlite_handle.catch_xml_interface(sql, xml_part_alarm);
	g_free(sql);
	sql=NULL;
	return xml;
}
int CTips_alarm::Alarm_init_sql(alarm_stut_t *salarm, uint32_t pagenum, int sort)
{
/*
	sql = NULL;
	char *Trojan_id = NULL;
	char *Sip = NULL;
	char *Dip = NULL;
	char *Dmac = NULL;
	char *Alarm_time = NULL;
	char *Type = NULL;
	char *Risk = NULL;
	char *Scc_flag = NULL;
	char *sqltemp = g_strdup("select Trojan_id, Sip, Dip, Dmac, Alarm_time, Type, Risk,Scc_flag from tips_Alarm where");

	tips_sqlite_handle.init_sql_num(salarm->Trojan_id, &Trojan_id);
	tips_sqlite_handle.init_sql_num(salarm->Sip, &Sip);
	tips_sqlite_handle.init_sql_num(salarm->Dip, &Dip);
	tips_sqlite_handle.init_sql_num(salarm->Dmac, &Dmac);
	tips_sqlite_handle.init_sql_num(salarm->Alarm_time, &Alarm_time);
	tips_sqlite_handle.init_sql_num(salarm->Type, &Type);
	tips_sqlite_handle.init_sql_num(salarm->Risk, &Risk);
	tips_sqlite_handle.init_sql_num(salarm->Scc_flag, &Scc_flag);

	printf("ggggggggggggggg\n");
	printf("sip=%s\n",Sip);
	printf("Dip=%s\n",Dip);
	printf("Dmac=%s\n",Dmac);
	char *SSip = g_strdup_printf("%c%s%c",'%', Sip, '%');
	char *DDip = g_strdup_printf("%c%s%c",'%', Dip, '%');
	char *DDmac = g_strdup_printf("%c%s%c",'%', Dmac, '%');
	char *AAlarm_time = g_strdup_printf("%c%s%c", '%', Alarm_time, '%');
	printf("ssip=======%s\n",SSip);
	Alarm_one_init(&sqltemp, (char *)"Trojan_id", Trojan_id, (char *)"=");
	Alarm_one_init(&sqltemp, (char *)"Sip", SSip, (char *)"like");
	Alarm_one_init(&sqltemp, (char *)"Dip", DDip, (char *)"like");
	Alarm_one_init(&sqltemp, (char *)"Dmac", DDmac, (char *)"like");
	Alarm_one_init(&sqltemp, (char *)"Alarm_time", AAlarm_time, (char *)"like");
	Alarm_one_init(&sqltemp, (char *)"Type", Type, (char *)"=");
	Alarm_one_init(&sqltemp, (char *)"Risk", salarm->Risk, (char *)"=");
	Alarm_one_init(&sqltemp, (char *)"Scc_flag", salarm->Scc_flag, (char *)"=");

	g_free(SSip);
	g_free(DDip);
	g_free(DDmac);
	g_free(AAlarm_time);

	free(Trojan_id);
	free(Sip);
	free(Dip);
	free(Dmac);
	free(Alarm_time);
	free(Type);
	free(Risk);
	free(Scc_flag);
	if(sql == NULL){
		g_free(sqltemp);
		sql = g_strdup("select Trojan_id, Sip, Dip, Dmac, Alarm_time, Type, Risk,Scc_flag from tips_Alarm");
		//return 1;
	}
	else{
		sql[strlen(sql)-3]='\0';
	}
	sqltemp = sql;
	int len = ONCE_MAXDATA;
	sql = g_strdup_printf("%s limit %d offset %d", sqltemp, len, len*pagenum);
	g_free(sqltemp);
*/	
	Alarm_init_sql_interface(salarm, pagenum, (char *)"select Trojan_id, Sip, Dip, Dmac, Alarm_time, Type, Risk,Scc_flag,Descry from tips_Alarm", sort);
	return 0;
}

int CTips_alarm::Copy_init_sql(alarm_stut_t *salarm, uint32_t pagenum)
{
/*
	sql = NULL;
	char *Trojan_id = NULL;
	char *Sip = NULL;
	char *Dip = NULL;
	char *Dmac = NULL;
	char *Alarm_time = NULL;
	char *Type = NULL;
	char *Risk = NULL;
	char *Scc_flag = NULL;
	char *sqltemp = g_strdup("select Trojan_id, Sip, Dip, Dmac, Alarm_time, Type, Risk,Scc_flag,Scc,Descry,Alarm_iid,Device_id from tips_Alarm where");

	tips_sqlite_handle.init_sql_num(salarm->Trojan_id, &Trojan_id);
	tips_sqlite_handle.init_sql_num(salarm->Sip, &Sip);
	tips_sqlite_handle.init_sql_num(salarm->Dip, &Dip);
	tips_sqlite_handle.init_sql_num(salarm->Dmac, &Dmac);
	tips_sqlite_handle.init_sql_num(salarm->Alarm_time, &Alarm_time);
	tips_sqlite_handle.init_sql_num(salarm->Type, &Type);
	tips_sqlite_handle.init_sql_num(salarm->Risk, &Risk);
	tips_sqlite_handle.init_sql_num(salarm->Scc_flag, &Scc_flag);

	printf("ggggggggggggggg\n");
	printf("sip=%s\n",Sip);
	printf("Dip=%s\n",Dip);
	printf("Dmac=%s\n",Dmac);
	char *SSip = g_strdup_printf("%c%s%c",'%', Sip, '%');
	char *DDip = g_strdup_printf("%c%s%c",'%', Dip, '%');
	char *DDmac = g_strdup_printf("%c%s%c",'%', Dmac, '%');
	char *AAlarm_time = g_strdup_printf("%c%s%c", '%', Alarm_time, '%');
	printf("ssip=======%s\n",SSip);
	Alarm_one_init(&sqltemp, (char *)"Trojan_id", Trojan_id, (char *)"=");
	Alarm_one_init(&sqltemp, (char *)"Sip", SSip, (char *)"like");
	Alarm_one_init(&sqltemp, (char *)"Dip", DDip, (char *)"like");
	Alarm_one_init(&sqltemp, (char *)"Dmac", DDmac, (char *)"like");
	Alarm_one_init(&sqltemp, (char *)"Alarm_time", AAlarm_time, (char *)"like");
	Alarm_one_init(&sqltemp, (char *)"Type", Type, (char *)"=");
	Alarm_one_init(&sqltemp, (char *)"Risk", salarm->Risk, (char *)"=");
	Alarm_one_init(&sqltemp, (char *)"Scc_flag", salarm->Scc_flag, (char *)"=");

	g_free(SSip);
	g_free(DDip);
	g_free(DDmac);
	g_free(AAlarm_time);

	free(Trojan_id);
	free(Sip);
	free(Dip);
	free(Dmac);
	free(Alarm_time);
	free(Type);
	free(Risk);
	free(Scc_flag);
	if(sql == NULL){
		g_free(sqltemp);
		sql = g_strdup("select Trojan_id, Sip, Dip, Dmac, Alarm_time, Type, Risk,Scc_flag,Scc,Descry,Alarm_iid,Device_id from tips_Alarm");
		//return 1;
	}
	else{
		sql[strlen(sql)-3]='\0';
	}
*/
	Alarm_init_sql_interface(salarm, pagenum, (char *)"select Trojan_id, Sip, Dip, Dmac, Alarm_time, Type, Risk,Scc_flag,Scc,Descry,Alarm_iid,Device_id,Alarm_judgetime,flag from tips_Alarm");
	return 0;
}

int CTips_alarm::Copy_exec_sql(int all)
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
		sql = g_strdup_printf("%s limit %d offset %d", sqltemp, len, len*(alarm_pagenum-1));
	else
		sql = g_strdup(sqltemp);
	g_free(sqltemp);
	int ret = tips_sqlite_handle.tips_Copy(sql, callback_alarm, (char *)"攻击窃密警报",0);
	g_free(sql);
	sql=NULL;
	return ret;
}

int CTips_alarm::Delete_init_sql(alarm_stut_t *salarm, uint32_t pagenum)
{
/*
	sql = NULL;
	char *Trojan_id = NULL;
	char *Sip = NULL;
	char *Dip = NULL;
	char *Dmac = NULL;
	char *Alarm_time = NULL;
	char *Type = NULL;
	char *Risk = NULL;
	char *Scc_flag = NULL;
	char *sqltemp = g_strdup("delete from tips_Alarm where");

	tips_sqlite_handle.init_sql_num(salarm->Trojan_id, &Trojan_id);
	tips_sqlite_handle.init_sql_num(salarm->Sip, &Sip);
	tips_sqlite_handle.init_sql_num(salarm->Dip, &Dip);
	tips_sqlite_handle.init_sql_num(salarm->Dmac, &Dmac);
	tips_sqlite_handle.init_sql_num(salarm->Alarm_time, &Alarm_time);
	tips_sqlite_handle.init_sql_num(salarm->Type, &Type);
	tips_sqlite_handle.init_sql_num(salarm->Risk, &Risk);
	tips_sqlite_handle.init_sql_num(salarm->Scc_flag, &Scc_flag);

	printf("ggggggggggggggg\n");
	printf("sip=%s\n",Sip);
	printf("Dip=%s\n",Dip);
	printf("Dmac=%s\n",Dmac);
	char *SSip = g_strdup_printf("%c%s%c",'%', Sip, '%');
	char *DDip = g_strdup_printf("%c%s%c",'%', Dip, '%');
	char *DDmac = g_strdup_printf("%c%s%c",'%', Dmac, '%');
	char *AAlarm_time = g_strdup_printf("%c%s%c", '%', Alarm_time, '%');
	printf("ssip=======%s\n",SSip);
	Alarm_one_init(&sqltemp, (char *)"Trojan_id", Trojan_id, (char *)"=");
	Alarm_one_init(&sqltemp, (char *)"Sip", SSip, (char *)"like");
	Alarm_one_init(&sqltemp, (char *)"Dip", DDip, (char *)"like");
	Alarm_one_init(&sqltemp, (char *)"Dmac", DDmac, (char *)"like");
	Alarm_one_init(&sqltemp, (char *)"Alarm_time", AAlarm_time, (char *)"like");
	Alarm_one_init(&sqltemp, (char *)"Type", Type, (char *)"=");
	Alarm_one_init(&sqltemp, (char *)"Risk", salarm->Risk, (char *)"=");
	Alarm_one_init(&sqltemp, (char *)"Scc_flag", salarm->Scc_flag, (char *)"=");

	g_free(SSip);
	g_free(DDip);
	g_free(DDmac);
	g_free(AAlarm_time);

	free(Trojan_id);
	free(Sip);
	free(Dip);
	free(Dmac);
	free(Alarm_time);
	free(Type);
	free(Risk);
	free(Scc_flag);
	if(sql == NULL){
		g_free(sqltemp);
		sql = g_strdup("delete from tips_Alarm");
		//return 1;
	}
	else{
		sql[strlen(sql)-3]='\0';
	}
*/
	Alarm_init_sql_interface(salarm, pagenum, (char *)"select rowid from tips_Alarm", 1);
	return 0;
}

int CTips_alarm::Delete_exec_sql(int all)
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
		sql = g_strdup_printf("delete from tips_Alarm where rowid in(%s limit %d offset %d)", sqltemp, len, len*(alarm_pagenum-1));
	else
		sql = g_strdup_printf("delete from tips_Alarm where rowid in(%s)", sqltemp);
	g_free(sqltemp);
	int ret = tips_sqlite_handle.Data_Delete_obj(sql);
	g_free(sql);
	sql=NULL;
	return ret;
}

int CTips_alarm::Count_init_sql(alarm_stut_t *salarm, uint32_t pagenum)
{
	//Alarm_init_sql_interface(salarm, pagenum, (char *)"select count(*) from tips_Alarm");
	char *temp = NULL;
	Alarm_init_sql_interface(salarm, pagenum, (char *)"select Alarm_id from tips_Alarm");
  temp = sql;
  sql = g_strdup_printf("select count(*) from tips_Alarm where Alarm_id in (%s)", temp);
  g_free(temp);
	return 0;
}

int CTips_alarm::Count_exec_sql(uint32_t *total_number)
{	
	int ret = tips_sqlite_handle.last_page(sql,0,total_number);
	g_free(sql);
	sql=NULL;
	return ret;
}

int CTips_alarm::Count_picture_sql(int *count, const char *search_type)
{                                                
        sql=NULL;                                
        //Alarm_init_sql_ilnterface(salarm, pagenum, (char *)"select count(*) from tips_Alarm");
        sql = g_strdup_printf("select count(*) from tips_Alarm where Alarm_time like '%s%%' group by Alarm_judgetime,Trojan_id,Sip,Dip",search_type);
        printf("sql======%s\n",sql);
        printf("I going ************************\n");
        *count = tips_sqlite_handle.picture_count(sql);
        printf("myself_sql*************is %d\n",*count);
        g_free(sql);
        return 0;
}


int CTips_alarm::Count_risk_sql(int *count,int search_type,const char *start_time,const char *end_time)
{                                                
        sql=NULL;                                
        //Alarm_init_sql_ilnterface(salarm, pagenum, (char *)"select count(*) from tips_Alarm");
        if(strcmp(start_time,end_time))
        	sql = g_strdup_printf("select count(*) from tips_Alarm where Alarm_time between '%s' and '%s' and Risk='%d' group by Alarm_judgetime,Trojan_id,Sip,Dip",start_time,end_time,search_type);
	else
        	sql = g_strdup_printf("select count(*) from tips_Alarm where Alarm_time like '%s%%' and Risk='%d' group by Alarm_judgetime,Trojan_id,Sip,Dip",start_time,search_type);
        printf("sql======%s\n",sql);
        printf("I going ************************\n");
        *count = tips_sqlite_handle.picture_count(sql);
        printf("myself_sql*************is %d\n",*count);
        g_free(sql);
        return 0;
}
