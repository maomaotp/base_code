#ifndef NETLOG_H
#define NETLOG_H
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include "data.h"
#include "net_guard_final/client.h"
#include "reporttask.h"
#include "sqlhandler.h"
#include "itimertask.h"

using namespace std;

class CNetlog{
	Sqlhandler * sqlhandler;
	char deviceID[12];
	time_t ftime, ttime;
	char param[8][1024];
//	sqlite3* db;
//	sqlite3* netlog_db;
	int cnt;
	void Audit();
	int my_pow (int num, int n);
	void getlargeIP (char* source, uint32_t* startIP, uint32_t* largeIP, char** startip, char** overip);
	public:
//	void setDB(sqlite3* dbhandler); 
	void deal();
	CNetlog(char* str);
	void Set_Sqlhandler(Sqlhandler * sql);
};
#endif
