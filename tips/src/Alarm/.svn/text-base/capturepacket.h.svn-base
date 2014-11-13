#ifndef _CapturePacket_H__
#define _CapturePacket_H__
#include "data.h"
#include "itipschecker.h"
#include "checkdata.h"
#include "chkmngdata.h"
#include "wtpcap.h"
#include <pcap.h>
#include <netinet/if_ether.h>

#define AlarmConfig	(char *)"./AlarmConfig"
#define EthTag (char *)"CaptureEth"
extern  bool GetEth(char *eth);

/*
struct ethhdr {
    unsigned char h_dest[6];
    unsigned char h_source[6];
    uint16_t h_proto; 
}; 
*/

class StartCapture
{
		public:
				void Start();	
				void tcp_callback(const struct pcap_pkthdr *hdr,const u_char *data);
				void udp_callback(const struct pcap_pkthdr *hdr,const u_char *data);
};
#endif
