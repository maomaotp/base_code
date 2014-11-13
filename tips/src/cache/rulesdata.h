#ifndef RULESDATA_H
#define RULESDATA_H
#include <sys/types.h>
#include <stdint.h>
struct spectro_rules{
	char content[1024];
};
struct dns_rules{
	u_int8_t type;//类型
	char dns_name[65];//域名
	u_int8_t match_mode;//匹配模式
	u_int8_t dangerous;//危险级别
};
struct mail_rules{
	char sign;
	char x;
	char endtime[20];
	char starttime[20];
	char content[256];
};
/*
struct blackip_rules{
	u_int type;
	u_int startip;
	u_int endip;
	u_short port;
	u_int8_t dangerous;
};*/
struct white_rules{
	u_int startip;
	u_int endip;
	u_short port;
};
struct net_shark{
	u_int startip;
	u_int endip;
	uint8_t protocol;
	uint16_t srcp;
	uint16_t dstp;	
};
struct ip_rules{
	char type;
	u_int startip;
	u_int endip;
	u_short port;
	char dangerous;
};
struct ipaudit_rules{
	u_int startip;
	u_int endip;
	u_short port;
};
struct ippos_rules{
	u_int startip;
	u_int endip;
	char region[28];
	char tirr[40];
	char flag;
};
struct sec_rules{
	char content[129];
};
struct load_struct{
void** rules;
int type;
};
#endif
