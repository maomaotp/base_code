#ifndef MANAGECP_IP_H
#define MANAGECP_IP_H
#include <iostream>
#include <stdio.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>

#ifndef CONFIG_FILE
#define CONFIG_FILE "/usr/local/tipschecker/etc/tipscheck/config"
#endif

using namespace std;


class CManageCP_IP{
public:
	CManageCP_IP();
	int read_manage_ip();
	void getlargeIP(char *source);
	int judge_login_ip(uint32_t ip);
private:
	uint32_t startIP;
	uint32_t largeIP;
};

#endif
