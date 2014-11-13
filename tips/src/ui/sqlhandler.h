#ifndef SQLHANDLER_H
#define SQLHANDLER_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "/usr/local/mysql/include/mysql.h"
class Sqlhandler{
	private:

	MYSQL mysql,*sock;
	pthread_mutex_t mutex;


	public:
	static Sqlhandler* getInstance();
	static Sqlhandler* instance;
	Sqlhandler();
	Sqlhandler(const Sqlhandler&);
	Sqlhandler& operator=(const Sqlhandler&);

	bool Init_Sql(char* serverip,char* user,char* passwd,char* dbname,int port,char* unixsocket,int flag);
	int Select_Sql_Exec(char* sql,void (*call_back)(char** result,int column,void* param),void* param);//返回-1-sql语句执行错误，返回-2获取查询结果错误。
	int Insert_Sql_Exec(char* sql);//返回-1错误.
	bool Begin();
	bool Commit();
	bool Rollback();
	//~Sqlhandler();
	void Exit_Sql();

};



#endif
