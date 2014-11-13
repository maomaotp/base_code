#ifndef SQLHANDLER_H
#define SQLHANDLER_H
#include <pthread.h>
#include "/usr/local/mysql/include/mysql.h"

#define SERVER_IP "192.168.1.120"
#define USER_NAME "dba"
#define PASSWD "123456"
#define DB_NAME "test"

//#define _THREAD

//void select_sql()

pthread_mutex_t mutex;
void Init_Sql(MYSQL* mysql);
int queryproXml(MYSQL* mysql, char* sql, char *xmlBuf);
int Select_Sql_Exec(MYSQL* mysql, char* sql, char *xmlBuf);
int stores_Sql_Exec(MYSQL* mysql, char* sql1, void (*call_back)(char** result,int row, char *resBuf), char *resBuf);
int Insert_Sql_Exec(MYSQL* mysql, char* sql);
int Exec_Begin();
int Exec_Rollback();

#endif
