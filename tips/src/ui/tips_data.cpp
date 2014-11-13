#include "tips_data.h"
#include "tips_xml_part.h"

extern CUser_sqlite_handle tips_sqlite_handle;

xml_part* xml_part_data[4] = {xml_backid,
			     xml_name,
                             xml_Start_time,
			     xml_tips_table};

int CTips_data::Data_one_init(char **sqltemp, char *name, char *value, char* match)
{
	if((value) && (strlen(value))){
		sql = g_strdup_printf("%s %s %s '%s' and",*sqltemp, name, match,value);
		g_free(*sqltemp);
		*sqltemp = sql;
	}
/*返回值？*/return 0;
}

int CTips_data::Data_init_sql(data_stut_t *data, uint32_t pagenum)
{
	data_pagenum = pagenum;
//	sql = g_strdup_printf("select BackID,BackName,BackTime,Oid from tips_back");
	sql = g_strdup_printf("select BackID,BackName,BackTime,Oid from tips_back where BackName like '%%%s%%' and BackTime like '%%%s%%'",data->name,data->time);
	return 0;
}

char* CTips_data::Data_exec_sql()
{
	char *sqltemp = sql;
	int len = ONCE_MAXDATA;
	sql = g_strdup_printf("%s limit %d offset %d", sqltemp, len, len*(data_pagenum-1));
	g_free(sqltemp);
	char *xml = tips_sqlite_handle.catch_xml_data(sql, xml_part_data);
	g_free(sql);
	sql=NULL;
	return xml;
}

int CTips_data::Count_init_sql(data_stut_t* sdata, uint32_t pagenum)
{
/*保留参数？*/pagenum = pagenum;
//	sql = g_strdup_printf("select BackID,BackName,BackTime,Oid from tips_back");
	sql = g_strdup_printf("select count(*) from tips_back where BackName like '%%%s%%' and BackTime like '%%%s%%'",sdata->name,sdata->time);
	return 0;
}

int CTips_data::Count_exec_sql()
{	
	int ret = tips_sqlite_handle.last_page(sql,1);
	g_free(sql);
	sql=NULL;
	return ret;
}
