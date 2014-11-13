#include <iostream>
#include "ipblackchecker.h"
/*
extern "C"
{
#include "init.h"
#include "client.h"
}
*/
using namespace std;
extern CCacheclient cacheclient;
extern pthread_mutex_t id_mutex;
int DEV_ID_IP  = 123456;
int IP_BLACK_NUM = 0;
bool libnet_init_flag = true;
//libnet_t * ppacket;


#define IP_LIST_LEN 4096
#define ISWHITE     0
#define ISBLACK     1
#define ISANKNOW    2
#define TRUE        0
#define FALSE       -1

static struct ip_rules white_ip[IP_LIST_LEN];
static int white_ip_len;
static struct ip_rules black_ip[IP_LIST_LEN];
static int black_ip_len;

static int change_dangerous(char src, char *dst)
{
	switch (src) {
					case '3':
									*dst = '2';
									break;
					case '5':
									*dst = '3';
									break;
					case '7':
									*dst = '4';
									break;
	}

	return 0;
}

int IPBlackChecker::Load_Rules(char* rule, int len)
{
	cout <<"============================================ ipblack load rules"<< endl;
	int i;
	struct ip_rules  temp_rule;

	bzero(&white_ip, sizeof(white_ip));
	white_ip_len = 0;
	bzero(&black_ip, sizeof(black_ip));
	black_ip_len = 0;

	for (i = 0; i < len && i < IP_LIST_LEN; i++) 
	{
		cout << "in load rules. start ip:" << ((struct ip_rules*)rule)[i].startip << endl;
		cout << " end ip:" << ((struct ip_rules*)rule)[i].endip << endl;
		cout << " port:" << ((struct ip_rules*)rule)[i].port << endl;
		cout << "type:" << ((struct ip_rules*)rule)[i].type << endl;
		cout << " cnt" << i << endl;

	    char startIP[IPSize] = {0};
	    inet_ntop(AF_INET,&(((struct ip_rules*)rule)[i].startip),startIP,IPSize);
	    printf("startIP:%s\n",startIP);
	    char endIP[IPSize] = {0};
	    inet_ntop(AF_INET,&(((struct ip_rules*)rule)[i].endip),endIP,IPSize);
	    printf("endIP:%s\n",startIP);

		temp_rule = ((struct ip_rules*)rule)[i];
		if (temp_rule.type == '1') { //WhiteIP_list
			white_ip[white_ip_len] = ((struct ip_rules*)rule)[i];
			change_dangerous(white_ip[i].dangerous, &(white_ip[i].dangerous));
			white_ip_len++;
			cout << "white" << endl;
		} 
		else 
		{
			black_ip[black_ip_len] = ((struct ip_rules*)rule)[i];
			change_dangerous(black_ip[i].dangerous, &(black_ip[i].dangerous));
			black_ip_len++;
			cout << "black" << endl;
		}
	}
	//sleep(4);
	return 0;
}

//by---yong 20130405PM 2:00
bool IPBlackChecker::CmpIP(map<u_short,struct ip_rules>::iterator it,map<u_short,struct ip_rules> *Xmap,uint32_t *dstip,uint32_t *srcip)
{
#if 0
				//cout<<"111111111111"<<endl;
				//cout<<"ntohl(*startIp):"<<(it->second.startip)<<endl;
				//cout<<"ntohl(*ENDIp):"<<(it->second.endip)<<endl;
				//cout<<"ntohl(*srcip):"<<ntohl(*srcip)<<endl;
				char buffer[1024] = {0};
				inet_ntop(AF_INET,srcip,buffer,sizeof(buffer));
				//		cout<<"SrcIP:"<<buffer<<endl;
				//		cout<<"ntohl(*dstip):"<<ntohl(*dstip)<<endl;
				inet_ntop(AF_INET,dstip,buffer,sizeof(buffer));
				//	cout<<"DstIP:"<<buffer<<endl;
				//	cout<<"222222222222"<<endl;
#endif 

				it = Xmap->begin();
				while (Xmap->end() != it)
				{
								//if(((ntohl(*srcip) >= ntohl(it->second.startip)) && (ntohl(*srcip) <= ntohl(it->second.endip)))										|| ((ntohl(*dstip) >= ntohl(it->second.startip)) && (ntohl(*dstip) <= ntohl(it->second.endip))))
								if(((ntohl(*srcip) >= (it->second.startip)) && (ntohl(*srcip) <= (it->second.endip)))                                        || ((ntohl(*dstip) >= (it->second.startip)) && (ntohl(*dstip) <= (it->second.endip))))
								{

												return true;
								}
								it ++;
				}
				return false;
}

static int iswhite(struct net_element *link)
{
	int      i;
	uint32_t sip;
	uint32_t dip;
	uint16_t sport;
	uint16_t dport;

	sip   = ntohl(link->srcip);
	dip   = ntohl(link->dstip);
	sport = ntohs(link->srcp);
	dport = ntohs(link->dstp);
    
	for(i = 0; i < white_ip_len; i++){
		if(sip >= white_ip[i].startip && sip <= white_ip[i].endip){
			if((sport == white_ip[i].port) || (white_ip[i].port == 0)){
				return ISWHITE;
			}
		}

		if(dip >= white_ip[i].startip && dip <= white_ip[i].endip){
			if((dport == white_ip[i].port) || (white_ip[i].port == 0)){
				return ISWHITE;
			}
		}
	}

	return ISANKNOW;
}

static int isblack(uint32_t ip, uint16_t port, char *risk)
{
	int i;

	//cout << "in isblack ip " << ip << endl;
	for (i = 0; i < black_ip_len; i++) {
		// cout << "i:" << i << endl;
		// cout << "black_ip_len:" << black_ip_len << endl;
		// cout << "startip: " << black_ip[i].startip << endl;
		// cout << "endip: " << black_ip[i].endip << endl;

		if ((ip >= black_ip[i].startip) && (ip <= black_ip[i].endip)) {
			// cout << "ip is ok" << endl;
			if ((port == black_ip[i].port) || (black_ip[i].port == 0)) {
				*risk = black_ip[i].dangerous;

				char zzzIP[IPSize] = {0};
				u_int32_t dkf = htonl(ip);
				inet_ntop(AF_INET,&(dkf),zzzIP,IPSize);
				printf("ooooooooIP:%s\n",zzzIP);
				//sleep(1);
				//		cout << "in isblack. startip:" << black_ip[i].startip << " endip:" << black_ip[i].endip << endl;
				//			cout << "ip:" << ip << endl;
				return ISBLACK;
			}
		}
	}

	return ISANKNOW;
}
#if 0
static int inlist(struct ip_rules *list, int list_len, struct net_element *link)
{
    int      i;
    uint32_t sip;
    uint32_t dip;
    uint16_t sport;
    uint16_t dport;

    sip   = ntohl(link->srcip);
    dip   = ntohl(link->dstip);
    sport = ntohs(link->srcp);
    dport = ntohs(link->dstp);

    for (i = 0; i <= list_len; i++) {
        if (sip >= list[i].startip && sip <= list[i].endip) {
            if ((sport == list[i].port) || (list[i].port == 0)) {
                    return TRUE;
            }
        }

        if (dip >= list[i].startip && dip <= list[i].endip) {
            if ((dport == list[i].port) || (list[i].port == 0)) {
               return TRUE;
            }
        }
    }

    return FALSE;
}
#endif

int IPBlackChecker::writemem(uint32_t sip, uint32_t dip, uint16_t sport, uint16_t dport,u_char srcmac[6],u_int8_t protocol,char risk)
{
    /*liuqiang code resolve waring*/
    if(0 == risk)
    {
        printf("OK");
    }
	
	cache_alert_t ip_alert;
	memset(&ip_alert,0,sizeof(ip_alert));
	INIT_ALERT_HEAD(&ip_alert);
	/*alert_id*/
	u_int32_t tmpint = 0;
	if(!ID(&tmpint))
	{
					return false;	
	}
	itoa(tmpint,ip_alert.alert_id);
	printf("XXXXXXXXXXXXXXX:<%s>\n",ip_alert.alert_id);
	/*plugin_id*/
	ip_alert.plugin_id = IPLinkPlugin_id;
	/*risk*/
#if 0
	map<u_int32_t,u_int8_t>::iterator l_it;; 
	u_int32_t hash=0;
	GreateHash(DomainName,NULL,&hash);
	l_it=Dgrous.find(hash);
	if(l_it != Dgrous.end())
	{
		ip_alert.risk = (l_it->second);
	}
	else
	{
		//return false;	
	}
#endif
	memcpy(ip_alert.risk,"12",3);
	/*trojan_id,非木马类告警可填写0*/			
	memcpy(ip_alert.trojan_id,"11111",6);
	/*alarm_time*/
	time_t mytime = 0;
	time(&mytime);
	struct tm *tm_p = localtime(&mytime);
	sprintf(ip_alert.alarm_time,"%d-%d-%d %d:%d:%d",(1900+tm_p->tm_year),(1+tm_p->tm_mon),(tm_p->tm_mday),(tm_p->tm_hour),(tm_p->tm_min),(tm_p->tm_sec));
	printf("alarm_time:%s\n",ip_alert.alarm_time);
	/*sip*/
	inet_ntop(AF_INET,&(sip),(ip_alert.sip),sizeof(ip_alert.sip));
	cout<<"ip_alert.sip:"<<ip_alert.sip<<endl;

	/*dip 初始值 为0 如果在Clear之前抓到Dns服务器返回的应答包 则此初始值作废*/
	inet_ntop(AF_INET,&(dip),(ip_alert.dip),sizeof(ip_alert.dip));
	cout<<"ip_alert.dip:"<<ip_alert.dip<<endl;

	/*sport dport*/
	ip_alert.sport = ntohs(sport);
	ip_alert.dport = ntohs(dport);

	/*dmac 内部主机mac*/
	snprintf(ip_alert.dmac, sizeof(ip_alert.dmac), "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",srcmac[0], srcmac[1],srcmac[2], srcmac[3],srcmac[4], srcmac[5]);			
	cout<<"mac:"<<ip_alert.dmac<<endl;
	/*protocol*/
	ip_alert.protocol = protocol;
	/*descry*/
	//snprintf(ip_alert.descry,sizeof(ip_alert.descry),"%s",DomainName);
	snprintf(ip_alert.descry, sizeof(ip_alert.descry), "ip黑名单报警:%s->%s\n",ip_alert.sip,ip_alert.dip);
	cout<<"7755:"<<ip_alert.descry<<endl;

    char geography[30] = {0};
    char flag;
    getAddr_ergodic(htonl(sip), geography, &flag);
    strcpy(ip_alert.scc, geography);
    ip_alert.scc_flag = flag;

	cacheclient.Send_Msg((char *)&ip_alert, sizeof(cache_alert_t));	


	return 0;
}

int IPBlackChecker::Start_Check(struct ip *packet, bool type, struct net_element *e)
{
	uint32_t sip;
	uint32_t dip;
	uint16_t sport;
	uint16_t dport;
	char risk;

    if(NULL == packet && 1 == type)
    {
        printf("**\n");
    }

	sip=ntohl(e->srcip);
	dip=ntohl(e->dstip);
	sport=ntohs(e->srcp);
	dport=ntohs(e->dstp);

	/*FIXME TEST*/
	sip =  1348961846;

	if(iswhite(e) == ISWHITE)
	{
		printf(">>>IP_iswhite");
		return 0;
	}

	if(isblack(sip, sport, &risk) == ISBLACK){

		writemem(e->srcip, e->dstip, e->srcp, e->dstp,e->srcmac, e->protocol,risk);
		return 0;
	}
	if(isblack(dip, dport, &risk) == ISBLACK)
	{
		writemem(e->srcip, e->dstip, e->srcp, e->dstp, e->srcmac,e->protocol,risk);
		return 0;
	}
	return 0;
}


int IPBlackChecker::ReLoad_Rules(char* rule,int len)
{
    if(NULL == rule || 0 == len)
    {
        return 0;
    }
    //cout<<"ipblackchecker reload_rules"<<endl;
    return 0;
}

#if 0
int IPBlackChecker::Black_Check(struct net_element* e,struct ip *addr)
{
	map<u_short,struct ip_rules>::iterator srcit;
	map<u_short,struct ip_rules>::iterator dstit;
	srcit = blacklist.find(ntohs(e->srcp));
	dstit = blacklist.find(ntohs(e->dstp));
	if(srcit != blacklist.end())
	{
		if(CmpIP(srcit,&blacklist,&(e->dstip),&(e->srcip)))
		{
			//StopLink(addr,e);	//阻断
			WtStructMem(e,srcit->first);
			//						WtFileMem(addr);
			return 0;
		}
	}
	else if(dstit != blacklist.end())
	{
		if(CmpIP(dstit,&blacklist,&(e->dstip),&(e->srcip)))
		{
			//StopLink(addr,e);	//阻断
			WtStructMem(e,dstit->first);
			//					WtFileMem(addr);
			return 0;
		}
	}
	return 1; 
}
#endif

#if 0
bool IPBlackChecker::LibnetTcpSend(u_int32_t SRCIP,u_int32_t DSTIP,u_int16_t SRCPORT,u_int16_t DSTPORT,struct ip *addr,libnet_t * ppacket)
{
	libnet_clear_packet(ppacket);	
	u_int32_t seqnum = ((struct tcphdr *)((char *)addr + IP_HL(addr)))->seq;
	u_int32_t acknum = ((struct tcphdr *)((char *)addr + IP_HL(addr)))->ack_seq;

	//cout<<"IM HERE------------------"<<endl;
	//cout<<"-------:"<<ntohl(SRCIP)<<endl;
	//cout<<"-------:"<<ntohl(DSTIP)<<endl;
	//cout<<"++:::::::::::::::"<<ntohs(SRCPORT)<<endl;
	//	cout<<"XX@@:"<<SRCPORT<<endl;
	//cout<<"++:::::::::::::::"<<ntohs(DSTPORT)<<endl;
	//	cout<<"XX@@:"<<DSTPORT<<endl;
	//cout<<"@@L:"<<e->srcp<<"|"<<e->dstp<<"|seq:"<<seqnum<<"|ack:"<<acknum<<"|"<<TH_RST<<"|"<<32767<<"|"<<LIBNET_TCP_H<<"|"<<ppacket<<endl;
	/*cout<<"send tcp block !!"<<endl;	
	if(libnet_build_tcp(SRCPORT,DSTPORT,seqnum,acknum,TH_RST,32767,0,0,LIBNET_TCP_H,NULL,0,ppacket,0) == -1) 
	{
		//	printf("libnet build tcp error\n");
		return false;
		}

		if(libnet_build_ipv4(LIBNET_IPV4_H + LIBNET_TCP_H,0,242,0,0,IPPROTO_TCP,0,SRCIP,DSTIP,NULL,0,ppacket,0) == -1) 
		{
		//	printf("libnet build ipv4 error\n");
		return false;
		}

		int ret = libnet_write(ppacket);
		if(ret == -1) 
		{
		//	printf("libnet write error errmsg: %s ret: %d\n", libnet_geterror(ppacket), ret);
		return false; 
		}
		*/
		return true;
}
#endif

#if 0
void IPBlackChecker::StopLink(struct ip *addr,struct net_element *e)
{
	static int flag = 0;
	char errbuf[LIBNET_ERRBUF_SIZE] = {0};

	if (flag == 0) {
		flag = 1;
		/*if(libnet_init_flag) {
			ppacket = libnet_init(LIBNET_RAW4,"eth1",errbuf);
			libnet_init_flag = false;
			}
			if(!ppacket) {
			return;
			}
			*/    
	}

	if((addr->ip_p) == IPPROTO_TCP)
	{

					//							LibnetTcpSend(e->srcip,e->dstip,ntohs(e->srcp),ntohs(e->dstp),addr,ppacket);
					//						LibnetTcpSend(e->dstip,e->srcip,ntohs(e->dstp),ntohs(e->srcp),addr,ppacket);

					//libnet_destroy(ppacket);
	}
	else if((addr->ip_p) == IPPROTO_UDP)
	{
					/*					int count = (IP_HL(addr) + 8);
											u_int8_t *UIPaddr = 0;
											UIPaddr = (u_int8_t *)malloc(count);	
											memcpy(UIPaddr,addr,count);
					//LibnetIcmpSend(e->srcip,e->dstip,UIPaddr,ppacket);	
					LibnetIcmpSend(e->dstip,e->srcip,UIPaddr,ppacket);	
					//libnet_destroy(ppacket);
					free(UIPaddr);
					UIPaddr = 0;
					*/
	}
	return ;
}
bool IPBlackChecker::LibnetIcmpSend(u_int32_t SRCIP,u_int32_t DSTIP,u_int8_t *addr,libnet_t *ppacket)
{
				//libnet_ptag_t
				//libnet_build_icmpv4_unreach(uint8_t type, uint8_t code, uint16_t sum,
				//const uint8_t* payload, uint32_t payload_s, libnet_t *l, libnet_ptag_t ptag);
				//cout<<"-------:"<<ntohl(SRCIP)<<endl;
				/*				//cout<<"-------:"<<ntohl(DSTIP)<<endl;
									if(-1 == libnet_build_icmpv4_unreach('3','3',0,addr,0,ppacket,0))
									{
				//	printf("libnet_build_icmpv4_unreach is error~!~!\n");	
				return false;
				}
				if(-1 == libnet_build_ipv4(LIBNET_IPV4_H + LIBNET_TCP_H,0,242,0,0,IPPROTO_TCP,0,SRCIP,DSTIP,NULL,0,ppacket,0)) 
				{
				//	printf("libnet_build_ipv4 is error~!\n");	
				return false;
				}
				int ret = libnet_write(ppacket);
				if(ret == -1) 
				{
				//	printf("libnet write error errmsg: %s ret: %d\n", libnet_geterror(ppacket), ret);
				return false; 
				}
				*/			return true;
}
#endif
#if 0
//by --- yong 2013.04.02PM 2:30
void IPBlackChecker::WtStructMem(struct net_element *e,u_int8_t port)
{
				char srcIP[IPSize] = {0};
				char dstIP[IPSize] = {0};
				inet_ntop(AF_INET,&(e->srcip),srcIP,IPSize);
				inet_ntop(AF_INET,&(e->dstip),dstIP,IPSize);

				cache_alert_t ipinfo;
				memset(&ipinfo,0,sizeof(ipinfo));
				ipinfo.l_type = type; //int 
				if(!ID(&(ipinfo.ip_black_id))) //u_int32_t
				{
								return ;	
				}
				ipinfo.sip = e->srcip; //u_int32_t

				ipinfo.dip = e->dstip;//u_int32_t


				//	ipinfo.risk = '3';//char
				map<u_int8_t,u_int8_t>::iterator l_it;; 
				l_it=Dgrous.find(port);
				if(l_it != Dgrous.end())
				{
								ipinfo.risk = (l_it->second);
								//		cout<<"ipinfo.risk=========="<<(48+ipinfo.risk)<<endl;
								//printf("ipinfo.risk===========%c\n",(char)(ipinfo.risk));
				}
				else
				{
								//	cout<<"not_ipinfo_risk........................."<<endl;
								return ;	
				}

				ipinfo.alarm_time = time(0); //time_t

				memcpy(ipinfo.type,"80",strlen("80"));// char*

				memset(ipinfo.descry,0,sizeof(ipinfo.descry));
				sprintf(ipinfo.descry,"%s->%s %d\0",srcIP,dstIP,ipinfo.alarm_time);

				cmempool->Mem_Write((void*)&ipinfo, sizeof(cache_alert_t));
				return ;
}
#if 0
void IPBlackChecker::WtFileMem(struct ip* addr)
{
				int Currentime = time((time_t *)NULL);
				char Name[NameSize] = {0};
				sprintf(Name,"%s/%d_%d_%s_%d_%d_%d.pcap\0",IPFileDIR,DataPacketType,Currentime,LOCALDEVID,ID_NUM,TorjanID,AlertType);	
				int iplen = ntohs(addr->ip_len);
				int ContentLen = (PcapPacketHeadSize + MacFrameLen + iplen);
				char *PcapMemBuff =  0;
				PcapMemBuff = (char *)malloc(ContentLen);
				wtpcap->WtPcapDataH(PcapMemBuff,iplen + MacFrameLen,iplen + MacFrameLen);

				struct macframes MF;
				memset(&MF,0,sizeof(MF));
				MF.type = MacFrameProtocolType;
				wtpcap->WtPcapMacFrame(PcapMemBuff,&MF);

				wtpcap->WtPcapData(PcapMemBuff,addr,iplen);	

				//filemem->Mem_Write2(Name,PcapMemBuff,ContentLen);	

				free(PcapMemBuff);
				PcapMemBuff = 0;

				return ;
}
#endif
#endif

void IPBlackChecker::Print_Error(int fd,char *Api)
{
				if(Api)
				{
								perror("Api--");	
				}
				close(fd);
				return ;
}

bool IPBlackChecker::ID(u_int32_t *id)
{
				pthread_mutex_lock(&id_mutex);
				if(ID_NUM >= TopLimit)
				{
								ID_NUM = 1;
				}
				else
				{
								++ID_NUM;
				}
				*id = ID_NUM;
				pthread_mutex_unlock(&id_mutex);
				return true;
}

