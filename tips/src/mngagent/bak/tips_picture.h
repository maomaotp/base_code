#ifndef _TIPS_PICTURE
#define _TIPS_PICTURE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "sqlite3.h"
#include "reporttask.h"
#include "tips_sqlite_manage.h"

class TipsPicture
{
  ReportTask report;
	sqlite3* db;
	long long int level[4];
	long long int point[7];
	time_t ObtainTime(char* date);
public:
	TipsPicture();
	long long int* Get_level();
	long long int* Get_point();
	void Set_DB(sqlite3* newdb);
	int Set_level_and_point(char* start_time, char* end_time);
};

#endif
