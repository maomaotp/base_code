#ifndef INITSQL_H_
#define INITSQL_H_
#include <arpa/inet.h>
#include <stdlib.h>

typedef struct cache_alarm_t
{
	char alarm_id[12];
	char device_id[12];
	char trojan_id[6];
	char sip[19];
	char dip[19];
	char dmac[20];
	time_t alarm_time;
	char type[3];
	char risk;
	char scc[65];
	char scc_flag;
	char descry[65];
}cache_alarm_t;


#endif
