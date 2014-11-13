#include "tips_picture.h"

extern pthread_mutex_t sqlite_sync_mutex;
extern int startrun;
extern CTips_sqlite_sync tips_sqlite_sync;

int callback(void* p, int argc, char** argv, char** argvv)
{
		long long int* value = (long long int*)p;
		*value = atoll(argv[0]);
		return 0;
}

TipsPicture::TipsPicture()
{
	for (int i = 0; i < 4; i ++)
		level[i] = 0;
	for (int i = 0; i < 7; i ++)
		point[i] = 0;
}

long long int* TipsPicture::Get_level()
{
	return level;
}

long long int* TipsPicture::Get_point()
{
	return point;
}

void TipsPicture::Set_DB(sqlite3* newdb)
{
	db = newdb;
}

int TipsPicture::Set_level_and_point(char* start_time, char* end_time)
{
  tips_sqlite_sync.tips_lock(2);
  char* errmsg = NULL;
  char* sql = NULL;
		int len = 0;
  
  for (int i = 0; i < 4; i ++)
	{
		len =  strlen("select count(*) from tips_Alarm where Risk=''") + sizeof (int);
		sql = (char*)calloc(1, len + 1);
		snprintf(sql, len + 1, "select count(*) from tips_Alarm where Risk='%d')", i + 1);
	  pthread_mutex_lock(&sqlite_sync_mutex);
		int ret = sqlite3_exec(db, sql, callback, (void*)&(level[i]), &errmsg);
		if (SQLITE_OK != ret)
		{
				fprintf(stderr, "%s\n", errmsg);
		}
		pthread_mutex_unlock(&sqlite_sync_mutex);
		free(sql);
		sql = NULL;
		if (NULL != errmsg)
			sqlite3_free(errmsg);
	}

  char sstime[21];
  char eetime[21];
	memset(sstime, 0, sizeof (sstime));
	memset(eetime, 0, sizeof (eetime));
  for (int i = 0; start_time[i]; i ++)
	{
			if ((' ' == start_time[i]) || (':' == start_time[i]))
				  sstime[i] = '-';
			else
					sstime[i] = start_time[i];
	}
	
	for (int i = 0; end_time[i]; i ++)
	{
			if ((' ' == end_time[i]) || (':' == end_time[i]))
				  eetime[i] = '-';
			else
					eetime[i] = end_time[i];
	}
		
	time_t stime = ObtainTime(sstime);
	time_t etime = ObtainTime(eetime);

  time_t space = (etime - stime) / 6;
  char mytime[20];
  for (int i = 0; i < 7; i ++)
  {
			memset(mytime, 0, sizeof (mytime));
			report.TurnTime(stime + space * i, mytime, 20);
			len = strlen("select count(*) from tips_Alarm where Alarm_time=''") + 20;
		  sql = (char*)calloc(1, len + 1);
			snprintf(sql, len + 1, "select count(*) from tips_Alarm where Alarm_time='%s'", mytime);
		  pthread_mutex_lock(&sqlite_sync_mutex);
		int ret = sqlite3_exec(db, sql, callback, (void*)&(point[i]), &errmsg);
		if (SQLITE_OK != ret)
		{
				fprintf(stderr, "%s\n", errmsg);
		}
		pthread_mutex_unlock(&sqlite_sync_mutex);
		free(sql);
		if (NULL != errmsg)
			sqlite3_free(errmsg);
		sql = NULL;
	}
	tips_sqlite_sync.tips_unlock(2);
	return 0;
}

time_t TipsPicture::ObtainTime(char* date)
{
	struct tm tm;
	int day = 0, month = 0, year = 0, hour = 0, min = 0, sec = 0, sum = 0, leap = 0,weekid = 0;
	int i = 2;
	char buf[1024];
	memset(buf, 0, sizeof (buf));
	strcpy(buf, date);
	char *token = NULL;
	token = strtok(buf, "-");

	year = atoi(token);
	tm.tm_year = year - 1900;
	while ((token = strtok(NULL, "-")) != NULL)
	{
		switch (i)
		{
			case 2:
						 month = atoi(token);
			       tm.tm_mon = month - 1;
						 break;
			case 3:
						 day = atoi(token);
						tm.tm_mday = day;
						break;
			case 4:
						hour = atoi(token);
			       if (hour < 0 || hour > 23)
			       {
							fprintf (stderr, "Error:%s\n", strerror(errno));
							exit(0);
							}
							tm.tm_hour = hour;
							break;
			case 5:
							min = atoi(token);
						if ((min < 0) || (min > 59))
						{
								fprintf (stderr, "Error:%s\n", strerror(errno));
								exit(0);
							}
						tm.tm_min = min;				
						break;
			case 6:
						sec = atoi(token);
						if ((sec < 0) || (sec > 59))
						{
								fprintf (stderr, "Error:%s\n", strerror(errno));
								exit(0);
						}
						tm.tm_sec = sec;
			default:
						break;
		}
		i++;
	}
	tm.tm_sec = sec;
	switch (month)
	{
		case 1:
				sum = 0;
				break;
		case 2:
				sum = 31;
				break;
		case 3:
				sum = 59;
				break;
		case 4:
				sum = 90;
				break;
		case 5:
				sum = 120;
				break;
		case 6:
				sum = 151;
				break;
		case 7:
				sum = 181;
				break;
		case 8:
				sum = 212;
				break;
		case 9:
				sum = 243;
				break;
		case 10:
				sum = 273;
				break;
		case 11:
				sum = 304;
				break;
		case 12:
				sum = 334;
				break;
		default:
				fprintf(stdout, "data error\n");
				break;
	}
	sum = sum + day; 
	if (year % 400 == 0 || (year % 4 == 0 && year % 100 != 0))
		leap = 1;
	else
		leap = 0;
	if (leap == 1 && month > 2)
		sum ++;
	if (month == 2)
	{
		month += 12;
		year --;
	}
	weekid = (day + 2 * month + 3 * (month + 1) / 5 + year + year / 4 - year / 100 + year / 400) % 7 + 1;
	if (weekid == 7)
		weekid = 0;
	tm.tm_wday = weekid;
	tm.tm_yday = sum - 1;
	tm.tm_isdst = 0;
	time_t time = mktime(&tm);
	
	return time;
}
