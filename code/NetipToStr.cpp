#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
using namespace std;

int main(int argc, char ** argv)
{
	if(argc < 3)
	{
		printf("Usage:%s -b netip -s netstr\n", argv[0]);
		return 0;
	}
	int ch;
	opterr = 0;
	while((ch = getopt(argc, argv,"b:s:")) != -1)
	switch(ch)
	{
		case 'b':
				{
					struct in_addr ip = {0};
					char ipstr[32] = {0};
					uint32_t  netip =  (uint32_t)strtoul(optarg, NULL, 10);
					printf("%0x\n",netip);
					ip.s_addr = htonl(netip);
					inet_ntop(AF_INET, &ip, ipstr, sizeof(ipstr));
					printf("ipstr: %s\n", ipstr);
				}
				break;
		case 's':
				{
					struct in_addr ip = {0};
					char ipstr[32] = {0};
					strncpy(ipstr, optarg, sizeof(ipstr)-1);
					inet_pton(AF_INET, ipstr, &ip);
					ip.s_addr = htonl(ip.s_addr);
					//printf("netip: %0x\n", ip.s_addr);
					printf("netip: %u\n", ip.s_addr);
				}
				break;
		default :
				break;
			
	}
}
