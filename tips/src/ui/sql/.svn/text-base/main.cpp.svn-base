#include "sqlhandler.h"
#include <stdio.h>
#include <stdlib.h>
void call_back_table_name(char** result,int column,void* param)
{
	*((int *)param) = atoll(result[0]);
	printf("%s\n",result[0]);
	///////////////////////////int *count =  (int *)param;
	///////////////////////////(*count)++;
	//printf("********************%d\n",*count);
	//*((int *)param)++;
	//*((int *)param)++;
	//printf("column========%d\n",column);
	//for(int i=0;i<column;++i)
	//{
		//printf("%s     ",result[0]);
	//}
	printf("\n");
}
void call_back(char** result,int column,void* param)
{
	int *count =  (int *)param;
	(*count)++;
	printf("********************%d\n",*count);
	//*((int *)param)++;
	//*((int *)param)++;
	//printf("column========%d\n",column);
	for(int i=0;i<column;++i)
	{
		printf("%s     ",result[i]);
	}
	printf("\n");
}
int main()
{
	Sqlhandler sqlhandler;
	int count = 0;
	bool err=sqlhandler.Init_Sql((char *)"127.0.0.1",(char *)"root",(char *)"123456",(char *)"xx",0,NULL,0);
	if(!err)
		printf("初始化sql错误!");
	//char* query_table_name=(char*)"SHOW COLUMNS FROM tttips_netlog;";
	char* query1=(char*)"select count(*) from tttips_netlog;";
	char* query=(char*)"select * from tttips_netlog;";
	//int ret_table_name=sqlhandler.Select_Sql_Exec(query_table_name,call_back_table_name,&count);
	//int ret=sqlhandler.Select_Sql_Exec(query1,call_back_table_name,&count);
	int ret1=sqlhandler.Select_Sql_Exec(query,call_back,&count);
	printf("ret==========%d\n",count);
	sqlhandler.Exit_Sql();
	
}
