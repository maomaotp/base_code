#include "initsql.h"

void TurnTime(const time_t clock, char *tips_clock, int clocksize)
{
	struct tm *tm;
	char *format=(char *)"%Y-%m-%d %H:%M:%S";
	tm = localtime(&clock);
	strftime(tips_clock,  clocksize, format,tm);

	return;
}

int sql_alarm_info(cache_alarm_t* alarm_data)
{
	uint32_t srcip, dstip;
	struct in_addr ipv4;
	char judgetime[21];
    char* sql;

	memset(judgetime, 0, 21);
	TurnTime(alarm_data->alarm_time, judgetime, 21);
	
	inet_pton(AF_INET, alarm_data->sip, &ipv4);
	srcip = ipv4.s_addr;
	inet_pton(AF_INET, alarm_data->dip, &ipv4);
	dstip = ipv4.s_addr;
	uint32_t dstiptemp = ntohl(dstip);
	uint32_t srciptemp = ntohl(srcip);
	
	fprintf(stdout, "time = %s\n", judgetime);	
	bool judge=ipmarkoff(&srcip);
	fprintf(stdout, "time1111111111111 = %s\n", judgetime);	
	uint32_t realsip = (judge ? srciptemp : dstiptemp);
	realsip = realsip;		//hyj 未使用变量
	fprintf(stdout, "time1111111111111 = %s\n", judgetime);	
	init_deviceID();
	fprintf(stdout, "time = %s\n", judgetime);
	fprintf(stdout, "judge = %d\n", judge);
	fprintf(stdout, "sip = %s\n", alarm_data->sip);	
	fprintf(stdout, "dip = %s\n", alarm_data->dip);
	fprintf(stdout, "mac = %s\n", alarm_data->dmac);
	fprintf(stdout, "type = %s\n", alarm_data->type);	

	sprintf(sql, "insert into tips_Alarm(Trojan_id,Sip,Dip,Dmac,Alarm_time,Type,Risk,Scc_flag,Scc,Descry,Alarm_iid,Device_id,Alarm_judgetime,flag) values('%s','%s','%s','%s','%s','%s','%c','%c','%s','%s','%u','%s','%s','0')",
						alarm_data->trojan_id,
						(judge ? alarm_data->dip : alarm_data->sip),
						(judge ? alarm_data->sip : alarm_data->dip),
						alarm_data->dmac,
						judgetime,
						alarm_data->type,
						alarm_data->risk,
						alarm_data->scc_flag,
						((alarm_data->scc != NULL)? alarm_data->scc : "unkown"),
						alarm_data->descry,
						count ++,
						deviceID,
						judgetime);
	printf(" alarm sql===%s\n", sql);
 
	return 0;
}

int main()
{
    cache_alarm_t* trojan = NULL;
    char saddr[20];

    if(NULL == (trojan = (cache_alarm_t *)malloc(sizeof(struct cache_alarm_t))))
}

#if 0
int CInit_sql::sql_alert_info(cache_alert_t* alert_data, char* sql)
{

	init_deviceID();
	//uint32_t dip = inet_addr(alert_data->dip);
	//uint32_t sip = inet_addr(alert_data->sip);
	//bool judge=ipmarkoff(&dip);
	//init_scc((judge ? sip : dip));
	//char deshash[100];
	//init_dnshash(alert_data->dnshash, deshash);
	sprintf(sql, "insert into tips_Alert(plugin_id,Dip,Dmac,Sip,Alarm_time,Risk,Scc_flag,Trojan_id,Sport,Protocol,Descry,Dnshash,Scc,Extra_text,Dport,Alert_iid,Device_id,flag) values('%d','%s','%s','%s','%s','%s','%c','%s','%d','%d','%s','%s','%s','%s','%d','%s','%s','0')",
						alert_data->plugin_id,
						alert_data->dip,
						alert_data->dmac,
						alert_data->sip,
						alert_data->alarm_time,
						alert_data->risk,
						alert_data->scc_flag,
						alert_data->trojan_id,
						alert_data->sport,
						alert_data->protocol,
						alert_data->descry,
						alert_data->dnshash,
						//deshash,
						(alert_data->scc != NULL) ? alert_data->scc:"unkown",
						alert_data->extra_text,
						alert_data->dport,
						alert_data->alert_id,
						deviceID);
  printf(" alert sql===%s\n", sql);

	return 0;		//hyj 返回值?
}

int CInit_sql::sql_opt_info(cache_opt_t* opt_data, char* sql)
{
	fprintf(stdout, "username = %s\n", opt_data->username);
	fprintf(stdout, "Otime = %s\n", opt_data->opttime);
	fprintf(stdout, "Optdsc = %s\n", opt_data->describe);
	sprintf(sql, "insert into tips_opt(username,Otime,Optdsc,flag) values ('%s','%s','%s','0');",\
			opt_data->username,
			opt_data->opttime,
			opt_data->describe);
	return 0;

}

#if 0
int CInit_sql::sql_secret_info(struct secret_info* secret_data,
										  char *sql)
{
//	bool judge=ipmarkoff(&(secret_data->sip));
#if 0
  if(judge_ip(secret_data->sip, 1, secret_data->sport))
		return 1;
  if(judge_ip(secret_data->dip, 1, secret_data->dport))
		return 1;
#endif
	char srcstr[IP_STR_LEN];
	char dststr[IP_STR_LEN];
/*
	uint16_t sport = ntohs(secret_data->sport);
	uint16_t dport = ntohs(secret_data->dport);
*/
	uint16_t sport = secret_data->sport;
	uint16_t dport = secret_data->dport;
	char event_time[20];
	memset(event_time, 0, 20);
  TurnTime(secret_data->event_time , event_time, 20);

  char judgetime[20];
	memset(judgetime, 0, 20);
  memcpy(judgetime, event_time, strlen(event_time)-JUDGE_TIME_ONEMIN);

	init_Dmac((judge ? secret_data->sip : secret_data->dip));

	memset(srcstr, 0, IP_STR_LEN);
	memset(dststr, 0, IP_STR_LEN);
	inet_ntop(AF_INET, &(secret_data->dip), dststr, IP_STR_LEN);
	inet_ntop(AF_INET, &(secret_data->sip), srcstr, IP_STR_LEN);
	init_deviceID();
	sprintf(sql, "insert into tips_Event(Event_class,Event_type,Event_time,\
					  Sip,Sport,Dip,Dport,Dmac,Sender,Receiver,Title,File_name,\
					  Descr,Sen_msg,Event_iid,Device_id,Event_judgetime,flag) values('%s','%s','%s',\
					  '%s','%hu','%s','%hu','%s','%s','%s','%s','%s','%s','%s',\
					  '%d','%s','%s','0')", 
					  secret_data->event_class,
					  secret_data->event_type,
					  event_time,
					  (judge ? dststr : srcstr),
					  (judge ? dport : sport),
					  (judge ? srcstr: dststr),
					  (judge ? sport : dport),
					  Dmac,
					  secret_data->sender,
					  secret_data->receiver,
					  secret_data->title,
					  secret_data->file_name,
					  secret_data->descr,
					  secret_data->sen_msg,
					  secret_data->event_id,
					  deviceID,
						judgetime);
	printf("sec sql====%s\n",sql);
	return 0;
}
#endif
int CInit_sql::sql_block_info(cache_block_t* block_data,
										  char *sql)
{
#if 0
  if(judge_ip(block_data->dip, 3, 0))
		return 1;
  if(judge_ip(block_data->sip, 1, block_data->sport))
		return 1;
#endif
#if 0
	char srcstr[IP_STR_LEN];
	char dststr[IP_STR_LEN];
	memset(srcstr, 0, IP_STR_LEN);
	memset(dststr, 0, IP_STR_LEN);
	inet_ntop(AF_INET, &(block_data->dip), dststr, IP_STR_LEN);
	inet_ntop(AF_INET, &(block_data->sip), srcstr, IP_STR_LEN);

	//uint16_t sport = ntohs(block_data->sport);
	uint16_t sport = block_data->sport;

//	bool judge=ipmarkoff(&(block_data->sip));


	char start_time[20];
	char expired_time[20];
	memset(start_time, 0, 20);
	memset(expired_time, 0, 20);
  TurnTime(block_data->start, start_time, 20);
  TurnTime(block_data->expired, expired_time, 20);
#endif
	init_deviceID();
	sprintf(sql, "insert into tips_block(Dip,Sip,Sport,Start_time,Expired_time,Block_desc,Device_id,flag) values('%s','%s','%hu','%s','%s','%s','%s','0')",
//					  (judge ? srcstr : dststr),
//					  (judge ? dststr : srcstr),
						block_data->dip,
						block_data->sip,
						block_data->sport,
						block_data->start_time,
						block_data->expired_time,
						block_data->block_desc,
					  deviceID);
	printf("sql=====%s\n", sql);
	return 0;
}

int CInit_sql::sql_ipblack_info(struct ipblack_info* ipblock_data,
										  char *sql)
{
	char srcstr[IP_STR_LEN];
	char dststr[IP_STR_LEN];
	memset(srcstr, 0, IP_STR_LEN);
	memset(dststr, 0, IP_STR_LEN);
	inet_ntop(AF_INET, &(ipblock_data->dip), dststr, IP_STR_LEN);
	inet_ntop(AF_INET, &(ipblock_data->sip), srcstr, IP_STR_LEN);



	bool judge=ipmarkoff(&(ipblock_data->sip));
	uint32_t tips_dip = ntohl(ipblock_data->dip);
	uint32_t tips_sip = ntohl(ipblock_data->sip);

  if(judge_ip(tips_dip, 2, 0))
		return 1;
  if(judge_ip(tips_sip, 2, 0))
		return 1;

	init_Dmac((judge ? ipblock_data->sip : ipblock_data->dip));
	init_scc((judge ? tips_dip : tips_sip));
	init_deviceID();

	char alarm_time[20];
	memset(alarm_time, 0, 20);
  TurnTime(ipblock_data->alarm_time , alarm_time, 20);

  char judgetime[20];
	memset(judgetime, 0, 20);
  memcpy(judgetime, alarm_time, strlen(alarm_time)-JUDGE_TIME_ONEMIN);
#if 0
	//by --- yong 2013.04.05 PM 18:31
	printf("(1)ipblock_data->alarm_time:%d\n",ipblock_data->alarm_time);
	//printf("(2)ipblock_data->alarm_time:%s\n",ipblock_data->alarm_time);
	sprintf(sql, "insert into tips_Alarm(Sip,Dip,Dmac,Alarm_time,\
					  Type,Risk,Scc_flag,Scc,Descry,Alarm_iid,Device_id)\
					  values('%s','%s','%s','%d','%s',\
								'%c','%c','%s','%s','%u','%s')",
								(judge ? dststr : srcstr),
								(judge ? srcstr : dststr),
								Dmac,
								ipblock_data->alarm_time,
								ipblock_data->type,
							 	ipblock_data->risk, 
								scc_flag,
								scc,
								ipblock_data->descry,
								ipblock_data->ip_black_id,
								deviceID);

#endif

	//printf("wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww\n");
	//printf("ipblock_data->risk=%c\n",ipblock_data->risk);
#if 1
	sprintf(sql, "insert into tips_Alarm(Trojan_id,Sip,Dip,Dmac,Alarm_time,\
					  Type,Risk,Scc_flag,Scc,Descry,Alarm_iid,Device_id, Alarm_judgetime,flag)\
					  values('0','%s','%s','%s','%s','%s',\
								'%c','%c','%s','%s','%u','%s','%s','0')",
								(judge ? dststr : srcstr),
								(judge ? srcstr : dststr),
								Dmac,
								alarm_time,
								ipblock_data->type,
							 	ipblock_data->risk, 
								scc_flag,
								scc,
								ipblock_data->descry,
								ipblock_data->ip_black_id,
								deviceID,
								judgetime);
#endif
	//printf("sqlwwwwww===%s\n",sql);
	return 0;
}

int CInit_sql::sql_dnsblack_info(struct dnsblack_info* dnsblock_data,
										  char *sql)
{
	char srcstr[IP_STR_LEN];
	char dststr[IP_STR_LEN];
	char deshash[100];
	memset(deshash, 0, 100);
	memset(srcstr, 0, IP_STR_LEN);
	memset(dststr, 0, IP_STR_LEN);
	inet_ntop(AF_INET, &(dnsblock_data->dip), dststr, IP_STR_LEN);
	inet_ntop(AF_INET, &(dnsblock_data->sip), srcstr, IP_STR_LEN);
//	printf("sip=%u:%s\n",dnsblock_data->sip,srcstr);
	//printf("dip=%u:%s\n",dnsblock_data->dip,dststr);
	uint16_t sport = ntohs(dnsblock_data->sport);
	uint16_t dport = ntohs(dnsblock_data->dport);

	uint32_t tips_dip = ntohl(dnsblock_data->dip);
	uint32_t tips_sip = ntohl(dnsblock_data->sip);


  if(judge_ip(tips_dip, 0, sport))
		return 1;
  if(judge_ip(tips_sip, 0, dport))
		return 1;
	//printf("tips_sip=%u\n",tips_dip);
	//printf("tips_dip=%u\n",tips_sip);

	bool judge=ipmarkoff(&(dnsblock_data->sip));
//	printf("real_ip=%u\n",(judge ? tips_sip : tips_dip));

	char alarm_time[20];
	memset(alarm_time, 0, 20);
  TurnTime(dnsblock_data->alert_time , alarm_time, 20);

  char judgetime[20];
	memset(judgetime, 0, 20);
  memcpy(judgetime, alarm_time, strlen(alarm_time)-JUDGE_TIME_ONEMIN);

	init_Dmac((judge ? dnsblock_data->sip : dnsblock_data->dip));
	init_scc((judge ? tips_dip : tips_sip));
	//init_scc(1052288259);
	init_deviceID();
	/*init_dnshash(dnsblock_data->dnshash, deshash);
	sprintf(sql, "insert into tips_Alert(Dip,Dmac,Sip,Alarm_time,Scc_flag,\
					  Sport,Protocol,Descry,Dnshash,Scc,Extra_text,Dport,\
					  Device_id,Alert_iid) values('%s','%s','%s','%s','%c','%hu','%hhu',\
											  '%s','%s','%s','%s','%hu','%s','%u');",
												(judge ? srcstr : dststr),
												Dmac,
												(judge ? dststr : srcstr),
												alarm_time,
												scc_flag,
												sport,
												dnsblock_data->protocol,
												dnsblock_data->descry,
												deshash,
												scc, 
												dnsblock_data->extra_text, 
												dport,
												deviceID,
												dnsblock_data->dns_id);*/
	sprintf(sql, "insert into tips_Alarm(Trojan_id,Sip,Dip,Dmac,Alarm_time,\
					  Type,Risk,Scc_flag,Scc,Descry,Alarm_iid,Device_id, Alarm_judgetime,flag)\
					  values('0','%s','%s','%s','%s','%s',\
								'%c','%c','%s','%s','%u','%s','%s','0')",
								(judge ? dststr : srcstr),
								(judge ? srcstr : dststr),
								Dmac,
								alarm_time,
								(char *)"80",
							 	dnsblock_data->risk, 
								scc_flag,
								scc,
								dnsblock_data->descry,
								dnsblock_data->dns_id,
								deviceID,
								judgetime);
	fprintf(stdout, "sql ========== %s\n", sql);
	return 0;
}

int CInit_sql::init_Dmac(uint32_t ip)
{
	memset(Dmac, 0, 20);
	get_mac.ip_change_mac(ip, Dmac);
	return 0;
}

int CInit_sql::init_deviceID()
{
	memset(deviceID, 0, 11);
	strcpy(deviceID,LOCALDEVID);
	return 0;
}

int CInit_sql::init_scc(uint32_t ip)
{
#if 1
	if(0 == ip)
	{
		scc_flag = '0';
		strcpy(scc, "!");
		return 0;
	}
#endif
	map<ipflag, ipscc>::iterator mit;
	ipflag sip;

	memset(scc, 0, 162);
	scc_flag = '0';

	sip.startip = ip;
	sip.endip	= ip;

	mit = ipmap->find(sip);
	if(mit == ipmap->end()){
		// printf("not find the ip\n");
		return 1;
	}
	scc_flag = mit->second.flag;
//	sprintf(scc, "%s,%s",mit->second.region,mit->second.territory);
	sprintf(scc, "%s",mit->second.territory);
	return 0;
}

int CInit_sql::init_dnshash(char *sdnshash, char *des)
{
	des=sdnshash;
	//sprintf(des,"%x-%x-%x-%x-%x-%x-%x-%x",sdnshash[0],sdnshash[1],sdnshash[2],sdnshash[3],sdnshash[4],sdnshash[5],sdnshash[6],sdnshash[7]);

	return 0;
}

void CInit_sql::TurnTime(const time_t clock, char *tips_clock, int clocksize)
{
	struct tm *tm;
	char *format=(char *)"%Y-%m-%d %H:%M:%S";
	tm = localtime(&clock);
	strftime(tips_clock,  clocksize, format,tm);
}
void CInit_sql::initmap(map<ipflag, ipscc> *smap)
{
	//ipmap = smap;
	
	smap = smap;		//hyj 保留参数
}

/*1传进来的是网络字节序有端口号,0是主机字节序有端口号，2是主机字节序无端口号，3是网络字节序无端口号，返回1在白名单内*/
int CInit_sql::judge_ip(uint32_t sip, int flag, u_short port)
{
	uint32_t ip = sip;
	if((flag == 0) || (flag == 3))
		ip = ntohl(sip);
	list<struct ip_rules>::iterator it;
#if 0
	for(it = global_iplist.begin(); it != global_iplist.end(); it++){
//		printf("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n");
//		printf("start=%u,endip=%u,sourceip=%u\n",it->startip, it->endip, ip);
		if((ip >= it->startip) && (ip <= it->endip) && (((flag == 2) || (flag == 3)) || (it->port == 0) || (it->port == port)))
			return 1;
	}
#endif
//	printf("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\n");
	port = port;		//hyj 保留参数
	return 0;
}

#endif
