#ifndef _GETMAC_H
#define _GETMAC_H
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <map>
#include <cstdio>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdint.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "arp.h"

using namespace std;

struct mac
{
	char mac[20];
};

class CGetMac
{
	map <uint32_t, struct mac> mac_map;
	void getmac(char* device,uint32_t ip, char* mac);
public:
		void ip_change_mac (uint32_t ip, char* mac);
};
#endif
