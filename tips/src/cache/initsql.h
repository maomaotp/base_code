#ifndef INITSQL_H_
#define INITSQL_H_
#include "data.h"
#include "rulesdata.h"
#include "reportdata.h"
#include <arpa/inet.h>
#include <cstring>
#include <cstdio>
#include <stdlib.h>
#include "getmac.h"
#include "data.h"
#include "cachestruct.h"
#include "auditinfo.h"
//#include "dbpthread.h"

#define UNIT_DYNAMIC 1 //当写入内容长度比unitsize大时使用多个unit
#define UNIT_STATIC 0 //当写入内容长度比unitsize大时不写入直接返回
struct Unit{
	u_int8_t flag;//标志位，最后一位为写入标志
	int free_size;//剩余空间大小
	int employ_size;//已经使用大小
	struct Unit* next_unit;//下一个内存单元地址(当head->type==1时有效)
};
struct Head{
	int block_size;
	int unit_size;
	int unit_count;
	int already;//已经使用的快数
	int type;//是否存在连续单元
	struct Head* next_block;//下一个内存快指针
	struct Unit* last_unit;//最后一个单元地址
	struct Unit* first_unit;//第一个地址单元
};

struct ipscc{
	u_int startip;
	u_int endip;
	char region[65];
	char territory[97];
	char flag;
};

struct ipflag{
	u_int startip;
	u_int endip;
	bool operator==(const ipflag srcflag);
	friend bool operator<(const ipflag dscflag, const ipflag srcflag);
};

class CInit_sql{
public:
	bool ipmarkoff(in_addr_t *addr);
	int init_deviceID();
	int init_scc(uint32_t ip);
	int init_Dmac(uint32_t ip);
	int sql_alarm_info(cache_alarm_t* alarm_data, char* sql);
	int sql_alert_info(cache_alert_t* alert_data, char* sql);
	int sql_netopt_info(cache_audit_t* netopt_data, char *sql);
	int sql_opt_info(cache_opt_t* opt_data, char* sql);
	int sql_secret_info(cache_secret_t* secret_data, char* sql);
	int sql_block_info(cache_block_t* block_data, char* sql);
	int sql_ipblack_info(ipblack_info* ipblock_data, char* sql);
	int sql_dnsblack_info(dnsblack_info* dnsblock_data, char* sql);
	int init_dnshash(char *sdnshash, char *des);
	void TurnTime(const time_t clock, char *tips_clock, int clocksize);
	void initmap(map<ipflag, ipscc> *smap);
	int judge_ip(uint32_t sip, int flag, u_short port);
private:
	CGetMac get_mac;
	//map<ipflag, ipscc> *ipmap;
	char Dmac[20];
	char scc_flag;
	char scc[162];
	char deviceID[11];
};

#endif
