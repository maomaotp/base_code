#include "sqlhandler.h"

bool Sqlhandler::Init_Sql(char* serverip,char* user,char* passwd,char* dbname,int port,char* unixsocket,int flag)
{
	pthread_mutex_init(&mutex,NULL);
	mysql_init(&mysql);
	char value = 1;
	mysql_options(&mysql,MYSQL_OPT_RECONNECT,&value);
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
	int column_count=mysql_field_count(sock);
	while(1)
	{
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
bool Sqlhandler::Begin()
{
	pthread_mutex_lock(&mutex);
	if(mysql_real_query(sock,"Begin;",(unsigned int)strlen("Begin ;")))
	{
		printf("Begin query:%s\n",mysql_error(sock));	
		pthread_mutex_unlock(&mutex);
		return false;
	}
	pthread_mutex_unlock(&mutex);
	return true;

}
bool Sqlhandler::Commit()
{
	pthread_mutex_lock(&mutex);
	if(mysql_real_query(sock,"COMMIT;",(unsigned int)strlen("COMMIT;")))
	{
		printf("Commit query:%s\n",mysql_error(sock));	
		pthread_mutex_unlock(&mutex);
		return false;
	}
	pthread_mutex_unlock(&mutex);
	return true;
}
bool Sqlhandler::Rollback()
{
	pthread_mutex_lock(&mutex);
	if(mysql_real_query(sock,"ROLLBACK;",(unsigned int)strlen("ROLLBACK;")))
	{
		printf("Rollback query:%s\n",mysql_error(sock));	
		pthread_mutex_unlock(&mutex);
		return false;
	}
	pthread_mutex_unlock(&mutex);
	return true;
}
void Sqlhandler::Exit_Sql()
{
	pthread_mutex_destroy(&mutex);
	Commit();
	mysql_close(sock);
}

Sqlhandler::Sqlhandler()
{
}

Sqlhandler::Sqlhandler(const Sqlhandler&)
{
}

Sqlhandler& Sqlhandler::operator=(const Sqlhandler&)
{
}

Sqlhandler* Sqlhandler::instance = new Sqlhandler();
Sqlhandler* Sqlhandler::getInstance()
{
	return instance;
}
