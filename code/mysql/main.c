#include "sqlhandler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void call_back_table_name(char** result,int column,void* param)
{
	*((int *)param) = atoll(result[0]);
	printf("%s\n",result[0]);
	printf("\n");
}
void call_back(char** result,int column,void* param)
{
	int *count =  (int *)param;
	int i;
	(*count)++;
	FILE *file = fopen("sql.log","a");

	for(i=0; i<column; ++i) {
		//printf("%d",i);
		printf("%s ",result[i]);
		fputs(result[i], file);
	}
	printf("\n");
	fputs("\n", file);

}

/*new code*/

void select_sql()
{
	int count = 0;
	MYSQL mysql;
	
	Init_Sql(&mysql);
	
	char* query = (char*)"select (fmId,wbRid,fmName,url,intro,logoAddr,weiboAddr,province,hdvAddr,microAddr,updateTime) from r_url where province=10";

	int ret1 = Select_Sql_Exec(&mysql, query, call_back, &count);
	if(ret1 < 0) {
		printf("select sql fail\n");
	}
	mysql_close(&mysql);
	
	return;
}

void stores_sql()
{
	int count = 0;
	MYSQL mysql;
	Init_Sql(&mysql);

	//char *query = (char *)"call query_incall(1,1,0,'18501158414','002','2013-09-16 12:00:00','2016-09-16 12:00:00')";
	char *sql = (char *)"call procedure_incall('001','001','2014-09-15 12:00:00','2014-09-15 13:00:00',\
							30,'test.log','/data/liuq/','18501158414','liuq',1,'001',0,1)";

	//int ret1 = stores_Sql_Exec(&mysql, sql, call_back, &count);
	printf("sql==%s\n", sql);
	int ret1 = Insert_Sql_Exec(&mysql, sql);
	if(ret1 < 0) {
		printf("call sql fail\n");
	}
	mysql_close(&mysql);
}

void insert_sql(int index)
{
	int i;
	char sql[256];
	MYSQL mysql;

	Init_Sql(&mysql);
	
	for(i=0; i<index; i++)
	{
		if(index == 10086) index++;
		sprintf(sql, "insert into tips_netlog(netlog_id, Dip, Dport, Sip, Sport,Protocol,Start_time, End_time,`Load`, `Upload`,Below_protocol,flag) values(%d,'10.172.0.110','6379','10.171.0.110','7963','6','1114-04-17 13:53:56','1214-04-17 13:54:05','0.02','0.70','6','0')",i+1000);
	
		printf("sql==%s\n", sql);
		int ret = Insert_Sql_Exec(&mysql, sql);
		if(ret < 0)
		{
			printf("insert sql fail\n");
		}
	}
	mysql_close(&mysql);	

	return;
}

int main(int argc, char* argv[])
{
	if(argc <2) {
		printf("select [select/insert]\n");
		exit(1);
	}

	if(0 == strcmp(argv[1], "select")){
		printf("execute select sql\n");
		select_sql();
	}
	else if(0 == strcmp(argv[1], "stores")){
		printf("execute stores\n");
		stores_sql();
	}
	else if(0 == strcmp(argv[1], "insert")){
		printf("execute insert the number is %d\n", atoi(argv[2]));
		insert_sql(atoi(argv[2]));
	}
	
	return 0;
}
