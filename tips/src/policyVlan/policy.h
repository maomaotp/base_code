#ifndef POLICY_H
#define POLICY_H
#include <list>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <pcap/pcap.h>
#include <pcap/vlan.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/if_ether.h>

typedef struct _addr_t
{
   in_addr_t  st_ip;
   in_addr_t  en_ip;
}addr_t;

typedef struct _policy_t
{
   u_int16_t  vlan_id;
   addr_t     ip_t; 
}policy_t;

void init_policy();
bool juage_policy(const u_char *packet);

static void getopt_reset();
static void add_policy(const char *it);
static bool check_policy(const char *pcy);
static void process_policy(const char *it);
static void covert_mask(const char *trains,char *mask);
static void spite_policy(int *temp,const char *str,char *p[]);
static bool find_policy(u_int16_t vlanid,const struct iphdr *ip);
static bool juage_ip(uint32_t ip,std::list<policy_t>::iterator &it);
static bool juage_vlan(u_int16_t vlanid,std::list<policy_t>::iterator &it);
static void getnet_policy(const char *iprange,const char *mask,policy_t *ply);
static void insert_policy(const int *counter,char * const pointer[],policy_t *ply);
#endif
