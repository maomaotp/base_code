#ifndef REPORTDATA_H
#define REPORTDATA_H
#include <sys/types.h>
struct netopt_audit{
	int l_type;//21 11  李春辉
	char family;
	u_int dstip;
	u_int srcip;
	u_short dstport;
	u_short srcport;
	u_int8_t protocol;
	time_t start;
	time_t end;
	time_t lasttime;
	u_int64_t load;
	u_int64_t upload;	
};
struct alarm_info{
	int l_type;//31 
	u_int event_id; 
	char family;
	char trojan_id[6];
	char sip[16];
	char dip[16];
	char dmac[18];
	char alarm_time[25];
	char type[3];
	char risk;
	char descry[65];
};
struct alert_info{ 
	int l_type;// 
	char family;
	char plugin_id[5];
	char risk[3];
	char trojan_id[6];
	char alert_time[25];
	char sip[16];
	char sport[6];
	char dip[16];
	char dport[6];
	char dmac[18];
	char protocol[4];
	char descry[65];
	char dnshash[33];
	char extra[65];
};
struct secret_info{
	int l_type;//13 梁彦鹏
	int event_id;
	char family;
	char event_class[5];
	char event_type[2];
	time_t event_time;
	u_int sip;
	u_short sport;
	u_int dip;
	u_short dport;
	char sender[64];
	char receiver[256];
	char title[128];
	char file_name[256];
	char descr[128];
	char sen_msg[128];
};
struct block_info{
	int l_type;//12 梁彦鹏
	u_int dip;
	u_int sip;
	u_short sport;
	time_t start;
	time_t expired;
	char block_desc[32];
};
struct ipblack_info{
	int l_type;//2   张勇
	u_int32_t ip_black_id;
	u_int sip;
	u_int dip;
	time_t alarm_time;
	char type[3];
	char risk;
	char descry[64];
};
struct dnsblack_info{
	int l_type;//4  张勇
	time_t alert_time;
	u_int32_t dns_id;
	u_int sip;
	u_short sport;
	u_int dip;
	u_short dport;
	u_int8_t protocol;
	char risk; //by -- yong 20130411
	char descry[65];
	char dnshash[32];
	char extra_text[65];
};
struct audit_info{
	int l_type;//
	u_int sip;
	u_short sport;
	u_int dip;
	u_short dport;
	u_int8_t protocol;
	

};
#endif
