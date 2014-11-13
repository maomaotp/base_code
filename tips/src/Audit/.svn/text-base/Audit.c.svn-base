/*
 * OpenDPI_demo.c
 * Copyright (C) 2009-2011 by ipoque GmbH
 * 
 * This file is part of OpenDPI, an open source deep packet inspection
 * library based on the PACE technology by ipoque GmbH
 * 
 * OpenDPI is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * OpenDPI is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with OpenDPI.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <netinet/in.h>
#include <time.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <getopt.h>
#include <signal.h>
#include "zlog.h"

#ifdef __linux__
#include <linux/ip.h>
#include <netinet/ip6.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <linux/if_ether.h>
#else
#include "linux_compat.h"
#endif

#include <pcap.h>
#include "ipq_api.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include "glib.h"
#include "client.h"
#include "cachestruct.h"
#include "./sharemem/sharemem.h"
#include "./connectlimit/connectlimit.h"
#define PCAP_FILE_PATH "/data/file/netshark"
#define UNIX_DOMAIN "/tmp/domain.netlog"
//#define NETSHARK_FILTER
#define AIP  10
#define BIP  172
#define BIPa 16
#define BIPb 31
#define CIP  192 
#define CIPa 168
#define INNET
#define FLOW_OUTTIME 10
#define SCAN_GAP 300
#define HASHTABLE_LEN  (0x80000)
#define HASH_OSDPI_FLOWS
#define HASH_OSDPI_IDS
#define KEYLEN 3
#define OSDPI_FLOW_KEYLEN 4
#define MAX_PROTO_LEN 10
#define rot(x,k) ((x)<<(k) | ((x)>>(32-(k))))
#define min(a,b,c) \
{ \
	a -= c; a ^= rot(c, 4); c += b; \
	b -= a; b ^= rot(a, 6); a += c; \
	c -= b; c ^= rot(b, 8); b += a; \
	a -= c; a ^= rot(c,16); c += b; \
	b -= a; b ^= rot(a,19); a += c; \
	c -= b; c ^= rot(b, 4); b += a; \
}

#define final(a,b,c) \
{ \
	c ^= b; c -= rot(b,14); \
	a ^= c; a -= rot(c,11); \
	b ^= a; b -= rot(a,25); \
	c ^= b; c -= rot(b,16); \
	a ^= c; a -= rot(c, 4); \
	b ^= a; b -= rot(a,14); \
	c ^= b; c -= rot(b,24); \
}

char ether_card[256];
#ifdef INNET
int innet_ip(uint32_t sip)
{
	uint32_t dst = htonl(sip);
	if(((BIP == (dst & 0xFF)) && ((((dst & 0xFF00) >> 8) >= BIPa) && (((dst & 0xFF00) >> 8) <= BIPb))) || (AIP == (dst&0xFF)) || ((CIP == (dst&0xFF))     && (CIPa == ((dst&0xFF00) >> 8))))
	{
		return 1;
	}
	return 0;
}
#endif
struct shm_struct_t * shm = NULL;

typedef struct iplist_rules
{
	uint32_t start_ip;
	uint32_t end_ip;
	uint16_t port;
}IPLIST;

#define MAX_RULE_NUM 1024
static IPLIST  ordered_rule[MAX_RULE_NUM];
static int tactics_flag = 0;
//static int rule_access_flag = 1;
static int rule_num = 0;
typedef struct tuple_t
{
	uint8_t  fin;
	uint32_t sip;
	uint32_t dip;
	uint16_t sport;
	uint16_t dport;
	uint32_t app_protocol;
	uint32_t below_protocol;
	uint32_t packet_size;
	struct timeval time_stamp;
}tuple_t;

typedef struct audit_flow_t
{
	struct audit_flow_t * prev_flow;
	struct audit_flow_t * next_flow;
	struct audit_flow_t * next;
	uint8_t  fin;
	uint32_t sip;
	uint32_t dip;
	uint16_t sport;
	uint16_t dport;
	uint32_t protocol;
	uint32_t below_protocol;
	uint64_t flow_up_size;
	uint64_t flow_down_size;
	struct timeval start_time;
	struct timeval end_time;
}audit_flow_t;

audit_flow_t * list_head = NULL;
audit_flow_t * list_tail = NULL;
uint32_t list_length = 0;
long flows_limit = 10000000;

static int	netshark_flag = 0;
static struct pcap_pkthdr * dump_header = NULL;
static u_char * dump_packet = NULL;
static struct stat file_stat;

typedef struct port_node
{
	uint16_t port;
	struct port_node * next;
}port_node;

typedef struct netshark_t
{
	int			iip_flag;
	uint32_t	istartip;
	uint32_t	iendip;
	int 		iport_flag;
	port_node * iport_list;
	int 		oip_flag;
	uint32_t	ostartip;
	uint32_t	oendip;
	int 		oport_flag;
	port_node * oport_list;
	char 		protocol[16];
	float		netshark_size;
	float		netshark_gap;
	char 		center_device_id[11];
	char		timestamp[11];
	uint32_t	start_time;
	char		filepath[1024];
	pcap_dumper_t * dumpfp;
	int  		file_flag;
	int  		netshark_flag;
	struct netshark_t * next;
}netshark_t;
netshark_t * netshark_list = NULL; 
extern char *optarg;
extern int optind, opterr, optopt;

//static char DIVECEID[11] = "1312039999";

static struct timeval ts;
static audit_flow_t ** hashtable = NULL;
static uint32_t  hashtable_used = 0;
pthread_mutex_t mutex;
pthread_mutex_t mutex_rule;


// cli options
//static char *_pcap_file = NULL;

// pcap
//static char _pcap_error_buffer[PCAP_ERRBUF_SIZE];
static pcap_t *_pcap_handle = NULL;
static int _pcap_datalink_type = 0;

// detection
static struct ipoque_detection_module_struct *ipoque_struct = NULL;
static u32 detection_tick_resolution = 1000;
//static char *prot_long_str[] = { IPOQUE_PROTOCOL_LONG_STRING };

//static char *prot_short_str[] = { IPOQUE_PROTOCOL_SHORT_STRING };

#ifdef IPOQUE_ENABLE_DEBUG_MESSAGES
static IPOQUE_PROTOCOL_BITMASK debug_messages_bitmask;
#endif

// results
static u64 raw_packet_count = 0;
static u64 ip_packet_count = 0;
static u64 total_bytes = 0;
static u64 protocol_counter[IPOQUE_MAX_SUPPORTED_PROTOCOLS + 1];
static u64 protocol_counter_bytes[IPOQUE_MAX_SUPPORTED_PROTOCOLS + 1];

// id tracking

struct osdpi_ip_addr {
#ifdef IPOQUE_DETECTION_SUPPORT_IPV6
	u32 is_ip_v6;
#endif
	union {
		u32 ipv4;
#ifdef IPOQUE_DETECTION_SUPPORT_IPV6
		struct in6_addr ipv6;
#endif
	};
};

typedef struct osdpi_id {
	struct osdpi_id *  next;
	struct osdpi_ip_addr ip;
	struct ipoque_id_struct *ipoque_id;
} osdpi_id_t;

static u32 size_id_struct = 0;
#define			MAX_OSDPI_IDS			0x20000
static struct osdpi_id *osdpi_ids;
//static u32 osdpi_id_count = 0;


// flow tracking
typedef struct osdpi_flow {
	struct osdpi_flow * next;
	struct ipoque_unique_flow_ipv4_and_6_struct key;
	struct ipoque_flow_struct *ipoque_flow;

	// result only, not used for flow identification
	u32 detected_protocol;
} osdpi_flow_t;

static u32 size_flow_struct = 0;
#define			MAX_OSDPI_FLOWS			0x80000
static struct osdpi_flow *osdpi_flows;
//static u32 osdpi_flow_count = 0;

#define NTOH(addr) \
		((unsigned char *)&addr)[3],\
		((unsigned char *)&addr)[2],\
		((unsigned char *)&addr)[1],\
		((unsigned char *)&addr)[0]
static uint32_t hash_fun(const uint32_t *k, size_t length, uint32_t initval);


#ifdef IPOQUE_ENABLE_DEBUG_MESSAGES
static int string_to_detection_bitmask(char *str, IPOQUE_PROTOCOL_BITMASK * dbm)
{
	u32 a;
	u32 oldptr = 0;
	u32 ptr = 0;
	IPOQUE_BITMASK_RESET(*dbm);

	printf("Protocol parameter given: %s\n", str);

	if (strcmp(str, "all") == 0) {
		printf("Protocol parameter all parsed\n");
		IPOQUE_BITMASK_SET_ALL(*dbm);
		printf("Bitmask is: " IPOQUE_BITMASK_DEBUG_OUTPUT_BITMASK_STRING " \n",
			   IPOQUE_BITMASK_DEBUG_OUTPUT_BITMASK_VALUE(*dbm));
		return 0;
	}
	// parse bitmask
	while (1) {
		if (str[ptr] == 0 || str[ptr] == ' ') {
			printf("Protocol parameter: parsed: %.*s,\n", ptr - oldptr, &str[oldptr]);
			for (a = 1; a <= IPOQUE_MAX_SUPPORTED_PROTOCOLS; a++) {

				if (strlen(prot_short_str[a]) == (ptr - oldptr) &&
					(memcmp(&str[oldptr], prot_short_str[a], ptr - oldptr) == 0)) {
					IPOQUE_ADD_PROTOCOL_TO_BITMASK(*dbm, a);
					printf("Protocol parameter detected as protocol %s\n", prot_long_str[a]);
				}
			}
			oldptr = ptr + 1;
			if (str[ptr] == 0)
				break;
		}
		ptr++;
	}
	return 0;
}
#endif

/*hyj 其中用到的部分全局变量也被注释掉了，如果使用次函数是发现全局变量没有定义，先检查是否被注释了
static void parseOptions(int argc, char **argv)
{
	int opt;

#ifdef IPOQUE_ENABLE_DEBUG_MESSAGES
	IPOQUE_BITMASK_SET_ALL(debug_messages_bitmask);
#endif

	while ((opt = getopt(argc, argv, "f:e:")) != EOF) {
		switch (opt) {
		case 'f':
			_pcap_file = optarg;
			break;
		case 'e':
#ifdef IPOQUE_ENABLE_DEBUG_MESSAGES
			// set debug logging bitmask to all protocols
			if (string_to_detection_bitmask(optarg, &debug_messages_bitmask) != 0) {
				printf("ERROR option -e needs a valid list of protocols");
				exit(-1);
			}

			printf("debug messages Bitmask is: " IPOQUE_BITMASK_DEBUG_OUTPUT_BITMASK_STRING "\n",
				   IPOQUE_BITMASK_DEBUG_OUTPUT_BITMASK_VALUE(debug_messages_bitmask));

#else
			printf("ERROR: option -e : DEBUG MESSAGES DEACTIVATED\n");
			exit(-1);
#endif
			break;
		}
	}

	// check parameters
	if (_pcap_file == NULL || strcmp(_pcap_file, "") == 0) {
		printf("ERROR: no pcap file path provided; use option -f with the path to a valid pcap file\n");
		exit(-1);
	}
}
*/

static void debug_printf(u32 protocol, void *id_struct, ipq_log_level_t log_level, const char *format, ...)
{
#ifdef IPOQUE_ENABLE_DEBUG_MESSAGES
	if (IPOQUE_COMPARE_PROTOCOL_TO_BITMASK(debug_messages_bitmask, protocol) != 0) {
		const char *protocol_string;
		const char *file;
		const char *func;
		u32 line;
		va_list ap;
		va_start(ap, format);

		protocol_string = prot_short_str[protocol];

		ipoque_debug_get_last_log_function_line(ipoque_struct, &file, &func, &line);

		printf("\nDEBUG: %s:%s:%u Prot: %s, level: %u packet: %llu :", file, func, line, protocol_string,
			   log_level, raw_packet_count);
		vprintf(format, ap);
		va_end(ap);
	}
#endif
/*hyj参数没有被使用？*/protocol = protocol; id_struct = id_struct; log_level = log_level; if(format){};
}

static void *malloc_wrapper(unsigned long size)
{
	return malloc(size);
}

static void free_wrapper(void *freeable)
{
	free(freeable);
}



static u32 hash_osdpi_ids(const struct osdpi_ip_addr *ip, int * ip_version)
{
  	if(ip->is_ip_v6 == 0) {
   		u32 hash_index = 0;
   		*ip_version = 4;
    	hash_index = hash_fun(&(ip->ipv4), 1, 1);
 //   	printf("%8x\n", hash_index);
    	return hash_index & (MAX_OSDPI_IDS-1);
    } else if (ip->is_ip_v6 == 1) {
     	*ip_version = 6;
     	return 0;
   	}
	
	return 0;
}


static void *get_id(const struct osdpi_ip_addr *ip)
{
	u32 i;
#ifdef HASH_OSDPI_IDS
	struct osdpi_id * id = NULL;
 	struct osdpi_id * pre_id = NULL;
  	int ip_version = 0;
   	struct osdpi_ip_addr mask_ip;
    memset(&mask_ip, 0, sizeof(struct osdpi_ip_addr));
    i = hash_osdpi_ids(ip, &ip_version);
	if(ip_version == 4) {		//update by hyj
	  	for(id = &osdpi_ids[i]; id != NULL; pre_id = id, id = id->next) {
			if(memcmp(&(id->ip),ip, sizeof(struct osdpi_ip_addr)) == 0) {
				return id->ipoque_id;
			} else if(memcmp(&(id->ip), &mask_ip, sizeof(struct osdpi_ip_addr)) == 0) {
				id->ip = *ip;
				return id->ipoque_id;
			}
		}
		if(id == NULL) {
	 	   	id = (struct osdpi_id *) calloc(1, sizeof(struct osdpi_id));
	    	if(id == NULL) {
 				printf("ERROR: malloc for struct osdpi_id failed\n");
				exit(-1);
    		}
            id->ipoque_id = (struct ipoque_id_struct *)calloc(1, size_id_struct);
      		if (id->ipoque_id == NULL) {
				printf("ERROR: malloc for ipoque_id_struct failed\n");
				exit(-1);
   			}
    		id->next = NULL;
     	 	id->ip = *ip;
     	  	pre_id->next = id;
       	 	return id->ipoque_id;
  	    }
	} else if(ip_version == 6) {		//update by hyj		
		return NULL;
	}	
#else
	for (i = 0; i < osdpi_id_count; i++) {
		if (memcmp(&osdpi_ids[i].ip, ip, sizeof(struct osdpi_ip_addr)) == 0) {
			return osdpi_ids[i].ipoque_id;
		}
	}
	if (osdpi_id_count == MAX_OSDPI_IDS) {
		printf("ERROR: maximum unique id count (%u) has been exceeded\n", MAX_OSDPI_IDS);
		exit(-1);
	} else {
		struct ipoque_id_struct *ipoque_id;
		osdpi_ids[osdpi_id_count].ip = *ip;
		ipoque_id = osdpi_ids[osdpi_id_count].ipoque_id;

		osdpi_id_count += 1;
		return ipoque_id;
	}
#endif
	return NULL;		//hyj 返回值?
}

static u32 hash_osdpi_flows_ipv4(u32 lower_ip, u32 upper_ip, u16 lower_port, u16 upper_port, u16 protocol)
{
	u32 key[OSDPI_FLOW_KEYLEN], hash_index = 0;
	int i = 0;
	memset(key, 0, OSDPI_FLOW_KEYLEN*sizeof(u32));
	for(i=0; i<OSDPI_FLOW_KEYLEN; i++)
	{
		if(0 == i)
			key[i] = lower_ip;
		if(1 == i)
			key[i] = upper_ip;
		if(2 == i)
			key[i] = ((lower_port<<16) + upper_port);
		if(3 == i)
			key[i] = key[i] | protocol;
	}
	hash_index = hash_fun(key, OSDPI_FLOW_KEYLEN, 1);
//	printf("%8x\n", hash_index);
	return hash_index & (HASHTABLE_LEN-1);
	
}

static u32 hash_osdpi_flows(struct ipoque_unique_flow_ipv4_and_6_struct *key, int * ip_version_ptr)
{
	if(key->is_ip_v6 == 0) {
		*ip_version_ptr = 4;
		return hash_osdpi_flows_ipv4(key->ip.ipv4.lower_ip, key->ip.ipv4.upper_ip, key->lower_port, key->upper_port, key->protocol);
	} else if(key->is_ip_v6 == 1) {
		return 0;
	}

	return 0;		//hyj
}


static struct osdpi_flow *get_osdpi_flow(const struct iphdr *iph, u16 ipsize)
{
	u32 i;
	const u8 *l4 = NULL;
	u16 l4_len = 0;
	u8 l4_protocol = 0;
	struct ipoque_unique_flow_ipv4_and_6_struct key, mask_key;
	u8 dir = 0;

	if (ipoque_detection_get_l4((u8*)iph, ipsize, &l4, &l4_len, &l4_protocol, 0) == 0) {
		if (ipoque_detection_build_key((u8*)iph, ipsize, l4, l4_len, l4_protocol, &key, &dir, 0) == 0) {
#ifdef  HASH_OSDPI_FLOWS			
			int ip_version = 0;
			struct osdpi_flow * flow = NULL;
			struct osdpi_flow * pre_flow = NULL;
			memset(&mask_key, 0, sizeof(struct ipoque_unique_flow_ipv4_and_6_struct));
			i = hash_osdpi_flows(&key,&ip_version);
		//	printf("%u\n", i);
			if(ip_version == 4)
			{
				for(flow = &osdpi_flows[i]; flow != NULL; pre_flow = flow, flow = flow->next){
					if (memcmp(&(flow->key), &key, sizeof(struct ipoque_unique_flow_ipv4_and_6_struct)) == 0) {
			//			printf("flow exist\n");
						return flow;
					} else if(memcmp(&(flow->key), &mask_key, sizeof(struct ipoque_unique_flow_ipv4_and_6_struct)) == 0) {
						flow->key = key;
		//				printf("flow create in osdpi_flows\n");
						return flow;
					}
				}
				if(flow == NULL) {
					flow = (struct osdpi_flow *) calloc(1,sizeof(struct osdpi_flow));
					if (flow == NULL) {
						printf("ERROR: malloc for struct osdpi_flow failed\n");
						exit(-1);
					}
					flow->ipoque_flow = (struct ipoque_flow_struct*)calloc(1, size_flow_struct);
					if (flow->ipoque_flow == NULL) {
						printf("ERROR: malloc for ipoque_flow_struct failed\n");
						exit(-1);
					}
					flow->next = NULL;
					flow->key = key;
					pre_flow->next = flow;
		//			printf("flow create in osdpi_flows next\n");
					return flow;
				}
			} else if(ip_version == 6){
				return flow;
			}
#else
			for (i = 0; i < osdpi_flow_count; i++) {
				if (memcmp(&osdpi_flows[i].key, &key, sizeof(struct ipoque_unique_flow_ipv4_and_6_struct)) == 0) {
					return &osdpi_flows[i];
				}
			} 
			if (osdpi_flow_count == MAX_OSDPI_FLOWS) {
				printf("ERROR: maximum flow count (%u) has been exceeded\n", MAX_OSDPI_FLOWS);
				exit(-1);
			} else {
				struct osdpi_flow *flow;
				osdpi_flows[osdpi_flow_count].key = key;
				flow = &osdpi_flows[osdpi_flow_count];

				osdpi_flow_count += 1;
				return flow;
			}
#endif

		}
	}

	return NULL;
}

static void setupDetection(void)
{
	u32 i;
	IPOQUE_PROTOCOL_BITMASK all;

	// init global detection structure
	ipoque_struct = ipoque_init_detection_module(detection_tick_resolution, malloc_wrapper, debug_printf);
	if (ipoque_struct == NULL) {
		printf("ERROR: global structure initialization failed\n");
		exit(-1);
	}
	// enable all protocols
	IPOQUE_BITMASK_SET_ALL(all);
	ipoque_set_protocol_detection_bitmask2(ipoque_struct, &all);

	// allocate memory for id and flow tracking
	size_id_struct = ipoque_detection_get_sizeof_ipoque_id_struct();
	size_flow_struct = ipoque_detection_get_sizeof_ipoque_flow_struct();

	osdpi_ids = (struct osdpi_id *)malloc(MAX_OSDPI_IDS * sizeof(struct osdpi_id));
	if (osdpi_ids == NULL) {
		printf("ERROR: malloc for osdpi_ids failed\n");
		exit(-1);
	}
	for (i = 0; i < MAX_OSDPI_IDS; i++) {
		memset(&osdpi_ids[i], 0, sizeof(struct osdpi_id));
		osdpi_ids[i].ipoque_id = (struct ipoque_id_struct *)calloc(1, size_id_struct);
		if (osdpi_ids[i].ipoque_id == NULL) {
			printf("ERROR: malloc for ipoque_id_struct failed\n");
			exit(-1);
		}
	}

	osdpi_flows = (struct osdpi_flow *)malloc(MAX_OSDPI_FLOWS * sizeof(struct osdpi_flow));
	if (osdpi_flows == NULL) {
		printf("ERROR: malloc for osdpi_flows failed\n");
		exit(-1);
	}
	for (i = 0; i < MAX_OSDPI_FLOWS; i++) {
		memset(&osdpi_flows[i], 0, sizeof(struct osdpi_flow));
		osdpi_flows[i].ipoque_flow = (struct ipoque_flow_struct *) calloc(1, size_flow_struct);
		if (osdpi_flows[i].ipoque_flow == NULL) {
			printf("ERROR: malloc for ipoque_flow_struct failed\n");
			exit(-1);
		}
	}

	// clear memory for results
	memset(protocol_counter, 0, (IPOQUE_MAX_SUPPORTED_PROTOCOLS + 1) * sizeof(u64));
	memset(protocol_counter_bytes, 0, (IPOQUE_MAX_SUPPORTED_PROTOCOLS + 1) * sizeof(u64));
}

static void terminateDetection(void)
{
	u32 i;

	ipoque_exit_detection_module(ipoque_struct, free_wrapper);

	for (i = 0; i < MAX_OSDPI_IDS; i++) {
		free(osdpi_ids[i].ipoque_id);
	}
	free(osdpi_ids);
	for (i = 0; i < MAX_OSDPI_FLOWS; i++) {
		free(osdpi_flows[i].ipoque_flow);
	}
	free(osdpi_flows);
}


static uint32_t hash_fun(const uint32_t *k, size_t length, uint32_t initval)
{
	uint32_t a,b,c;
	a = b = c = 0xdeadbeef + (((uint32_t)length)<<2) + initval;
	while(length > 3)
	{
		a += k[0];
		b += k[1];
		c += k[2];
		min(a,b,c);
		length -= 3;
		k += 3;
	}
	switch(length)
	{
		case 3 : c += k[2];
		case 2 : b += k[1];
		case 1 : a += k[0];
			final(a, b, c);
		case 0 : break;
	}
	
	return c & (HASHTABLE_LEN-1);
}


static uint32_t hash_tuple(uint32_t sip, uint32_t dip, uint16_t sport, uint16_t dport)
{
	uint32_t key[KEYLEN], hash_index;
	int i = 0;
	memset(key, 0, KEYLEN*sizeof(uint32_t));
	for(i=0; i<KEYLEN; i++)
	{
		if(0 == i)
			key[i] = sip;
		if(1 == i)
			key[i] = dip;
		if(2 == i)
			key[i] = ((sport<<16) + dport);
	}
//	printf("%4d %4d\n", key[2]>>16, key[2]&0xffff);
	hash_index = hash_fun(key, KEYLEN, 1);
//	printf("%8x\n", hash_index);
	return hash_index;
	
}

static audit_flow_t* malloc_audit_flow()
{
	audit_flow_t *p = NULL;
   	p = (audit_flow_t*)calloc(1,sizeof(audit_flow_t));
	if(p == NULL)
	{	
		printf("malloc audit_flow  failed\n");
		return NULL;
	}

	return p;
}

static audit_flow_t* free_audit_flow(audit_flow_t * p)
{
	if(p)
	free(p);
	return NULL; //返回值?
}

static int create_flow(uint32_t hash_index, tuple_t * tuple_ptr)
{
	audit_flow_t * temp_ptr = hashtable[hash_index];
	while(temp_ptr != NULL && temp_ptr->next != NULL)
	{
		temp_ptr = temp_ptr->next;
	}
	if(temp_ptr == NULL)
	{
		temp_ptr = malloc_audit_flow();
		if(temp_ptr == NULL)
		{
			printf("malloc_audit_flow() failed\n");
			return -1;
		}
		hashtable[hash_index] = temp_ptr;
		hashtable_used++;
	}
	else
	{	
		temp_ptr->next = malloc_audit_flow();
		if(temp_ptr->next == NULL)
		{
			printf("malloc_audit_flow() failed\n");
			return -1;
		}
		temp_ptr = temp_ptr->next;
	}
	temp_ptr->next = NULL;
	temp_ptr->sip = tuple_ptr->sip;
//	printf("c sip: %u\n", tuple_ptr->sip);
	temp_ptr->dip = tuple_ptr->dip;
	temp_ptr->sport = tuple_ptr->sport;
	temp_ptr->dport = tuple_ptr->dport;
	temp_ptr->protocol = tuple_ptr->app_protocol;
	temp_ptr->below_protocol = tuple_ptr->below_protocol;
	temp_ptr->flow_up_size = tuple_ptr->packet_size;
	temp_ptr->flow_down_size = 0;
//	printf("time_stamp=%u\n",tuple_ptr->time_stamp);
	temp_ptr->start_time = tuple_ptr->time_stamp;
	temp_ptr->end_time = tuple_ptr->time_stamp;
//	printf("end_time=%u\n",hashtable[hash_index]->end_time);
//	printf("create_flow hashtable[hash_index]=%p temp_ptr=%p\n", hashtable[hash_index], temp_ptr);

	if(list_head == NULL)
	{
		temp_ptr->next_flow = NULL;
		temp_ptr->prev_flow = NULL;
		list_head = temp_ptr;
		list_tail = list_head;
		list_length++;
	}
	else if(list_head != NULL)
	{
		temp_ptr->next_flow = NULL;
		temp_ptr->prev_flow = list_tail;
		list_tail->next_flow = temp_ptr;
		list_tail = temp_ptr;
		list_length++;
	}
	return 0;
}

static int find_ip_and_port(uint32_t ip, uint16_t port, int left, int right)
{
	if(left <= right)
	{
		int mid = (left + right)/2;
	//	printf("\nleft_ip=%s  ", inet_ntoa(htonl(ordered_rule[left].start_ip)));
	//	printf("right_ip=%s\n",inet_ntoa(htonl(ordered_rule[right].start_ip)));
	//	printf("\nip=%s\n", inet_ntoa(htonl(ip)));
	//	printf("\nip=%x left_ip=%x right_ip=%x\n", ip, ordered_rule[left].start_ip, ordered_rule[right].start_ip);
		if(ip >= ordered_rule[mid].start_ip && ip <= ordered_rule[mid].end_ip)
		{
	//		if(port == ordered_rule[mid].port)
	//			printf("find_ip_and_port\n");
				return mid;
	//		else
	//			return -1;
		}
		else if(ip < ordered_rule[mid].start_ip)
			return find_ip_and_port(ip, port, left, mid-1);
		else if(ip > ordered_rule[mid].start_ip)
			return find_ip_and_port(ip, port, mid+1, right);
		else
			return -1;			//add by hyj
	}
	else
		return -1;
}

static int filt_flow(uint32_t sip, uint32_t dip, uint16_t sport, uint16_t dport)
{
	int rule_index = -1;
	rule_index = find_ip_and_port(sip, sport, 0, rule_num-1);
	if(rule_index == -1)
	{
		rule_index = find_ip_and_port(dip, dport, 0, rule_num-1);
	}
//	printf("rule_index=%d\n", rule_index);
	return rule_index;
}


static int exportDB(audit_flow_t * temp_ptr)
{
		if(tactics_flag == 1)
		{	
			int ret = -1;
			pthread_mutex_lock(&mutex_rule);
			ret = filt_flow(temp_ptr->sip, temp_ptr->dip, temp_ptr->sport, temp_ptr->dport);
			pthread_mutex_unlock(&mutex_rule);
			if(ret != -1)
			{
		//		printf("filt_flow..............................\n");
				return 0;
			}
		}
		if(temp_ptr->protocol != 0 && temp_ptr->flow_up_size != 0 && temp_ptr->flow_down_size != 0)
		{
#if 0
			time_t flow_start_time = 0, flow_end_time = 0;
			struct tm *ltime;
			char strtime[50];
			flow_start_time = (time_t) temp_ptr->start_time.tv_sec;
			flow_end_time = (time_t) temp_ptr->end_time.tv_sec;
			ltime = localtime(&flow_start_time);
			strftime(strtime, sizeof(strtime), "%F %T", ltime);
			printf("\nsip=%d.%d.%d.%d, dip=%d.%d.%d.%d\n", NTOH(temp_ptr->sip), NTOH(temp_ptr->dip));
		//	printf("\nsip=%s ", inet_ntoa(ntohl(temp_ptr->sip)));
		//	printf("dip=%s\n", inet_ntoa(ntohl(temp_ptr->dip)));
			printf("sport=%u  dport=%u\n", temp_ptr->sport, temp_ptr->dport);
			printf("protocol=%s\n", prot_short_str[temp_ptr->protocol]);
			printf("flow_up_size=%u\n", temp_ptr->flow_up_size);
			printf("flow_down_size=%u\n", temp_ptr->flow_down_size);
			printf("start_time=%s\n", strtime);
//			printf("start_time=%u\n", flow_start_time);
			ltime = localtime(&flow_end_time);
			strftime(strtime, sizeof(strtime), "%F %T", ltime);
			printf("end_time=%s\n", strtime);
//			printf("end_time=%u\n", flow_end_time);
			time_t lt = time(NULL);
			ltime = localtime(&lt);
			strftime(strtime, sizeof(strtime), "%F %T", ltime);
			printf("now_time=%s\n", strtime);
//			printf("end_time=%u\n", lt);
			printf("\n");
			fflush(stdout);
			
#endif 
			cache_audit_t audit_entry;
			memset(&audit_entry, 0, sizeof(cache_audit_t));
		//	audit_entry.head.type = AUDIT;
			INIT_AUDIT_HEAD(&audit_entry);
		//	printf("sip=%d.%d.%d.%d  dip=%d.%d.%d.%d\n", NTOH(temp_ptr->sip), NTOH(temp_ptr->dip));
#ifdef INNET
			if(innet_ip(temp_ptr->sip))
			{
		//		printf("innet_ip return 1\n");
#endif
				audit_entry.sip = temp_ptr->sip;
				audit_entry.dip = temp_ptr->dip;
				audit_entry.sport = temp_ptr->sport;
				audit_entry.dport = temp_ptr->dport;
				audit_entry.protocol = temp_ptr->protocol;
				audit_entry.below_protocol = temp_ptr->below_protocol;
				audit_entry.up_KB = temp_ptr->flow_up_size/1024.0;
				audit_entry.down_KB = temp_ptr->flow_down_size/1024.0;
				audit_entry.start_time = (time_t) temp_ptr->start_time.tv_sec;
				audit_entry.end_time = (time_t) temp_ptr->end_time.tv_sec;

#ifdef INNET
			}
			else 
			{
		//		printf("innet_ip return 0\n");
				audit_entry.sip = temp_ptr->dip;
				audit_entry.dip = temp_ptr->sip;
				audit_entry.sport = temp_ptr->dport;
				audit_entry.dport = temp_ptr->sport;
				audit_entry.protocol = temp_ptr->protocol;
				audit_entry.below_protocol = temp_ptr->below_protocol;
				audit_entry.up_KB = temp_ptr->flow_down_size/1024.0;
				audit_entry.down_KB = temp_ptr->flow_up_size/1024.0;
				audit_entry.start_time = (time_t) temp_ptr->start_time.tv_sec;
				audit_entry.end_time = (time_t) temp_ptr->end_time.tv_sec;
			}
#endif
			Send_Msg((void*)&audit_entry, sizeof(cache_audit_t));
		}
	return 0;		//hyj 返回值？
}

static int delete_flow(uint32_t hash_index, tuple_t * tuple_ptr)
{
	audit_flow_t * temp_ptr = hashtable[hash_index];
	audit_flow_t * pre_temp_ptr = temp_ptr;
	while(temp_ptr != NULL)
	{
		if((temp_ptr->sip == tuple_ptr->sip && temp_ptr->dip == tuple_ptr->dip &&
		   temp_ptr->sport == tuple_ptr->sport && temp_ptr->dport == tuple_ptr->dport)
		  || (temp_ptr->sip == tuple_ptr->dip && temp_ptr->dip == tuple_ptr->sip &&
		   temp_ptr->sport == tuple_ptr->dport && temp_ptr->dport == tuple_ptr->sport))
		{
			exportDB(temp_ptr);
			if(hashtable[hash_index] == temp_ptr)
			{	
				hashtable[hash_index] = temp_ptr->next;
				if(hashtable[hash_index] == NULL)
					hashtable_used--;
			}
			else
			{	
				pre_temp_ptr->next = temp_ptr->next;
			}
			if(temp_ptr->prev_flow != NULL)
				temp_ptr->prev_flow->next_flow = temp_ptr->next_flow;
			if(temp_ptr->next_flow != NULL)
			   	temp_ptr->next_flow->prev_flow = temp_ptr->prev_flow;
			if(temp_ptr->prev_flow == NULL)
				list_head = temp_ptr->next_flow;
			if(temp_ptr->next_flow == NULL)
				list_tail = temp_ptr->prev_flow;
			free_audit_flow(temp_ptr);
			temp_ptr = NULL;
			list_length--;
//			printf("hashtable_used=%d\n", hashtable_used);
			return 0;
		}
		pre_temp_ptr = temp_ptr;
		temp_ptr = temp_ptr->next;
	}
	return -1;
}


static int insert_flow_up(uint32_t hash_index, tuple_t * tuple_ptr)
{
	audit_flow_t * temp_ptr = hashtable[hash_index];
	while(temp_ptr != NULL)
	{
		if(temp_ptr->sip == tuple_ptr->sip && temp_ptr->dip == tuple_ptr->dip &&
		   temp_ptr->sport == tuple_ptr->sport && temp_ptr->dport == tuple_ptr->dport)
		{
			temp_ptr->protocol = tuple_ptr->app_protocol;
			temp_ptr->flow_up_size += tuple_ptr->packet_size;
//			printf("time_stamp=%u\n",tuple_ptr->time_stamp);
			temp_ptr->end_time = tuple_ptr->time_stamp;
			if(tuple_ptr->fin == 1 && temp_ptr->fin == 0)
			{
				temp_ptr->fin = 1;
			}
			else if(temp_ptr->fin >= 1)
			{
				(temp_ptr->fin)++;
			}
			if(temp_ptr->fin >= 4)
			{
		//		printf("temp_ptr->fin = %d\n", temp_ptr->fin);
				delete_flow(hash_index, tuple_ptr);
			}
//			printf("end_time=%u\n",hashtable[hash_index]->end_time);	
//			printf("insert_flow_up hashtable[hash_index]=%p temp_ptr=%p\n", hashtable[hash_index], temp_ptr);
			break;
		}
		temp_ptr = temp_ptr->next;
	}
	return 0;
}
static int insert_flow_down(uint32_t hash_index, tuple_t * tuple_ptr)
{
	audit_flow_t * temp_ptr = hashtable[hash_index];
	while(temp_ptr != NULL)
	{
		if(temp_ptr->sip == tuple_ptr->dip && temp_ptr->dip == tuple_ptr->sip &&
		   temp_ptr->sport == tuple_ptr->dport && temp_ptr->dport == tuple_ptr->sport)
		{
			temp_ptr->protocol = tuple_ptr->app_protocol;
			temp_ptr->flow_down_size += tuple_ptr->packet_size;
//			printf("flow_down_size=%u\n",hashtable[hash_index]->flow_down_size);
//			printf("time_stamp=%u\n",tuple_ptr->time_stamp);
			temp_ptr->end_time = tuple_ptr->time_stamp;
			if(tuple_ptr->fin == 1 && temp_ptr->fin == 0)
			{
				temp_ptr->fin = 1;
			}
			else if(temp_ptr->fin >= 1)
			{
				(temp_ptr->fin)++;
			}
			if(temp_ptr->fin >= 4)
			{
		//		printf("temp_ptr->fin = %d\n", temp_ptr->fin);
				delete_flow(hash_index, tuple_ptr);
			}
//			printf("end_time=%u\n",hashtable[hash_index]->end_time);
//			printf("insert_flow_down hashtable[hash_index]=%p temp_ptr=%p\n", hashtable[hash_index], temp_ptr);
			break;
		}
		temp_ptr = temp_ptr->next;
	}
	return 0;
}


static int hash_audit(struct tuple_t * tuple_ptr)
{
	uint32_t hash_index_up = 0, hash_index_down = 0;
	hash_index_up = hash_tuple(tuple_ptr->sip, tuple_ptr->dip, tuple_ptr->sport, tuple_ptr->dport);
	pthread_mutex_lock(&mutex);
	if(hashtable[hash_index_up] != NULL)
	{
		insert_flow_up(hash_index_up, tuple_ptr);
	}
	else
	{
		hash_index_down = hash_tuple(tuple_ptr->dip, tuple_ptr->sip, tuple_ptr->dport, tuple_ptr->sport);
		if(hashtable[hash_index_down] != NULL)
		{
			insert_flow_down(hash_index_down, tuple_ptr);
		}
		else  if(tuple_ptr->fin == 0)
		{
			static long i = (unsigned int)0;
			if(list_length < (unsigned int)flows_limit )		//update by hyj
			{
				create_flow(hash_index_up, tuple_ptr);
				i++;
				//printf("%d flows=%d\n",i, list_length);
			}
		}
	}
	pthread_mutex_unlock(&mutex);
	return 0;
}


static int netshark_write_file(netshark_t * netshark_node)
{
	if((1 == netshark_node->netshark_flag) && (0 == netshark_node->file_flag))
	{
		if((netshark_node->netshark_gap <= 0) && (netshark_node->netshark_size <= 0))
		{
			return 0;
		}
		static uint32_t filenu = 1;
		char filename[128] = {0};
		char cmd[1024];
		memset(cmd, 0, sizeof(cmd));
		sprintf(cmd,"cat /usr/local/tipschecker/etc/tipscheck/device.conf  | grep DEVICE_ID | cut -d'=' -f2");
		FILE * fp = popen(cmd, "r");
		memset(cmd, 0, sizeof(cmd));
		fread(cmd, 1, 1024, fp);
		if(cmd[strlen(cmd)-1] == '\n')
		{
			cmd[strlen(cmd)-1] = '\0';
		}
		sprintf(filename,"1001_%010d_%s_%s_%03d.pcap",netshark_node->start_time,cmd,netshark_node->center_device_id,filenu);
		printf("filename=%s\n",filename);
		sprintf(netshark_node->filepath,"%s/%s", PCAP_FILE_PATH, filename);
		printf("filepath=%s\n",netshark_node->filepath);
		netshark_node->dumpfp = pcap_dump_open(_pcap_handle, netshark_node->filepath);
		if(netshark_node->dumpfp == NULL)
		{
			printf("Error on opening output file\n");
			return -1;
		}
		netshark_node->file_flag = 1;
		filenu++;
		if(filenu%1000 == 0)
			filenu = 1;
	}
	if((1 == netshark_node->netshark_flag) && (1 == netshark_node->file_flag))
	{
		uint32_t current_time = time(NULL);
		float now_gap = (current_time - netshark_node->start_time)/60.0;
		//printf("now_gap=%f\n", now_gap);
		if((netshark_node->netshark_gap > 0) && (now_gap >= netshark_node->netshark_gap))
		{
			pcap_dump_close(netshark_node->dumpfp);
			netshark_node->file_flag = 0;
			netshark_node->netshark_flag = 0;
			return 0;
		}
		stat(netshark_node->filepath,&file_stat);
		float now_size = file_stat.st_size/1024.0/1024.0;
		if(netshark_node->netshark_size > 0 && now_size >= (netshark_node->netshark_size-0.2))
		{
			pcap_dump_close(netshark_node->dumpfp);
			netshark_node->file_flag = 0;
			//netshark_node->netshark_flag = 0;
			return 0;
		}
		else
		{
			pcap_dump((u_char*)netshark_node->dumpfp, dump_header, dump_packet);
		//	file_flag = 0;
		//	netshark_flag = 0;
		}
	}
	return 0;
}

int search(uint32_t port, port_node * port_list)
{
	int ret = 0;
	port_node * port_ptr = port_list;
	while(port_ptr !=NULL)
	{
		if(port == port_ptr->port)
		{
			//printf("port=%u\n", port);
			return 1;
		}
		port_ptr = port_ptr->next;	
	}
	ret = ret; 		//hyj 保留变量
	return 0;
}

static int netshark_filter(tuple_t * tuple, netshark_t ** netshark_node)
{
	//printf("netshark_filter\n");
	if(netshark_list == NULL)
	{
		printf("netshark_list == NULL\n");
		return 0;
	}
	netshark_t * netshark_ptr = netshark_list;
	while(netshark_ptr != NULL /*&& netshark_ptr->netshark_flag != 0*/)
	{
//#ifdef NETSHARK_FILTER
		if(netshark_ptr->netshark_flag == 0)
		{
			netshark_ptr = netshark_ptr->next;
			*netshark_node = netshark_ptr;
			continue;
		}
		if(innet_ip(tuple->sip))
		{
			if( netshark_ptr->iip_flag == 0 || (netshark_ptr->iip_flag == 1 && tuple->sip >= netshark_ptr->istartip && tuple->sip <= netshark_ptr->iendip))
			{
	//											printf("iip_flag=%d tuple->sip=%u  netshark_ptr->istartip=%u\n",netshark_ptr->iip_flag,tuple->sip, netshark_ptr->istartip);
				if(netshark_ptr->iport_flag == 0 || (netshark_ptr->iport_flag == 1 && search(tuple->sport,netshark_ptr->iport_list) ))	
				{
	//											printf("iport_flag=%d\n",netshark_ptr->iport_flag);
					if( netshark_ptr->oip_flag == 0 || (netshark_ptr->oip_flag == 1 && tuple->dip >= netshark_ptr->ostartip && tuple->dip <= netshark_ptr->oendip))
					{
	//											printf("oip_flag=%d\n",netshark_ptr->oip_flag);
						if(netshark_ptr->oport_flag == 0 || (netshark_ptr->oport_flag == 1 && search(tuple->dport,netshark_ptr->oport_list) ))	
						{
	//											printf("oport_flag=%d\n",netshark_ptr->oport_flag);
								if(!strcmp(netshark_ptr->protocol,"0"))
								{
//#endif			
									//printf("protocol == 0 netshark_write_file\n");
									netshark_write_file(netshark_ptr);
//#ifdef NETSHARK_FILTER
								}
								else if(tuple->below_protocol == 6 && !strncasecmp(netshark_ptr->protocol,"TCP", 3))
								{
									//printf("protocol == TCP netshark_write_file\n");
									netshark_write_file(netshark_ptr);
								}
								else if(tuple->below_protocol == 17 && !strncasecmp(netshark_ptr->protocol,"UDP", 3))
								{
//#endif
									//printf("protocol == UDP netshark_write_file\n");
									netshark_write_file(netshark_ptr);
//#ifdef NETSHARK_FILTER
								}
						}
					}
				}
			}
		}
		else
		{
			if( netshark_ptr->iip_flag == 0 || (netshark_ptr->iip_flag == 1 && tuple->dip >= netshark_ptr->istartip && tuple->sip <= netshark_ptr->iendip))
			{
	//											printf("iip_flag=%d tuple->sip=%u  netshark_ptr->istartip=%u\n",netshark_ptr->iip_flag,tuple->sip, netshark_ptr->istartip);
				if(netshark_ptr->iport_flag == 0 || (netshark_ptr->iport_flag == 1 && search(tuple->dport,netshark_ptr->iport_list) ))	
				{
	//											printf("iport_flag=%d\n",netshark_ptr->iport_flag);
					if( netshark_ptr->oip_flag == 0 || (netshark_ptr->oip_flag == 1 && tuple->sip >= netshark_ptr->ostartip && tuple->dip <= netshark_ptr->oendip))
					{
	//											printf("oip_flag=%d\n",netshark_ptr->oip_flag);
						if(netshark_ptr->oport_flag == 0 || (netshark_ptr->oport_flag == 1 && search(tuple->sport,netshark_ptr->oport_list) ))	
						{
	//											printf("oport_flag=%d\n",netshark_ptr->oport_flag);
								if(!strcmp(netshark_ptr->protocol,"0"))
								{
//#endif						
									//printf("protocol == 0 netshark_write_file\n");
									netshark_write_file(netshark_ptr);
//#ifdef NETSHARK_FILTER
								}
								else if(tuple->below_protocol == 6 && !strcmp(netshark_ptr->protocol,"TCP"))
								{
									//printf("protocol == TCP netshark_write_file\n");
									netshark_write_file(netshark_ptr);
								}
								else if(tuple->below_protocol == 17 && !strcmp(netshark_ptr->protocol,"UDP"))
								{
//#endif
									//printf("protocol == UDP netshark_write_file\n");
									netshark_write_file(netshark_ptr);
//#ifdef NETSHARK_FILTER
								}

						}
					}
				}
			}
		}
//#endif
		netshark_ptr = netshark_ptr->next;
	}
	*netshark_node = netshark_ptr;
	return 1;
}

static int netshark(const struct pcap_pkthdr * dump_header, const u_char * dump_packet, tuple_t * tuple)
{
	static uint32_t start_time = 0, current_time = 0;
	netshark_t * netshark_node = NULL;
	if( 1 != netshark_filter(tuple, &netshark_node))
	{
		printf("netshark_filter not return 1\n");
		return -1;
	}
#if 0
	int ret = 0;
	if(netshark_node != NULL)
	{
		printf("netshark_write_file start\n");
		ret = netshark_write_file(netshark_node);
	}
	else
	{
		printf("netshrk_node == NULL\n");
	}
#endif
	current_time = current_time; start_time = start_time;//保留变量
	if(dump_header){}; if(dump_packet){};				//保留参数
	return 0;
}

static int packet_audit(const struct iphdr *iph, uint16_t ipsize, u32 protocol, uint16_t rawsize, const uint64_t time_stamp)
{
	tuple_t tuple;
	memset(&tuple, 0, sizeof(tuple_t));
	if(iph->version == 4 && ipsize >= sizeof(struct iphdr))
	{
		if(iph->protocol == 6)
		{
			//get TCP tuple
			struct tcphdr * packet_tcphdr =(struct tcphdr*) (((const u8 *)iph)+iph->ihl*4);
			uint16_t packet_tcpsize = ipsize - (iph->ihl*4);
			if(packet_tcpsize >= sizeof(struct tcphdr))
			{
				tuple.fin = (uint8_t)(packet_tcphdr->fin);
				tuple.sip = htonl(iph->saddr);
				tuple.dip = htonl(iph->daddr);
				tuple.sport = htons(packet_tcphdr->source);
				tuple.dport = htons(packet_tcphdr->dest);
				tuple.app_protocol = protocol;
				tuple.below_protocol = 6;
				tuple.packet_size = rawsize;
				tuple.time_stamp = ts;
#if 0
				printf("fin=%u, sip=%d.%d.%d.%d, dip=%d.%d.%d.%d, sport=%u, dport=%u, app_protocl=%s, packet_size=%u, timestamp=%u\n", 
					tuple.fin, NTOH(tuple.sip), NTOH(tuple.dip), tuple.sport, tuple.dport, 
					prot_long_str[tuple.app_protocol], tuple.packet_size, tuple.time_stamp);
#endif

			}
#if 1
//			printf("tuple.sip=%u\n",tuple.sip);
//			printf("tuple.sport=%u\n",tuple.sport);
//			printf("tuple.dip=%u\n",tuple.dip);
//			printf("tuple.dport=%u\n",tuple.dport);
//			printf("TCP\n");
			if(1 == netshark_flag)
			{
				netshark(dump_header, dump_packet, &tuple);
			}
#endif 
			hash_audit(&tuple);
		}
		if(iph->protocol == 17 && ipsize >= sizeof(struct iphdr))
		{
			//get UDP tuple
			struct udphdr * packet_udphdr = (struct udphdr*)(((const u8 *)iph)+iph->ihl*4);
			uint16_t packet_udpsize = ipsize - (iph->ihl*4);
			if(packet_udpsize >= sizeof(struct udphdr))
			{
				tuple.sip = htonl(iph->saddr);
				tuple.dip = htonl(iph->daddr);
				tuple.sport = htons(packet_udphdr->source);
				tuple.dport = htons(packet_udphdr->dest);
				tuple.app_protocol = protocol;
				tuple.below_protocol = 17;
				tuple.packet_size = rawsize;
				tuple.time_stamp = ts;
			}
#if 0
			printf("tuple.sip=%u\n",tuple.sip);
			printf("tuple.sport=%u\n",tuple.sport);
			printf("tuple.dip=%u\n",tuple.dip);
			printf("tuple.dport=%u\n",tuple.dport);
			if(1 == netshark_flag)
			{
				netshark(dump_header, dump_packet, &tuple);
			}
#endif
			hash_audit(&tuple);
		}
//		hash_audit(&tuple);
	}
	if(time_stamp){};			//保留参数
	return 0;
}



static unsigned int packet_processing(const uint64_t time, const struct iphdr *iph, uint16_t ipsize, uint16_t rawsize)
{
	struct ipoque_id_struct *src = NULL;
	struct ipoque_id_struct *dst = NULL;
	struct osdpi_flow *flow = NULL;
	struct ipoque_flow_struct *ipq_flow = NULL;
	u32 protocol = 0;

	struct osdpi_ip_addr src_ip, dst_ip;
	memset(&src_ip, 0, sizeof(struct osdpi_ip_addr));
	memset(&dst_ip, 0, sizeof(struct osdpi_ip_addr));

#ifdef IPOQUE_DETECTION_SUPPORT_IPV6
	if (iph->version == 6 && ipsize >= sizeof(struct ip6_hdr)) {
		struct ip6_hdr *ip6h = (struct ip6_hdr *)iph;
		src_ip.is_ip_v6 = 1;
		src_ip.ipv6 = ip6h->ip6_src;
		dst_ip.is_ip_v6 = 1;
		dst_ip.ipv6 = ip6h->ip6_dst;
	} else
#endif
	if (iph->version == 4 && ipsize >= sizeof(struct iphdr)) {
		src_ip.ipv4 = iph->saddr;
		dst_ip.ipv4 = iph->daddr;
	} else {
		return 1;
	}

	src = (struct ipoque_id_struct *)get_id(&src_ip);
	dst = (struct ipoque_id_struct *)get_id(&dst_ip);

	flow = get_osdpi_flow(iph, ipsize);
	if (flow != NULL) {
		ipq_flow = flow->ipoque_flow;
	} else {
		return 1;
	}

	ip_packet_count++;
	total_bytes += rawsize;

#ifndef IPOQUE_ENABLE_DEBUG_MESSAGES
#if 0
	if (ip_packet_count % 499 == 0) {
		printf("\rip packets scanned: \x1b[33m%-10llu\x1b[0m ip bytes scanned: \x1b[34m%-10llu\x1b[0m",
			   ip_packet_count, total_bytes);
	}
#endif
#endif

	// only handle unfragmented packets
	if (iph->version == 4 && (iph->frag_off & htons(0x1FFF)) != 0) {

		static u8 frag_warning_used = 0;
		if (frag_warning_used == 0) {
			printf("\n\nWARNING: fragmented ip packets are not supported and will be skipped \n\n");
			sleep(2);
			frag_warning_used = 1;
		}
		return 0;

	} else {

		// here the actual detection is performed
		protocol = ipoque_detection_process_packet(ipoque_struct, ipq_flow, (uint8_t *) iph, ipsize, time, src, dst);

	}

	protocol_counter[protocol]++;
	protocol_counter_bytes[protocol] += rawsize;

	if (flow != NULL) {
		flow->detected_protocol = protocol;
	}
#if 1	
	int error_packet_audit = packet_audit(iph, ipsize, protocol, rawsize, time);
	if(error_packet_audit)
	{
		printf("packet_audit error\n");
	}
#endif
	return 0;
}

	
/*hyj 其中用到的部分全局变量也被注释掉了，如果使用次函数是发现全局变量没有定义，先检查是否被注释了
static void printResults(void)
{
	u32 i;
#if 0
	printf("\x1b[2K\n");
	printf("pcap file contains\n");
	printf("\tip packets:   \x1b[33m%-13llu\x1b[0m of %llu packets total\n", ip_packet_count, raw_packet_count);
	printf("\tip bytes:     \x1b[34m%-13llu\x1b[0m\n", total_bytes);
	printf("\tunique ids:   \x1b[35m%-13u\x1b[0m\n", osdpi_id_count);
	printf("\tunique flows: \x1b[36m%-13u\x1b[0m\n", osdpi_flow_count);
#endif
	printf("\n\ndetected protocols:\n");
	for (i = 0; i <= IPOQUE_MAX_SUPPORTED_PROTOCOLS; i++) {
		u32 protocol_flows = 0;
		u32 j;

		// count flows for that protocol
		for (j = 0; j < osdpi_flow_count; j++) {
			if (osdpi_flows[j].detected_protocol == i) {
				protocol_flows++;
			}
		}

		if (protocol_counter[i] > 0) {
			printf("\t\x1b[31m%-20s\x1b[0m packets: \x1b[33m%-13llu\x1b[0m bytes: \x1b[34m%-13llu\x1b[0m "
				   "flows: \x1b[36m%-13u\x1b[0m\n",
				   prot_long_str[i], protocol_counter[i], protocol_counter_bytes[i], protocol_flows);
		}
	}
	printf("\n\n");
}
*/

/*hyj 其中用到的部分全局变量也被注释掉了，如果使用次函数是发现全局变量没有定义，先检查是否被注释了
static void openPcapFile(void)
{
	_pcap_handle = pcap_open_offline(_pcap_file, _pcap_error_buffer);

	if (_pcap_handle == NULL) {
		printf("ERROR: could not open pcap file: %s\n", _pcap_error_buffer);
		exit(-1);
	}
	_pcap_datalink_type = pcap_datalink(_pcap_handle);
}
*/
static void openPcapLive(void)
{
	char errBuf[PCAP_ERRBUF_SIZE], * device;
//	device = pcap_lookupdev(errBuf);
	device = ether_card;
	if(device)
	{
		printf("success: device: %s\n", device);
	}
	else
	{
		printf("error: %s\n", errBuf);
		exit(1);
	}
	_pcap_handle = pcap_open_live(device, 65535, 1, 0, errBuf);
	_pcap_datalink_type = pcap_datalink(_pcap_handle);
}

/*hyj 其中用到的部分全局变量也被注释掉了，如果使用次函数是发现全局变量没有定义，先检查是否被注释了
static void closePcapFile(void)
{
	if (_pcap_handle != NULL) {
		pcap_close(_pcap_handle);
	}
}
*/

// executed for each packet in the pcap file
static void pcap_packet_callback(u_char * args, const struct pcap_pkthdr *header, const u_char * packet)
{
	const struct ethhdr *ethernet = (struct ethhdr *) packet;
	struct iphdr *iph;
	int vlan_flag = 0;
	u16 type;
//	printf("%0x\n", ntohs(ethernet->h_proto));
	if(0x8100 == ntohs(ethernet->h_proto))
	{
		vlan_flag = 1;
		iph = (struct iphdr *) &packet[sizeof(struct ethhdr) + 4];
		type = (uint16_t)packet[sizeof(struct ethhdr) + 2];
	}
	else 
	{
		iph = (struct iphdr *) &packet[sizeof(struct ethhdr)];
		type = ethernet->h_proto;
	}
	u64 time;
	static u64 lasttime = 0;

	raw_packet_count++;
	ts = header->ts;	
	dump_header =(struct pcap_pkthdr*) header;
	dump_packet =(u_char *)packet;
	time =
		((uint64_t) header->ts.tv_sec) * detection_tick_resolution +
		header->ts.tv_usec / (1000000 / detection_tick_resolution);
	if (lasttime > time) {
		// printf("\nWARNING: timestamp bug in the pcap file (ts delta: %llu, repairing)\n", lasttime - time);
		time = lasttime;
	}
	lasttime = time;


	//type = ethernet->h_proto;

	// just work on Ethernet packets that contain IP
	if (_pcap_datalink_type == DLT_EN10MB &&
		(type == htons(ETH_P_IP)
#ifdef IPOQUE_DETECTION_SUPPORT_IPV6
		|| type == htons(ETH_P_IPV6)
#endif
		)
		&& header->caplen >= sizeof(struct ethhdr)) {

		if (header->caplen < header->len) {
			static u8 cap_warning_used = 0;
			if (cap_warning_used == 0) {
				printf
					("\n\nWARNING: packet capture size is smaller than packet size, DETECTION MIGHT NOT WORK CORRECTLY OR EVEN CRASH\n\n");
				sleep(2);
				cap_warning_used = 1;
			}
		}

		if (0 == vlan_flag && header->len >= (sizeof(struct ethhdr) + sizeof(struct iphdr))) 
		{
			//printf("%0x\n", htons(ethernet->h_proto));
			packet_processing(time, iph, header->len - sizeof(struct ethhdr), header->len);
		}
		else if (1 == vlan_flag && header->len >= (sizeof(struct ethhdr) + 4 + sizeof(struct iphdr)))
		{
			//printf("%0x VLAN\n", htons(ethernet->h_proto));
			packet_processing(time, iph, header->len - sizeof(struct ethhdr) - 4, header->len);
		}
	}
	args = args;			//保留参数
}

static void runPcapLoop(void)
{
	if (_pcap_handle != NULL) {
		pcap_loop(_pcap_handle, -1, &pcap_packet_callback, NULL);
	}
}


//static void* traverse_timeout(void* data)
void* traverse_timeout(void* data)
{
//	printf("scan thread\n");
	uint32_t hash_index_up;
	audit_flow_t * temp_ptr = NULL;
	audit_flow_t * temp_ptr_next_flow = NULL;
	static int i=0;
	int ret = -1;
	tuple_t tuple;
	memset(&tuple, 0, sizeof(tuple));
	while(hashtable != NULL)
	{
		i = 1;
//	printf("scan thread\n");
	pthread_mutex_lock(&mutex);
		if(hashtable == NULL)
			break;
		for(temp_ptr = list_head; temp_ptr != NULL;i++)
		{	
			temp_ptr_next_flow = temp_ptr->next_flow;
		//	printf("temp_ptr=%d\n", temp_ptr);
			time_t t=time(NULL)-(temp_ptr->end_time.tv_sec);
		//	if(t<0)  exit(0);
			if(temp_ptr->end_time.tv_sec != 0  && t >= FLOW_OUTTIME)
			{
	//			printf("\nlocate=%d hashtable_used=%d\n",i, hashtable_used);
	//			printf("timeout=%u,,,,\n",time(NULL)-(temp_ptr->end_time.tv_sec));
		//		printf("thread delete flow\n");
				tuple.sip = temp_ptr->sip;
				tuple.dip = temp_ptr->dip;
				tuple.sport = temp_ptr->sport;
			//	printf("sport d :%u\n", temp_ptr->sport);
				tuple.dport = temp_ptr->dport;
				hash_index_up = hash_tuple(tuple.sip, tuple.dip, tuple.sport, tuple.dport);
			//	exportDB(temp_ptr);
				ret = delete_flow(hash_index_up, &tuple);
			}
			fflush(stdout);
			if(ret == 0) 
			{
				temp_ptr = temp_ptr_next_flow;
			}
			else 
			{
				temp_ptr = temp_ptr->next_flow;
			}
		}
	pthread_mutex_unlock(&mutex);
		sleep(SCAN_GAP);
	}
	printf("hashtable is not exist, scan thread end\n");
	fflush(stdout);
	
	data = data;		//保留参数

	return (void*)0;
}

static int scanhashtable()
{
	int  ret = -1;
	pthread_t thread_id;
	ret = pthread_create(&thread_id, NULL, traverse_timeout, (void*)&ret);
	if(ret == 0)
	{
		printf("create thread of scanhashtable\n");
		return 0;
	}
	return 0;
}

/*hyj 其中用到的部分全局变量也被注释掉了，如果使用次函数是发现全局变量没有定义，先检查是否被注释了
static int create_server_sock()
{
	struct sockaddr_in s_addr;
	unsigned int  listnum;
	int sockfd;
	int opt=1;
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		printf("socket error!\n");
		exit(0);
	}
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	listnum=3;
	bzero(&s_addr, sizeof(s_addr));
	s_addr.sin_family = AF_INET;
	s_addr.sin_port = htons(54321);
	s_addr.sin_addr.s_addr = INADDR_ANY;
	if((bind(sockfd, (struct sockaddr *)&s_addr, sizeof(struct sockaddr_in))) == -1)
	{
		printf("bind error!\n");
		exit(0);
	}
	if(listen(sockfd, listnum) == -1)
	{
		printf("listen error!\n");
		exit(0);
	}

	return sockfd ;

}
*/

/*hyj 其中用到的部分全局变量也被注释掉了，如果使用次函数是发现全局变量没有定义，先检查是否被注释了
static int accept_rule(int sockfd, IPLIST * rule)
{
	struct sockaddr_in c_addr;
	socklen_t len = 0;
	char buf[96*MAX_RULE_NUM];
	int connect_host;
	int i,byte = 0;
	int num = 0;
	len = sizeof(struct sockaddr_in);
	if((connect_host = accept(sockfd, (struct sockaddr *)&c_addr, &len)) != -1)
	{
		printf("\n*****************通信开始***************\n");
		printf("正在与您通信的客户端是：%s: %d\n",inet_ntoa(c_addr.sin_addr),ntohs(c_addr.sin_port));
		byte = recv(connect_host, buf, sizeof(buf), 0);
		if(byte <= 0)
			num = 0;
		else
			num = byte/sizeof(IPLIST);
		if(num > MAX_RULE_NUM)
		{
			printf("more than max_rule_num,truncate rule\n");
			num = MAX_RULE_NUM;
		}
		for(i=0; i<num; i++)
			memcpy(&rule[i], buf + i*sizeof(IPLIST), sizeof(IPLIST));
#if 0
		for(i=0; i<num; i++)
		{
			rule[i].start_ip = ntohl(rule[i].start_ip);
			rule[i].end_ip = ntohl(rule[i].end_ip);
			rule[i].port = ntohs(rule[i].port);
		}
#endif
		printf("byte: %d, num: %d\n",byte, num);	
		close(connect_host);
	}
	return num;
}
*/

static int compare(const void * val1, const void * val2)
{
	IPLIST * rule1 = (IPLIST *) val1;
	IPLIST * rule2 = (IPLIST *) val2;
	if(rule1->start_ip < rule2->start_ip)
		return -1;
	else if(rule1->start_ip > rule2->start_ip)
		return 1;
	else 
		return 0;
}

static int sort_rule(IPLIST * rule, size_t num)
{
	qsort(rule, num, sizeof(IPLIST), compare);
	return 0;
}

static int sync_rule(IPLIST * rule, int num)
{
	tactics_flag = 1;
	pthread_mutex_lock(&mutex_rule);
	memcpy(ordered_rule, rule, num*sizeof(IPLIST));
	rule_num = num;
	pthread_mutex_unlock(&mutex_rule);
	printf("sync_rule successful\n");

	return 0; 		//返回值？
}

uint32_t get_hostmask(int bit)
{
	uint32_t mask = 1;
	int i = 0;
	for(i=1; i<bit; i++)
	{
		mask = mask<<1;
		mask++;
	}
	return mask;
}

uint32_t get_netmask(int bit)
{
	uint32_t mask = 0xffffffff;
	uint32_t hostmask = get_hostmask(32-bit);
	mask = mask ^ hostmask;
	return mask;
}

int parse_iip(const char * optarg, netshark_t * netshark_ptr)
{
	char buffer[32] = {0};
	strcpy(buffer,optarg);
	printf("optarg=%s\n", optarg);
	if(0 == strncmp("0", optarg, 1))
	{
		netshark_ptr->iip_flag = 0;
		return 0;
	}
	uint32_t netmask,hostmask,ip;
	char * c_ptr = strstr(buffer, "/");
	printf("c_ptr=%s\n",c_ptr);
	if(c_ptr != NULL)
	{
		int mask_bit = atoi(++c_ptr);
		printf("mask_bit=%d\n",mask_bit);
		netmask = get_netmask(mask_bit);
		hostmask = get_hostmask(32-mask_bit);
		char * ipstr = strtok(buffer, "/");
		printf("ipstr=%s\n",ipstr);
		ip = inet_addr(ipstr);
		ip = ntohl(ip);
		uint32_t netip = ip & netmask;
		printf("ip=%08x\n",ip);
		printf("netmask=%08x\n",netmask);
		printf("hostmask=%08x\n",hostmask);
		printf("netip=%08x\n",netip);
		if((ip | netip) == netip)
		{
			printf("ip == netip\n");
			netshark_ptr->istartip = netip;
			printf("istartip=%08x\n",netshark_ptr->istartip);
			netshark_ptr->iendip = netip + hostmask;
			printf("iendip=%08x\n",netshark_ptr->iendip);
		}
		else
		{
			netshark_ptr->istartip = ip;
			printf("istartip=%08x\n",netshark_ptr->istartip);
			netshark_ptr->iendip = ip;
			printf("iendip=%08x\n",netshark_ptr->iendip);
		}
	
	}
	else
	{
		ip = inet_addr(buffer);
		ip = ntohl(ip);
		netshark_ptr->istartip = ip;
		printf("istartip=%08x\n",netshark_ptr->istartip);
		netshark_ptr->iendip = ip;
		printf("iendip=%08x\n",netshark_ptr->iendip);
	}
	netshark_ptr->iip_flag = 1;
	return 0;	
}

int parse_oip(const char * optarg, netshark_t * netshark_ptr)
{
	char buffer[32] = {0};
	strcpy(buffer,optarg);
	if(0 == strncmp("0", optarg, 1))
	{
		netshark_ptr->oip_flag = 0;
		return 0;
	}
	uint32_t netmask,hostmask,ip;
	char * c_ptr = strstr(buffer, "/");
	if(c_ptr != NULL)
	{
		printf("c_ptr=%s\n",c_ptr);
		int mask_bit = atoi(++c_ptr);
		printf("mask_bit=%d\n",mask_bit);
		netmask = get_netmask(mask_bit);
		hostmask = get_hostmask(32-mask_bit);
		char * ipstr = strtok(buffer, "/");
		printf("ipstr=%s\n",ipstr);
		ip = inet_addr(ipstr);
		ip = ntohl(ip);
		uint32_t netip = ip & netmask;
		printf("ip=%08x\n",ip);
		printf("netmask=%08x\n",netmask);
		printf("hostmask=%08x\n",hostmask);
		printf("netip=%08x\n",netip);
		if((ip | netip) == netip)
		{
			printf("ip == netip\n");
			netshark_ptr->ostartip = netip;
			printf("ostartip=%08x\n",netshark_ptr->ostartip);
			netshark_ptr->oendip = netip + hostmask;
			printf("oendip=%08x\n",netshark_ptr->oendip);
		}
		else
		{
			netshark_ptr->ostartip = ip;
			printf("ostartip=%08x\n",netshark_ptr->ostartip);
			netshark_ptr->oendip = ip;
			printf("oendip=%08x\n",netshark_ptr->oendip);
			
		}
		
	}
	else
	{
		ip = inet_addr(buffer);
		ip = ntohl(ip);
		netshark_ptr->ostartip = ip;
		printf("ostartip=%08x\n",netshark_ptr->ostartip);
		netshark_ptr->oendip = ip;
		printf("oendip=%08x\n",netshark_ptr->oendip);
	}
	netshark_ptr->oip_flag = 1;
	return 0;	
}

int parse_iport(const char * optarg, netshark_t * netshark_ptr)
{
	char buffer[1024] = {0};
	strcpy(buffer, optarg);
	if(netshark_ptr == NULL)
		return -1;
	if(0 == strncmp("0", optarg, 1))
	{
		netshark_ptr->iport_flag = 0;
		return 0;
	}
	port_node * port_ptr = NULL;
	netshark_ptr->iport_list =(port_node *) calloc(1, sizeof(port_node));
	port_ptr = netshark_ptr->iport_list;
	char * token = strtok(buffer, ",");
	port_ptr->port =(uint16_t) atoi(token); 
	while((token=strtok(NULL, ",")) != NULL)
	{
		port_ptr->next =(port_node *) calloc(1, sizeof(port_node));
		port_ptr = port_ptr->next;
		port_ptr->port =(uint16_t) atoi(token); 
	}
	port_ptr = netshark_ptr->iport_list;
	while(port_ptr != NULL)
	{
		printf("%u\n",port_ptr->port);
		port_ptr = port_ptr->next;
	}
	netshark_ptr->iport_flag = 1;
	return 0;	
}

int parse_oport(const char * optarg, netshark_t * netshark_ptr)
{
	char buffer[1024] = {0};
	strcpy(buffer, optarg);
	if(netshark_ptr == NULL)
					return -1;
	if(0 == strncmp("0", optarg, 1))
	{
		netshark_ptr->oport_flag = 0;
		return 0;
	}
	port_node * port_ptr = NULL;
	netshark_ptr->oport_list =(port_node *) calloc(1, sizeof(port_node));
	port_ptr = netshark_ptr->oport_list;
	char * token = strtok(buffer, ",");
	port_ptr->port =(uint16_t) atoi(token); 
	while((token=strtok(NULL, ",")) != NULL)
	{
		port_ptr->next =(port_node *) calloc(1, sizeof(port_node));
		port_ptr = port_ptr->next;
		port_ptr->port =(uint16_t) atoi(token); 
	}
	port_ptr = netshark_ptr->oport_list;
	while(port_ptr != NULL)
	{
		printf("%u\n",port_ptr->port);
		port_ptr = port_ptr->next;
	}
	netshark_ptr->oport_flag = 1;
	return 0;	
}

int parse_protocol(const char * optarg, netshark_t * netshark_ptr)
{
	strncpy(netshark_ptr->protocol, optarg, 3);
	printf("protocol=%s\n",netshark_ptr->protocol);
	return 0;	
}

int parse_size(const char * optarg, netshark_t * netshark_ptr)
{
	netshark_ptr->netshark_size = atof(optarg);
	printf("size=%f\n",netshark_ptr->netshark_size);
	return 0;	
}


int parse_time(const char * optarg, netshark_t * netshark_ptr)
{
	netshark_ptr->netshark_gap =atof(optarg);
	printf("gap=%f\n",netshark_ptr->netshark_gap);

	return 0;	
}

int parse_cdid(const char * optarg, netshark_t * netshark_ptr)
{
	strncpy(netshark_ptr->center_device_id, optarg,10);
	printf("center_device_id=%s\n",netshark_ptr->center_device_id);
	return 0;	
}

int parsecmd(const char* cmdstr)
{
	printf("parsecmd start  cmdstr==%s\n", cmdstr);
	//printf("cmdstr len=%d\n", strlen(cmdstr));
	char cmdbuf[1024];
	char* ptr = NULL;
	char* tail_ptr = NULL;
	int offset = 0, c = 0;	
	char argv[3];
		
	argv[1] = argv[1];	offset = offset;		//hyj 保留变量

	memset(cmdbuf, 0, sizeof(cmdbuf));
	//printf("cmdbuf==%s\n", cmdstr);
	strcpy(cmdbuf, cmdstr);
	//cmdbuf[strlen(cmdbuf)] = '\0';
	//printf("cmdbuf==%s\n", cmdbuf);
	ptr = cmdbuf;

	netshark_t * netshark_ptr = netshark_list, * netshark_qtr = NULL;
#if 1
	if(netshark_list == NULL)
	{
		netshark_list = calloc(1,sizeof(netshark_t));
		netshark_ptr = netshark_list;
	}
	else
	{
		while(netshark_ptr != NULL)
		{	
			netshark_qtr = netshark_ptr;
			netshark_ptr = netshark_ptr->next;
		}
		netshark_ptr = calloc(1,sizeof(netshark_t));
		netshark_qtr->next = netshark_ptr;
	}
	//printf("cmdstr len=%d\n", strlen(cmdstr));
#endif
	while(' ' == *ptr)
	{
		ptr++;
	}
	tail_ptr = ptr + strlen(ptr) - 1;
	while(' ' == *tail_ptr)
	{
		*tail_ptr-- = '\0';
	}
	if(!strncmp(ptr, "netshark",8))
	{
		char* temp_v[30]={0};
		memset(temp_v, 0, sizeof(temp_v));
		int temp_c = 1;
		char * delim =" ";
		//	printf("%s\n",ptr);
		char* temp_str = strtok(ptr,delim);
		//	printf("temp_str=%s\n",temp_str);
		while((temp_str=strtok(NULL,delim)) != NULL)
		{
		//	printf("temp_v[%d]=%s\n",temp_c, temp_str);
			temp_v[temp_c]= temp_str;
			//printf("temp_v[%d]=%s\n",temp_c, temp_v[temp_c]);
			temp_c++;
			if(temp_c >= 30)
				printf("temp_v flowout\n");
		}
	//	printf("temp_c=%d\n",temp_c);
#if 1
		optarg = NULL;
		optind = 0, opterr = 0, optopt = 0;
		while(1)
		{
			int option_index = 0;
			static struct option long_options/*hyj 初始化格式错误？ = {0}*/;
			c = getopt_long(temp_c, (char* const *)temp_v, "i:o:n:w:p:s:t:g:", (const struct option*) &long_options, &option_index);
			if(c == -1)
			{
				netshark_flag = 1;
				netshark_ptr->netshark_flag = 1;
				netshark_ptr->start_time = time(NULL);
				break;
			}
			switch(c)
			{
				case 'i':
						printf("i with arg %s\n", optarg);
						parse_iip(optarg, netshark_ptr);
						break;
				case 'o':
						printf("o with arg %s\n", optarg);
						parse_oip(optarg, netshark_ptr);
						break;
				case 'n':
						printf("n with arg %s\n", optarg);
						parse_iport(optarg, netshark_ptr);
						break;
				case 'w':
						printf("w with arg %s\n", optarg);
						parse_oport(optarg, netshark_ptr);
						break;
				case 'p':
						printf("p with arg %s\n", optarg);
						parse_protocol(optarg, netshark_ptr);
						break;
				case 's':
						printf("s with arg %s\n", optarg);
						parse_size(optarg, netshark_ptr);
						break;
				case 't':
						printf("t with arg %s\n", optarg);
						parse_time(optarg, netshark_ptr);
						break;
				case 'g':
						printf("g with arg %s\n", optarg);
						parse_cdid(optarg, netshark_ptr);
						break;
				default:
						break;
			}
		}
	}
	else if(!strncmp(ptr, "stopnetshark",12))
	{
//		free_all(netshark_list);
		netshark_flag = 0;
	}

#endif
	return 0;			//返回值？
}
int Init_accept_server()
{
	unlink(UNIX_DOMAIN);
	int chksock = 0;
	chksock=socket(PF_UNIX,SOCK_STREAM,0);
	if(chksock<0)
	{   
		close(chksock);
		exit(0);
	}
    struct sockaddr_un saddr;
    saddr.sun_family=AF_UNIX;
    strncpy(saddr.sun_path,UNIX_DOMAIN,sizeof(saddr.sun_path)-1);
    int err=bind(chksock,(struct sockaddr*)&saddr,sizeof(saddr));
    if(err<0)
    {
          close(chksock);
          unlink(UNIX_DOMAIN);
          exit(0);
    }	
    err=listen(chksock,1);
    if(err<0)
    {
          close(chksock);
          unlink(UNIX_DOMAIN);
          exit(0);
    }
	printf("Init_accept_server finished\nserver_sock = %d\n",chksock);
	return chksock;
}

int accept_policy(int server_sock,char * cmdbuff, int* len)
{
//	char cmdbuff[1024*1024*11];
    int cmdlen = 0;
    int getlen = 0;
    int cnt = 0;
      int mngsock = 0;
    struct sockaddr_un daddr;
    socklen_t socklen=sizeof(daddr);
    mngsock=accept(server_sock,(struct sockaddr*)&daddr,&socklen);
    if(mngsock<0)
    {
		close(mngsock); //20130410PM
      	return -1;
    }
    int ct = read (mngsock, (int*)&getlen, sizeof (getlen));
    cmdlen = 0;
    cnt = 0;
    while(cmdlen < getlen)
    {
#if 1
	  	char buff[1024*200]; 
 		memset (buff, 0, sizeof (buff));
#endif 
	    if ((unsigned int)getlen < sizeof (buff))		//update by hyj
		      cnt = read(mngsock, buff, getlen - cmdlen);
	    else
	      cnt = read(mngsock, buff, sizeof (buff));
		memcpy (cmdbuff+cmdlen, buff, cnt);
	    cmdlen += cnt;
	}
	*len = cmdlen;

	ct = ct;		//hyj 保留变量
			
	return 0;
}

static void accept_tactics()
{
	int server_sock = Init_accept_server();
	char buf[1024*1024*7];
	memset(buf, 0, sizeof(buf));
	int len = 0;
#if 1
	int i, ret, byte = 0, num = 0;
	int sockfd;
	IPLIST rule[MAX_RULE_NUM];
//	sockfd = create_server_sock();
	while(1)
	{
		ret = -1;
		memset(rule, 0, sizeof(rule));
		memset(buf, 0, sizeof(buf));
		ret = accept_policy(server_sock, buf, &len);
//	   	num = accept_rule(sockfd, rule);
#if 1
		if(0 == ret)
		{
			int i = 0;
			printf("%s\n", buf);
			if(!strncmp(buf,"L+3",3))
			{
				char num_str[10] = {0};
				char* p = strstr(buf,"{");
				char* q = strstr(buf,"}");
				memcpy(num_str,p+1,q-p-1);
				num_str[strlen(num_str)] = '\0';
				num = atoi(num_str);
				printf("num=%d\n", num);
				int offset = 6+strlen(num_str);
				printf("offset=%d\n",offset);
				printf("%s\n", buf+offset);
#if 1
				if(num > MAX_RULE_NUM)
				{
						printf("MAX_RULE_NUM too small\n");
						continue;
				}
				memcpy(rule,(IPLIST*)(buf+offset),num*sizeof(IPLIST));
				if(num <= 0)
				{
					printf("Audit: no rule\n");
					continue;
				}
				ret = sort_rule(rule, num);
				if(ret == -1)
				{
					printf("Audit: sort_rule failed\n");
				}
				ret = sync_rule(rule, num);
				if(ret == -1)
				{
					printf("Audit: sync_rule failed\n");
				}
				i = 1;
				Set_status_shm(shm,30,&i,sizeof(i));
#endif
			}
			else if(!strncmp(buf,"C+11",4) || !strncmp(buf,"C+12",4))
			{
				char* cmdstr = NULL;
				cmdstr = buf+7;
				printf("cmdstr==%s\n", cmdstr);
				parsecmd(cmdstr);
				//printf("parsecmd end cmdstr==%s\n", cmdstr);
			}
		}
#endif
	}
	close(sockfd);
	
	i = i; byte = byte;			//保留变量
#endif
//		char * cmdstr = "netshark -i 192.168.0.0/24 -o 202.101.22.21/24 -n 21,433,80 -w 15,53,20 -p UDP -s 1 -t 1 -g 1203010088";
//		char * cmdstr = "netshark -i 172.16.0.0/24 -o 172.16.0.0/24 -p TCP -s 100 -t 1 -g 1203010088";
}


static void accepttactics()
{
	int  ret = -1;
	pthread_t thread_id;
	ret = pthread_create(&thread_id, NULL, (void*)accept_tactics, NULL);
	if(ret == 0)
	{
		printf("create thread of accepttactics\n");
	}
}

static int createhashtable()
{
	hashtable =(audit_flow_t**)calloc(HASHTABLE_LEN, sizeof(audit_flow_t*));
	if(hashtable == NULL)
	{
		printf("create hashtable failed\n");
		return -1;
	}
	else
	{
		printf("create hashtable\n");
		return 0;		//hyj返回值？
	}
}

/*hyj 其中用到的部分全局变量也被注释掉了，如果使用次函数是发现全局变量没有定义，先检查是否被注释了
static int removehashtable()
{
	uint32_t i = 0;
	audit_flow_t * temp_ptr = NULL;
	tuple_t tuple;
	memset(&tuple, 0, sizeof(tuple));
	pthread_mutex_lock(&mutex);
	for(i=0; i<HASHTABLE_LEN; i++)
	{
		if(hashtable[i] != NULL)
		{	temp_ptr = hashtable[i];
			while(temp_ptr != NULL)
			{	
				tuple.sip = temp_ptr->sip;
				tuple.dip = temp_ptr->dip;
				tuple.sport = temp_ptr->sport;
				tuple.dport = temp_ptr->dport;
				delete_flow(i, &tuple);
				temp_ptr = temp_ptr->next;
			}
		}
	}
	if(hashtable != NULL)
	{
		free(hashtable);
		hashtable = NULL;
		printf("remove hashtable\n");
	}
	pthread_mutex_unlock(&mutex);

	return 0; 		//返回值?
}
*/

static void WtPid()
{
	FILE *fp;
	char id_buf[10];
	pid_t pid;
	memset(id_buf,0,10);
	pid = getpid();
    dzlog_info("Audit start now,pid==%d", pid);
	sprintf(id_buf,"%d",pid);
	if(NULL == (fp = fopen((char *)"/usr/local/tipschecker/etc/tipscheck/auditpid","w+")))
	{
		perror("fopen");
		return;
	}
	fwrite(id_buf,strlen(id_buf),1,fp);
	fclose(fp);
	return;
}

int init_netlog()
{
	int ret = Init_shm(&shm);
	if(-1 == ret){
		dzlog_info("Init_shm failed");
		return -1;
	}
//	parseOptions(argc, argv);
	setupDetection();
//	openPcapFile();
	flows_limit = get_limit();
	createhashtable();
	accepttactics();
	scanhashtable();
	Init_Client("127.0.0.1");

	return 0;	//返回值?
}


void handle_func(int sig)
{
	dzlog_info("tipsAudit: i receive the signal and sleep 30 now!\n");
    sleep(30);
    dzlog_info("i wake up begin to work");
}

int start_netlog()
{
	openPcapLive();	
	runPcapLoop();
	return 0;		//返回值? 
}

int end_netlog()
{
	Exit_Client();
//	sleep(5);
//	removehashtable();
//	closePcapFile();
//	printResults();
	terminateDetection();
	
	return 0;		//返回值?
}

int main(int argc, char **argv)
{
	if(argc <2)
	{
		printf("Usage: %s \"<netcard>\"\n", argv[0]);
		return 0;
	}
	strncpy(ether_card, argv[1], strlen(argv[1]));

    //zlog init
    if(dzlog_init("/usr/local/tipschecker/etc/log.conf", "Audit")){
        printf("dzlog init failed\n");
    }
    WtPid();

    signal(SIGUSR1, handle_func);
	
	init_netlog();
	start_netlog();
	end_netlog();

	return 0;
}
