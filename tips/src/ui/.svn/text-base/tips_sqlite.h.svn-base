#ifndef TIPS_SQLITE_H
#define TIPS_SQLITE_H

#include <stdio.h>
#include <sqlite3.h>
#include <glib.h>
#include <stdlib.h>
#include "gsad_base.h"
#include <iostream>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include <errno.h>
#include "sqlhandler.h"
#include "tips_time.h"

using namespace std;

#define ONCE_MAXDATA 50

typedef int (*tips_callback)(void*,int,char**,char**);
typedef void (*tips_callback_mysql)(char**,int,void*);

typedef struct xml_stut{
	char *xml;
	void *fun;
}xml_stut_t;

typedef struct db_now{
	unsigned long long int nowspace;
	sqlite3 *db;
}db_now_t;


typedef char* xml_part(char* value, char **xmlpart);
typedef xml_part* (*xml_fun_opt)[1];


void callback_alarm(char **argv, int argc, void *dbnow);
void callback_event(char **argv, int argc, void *dbnow);
void callback_netlog(char **argv, int argc, void *dbnow);
void callback_opt(char **argv, int argc, void *dbnow);
int tipscallback_foradmit_time(void* source_xml, int argc, char **argv, char **argvv);

class CUser_sqlite_handle{
public:
	CUser_sqlite_handle();
	~CUser_sqlite_handle();
	int User_sqlite_interface(sqlite_user_t *user,
				  tips_callback tipscallbace);
	int User_admit_sqlite_interface(user_t *user,
				  tips_callback tipscallbace);
	int User_login_judge(sqlite_user_t* user);
	int User_admit_login_judge(user_t* user);
	char *User_catch(char *role = NULL);
	int User_insert(sqlite_user_t* user);
	char *User_search(const char *username,const char *role);
	int User_delete(const char *username);
	int User_stop(const char *username);
	int User_start(const char *username);
	int get_admit_time(const char *username);
	char* User_Role_Judge(const char *username);
	int User_Update(sqlite_user_t *user);
	int User_Update_myself(sqlite_user_t *user);
	char* catch_xml_interface(char *sql, void *xmlpartfun);
	int catch_opt(char *user, char *des);
	int insert_login_time(const char *user);
	int init_sql_num(char* src, char** dest);
	////time_t MyObtainTime(char* date);//划分时间段
	void TurnTime(const time_t clock, char *tips_clock, int clocksize);
	int tips_Copy(char *findsql, tips_callback_mysql tipscopy, char *name, int oid);
	char* catch_xml_data(char *sql, void *xmlpartfun);
	int Data_delete(unsigned long long int oid, char *tips_table);
	int Data_rebuild(unsigned long long int oid, char *tips_table);
	int Data_Delete_obj(char *sql);
	int Data_rebuild_netlog(unsigned long long int oid, char *tips_table);
	int Data_rebuild_event(unsigned long long int oid, char *tips_table);
	int Data_rebuild_opt(unsigned long long int oid, char *tips_table);
	char *One_Data_catch(unsigned long long int oid);
	int last_page(char *tablename, int judge=0,uint32_t *total_number=NULL);
	int picture_count(char *sql,int judge=0);
private:
	sqlite3 *db;
	sqlite3 *bdb;
	Sqlhandler sqlhandler;
};

#endif
