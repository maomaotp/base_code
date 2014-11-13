#include "sqlhandler.h"
#include <stdio.h>
bool Sqlhandler::Init_Sql(char* serverip,char* user,char* passwd,char* dbname,int port,char* unixsocket,int flag)
{
	pthread_mutex_init(&mutex,NULL);
	mysql_init(&mysql);
	sock=mysql_real_connect(&mysql,serverip,user,passwd,dbname,port,unixsocket,flag);
	if(!sock)
	{
		printf("connect:%s\n",mysql_error(sock));
		return false;
	}
	return true;
}

int Sqlhandler::Select_Sql_Exec(char* sql,void (*call_back)(char** result,int column,void* param),void* param)
{
	pthread_mutex_lock(&mutex);
	if(mysql_query(sock,sql))
	{
		printf("error making query:%s\n",mysql_error(sock));
		pthread_mutex_unlock(&mutex);
		return -1;
	}
	MYSQL_RES* res=mysql_use_result(sock);//获取查询结果
	if(!res)
	{
		printf("err use_result:%s\n",mysql_error(sock));
		pthread_mutex_unlock(&mutex);
		return -2;
	}
	MYSQL_ROW row;
//	int row_count=mysql_num_rows(res);
	int column_count=mysql_field_count(sock);
//	for(int i=0;i<=(row_count=mysql_num_rows(res));++i)
	while(1)
	{
//	printf("row_count=========%d,,,column_count======%d\n",row_count,column_count);
		row=mysql_fetch_row(res);
		if(!row)break;
		call_back((char**)row,column_count,param);
	}
	mysql_free_result(res);
	pthread_mutex_unlock(&mutex);
	return 1;
}
int Sqlhandler::Insert_Sql_Exec(char* sql)
{
	pthread_mutex_lock(&mutex);
	if(mysql_query(sock,sql))
	{
		printf("error making query:%s\n",mysql_error(sock));
		pthread_mutex_unlock(&mutex);
		return -1;
	}
	int ret=mysql_affected_rows(sock);
	pthread_mutex_unlock(&mutex);
	return ret;
}
bool Sqlhandler::Exec_Begin()
{
	printf("begin\n");
}
bool Sqlhandler::Exec_Commit()
{
	printf("commit\n");
}
bool Sqlhandler::Exec_Rollback()
{
	printf("rollback\n");
}
void Sqlhandler::Exit_Sql()
{
	pthread_mutex_destroy(&mutex);
	Exec_Commit();
	mysql_close(sock);
}
