#ifndef ARP_H_H_H   
#define ARP_H_H_H      
#include <linux/if_arp.h>   
#pragma pack (1)   
struct arp_packet   
{   
    struct ethhdr       ethd;   
    struct arphdr       arphd;   
    unsigned char       eth_src_addr[6];   
    unsigned long       inet_src_addr;   
    unsigned char       eth_dst_addr[6];   
    unsigned long       inet_dst_addr;   
    unsigned char       padding[18];   
};   
#pragma pack ()   
void print_ARPMsg(struct arp_packet *packet, char* my_mac);   
void print_data(unsigned char *buf);   
unsigned char* BuildArpRequestPacket(unsigned char* Eth_Src_Addr,unsigned long Inet_Src_Addr,unsigned long Inet_Dst_Addr);   
int get_ifindex(char *dev);
int get_local_mac (char*, unsigned char*);
unsigned long get_local_ip(char *dev);   
#endif 
