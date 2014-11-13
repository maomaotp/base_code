#ifndef REPORTTASK_H
#define REPORTTASK_H
#include "itimertask.h"
#include <iostream>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include "ointerface.h"
#include "net_guard_final/client.h"
#define DATADABE_PATH "tips.db"

class ReportTask:public ITimerTask{
	void Scan_Dir(const char* path);
//	void Get_table_id(sqlite3* db, char** azResult, int nrow, int ncolumn, char* errMsg);
	//void Updata_table_id(sqlite3* db, char* errMsg);
//	void My_equal(uint32_t *n, char* p);
//	void report_alarm(char** result,int column,void* param);
//	void select_count(char** result,int column,void* param);
//	void My_Strcpy(char* dest, char* src, int);
//	void Get_block(sqlite3* db, char** azResult, int nrow, int ncolumn, void** cache, char*);
//	void Get_senstive_alert(sqlite3* db, char** azResult, int nrow, int ncolumn, void** cache, char*);
//	void Get_alert(sqlite3* db, char** azResult, int nrow, int ncolumn, void** cache, char*);
	void Get_alarm();
	void Get_opt();
	void Get_block();
	void Get_alert();
//	void Get_netlog(sqlite3* db, char** azResult, int nrow, int ncolumn, char* errMsg);
//	void My_equalc(char* c, char* str);
	public:
	void Update_flag(int n, char* str);
	void Send_Err(char* str);
	//void Send_Err1(int, char* str);
	void TurnTime(const time_t clock, char *tips_clock, int clocksize);
	virtual void Start_Task();
	virtual void Init_Task();
};
#endif
