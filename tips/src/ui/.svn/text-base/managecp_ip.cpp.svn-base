#include "managecp_ip.h"
CManageCP_IP::CManageCP_IP()
{
	startIP=0;
	memset(&largeIP, 255, sizeof(largeIP));
}


int CManageCP_IP::read_manage_ip()//读出管理机IP地址,就是可以控制某一个网段的机器可以登录格式为：172.16.0.210/24，则只有在IP210和220之前的ip可以登录界面进行管理。
{
	FILE* fp = NULL;
	fp = fopen(CONFIG_FILE,"r");
	if(fp == NULL){
		return 1;
	}
	
	char buf[1024];
	memset(buf, 0, 1024);
	while(NULL != fgets(buf, 1023, fp)){
		if(!strncmp("SUPERVISOR", buf, 10)){
			getlargeIP(buf+11);
			fclose(fp);
			return 0;
		}
	}
	startIP = 0;
	memset(&largeIP, 255,sizeof(largeIP));
	fclose(fp);
	return 1;
}

int my_pow(int num,int n)
{
		int sum = 1;
		int i = 1;
		for(i = 1;i <= n;i++)
		{
				sum *= num;
		}
		return sum;
}

void CManageCP_IP::getlargeIP(char *source)
{
		int amount = 0;
		int endip = 0;
		int yanma = 0;
		char *tmp = 0;
		char *tmp2 = 0;
		char *fl = strchr(source,'/');
		if((tmp = strtok(source,"/")))
		{
				if(inet_pton(AF_INET,tmp,&startIP))
				{
						startIP = ntohl(startIP);
						if(fl && (tmp2 = strtok(NULL,"/")))
						{
								amount = atoi(tmp2);	
								endip = (my_pow(2,(32 - amount)) - 1);
								yanma = ~endip;
								largeIP = ((startIP) & ((yanma))) | (endip);
								return;
						}
						else 
						{
								largeIP = startIP;
						}
				}
		}
		return;
}


/*0表示可以登录，1表示不能登录*/
int CManageCP_IP::judge_login_ip(uint32_t ip)
{
	printf("startIP====wwwuuuu%u,%u\n",startIP,largeIP);
	if((ip >= startIP) && (ip <= largeIP)){
		return 0;
	}
	return 1;

}
