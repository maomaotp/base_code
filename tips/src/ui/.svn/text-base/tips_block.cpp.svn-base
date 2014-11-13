#include "tips_block.h"
#include "tips_xml_part.h"

extern CUser_sqlite_handle tips_sqlite_handle;

xml_part* xml_part_opt[6] = {xml_Dip,
			     xml_Sip,
                             xml_Sport,
                             xml_Start_time,
			     xml_Expired_time,
			     xml_Block_desc};

int CTips_block::Block_one_init(char **sqltemp, char *name, char *value, char* match)
{
	if((value) && (strlen(value))){
		sql = g_strdup_printf("%s %s %s '%s' and",*sqltemp, name, match,value);
		g_free(*sqltemp);
		*sqltemp = sql;
	}
/*返回值？*/return 0;
}

int CTips_block::Block_init_sql(block_stut_t *sblock)
{
	sql = NULL;
	char *dip = NULL;
	char *sip = NULL;
	char *sport = NULL;
	char *start_time = NULL;
	char *expired_time = NULL;
	char *sqltemp = strdup("select Dip,Sip,Sport,Start_time,Expired_time,Block_desc from tips_block where");

	tips_sqlite_handle.init_sql_num(sblock->dip, &dip);
	tips_sqlite_handle.init_sql_num(sblock->sip, &sip);
	tips_sqlite_handle.init_sql_num(sblock->Sport, &sport);
	tips_sqlite_handle.init_sql_num(sblock->Start_time, &start_time);
	tips_sqlite_handle.init_sql_num(sblock->Expired_time, &expired_time);

	char *ssip = g_strdup_printf("%c%s%c",'%', sip, '%');
	char *ddip = g_strdup_printf("%c%s%c",'%', dip, '%');
	char *sstart_time = g_strdup_printf("%c%s%c",'%', start_time, '%');
	char *eexpired_time = g_strdup_printf("%c%s%c",'%', expired_time, '%');

	Block_one_init(&sqltemp, (char *)"Dip", ddip, (char *)"like");
	Block_one_init(&sqltemp, (char *)"Sip", ssip, (char *)"like");
	Block_one_init(&sqltemp, (char *)"Sport", sport, (char *)"=");
	Block_one_init(&sqltemp, (char *)"Start_time", sstart_time, (char *)"like");
	Block_one_init(&sqltemp, (char *)"Expired_time", eexpired_time, (char *)"like");

	g_free(ssip);
	g_free(ddip);
	g_free(sstart_time);
	g_free(eexpired_time);

	free(dip);
	free(sip);
	free(sport);
	free(start_time);
	free(expired_time);
	if(sql == NULL){
		g_free(sqltemp);
		sql = strdup("select Dip,Sip,Sport,Start_time,Expired_time,Block_desc from tips_block");
		//return 1;
	}
	else{
		sql[strlen(sql)-3]='\0';
	}
	return 0;
}

char* CTips_block::Block_exec_sql()
{
	printf("in Block_exec_sql================================================================\n");
	char *xml = tips_sqlite_handle.catch_xml_interface(sql, xml_part_opt);
	g_free(sql);
	sql=NULL;
	printf("leave Block_exec_sql================================================================\n");
	return xml;
}
