#ifndef TIPS_PCAP_H
#define TIPS_PCAP_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/time.h>
#include <math.h>
#include <map>
#include <string>
#include <netinet/ip.h>
#include <pthread.h>
#include <signal.h>
#include <list>
#include <iostream>
using namespace std;

#define BUFFSIZE 256
#define FileName 256
#define IPSize 16
#define PcapDataType (char *)"1001" //原始网络报文数据

#define PcapFileHeadSize 24
#define MacFrameLen 14 
#define PcapPacketHeadSize  16

extern u_int16_t ProctolFlag;
extern u_int32_t IPFlag;
extern u_int16_t PortFlag;

struct macframes
{
		char dstmac[6];	
		char srcmac[6];	
		u_int16_t type;
};
#define MacFrameProtocolType 0x0008 //0x0800 IP --- MacFrames

struct pcap_file_head
{
		u_int32_t magic;
		u_short version_major;
		u_short version_minor;
		int thiszone;
		u_int32_t sigfigs;
		u_int32_t snaplen;
		u_int32_t linktype;
};

struct pcap_packet_head 
{
		struct timeval ts;
		u_int32_t caplen;
		u_int32_t len;
};
#define PMAGIC 0xa1b2c3d4 
#define PVMAJOR 2
#define PVMINOR 4
#define THISZONE 0
#define SIGFIGS 0
#define SNAPLEN 0xffff

#define LINKBSD  0           // BSD loopback devices, except for later OpenBSD
#define LINKETHER  1            //Ethernet, and Linux loopback devices
#define LINKTR  6            //802.5 Token Ring
#define LINKARCNET  7            //ARCnet
#define LINKSLIP  8            //SLIP
#define LINK999  9            //PPP
#define LINKFDDI  10           //FDDI
#define LINKLLC  100         //LLC/SNAP-encapsulated ATM
#define LINKRAWIP  101         //“raw IP”, with no link
#define LINKBSDSLIP  102         //BSD/OS SLIP
#define LINKBSDPPP  103         //BSD/OS PPP
#define LINKCISCO  104         //Cisco HDLC
#define LINK11  105         //802.11
#define LINKOPENBSD  108         //later OpenBSD loopback devices (with the AF_value in network byte order)
#define LINKCOOKED  113         //special Linux “cooked” capture
#define LINKTALK  114         //LocalTalk


class Pcap{
		public:
		void WtPcapFileH(FILE *fp,char *PcapMemBuff);
		void WtPcapMacFrame(char *PcapMemBuff,struct macframes *MF);
		void WtPcapData(char *PcapMemBuff,void *addr,int ip_len);
		void WtPcapDataH(char *PcapMemBuff,int Caplen,int Truelen);
};

#endif
