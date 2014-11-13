#include "capturepacket.h"

extern "C"
{
#include "init.h"
#include "client.h"
}
void ip_callback(const struct pcap_pkthdr *hdr,const u_char *data);

void mycallback(u_char *par, const struct pcap_pkthdr *hdr, const u_char *data)
{
    u_char* dest = NULL;

    par = NULL;
    if(NULL == hdr || NULL == data) {
        dzlog_info("the packet is NULL!!!");
        return;
    }
    
    const struct ethhdr *ethernet = (struct ethhdr *) data;
    if(0x8100 == ntohs(ethernet->h_proto)) {
		dzlog_info("the packet is vlan type");
        u_char* src = (u_char *)data;
		dest =(u_char *)(data+4);
        memmove(dest, src, 12);
    }
    else {
        dest = (u_char *)data; 
    }
    
	ip_callback(hdr, dest);/*ip callback*/
	trojan_process(hdr, dest);/*传给木马检测模块*/
}

void StartCapture::Start()
{
	char Eth[256] = {0};
	char error[256] = {0};
	if(GetEth(Eth)) {
		pcap_t* handle = NULL;
		handle = pcap_open_live(Eth,1600,1,0,error);
		pcap_loop(handle,-1,mycallback,NULL);
	}
	return;
}


/*FIXME 
 * 以下函数用于解析判断tcp\udp包，方便直接将tcp\udp包送入相对应模块
 * ip/tcp/udp->list,ip/tcp/udp->mutex以及checker在checkobj.cpp中定义
 */
extern list<struct check_struct> iplist;
extern list<struct check_struct> tcplist;
extern list<struct check_struct> udplist;
extern pthread_mutex_t ipmutex,tcpmutex,udpmutex;

extern ITipsChecker* checker[CLASSCOUNT];


#define AIP  10
#define BIP  172
#define BIPa 16
#define BIPb 31
#define CIP  192
#define CIPa 168
struct net_element element;

//void Get_Netelement(const struct pcap_pkthdr *hdr,const u_char *data)
void Get_Netelement(const u_char* hdr)
{
	memset(&element,0,sizeof(element));

	//获取mac地址
	struct macframes* macget = (struct macframes*)hdr;
	memcpy(element.srcmac, macget->srcmac, 6);
	memcpy(element.dstmac,macget->dstmac, 6);

	//获取ip
	struct ip* packet  = (struct ip *)(hdr+14);
	unsigned int head = packet->ip_hl * 4;
	element.srcip = packet->ip_src.s_addr;
	element.dstip = packet->ip_dst.s_addr;
	char* tmp = (char*)packet;
	tmp = tmp + head;
	//IPPROTO_UDP == 17 IPPROTO_IPIP = 4
	if((element.protocol = packet->ip_p) == 6)
	{
		struct tcphdr* mytcp = (struct tcphdr*)tmp;
		element.srcp = mytcp->source;
		element.dstp = mytcp->dest;
	}
	else if((element.protocol = packet->ip_p) == 17)
	{
		struct udphdr* myudp = (struct udphdr*)tmp;
		element.srcp = myudp->source;
		element.dstp = myudp->dest;
	}
}

/*FIXME 
 * 判断内外网函数 
 * true:内网
 * false:外网
 */
bool ipmarkoff(in_addr_t *addr)
{
	u_int dst = *addr;
	if(((BIP == (dst & 0xFF)) && ((((dst & 0xFF00) >> 8) >= BIPa) && (((dst & 0xFF00) >> 8) <= BIPb))) || (AIP == (dst&0xFF)) || ((CIP == (dst&0xFF)) && (CIPa == ((dst&0xFF00) >> 8)))) {
		return true;
	}
	return false;
}

void ip_callback(const struct pcap_pkthdr* hdr,const u_char* data)
{
	//获取ip\mac\port
	//Get_Netelement(hdr,data);
	if(NULL == hdr || NULL == data) {
		dzlog_info("the packet head or data is NULL!!!");
		return;
	}
	Get_Netelement(data);

    //截断成ip包,发送给ip链表中的子模块	
	pthread_mutex_lock(&ipmutex);
	list<struct check_struct>::iterator beg=iplist.begin();
	while(beg!=iplist.end()) {
		beg->checker->Start_Check((struct ip*)(data + 14),ipmarkoff(&(element.srcip)),&element);
		beg++;
	}
	pthread_mutex_unlock(&ipmutex);
	return;
}

