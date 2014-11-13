#ifndef SQLHANDLER_H
#define SQLHANDLER_H
#include <pthread.h>
#include "/usr/local/mysql/include/mysql.h"

#define SERVER_IP "127.0.0.1"
#define USER_NAME "root"
#define PASSWD "123456"
#define DB_NAME "fm"

//#define _THREAD


pthread_mutex_t mutex;
void Init_Sql(MYSQL* mysql);
int Select_Sql_Exec(MYSQL* mysql, char* sql, void (*call_back)(char** result,int column,void* param), void* param);
int stores_Sql_Exec(MYSQL* mysql, char* sql1, void (*call_back)(char** result,int column,void* param), void* param);
int Insert_Sql_Exec(MYSQL* mysql, char* sql);
int Exec_Begin();
int Exec_Rollback();

#endif
