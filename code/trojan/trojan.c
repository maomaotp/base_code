#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pcap.h>
#include <time.h>
#include <net/if.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <assert.h>

#include "decode.h"
#include "init.h"

#define ALARM       0

#define DEBUG

#define IP_LIST_LEN 4096
#define ISWHITE     0
#define ISBLACK     1
#define ISANKNOW    2
#define TRUE        0
#define FALSE       -1


struct ip_rules white_ip[IP_LIST_LEN];
int white_ip_len;
struct ip_rules black_ip[IP_LIST_LEN];
int black_ip_len;

char buf[20];
extern struct filerule *rule[800];

cache_alarm_t trojan;


static int detect_func(Packet *p)
{
	int i;
	uint32_t sip = p->iph->ip_src.s_addr;
	uint32_t dip = p->iph->ip_dst.s_addr;
    
    //white ip detection
	for(i = 0; i < white_ip_len; i++){
		if( ((sip >= white_ip[i].startip) && (sip <= white_ip[i].endip)) || ((dip >= white_ip[i].startip) && (dip <= white_ip[i].endip)) ){
#ifdef BUDEG
                printf(">>>>>Packet:src_ip==%u, dst_ip==%u, is white\n", sip, dip);
                printf(">>>>>strategy: white->startIP=%u, white->endIP=%u\n", white_ip[i].startip, white_ip[i].endip);
#endif
			if( white_ip[i].port == 0 ){
				return ISWHITE;
			}
            else if( (p->dp == white_ip[i].port) || (p->sp == white_ip[i].port) ){
#ifdef DEBUG
                printf("the strategy port == %d\n", white_ip[i].port);
#endif
				return ISWHITE;
			}
		}
	}

    //black ip detection
	for(i = 0; i < black_ip_len; i++){
		if( ((sip >= black_ip[i].startip) && (sip <= black_ip[i].endip)) || ((dip >= black_ip[i].startip) && (dip <= black_ip[i].endip)) ){
#ifdef DEBUG
                printf(">>>>>Packet:src_ip==%u, dst_ip==%u, is white\n", sip, dip);
                printf(">>>>>strategy: black->startIP=%u, black->enIP=%u\n", black_ip[i].startip, black_ip[i].endip);
#endif
			if( white_ip[i].port == 0 ){
				return ISBLACK;
			}
            else if( (p->dp == black_ip[i].port) || (p->sp == black_ip[i].port) ){
#ifdef DEBUG
                printf("the strategy port == %d\n", black_ip[i].port);
#endif
				return ISBLACK;
			}
		}
	}
	return ISANKNOW;
}

void alar(struct filerule *rule, Packet *p)
{
    static uint32_t counter=1;

    if( (rule== NULL) || (p == NULL) ) {
        printf("alar:the pointer is NULL\n");
        return ;
    }

    printf("rule->identifier==%s\n", rule->identifier);

    strncpy( trojan.trojan_id, rule->identifier, 5 );
    strcpy( trojan.sip, inet_ntoa(p->iph->ip_src) );
    strcpy( trojan.dip, inet_ntoa(p->iph->ip_dst) );

    time(&(trojan.alarm_time));
    strcpy(trojan.type, rule->type);
    //strncpy(trojan.dmac, save->dmac,18);
    trojan.risk= '2';
    trojan.head.type= ALARM;
    sprintf(trojan.alarm_id, "%u", counter++);
    strcpy(trojan.descry, rule->name);

    char file_name[128];
    char file_content[128];
    strcpy(file_name, "cat /usr/local/tipschecker/etc/deviceid.conf | cut -d'=' -f2");
    FILE *file=popen(file_name, "r");
    fread(file_content, 1, 128, file);
    pclose(file);
    file=NULL;
    file_content[strlen(file_content)-1] = '\0';
    strcpy(trojan.device_id, file_content);

    char geography[30] = {0};
    char flag;
    getAddr_ergodic(htonl(p->iph->ip_src.s_addr), geography, &flag);

    strcpy(trojan.scc, geography);
    trojan.scc_flag = flag;

    Send_Msg((char* )&trojan, sizeof(trojan));
    //writefile(save);

    printf(">>>*********trojan*************<<<\n ...trojan_descry:%s\n ...strojan_id:%s\n ...trojan_name:%s\n ...trojan_time:%ld\n ...trojan_type:%s\n ...srcip:%s\n ...destip:%s\n ...geography:%s\n >>>send to cache success<<<\n\n", trojan.descry, trojan.trojan_id, trojan.descry, trojan.alarm_time, trojan.type, trojan.sip, trojan.dip, geography);
}


void DecodeTCP(const u_int8_t * pkt, const u_int32_t len, Packet * p)
{
    u_int32_t hlen;
    int i = 1;

    if(len < 20) {
        p->tcph = NULL;
        return;
    }

    /* lay TCP on top of the data cause there is enough of it! */
    p->tcph = (TCPHdr *) pkt;

    /* multiply the payload offset value by 4 */
    hlen = TCP_OFFSET(p->tcph) << 2;

    if(hlen < 20) {
        p->tcph = NULL;
        return;
    }

    if(hlen > len) {
        p->tcph = NULL;
        return;
    }

    /* stuff more data into the printout data struct */
    p->sp = ntohs(p->tcph->th_sport);
    p->dp = ntohs(p->tcph->th_dport);

    /* if options are present, decode them */
    p->tcp_options_len = hlen - 20;

    /* set the data pointer and size */
    p->data = (u_int8_t *) (pkt + hlen);

    if(hlen < len) {
        p->dsize = (u_short)(len - hlen);
    }
    else {
        p->dsize = 0;
    }

    if(p->dsize == 0) {
        return;
    }
    //black and white IP decetion
    int detect_r = detect_func(p);
    if(detect_r == ISWHITE){
        return;
    }
    else if(detect_r == ISBLACK) {
        alar(rule[0], p);
        return;
    }

    while(rule[i]) {
        if(0 == memcmp(p->data+rule[i]->begin, rule[i]->content, rule[i]->length/2)) {
            alar(rule[i], p);
            printf(">>>>>>rule[i] == %d<<<<<<<<<\n\n\n\n\n", rule[i]->begin);
            break;;
        }
        i++;
    }

#ifdef DEBUG_1
    printf(">>>TCP packet>>>\n");
    printf("src_ip:%u------%s\n", p->iph->ip_src.s_addr, inet_ntoa(p->iph->ip_src));
    printf("src_ip:%u------%s\n", p->iph->ip_dst.s_addr, inet_ntoa(p->iph->ip_dst));
    printf("p->sport:%u\n", p->sp);
    printf("p->dport:%u\n\n", p->dp);
    printf("data->len==%u   data:", p->dsize);
	/*
    for(i=0; i < p->dsize; i++){
        printf("%02x", p->data[i]);
    }
	*/
    printf("\n");
#endif
}


void DecodeIP(const u_int8_t* pkt, const u_int32_t len, Packet* p)
{
    u_int32_t hlen;
    u_int32_t ip_len; /* length from the start of the ip hdr to the pkt end */

    /* lay the IP struct */
    p->iph = (IPHdr *) pkt;

    /* do a little validation */
    if(len < IP_HEADER_LEN) //IP_HEADER_LEN == 20
    {
        p->iph = NULL;
        return;
    }

    if(IP_VER(p->iph) != 4)
    {
        p->iph = NULL;
        return;
    }

    /* set the IP datagram length */
    ip_len = ntohs(p->iph->ip_len);

    /* set the IP header length */
    hlen = IP_HLEN(p->iph) << 2;

    /* header length sanity check */
    if(hlen < IP_HEADER_LEN) {
        p->iph = NULL;
        return;
    }

    if (ip_len > len) {
        ip_len = len;
    }

    if(ip_len < hlen) {
        p->iph = NULL;
        return;
    }

    /* set the remaining packet length */
    ip_len -= hlen;

    /* check for fragmented packets */
    p->frag_offset = ntohs(p->iph->ip_off);

    /* 
     * get the values of the reserved, more 
     * fragments and don't fragment flags 
     */
    p->rf = (u_int8_t)((p->frag_offset & 0x8000) >> 15);
    p->df = (u_int8_t)((p->frag_offset & 0x4000) >> 14);
    p->mf = (u_int8_t)((p->frag_offset & 0x2000) >> 13);

    /* mask off the high bits in the fragment offset field */
    p->frag_offset &= 0x1FFF;

    if(p->frag_offset || p->mf) {
        /* set the packet fragment flag */
        p->frag_flag = 1;
    }
   
    /* if this packet isn't a fragment */
    if(!(p->frag_flag)) {
        switch(p->iph->ip_proto)
        {
            case IPPROTO_TCP:
                DecodeTCP(pkt + hlen, ip_len, p);
                return;

            case IPPROTO_UDP:
                //DecodeUDP(pkt + hlen, ip_len, p);
                return;

            case IPPROTO_ICMP:
                //DecodeICMP(pkt + hlen, ip_len, p);
                return;

            default:
                return;
        }
    }
    else {
        /* set the payload pointer and payload size */
        p->data = pkt + hlen;
        p->dsize = (u_short) ip_len;
    }
}

void decodeEthPkt(Packet* p, const struct pcap_pkthdr* pkthdr, const u_int8_t* pkt)
{
    u_int32_t pkt_len;
    u_int32_t cap_len;

	bzero((char *) p, sizeof(Packet));

    pkt_len = pkthdr->len;
    cap_len = pkthdr->caplen;

    p->pkth = pkthdr;
    p->pkt = pkt;
    p->eh = (EtherHdr *)pkt;

    switch(htons(p->eh->ether_type)) {
        case ETHERNET_TYPE_PPPoE_DISC:
        
        case ETHERNET_TYPE_PPPoE_SESS:
            return;
        
        case ETHERNET_TYPE_IP:
            DecodeIP(p->pkt + ETHERNET_HEADER_LEN, cap_len-ETHERNET_HEADER_LEN, p);
            return;
        
        case ETHERNET_TYPE_ARP:
        
        case ETHERNET_TYPE_REVARP:
        
        case ETHERNET_TYPE_8021Q:
        
        default:
            return;
    }

    return ;
}
/*
void mycallback(u_char* argument, const struct pcap_pkthdr* pkthdr, const u_char* pkt)
{
    Packet p;
	
	assert(pkthdr && pkt);

	if(argument == NULL) {
    }

#ifdef DEBUG_1
   printf(">>>packet length:%d\n", pkthdr->len);
   printf(">>>number of bytes:%d\n", pkthdr->caplen);
   printf(">>>recieved time:%s\n", ctime((const time_t *)&pkthdr->ts.tv_sec));
#endif

   p.packet_flags = 0;
   decodeEthPkt(&p, pkthdr, pkt);
   assert(p.pkth && p.pkt);
}


void capturePk()
{
    char pcap_err[20];

    while(1) {
        pcap_t* handle = NULL;
        //the third parameter is promisc
        if((handle = pcap_open_live(buf, 1600, 1, 0, pcap_err)) == NULL) {
            printf("pcap_open_live:%s\n", pcap_err);
            return;
        }
        pcap_loop(handle, -1, mycallback, NULL);
    }
}

int main(int argc, char* argv[])
{
    if(argc != 2) {
        printf("argv error!\n >>>like this:tipsalarm eth2\n");
        exit(1);
    }

    init();
    
    //recv daemon sleep signal!
    signal(SIGUSR1, handle_func);

    strcpy(buf, argv[1]);
    printf("\n\n>>>>>IDS start interface:%s\n", buf);

    capturePk();
    return 0;
}
*/
