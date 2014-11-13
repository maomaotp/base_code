#ifndef SQLHANDLER_H
#define SQLHANDLER_H
#include <pthread.h>
#include "/usr/local/mysql/include/mysql.h"
class Sqlhandler{
	private:
	MYSQL mysql,*sock;
	pthread_mutex_t mutex;
//	private:

	public:
	bool Init_Sql(char* serverip,char* user,char* passwd,char* dbname,int port,char* unixsocket,int flag);
	int Select_Sql_Exec(char* sql,void (*call_back)(char** result,int column,void* param),void* param);//返回-1-sql语句执行错误，返回-2获取查询结果错误。
	int Insert_Sql_Exec(char* sql);//返回-1错误.
	bool Exec_Begin();
	bool Exec_Commit();
	bool Exec_Rollback();
	//~Sqlhandler();
	void Exit_Sql();

};



#endif
