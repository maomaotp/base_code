#include <netinet/if_ether.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include "init.h"
#include "UnTrojan.h"
#include "juageIP.h"


unkown_trojan **U_trojan;
struct cache_alert_t alert;

void createunkown()
{
  U_trojan = (unkown_trojan **)calloc(UNSIZE,sizeof(struct unkown_trojan));
  if(NULL == U_trojan)
  {
	  dzlog_info("createUnkown():calloc fail!!!");
	  exit(-1);
  }
  return;
}

void clearunhash()
{
    int i=0;
    struct unkown_trojan *temp=NULL;
    time_t now=0;
	struct unkown_trojan *pos=NULL;

	for(;i<UNSIZE;i++)
	{
		temp=U_trojan[i]; 
		time(&now);
	  
		if(NULL != temp && isovertime(now,temp,&pos))
		{
			if(NULL == pos->prev && NULL != pos->next)
			{
				pos->next->prev=NULL;
				U_trojan[i]=pos->next;
				temp=pos->next;
			}
			else if(NULL != pos->next && NULL != pos->prev){
				pos->next->prev=pos->prev;
				pos->prev->next=pos->next;
			}
			else if(NULL == pos->next && NULL != pos->prev){
				pos->prev->next=NULL;
			}
			else if(NULL == pos->next && NULL == pos->prev){
				U_trojan[i]=NULL;
			}
			clearun(pos);
			pos=NULL;
		}		
	}
    return;
}

void clearun(struct unkown_trojan *it)
{
	if(NULL != it->message)
	{
		free(it->message);
		it->message=NULL;
	}
	if(NULL != it){
		free(it);
	}
	return;
}

bool isovertime(time_t now,struct unkown_trojan *begin,struct unkown_trojan **pos)
{
   struct unkown_trojan *temp=NULL;
   temp=begin;

	while(NULL != temp)
	{
		if(now - temp->last_time >= TIMEE)
		{
			*pos=temp;
			return true;
		}
	    temp=temp->next;
	}
    return false;
}

void enterunkprocess(const struct pcap_pkthdr *head, const u_char *packet)
{
    struct iphdr *ip=NULL;
    ip=(struct iphdr *)(packet+14);
   
    processippacket(ip,packet);

    if(NULL == head)
    {
        return;
    }
    return;
}

bool finduntrojan(const struct tcphdr *tcp,const struct iphdr *ip,bool *isnull,uint32_t *positiontemp,struct unkown_trojan **pt)
{
    uint32_t temp=0;
	uint32_t location=0;
	uint32_t position=0;
	uint32_t hashtemp=0;
	int result=0;
	unkown_trojan *pos=NULL;
    uint32_t  index=0;
	bool re=false;


	temp=(tcp->source)<<16|(tcp->dest);
	hashtemp=temp;
    position=jhash_3words(ip->saddr,ip->daddr,temp,0);
	position%=UNSIZE;
	memset((void*)&temp,0,sizeof(temp));
	temp=(tcp->dest)<<16|(tcp->source);
	location=jhash_3words(ip->daddr,ip->saddr,temp,0);
	location%=UNSIZE;
    //判断hash表，该条key对应的是否存在消息，存在则取出存放在pos，不存在re==false,result数组下标如果有值一定为1
	re = searchunstream(position, location, ip, hashtemp, &result, &pos, &index);
	if(result ==1)
	{
		*isnull=true;
	}

    if(!re)
    {
	    *positiontemp=position;
	}
	else
	{
		if(NULL == pos)
		{
			dzlog_info("finduntrojan():pos==NULL!!!");
			return false;
		}
		*pt=pos;
	}

	return re;
}

void packmd5c(char *data,unsigned char *value)
{
   MD5_CTX ctx;
   MD5_Init(&ctx);
   MD5_Update(&ctx,data,strlen(data));
   MD5_Final(value,&ctx);
}

void createmd5c(const struct iphdr *ip,uint32_t hashtemp,unsigned char *digest,uint32_t hashtep,unsigned char *summary)
{
    char str[128]={0};
    snprintf(str,sizeof(str),"%u%u%u",ip->saddr,ip->daddr,hashtemp);
    packmd5c(str,digest);

    if(summary)
    {
	    memset(str,0,sizeof(str));
        snprintf(str,sizeof(str),"%u%u%u",ip->daddr,ip->saddr,hashtep);
	    packmd5c(str,summary);
    }
}
bool searchunstream(uint32_t position, uint32_t location, const struct iphdr *ip, uint32_t hashtemp,int *result, unkown_trojan **pos,uint32_t *index)
{
	if( NULL == U_trojan[position] && NULL == U_trojan[location])
	{
		*result=1;
		return false;
	}

	unkown_trojan *temp=NULL;
    unsigned char mess[16]={0};

	createmd5(ip,hashtemp,mess,0,NULL);

	if(NULL != U_trojan[position])
	{
		temp=U_trojan[position];
		if(NULL==temp || NULL==temp->message)
		{
			dzlog_info("searchunstream():temp==NULL or temp->message==NULL!!!");
			return false;
		}
		if(memcmp(temp->message,mess,16) == 0 || memcmp(temp->message+16,mess,16) == 0)
		{
			*pos=temp;
			*index=position;
			return true;
	    }
        temp=temp->next;
	}

	if(NULL != U_trojan[location])
	{
		temp=U_trojan[location];
		if(NULL == temp || NULL == temp->message)
		{
			dzlog_info("searchunstream():temp==NULL or temp->message==NULL!!!");
			return false;
		}
		if(memcmp(temp->message,mess,16) == 0 || memcmp(temp->message+16,mess,16) == 0)
		{
			*pos=temp;
			*index=location;
			return true;
		}
		temp=temp->next;
	}

	return false;
}


void alarmutrojan(const struct iphdr *ip,const struct tcphdr *tcp)
{
    static uint32_t counter=1;
    if(0 == counter)
    {
       counter =1;
    }
    char order[128]={0};
    sprintf(order,"cat /usr/local/tipschecker/etc/deviceid.conf | cut -d'=' -f2");
    FILE *file=popen(order,"r");
    fread(alert.device_id,1,12,file);
    pclose(file);

    sprintf(alert.alert_id,"%u",counter);
    alert.head.type=ALERT;
    alert.plugin_id=1006;
	alert.scc_flag='0';
    strcpy(alert.risk,"45");
    char srcip[INET_ADDRSTRLEN] = { 0 };
    char destip[INET_ADDRSTRLEN] = { 0 };

    if(NULL == inet_ntop(AF_INET,&ip->saddr,srcip,sizeof(srcip)))
    {
        perror("inet_ntop");
        return;
    }
    if(NULL == inet_ntop(AF_INET,&ip->daddr,destip,sizeof(destip)))
    {
        perror("inet_ntop");
        return;
    }
  
    if(ipmarkoff((in_addr_t *)&ip->saddr))
    {
		strncpy(alert.sip,srcip,19);
		strncpy(alert.dip,destip,19);
    }
    else
    {
		strncpy(alert.sip,destip,19);
		strncpy(alert.dip,srcip,19);
    }
    time_t now=time(NULL);

    TurnTime(now,alert.alarm_time,sizeof(alert.alarm_time)); //格式化时间
    alert.sport=ntohs(tcp->source);
    alert.dport=ntohs(tcp->dest);
    alert.protocol=6;
    strcpy(alert.descry,"数据流量异常，通信异常，疑似木马");
   
    //Send_Msg((void*)&alert,sizeof(alert));
    memset(&alert,0,sizeof(cache_alert_t));
    counter++;
    return;
}

void TurnTime(const time_t clock,char *buffer,int size)
{
    struct tm *tm;
    char *format=(char *)"%Y-%m-%d %H:%M:%S";
    tm = localtime(&clock);
    strftime(buffer,size, format,tm);
    return;
}

void addnode(uint32_t pos,const struct iphdr *ip,const struct tcphdr *tcp)
{
	uint32_t  forwardwords=0;
	forwardwords=(tcp->source)<<16|(tcp->dest);

	uint32_t reservewors=0;
	reservewors=(tcp->dest)<<16|(tcp->source);

	unsigned char forwardmessage[16]={0};
	unsigned char reservemessage[16]={0};

    createmd5(ip,forwardwords,forwardmessage,reservewors,reservemessage);

    U_trojan[pos] = (unkown_trojan *)calloc(1,sizeof(unkown_trojan));
	if(NULL == U_trojan[pos])
	{
	    dzlog_info("addnode():calloc fail!!!");
		return;
	}
    U_trojan[pos]->message = (u_char*)calloc(1,MESSAGE_SIZE);
	if(NULL == U_trojan[pos]->message)
	{
		dzlog_info("addnode() calloc failed");
		free(U_trojan[pos]);
		return;
	}

	memcpy(U_trojan[pos]->message,forwardmessage,16);
	memcpy(U_trojan[pos]->message+16,reservemessage,16);
	
	U_trojan[pos]->last_time=time(NULL);
	U_trojan[pos]->index=pos;

	if(ipmarkoff((in_addr_t *)&ip->saddr))
	{
		U_trojan[pos]->upload=ntohs(ip->tot_len)-ip->ihl*4-20;
	}
	else
	{
		U_trojan[pos]->download=ntohs(ip->tot_len)-ip->ihl*4-20;
	}
	return;
}


struct unkown_trojan* processnode(uint32_t pos,const struct iphdr *ip,const struct tcphdr *tcp)
{
	struct unkown_trojan *temp=NULL;

	if(NULL==ip || NULL==tcp)
	{
		dzlog_info("func(processnode):ip==NULL or tcp==NULL!!!");
		return temp;
	}
	uint32_t  forwardwords=0;
	forwardwords=(tcp->source)<<16|(tcp->dest);
    
	uint32_t reservewors=0;
	reservewors=(tcp->dest)<<16|(tcp->source);
  
	unsigned char forwardmessage[16]={0};
	unsigned char reservemessage[16]={0};

    createmd5(ip,forwardwords,forwardmessage,reservewors,reservemessage);
    
	temp = (struct unkown_trojan *)calloc(1,sizeof(struct unkown_trojan));
	if(NULL == temp)
	{
	    dzlog_info("calloc failed");
		return temp;
	}
	temp->message = (u_char*)calloc(1,MESSAGE_SIZE);
	if(NULL == temp->message)
	{
		free(temp);
		return temp;
	}

	memcpy(temp->message,forwardmessage,16);
	memcpy(temp->message+16,reservemessage,16);

	temp->last_time=time(NULL);
	temp->index=pos;

	if(ipmarkoff((in_addr_t *)&ip->saddr))
	{
		temp->upload=ntohs(ip->tot_len)-ip->ihl*4-20;
	}
	else
	{
		temp->download=ntohs(ip->tot_len)-ip->ihl*4-20;
	}
	return temp;
}

void deleteun(struct unkown_trojan* it)
{
	if(it==NULL)
	{
		dzlog_info("unkown_trojan pointer it==NULL!!!");
		return;
	}
	if(NULL == it->prev && NULL == it->next)
	{
		free(it->message);
		it->message=NULL;
		U_trojan[it->index]=NULL;
	}
	else if(NULL == it->prev && NULL !=it->next)
	{
		it->next->prev=NULL;
		U_trojan[it->index]=it->next;
		free(it->message);
		it->message=NULL;
	}
	else if(NULL != it->prev && NULL == it->next)
	{
		it->prev->next=NULL;
		free(it->message);
		it->message=NULL;
	}
	else if(NULL != it->next && NULL != it->prev)
	{
		it->prev->next=it->next;
		it->next->prev=it->prev;
		free(it->message);
		it->message=NULL;
	}
	free(it);
}

void processtcppacket(const struct tcphdr *tcp,const struct iphdr *ip,const u_char *packet)
{ 
	bool isnull=false;
	uint32_t pos=0;
	struct unkown_trojan *p=NULL;
	char *data=NULL;

	if(NULL==ip || NULL==tcp || NULL==packet)
	{
		dzlog_info("packet is NULL!!!");
		return;
	}
	if(finduntrojan(tcp,ip,&isnull,&pos,&p))
	{
		if(tcp->fin || tcp->rst)
		{
			if(!p->report  && (p->upload > 3*p->download))
			{
				alarmutrojan(ip,tcp);
			}
			deleteun(p);
			p=NULL;
			return;
		}
		data=(char *)(packet+14+ip->ihl*4+20);

		if(ipmarkoff((in_addr_t *)&ip->saddr))
		{
			p->upload+=ntohs(ip->tot_len)-ip->ihl*4-20;
			p->last_time=time(NULL);
		}
		else
		{
			p->download+=ntohs(ip->tot_len)-ip->ihl*4-20;
			p->last_time=time(NULL);
		}

		char *str=NULL;
		str=data;

		if(p->ishttp==1 && (NULL ==strstr(str,"text/html") || NULL == strstr(str,"text/plain")))
		{
			alarmutrojan(ip,tcp);
			p->report=true;
		}
		else
		{
			p->ishttp=2;
		}
		if(p->upload > 3*p->download && !p->report){
			alarmutrojan(ip,tcp);
			p->report=true;
		}
	}
	else
	{
		if(0 == ntohs(ip->tot_len)-ip->ihl*4-20)
		{
		    dzlog_info("ntohs func failed");
			return;
		}
       
		data=(char *)(packet+14+ip->ihl*4+20);

		if(ipmarkoff((in_addr_t *)&(ip->saddr)) && ipmarkoff((in_addr_t *)&ip->daddr))
		{
			return;
		}

		if(isnull)
		{
			addnode(pos,ip,tcp);	    
			if(0 == memcmp(data,"POST",4) || 0 == memcmp(data,"GET",3))
			{
				U_trojan[pos]->ishttp=1;
			}
		}
		else
		{
			if(NULL == U_trojan[pos])
			{
				addnode(pos,ip,tcp);
				if(0==memcmp(data,"POST",4) || 0==memcmp(data,"GET",3))
				{
					U_trojan[pos]->ishttp=1;
				}
			}
			else
			{
				struct unkown_trojan *temp=NULL;
				struct unkown_trojan *thisnode=NULL;

				temp=U_trojan[pos];

				while(NULL != temp->next)
				{
					temp=temp->next;
				}

				thisnode=processnode(pos,ip,tcp);
				if(NULL == thisnode)
				{
					dzlog_info("thisnode calloc fail!!!");
					return;
                }
				temp->next=thisnode;
				thisnode->prev=temp;
				if(0 == memcmp(data,"POST",4) || 0 == memcmp(data,"GET",3))
				{
					thisnode->ishttp=1;
				}
			}
		} 
    }
    return;
}

void processippacket(const struct iphdr *ip,const u_char *data)
{
	struct tcphdr *tcp=NULL;
	tcp=(struct tcphdr *)(data+14+ip->ihl*4);

	if(NULL == ip)
	{
		dzlog_info("processippacket():ip==NULL!!!\n");
		return;
	}
	if(ip->protocol >IPPROTO_SCTP)
	{
	   alarmutrojan(ip,tcp);
	   return ;
	}

    processtcppacket(tcp,ip,data);
    return;
}

