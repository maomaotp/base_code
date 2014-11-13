#include "tips_opt.h"
#include "tips_xml_part.h"

extern CUser_sqlite_handle tips_sqlite_handle;

xml_part* xml_part_operator[3] = {xml_username,
				  xml_Otime,
                                  xml_Optdsc};

int CTips_opt::Opt_one_init(char **sqltemp, char *name, char *value, char* match)
{
	if((value) && (strlen(value))){
		if(!strcmp(name,"username"))
		sql = g_strdup_printf("%s %s %s '%%%s%%' and",*sqltemp, name, match,value);
		else if(!strcmp(name,"Optdsc"))
		sql = g_strdup_printf("%s %s %s '%%%s%%' and",*sqltemp, name, match,value);
		else
		sql = g_strdup_printf("%s %s%s\"%s\" and",*sqltemp, name, match,value);
		g_free(*sqltemp);
		*sqltemp = sql;
	}
/*返回值？*/return 0;
}

int CTips_opt::Opt_init_sql_interface(opt_stut_t *sopt, char *sqlpart,int how)
{
	sql = NULL;
	char *username = NULL;
	char *sqltemp = g_strdup_printf("%s where", sqlpart);
	tips_sqlite_handle.init_sql_num(sopt->username, &username);
	Opt_one_init(&sqltemp, (char *)"username", username, (char *)"like");
	if(strcmp(sopt->Otime,sopt->Etime))
	{
		Opt_one_init(&sqltemp, (char *)"Otime", sopt->Otime, (char *)">=");
		Opt_one_init(&sqltemp, (char *)"Otime", sopt->Etime, (char *)"<=");
	}
	else
	{
        	sql = g_strdup_printf("%s %s %s '%s%%' and",sqltemp,(char *)"Otime",(char *)"like",sopt->Otime);
        	sqltemp = sql;
	}
	Opt_one_init(&sqltemp, (char *)"Optdsc", sopt->Optdsc, (char *)"like");
	free(username);

	if(sql == NULL){
		g_free(sqltemp);
		sql = g_strdup(sqlpart);
		//return 1;
	}
	else{
		sql[strlen(sql)-3]='\0';
	}
	if(1==how)
	{
		sqltemp = sql;
  		sql = g_strdup_printf("%s order by Otime desc", sqltemp);
		g_free(sqltemp);
	}
	else
	{
		sqltemp = sql;
  		sql = g_strdup_printf("%s order by Otime asc", sqltemp);
		g_free(sqltemp);
	}
	return 0;
}

int CTips_opt::Opt_init_sql(opt_stut_t *sopt, uint32_t pagenum,int sort)
{
/*
	sql = NULL;
	char *username = NULL;
	char *sqltemp = g_strdup("select username,Otime,Optdsc from tips_opt where");
	tips_sqlite_handle.init_sql_num(sopt->username, &username);
	Opt_one_init(&sqltemp, (char *)"username", username, (char *)"=");
	Opt_one_init(&sqltemp, (char *)"Otime", sopt->Otime, (char *)">=");
	Opt_one_init(&sqltemp, (char *)"Otime", sopt->Etime, (char *)"<=");
	Opt_one_init(&sqltemp, (char *)"Optdsc", sopt->Optdsc, (char *)"=");
	free(username);

	if(sql == NULL){
		g_free(sqltemp);
		sql = g_strdup("select username,Otime,Optdsc from tips_opt");
		//return 1;
	}
	else{
		sql[strlen(sql)-3]='\0';
	}
*/
	opt_pagenum = pagenum;
	Opt_init_sql_interface(sopt, (char *)"select username,Otime,Optdsc from tips_opt",sort);
	return 0;
}

char* CTips_opt::Opt_exec_sql(int how)
{
	char *sqltemp = sql;
#if 1
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
#endif
	//int len = ONCE_MAXDATA;
	if(!how)
		sql = g_strdup_printf("%s limit %d offset %d", sqltemp, len, len*(opt_pagenum-1));
	else
		sql = g_strdup(sqltemp);
	g_free(sqltemp);
	printf("sql*******************************:%s\n",sql);
	char *xml = tips_sqlite_handle.catch_xml_interface(sql, xml_part_operator);
	g_free(sql);
	printf("wokaokowjfiejfie\n");
	printf("xml=======*()*(*)*(*)%s",xml);
	sql=NULL;
	return xml;
}

int CTips_opt::Count_init_sql(opt_stut_t* sopt, uint32_t pagenum)
{
/*保留参数？*/pagenum = pagenum;
	Opt_init_sql_interface(sopt, (char *)"select count(*) from tips_opt");
	return 0;
}

int CTips_opt::Count_exec_sql(uint32_t *total_number)
{	
	printf("sql:::::::::::::::::::;%s\n",sql);
	int ret = tips_sqlite_handle.last_page(sql,0,total_number);
	g_free(sql);
	sql=NULL;
	printf("ret ==================%d\n",ret);
	return ret;
}


int CTips_opt::Count_picture_sql(int *count, const char *search_type)
{                                                
        sql=NULL;                                
        //Alarm_init_sql_ilnterface(salarm, pagenum, (char *)"select count(*) from tips_Alarm");
        sql = g_strdup_printf("select count(*) from tips_opt where Otime like '%%%s%%'",search_type);
        printf("sql======%s\n",sql);
        printf("I going ************************\n");
        *count = tips_sqlite_handle.picture_count(sql);
        printf("myself_sql*************is %d\n",*count);
        g_free(sql);
        return 0;
}

int CTips_opt::Copy_exec_sql(int all)
{
	char *sqltemp = sql;
#if 1
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
#endif
	//int len = ONCE_MAXDATA;
	if(!all)
		sql = g_strdup_printf("%s limit %d offset %d", sqltemp, len, len*(opt_pagenum-1));
	else
		sql = g_strdup(sqltemp);
	g_free(sqltemp);
	int ret = tips_sqlite_handle.tips_Copy(sql, callback_opt, (char *)"审计信息",3);
	g_free(sql);
	sql=NULL;
	return ret;
}

int CTips_opt::Copy_init_sql(opt_stut_t *sopt, uint32_t pagenum)
{
	opt_pagenum = pagenum;
	Opt_init_sql_interface(sopt, (char *)"select username,Otime,Optdsc,flag from tips_opt");
	return 0;
}
