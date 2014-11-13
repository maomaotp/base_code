#ifndef DATA_H
#define DATA_H
#include<iostream>
#include<sys/types.h>
#include <stdint.h>
#define DEVICEID "1203010088"

struct config_struct{
	char device_id[12];
	char device_locate[7];
	char device_made[3];
	char device_type[3];
	char ip[17];
	char mac[20];
	char mask[17];
	char gateway[17];
	char center_ip[17];
	char version[17];
	char dns_version[17];
	char ip_version[17];
	char swip_version[17];
	char geoip_version[17];
	char sen_version[17];
	char spe_version[17];
	char own[17];
};
struct status_struct{
char device_id[12];
char stime[21];
int mem;
int cpu;
double disk;
char mirror;
int flow;
int active_peer;
char module_state[9];
};//设备状态信息
struct check_status{
char mirror;
int flow;
int active_peer;
int module_state;//待定
};
struct dev_struct{
char device_id[12];
char device_locate[7];
char device_made[3];
};//设备配置信息

struct alarm{
char alarm_id[12];
char device_id[12];
char trojan_id[6];
char sip[19];
char dip[19];
char dmac[20];
char alarm_time[25];
char type[3];
char risk;
char scc[65];
char scc_flag;
char descr[65];
}; 
struct alert{
char alert_id[12];
char device_id[12];
uint32_t plugin_id;
char risk;
char trojan_id[6];
char alarm_time[25];
char sip[19];
uint32_t sport;
char dip[19];
uint32_t dport;
char dmac[20];
u_short protocol; 
char descr[65];
char dnshash[33];
char scc[65];
char scc_flag;
char extra_text[65];
};
struct senstive_alert{
char event_id[12];
char device_id[12];
uint32_t event_class;
char event_type[3];
char event_time[25];
char sip[19];
uint32_t sport;
char dip[19];
uint32_t dport;
char dmac[20];
char sender[65];
char receiver[257];
char title[129];
char file_name[257];
char descr[129];
char sen_msg[129];
};
struct session_blocked{
char device_id[12];
char dip[19]
char sip[19];
uint32_t sport;
char start_time[25];
char expired_time[25];
char block_desc[33];
};
#endif
