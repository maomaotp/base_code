#include "tips_alert.h"
#include "tips_xml_part.h"

extern CUser_sqlite_handle tips_sqlite_handle;

xml_part* xml_part_alert[7] = {xml_plugin_id,
			       xml_Risk,
                               xml_Alarm_time,
                               xml_Sip,
			       xml_Dip,
			       xml_Dmac,
			       xml_Scc_flag};

int CTips_alert::Alert_one_init(char **sqltemp, char *name, char *value, char* match)
{
	if((value) && (strlen(value))){
		sql = g_strdup_printf("%s %s %s '%s' and",*sqltemp, name, match,value);
		g_free(*sqltemp);
		*sqltemp = sql;
	}
/*返回值？*/return 0;
}

int CTips_alert::Alert_init_sql(alert_stut_t *salert)
{
	sql = NULL;
	char *plugin_id = NULL;
	char *Risk = NULL;
	char *Alarm_time = NULL;
	char *Sip = NULL;
	char *Dip = NULL;
	char *Dmac = NULL;
	char *Scc_flag = NULL;
	char *sqltemp = strdup("select plugin_id,Risk,Alarm_time,Sip,Dip,Dmac,Scc_flag from tips_Alert where");
	tips_sqlite_handle.init_sql_num(salert->plugin_id, &plugin_id);
	tips_sqlite_handle.init_sql_num(salert->Risk, &Risk);
	tips_sqlite_handle.init_sql_num(salert->Alarm_time, &Alarm_time);
	tips_sqlite_handle.init_sql_num(salert->Sip, &Sip);
	tips_sqlite_handle.init_sql_num(salert->Dip, &Dip);
	tips_sqlite_handle.init_sql_num(salert->Dmac, &Dmac);
	tips_sqlite_handle.init_sql_num(salert->Scc_flag, &Scc_flag);

	char *SSip = g_strdup_printf("%c%s%c",'%', Sip, '%');
	char *DDip = g_strdup_printf("%c%s%c",'%', Dip, '%');
	char *DDmac = g_strdup_printf("%c%s%c",'%', Dmac, '%');
	char *AAlarm_time = g_strdup_printf("%c%s%c",'%', Alarm_time, '%');

	Alert_one_init(&sqltemp, (char *)"plugin_id", plugin_id, (char *)"=");
	Alert_one_init(&sqltemp, (char *)"Risk", Risk, (char *)"=");
	Alert_one_init(&sqltemp, (char *)"Alarm_time", AAlarm_time, (char *)"like");
	Alert_one_init(&sqltemp, (char *)"Sip", SSip, (char *)"like");
	Alert_one_init(&sqltemp, (char *)"Dip", DDip, (char *)"like");
	Alert_one_init(&sqltemp, (char *)"Dmac", DDmac, (char *)"like");
	Alert_one_init(&sqltemp, (char *)"Scc_flag", Scc_flag, (char *)"=");

	g_free(SSip);
	g_free(DDip);
	g_free(DDmac);
	g_free(AAlarm_time);

	free(plugin_id);
	free(Risk);
	free(Alarm_time);
	free(Sip);
	free(Dip);
	free(Dmac);
	free(Scc_flag);
	if(sql == NULL){
		g_free(sqltemp);
		sql = strdup("select plugin_id,Risk,Alarm_time,Sip,Dip,Dmac,Scc_flag from tips_Alert");
		//return 1;
	}
	else{
		sql[strlen(sql)-3]='\0';
	}
	return 0;
}

char* CTips_alert::Alert_exec_sql()
{
	printf("in Block_exec_sql================================================================\n");
	char *xml = tips_sqlite_handle.catch_xml_interface(sql, xml_part_alert);
	g_free(sql);
	sql=NULL;
	printf("leave Block_exec_sql================================================================\n");
	return xml;
}
