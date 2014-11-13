#ifndef RULESDATA_H
#define RULESDATA_H
#include <stdint.h>
struct dns_rules{
	char type;
	char dns_name[65];
	char match_mode;
	char dangerous; 
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
	uint32_t startip;
	uint32_t endip;
	char region[28];
	char tirr[40];
	char flag;
};
struct sec_rules{
	char content[129];
};
struct spectro_rules{
	char content[1024];
};
struct mail_rules{
	char sign;
	char x;
	char endtime[20];
	char starttime[20];
	char content[256];
}; 
struct load_struct{
void** rules;
int type;
};
#endif
