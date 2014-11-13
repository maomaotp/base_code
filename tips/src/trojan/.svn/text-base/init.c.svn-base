#include "init.h"
#include "funclist.h"

extern struct func *head;

void init()
{
	signal(SIGALRM, clearhash);
	alarm(180);
	readrule();
	readIPfile();

	addfunc();
	initpacphead();
	createHashTable();
	createunkown();
	
	char localIP[] = "127.0.0.1";
	int err=Init_Client(localIP);
    if(err<0){
        dzlog_info("init client failed!!!");
		return;
	}
	return;
}

void trojan_process(const struct pcap_pkthdr *hdr,const u_char *bytes)
{
	struct iphdr *ip=NULL;
	ip=(struct iphdr*)(bytes+14);
	          
	if(ip->protocol != 6){
		//dzlog_info("protocol is not TCP");
		return;
	}
	head->process(hdr,bytes);
	head->next->process(hdr, bytes);
}
