#include "sqlhandler.h"
#include <stdio.h>


void Init_Sql(MYSQL* mysql)
{
	#ifdef _THREAD
	pthread_mutex_init(&mutex,NULL);
	printf("thread init\n");
	#endif
	mysql_init(mysql);
	
	if(NULL == mysql_real_connect(mysql, SERVER_IP, USER_NAME, PASSWD, DB_NAME, 0, NULL, 0))
	{
		printf("connect:%s\n",mysql_error(mysql));
		return;
	}
	printf("connect success!\n");

	return;
}

int Select_Sql_Exec(MYSQL* mysql, char* sql, void (*call_back)(char** result, int column, void* param), void* param)
{
#ifdef _THREAD
	pthread_mutex_lock(&mutex);
#endif
	if(mysql_query(mysql,sql)) {
		printf("error making query:%s\n",mysql_error(mysql));
#ifdef _THREAD
		pthread_mutex_unlock(&mutex);
#endif
		return -1;
	}
	MYSQL_RES* res=mysql_use_result(mysql);//获取查询结果
	if(!res) {
		printf("err use_result:%s\n",mysql_error(mysql));
#ifdef _THREAD
		pthread_mutex_unlock(&mutex);
#endif
		return -2;
	}
	MYSQL_ROW row;
//	int row_count=mysql_num_rows(res);
	int column_count=mysql_field_count(mysql);
//	for(int i=0;i<=(row_count=mysql_num_rows(res));++i)
	while(1)
	{
//	printf("row_count=========%d,,,column_count======%d\n",row_count,column_count);
		row=mysql_fetch_row(res);
		if(!row)break;
		call_back((char**)row,column_count,param);
	}
	mysql_free_result(res);
#ifdef _THREAD
	pthread_mutex_unlock(&mutex);
#endif
	return 1;
}

int Insert_Sql_Exec(MYSQL* mysql, char* sql)
{

	#ifdef _THREAD
	pthread_mutex_lock(&mutex);
	#endif
	if(mysql_query(mysql, sql))
	{
		printf("error making query:%s\n",mysql_error(mysql));
		#ifdef _THREAD
		pthread_mutex_unlock(&mutex);
		#endif
		return -1;
	}
	int ret=mysql_affected_rows(mysql);
	#ifdef _THREAD
	pthread_mutex_unlock(&mutex);
	#endif
	
	return ret;
}

int Exec_Begin()
{
	printf("begin\n");
}

int Exec_Rollback()
{
	printf("rollback\n");
}

int stores_Sql_Exec(MYSQL* mysql, char* sql, void (*call_back)(char** result, int column, void* param), void* param)
{
	if(mysql_query(mysql,sql)) {
		printf("error making query:%s\n",mysql_error(mysql));
		return -1;
	}
	MYSQL_RES* res=mysql_use_result(mysql);//获取查询结果
	if(!res) {
		printf("err use_result:%s\n",mysql_error(mysql));
		return -2;
	}
	MYSQL_ROW row;
	int column_count=mysql_field_count(mysql);
	while(1)
	{
		row=mysql_fetch_row(res);
		if(!row)break;
		call_back((char**)row,column_count,param);
	}
	mysql_free_result(res);
	return 1;
}
