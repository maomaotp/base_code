#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <pthread.h>
#include <netinet/ip.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>
#include <netinet/if_ether.h>
#include <libnet.h>
#include <pcap.h>
#include <signal.h>

#include "zlog.h"
#include "netblock.h"

#define UNIX_DOMAIN "/tmp/domain.block"
#define DEBUG


blockinfo* block;
static libnet_t *net=NULL;
pcap_t *handle=NULL;

void WtPid()
{
	FILE *fp;
	char id_buf[10];
	pid_t pid;
	memset(id_buf,0,10);
	pid = getpid();
	sprintf(id_buf,"%d",pid);
	if(NULL == (fp = fopen("/usr/local/tipschecker/etc/tipscheck/netblockid","w+")))
	{
		dzlog_info("fopen file error!!!");
		return;
	}
	fwrite(id_buf,strlen(id_buf),1,fp);
	fclose(fp);
	
	dzlog_info("netblick start now pid:%d", pid);
	return;
}

void handle_func(int sig)
{
	dzlog_info("netblock: i received the signal and sleep 30 now!\n");
    sleep(30);
    dzlog_info("i wake up begin to work");
}

/*********************************liuq*****************************************/

int ip_Packet(const u_char *packet)
{
   struct ether_header *ether=NULL;
   ether=(struct ether_header *)packet;

    if( htons(ether->ether_type) == ETHERNET_TYPE_IP )
        return 1;
    else
        return 0;
}


void init_net()
{
    char errormess[64]={0};
    
    net=libnet_init(LIBNET_RAW4,"eth0",errormess);
    if(!net)
    {
        printf("libnet error\n");
        exit(-1);
    }
    //signal(SIGALRM,my_alarm);
    //alarm(2);
}

void reset_block()
{
    block->srcip = 0;
    block->dstip = 0;
}

void reset_tcp(struct iphdr *ip,struct tcphdr *tcp)
{
#if 0
    init_net();
    libnet_clear_packet(net);
    libnet_build_tcp(ntohs(tcp->dest),ntohs(tcp->source),ntohl(tcp->ack_seq),0,TH_RST,0,0,0,LIBNET_TCP_H ,NULL,0,net,0);
    libnet_build_ipv4(LIBNET_IPV4_H + LIBNET_TCP_H ,0,242,0,64,IPPROTO_TCP,0,ip->daddr,ip->saddr,NULL,0,net,0);
    libnet_write(net);
#endif
    kill(0, SIGUSR2);
#ifdef DEBUG
    printf("send reset ok");
#endif
}

void mycallback(u_char *user,const struct pcap_pkthdr *h,const u_char *data)
{
    struct iphdr *ip=NULL;

    if(ip_Packet(data) == 0) {
        return;
    }
      
    ip=(struct iphdr *)(data + sizeof(struct ether_header));

#ifdef DEBUG
    char buf_sip[24];
    char buf_dip[24];
    printf(">>>>>>>ip packet:\n");
    inet_ntop(AF_INET,&(ip->saddr), buf_sip, 24);
    printf("src_ip:%u-----%s\n", ntohl(ip->saddr), buf_sip);
    
    inet_ntop(AF_INET,&(ip->daddr), buf_dip, 24);
    printf("dst_ip:%u-----%s\n", ntohl(ip->daddr), buf_dip);
#endif
  
    if(ip->protocol == IPPROTO_TCP) {
         struct tcphdr *tcp=NULL;
         tcp=(struct tcphdr *)(data + sizeof(struct ether_header)+ip->ihl*4);

#ifdef DEBUG
         printf(">>tcp packet******************************:\n");
#endif
         if(block->srcip == 0) {
             return;
         }
         if( block->srcip == ntohl(ip->saddr) || block->srcip == ntohl(ip->daddr) ) {
#ifdef DEBUG
             printf("222222222222222\n");
#endif
             reset_tcp(ip, tcp);
             //initAche_block_t(&block_info, &now, ip, tcp);
         }
    }
}

void processOrder(int *temp,const char *str,char *p[])
{
    int counter=0;
    char *pos=NULL;

    char *inter=(char*)str;
    char *btr=(char*)str;
    char *postemp=NULL;

    while((pos=strchr(inter,' '))) {
        *pos='\0';
        p[counter]=btr;
    	counter++;
    	pos++;
    	btr=pos;
    	inter=pos;
    	postemp=pos;
    }

    p[counter]=postemp;

    *temp=counter+1;
}

void close_loop(int sig)
{
#ifdef DEBUG
    printf("recv the signal start func: pcap_loop\n");
#endif
    if(handle == NULL) {
        printf("error the handle is NULL!!\n");
        return;
    }
    pcap_breakloop(handle);
#ifdef DEBUG
    printf("break and close complete\n");
#endif
}

void start_loop(int sig)
{
#ifdef DEBUG
    printf(" recv the signal start func:pcap_loop\n");
#endif
    pcap_loop(handle,-1,mycallback,NULL);
}

void* start_capture(void* arg)
{
    while(1) {
        signal(SIGUSR1, start_loop);
    }
}

void* stop_capture(void* arg)
{
    while(1) {
        signal(SIGUSR2, close_loop); 
    }

}

void parse(char* buf, blockinfo* block)
{
    int count = 0;
    char* pointer[12] = {0};
    char ch = '0';

    if(buf == NULL) {
        return;
    }
    
    reset_block();
    processOrder(&count, buf, pointer);

    while( (ch = getopt(count, pointer, "i:t:o:n:w:")) != -1 ) {
        switch(ch) {
            case 'i':
                block->srcip = ntohl(inet_addr(optarg));
                printf("the block_saddr==%s, block_num==%u\n", optarg, block->srcip);
                break;
            default:
                break;
        }
    }
}

void init_sock()
{
	int clientfd, serverfd;
	char cmdbuf[128];
	socklen_t len;
    struct sockaddr_un saddr, caddr;
    
    saddr.sun_family=AF_UNIX;
    strncpy(saddr.sun_path,UNIX_DOMAIN,sizeof(saddr.sun_path)-1);
    
    unlink(UNIX_DOMAIN);

    if( (serverfd = socket(PF_UNIX, SOCK_STREAM,0)) < 0 ) {
        dzlog_info("socket fail!!!");
        close(serverfd);
      	exit(0);
    }
    
    if( bind(serverfd, (struct sockaddr*)&saddr, sizeof(saddr)) < 0 ) {
        dzlog_info("bind error");
        close(serverfd);
        unlink(UNIX_DOMAIN);
        exit(0);
    }	
    
    listen(serverfd, 5);
#ifdef DEBUG
    printf("listen\n");
#endif

    if ( (block = malloc(sizeof(block))) == NULL ) {
        printf("malloc failed\n");
        return;
    }
    block->srcip = 0;
    block->dstip = 0;
    
    while(1) {
        len = sizeof(caddr);
        
        if( (clientfd = accept(serverfd, (struct sockaddr *)&caddr, &len)) < 0 ) {
            dzlog_error("accept error!!!");
            return;
        }
        memset(cmdbuf, 0, sizeof(cmdbuf));
        
        read(clientfd, cmdbuf, sizeof(cmdbuf));
#ifdef DEBUG
        printf("read cmdbuf==%s\n", cmdbuf);
#endif
        if( strncmp(cmdbuf, STARTBLOCK, 8) == 0 ) {
#ifdef DEBUG
            printf("netblock ****\n");
#endif
            parse(cmdbuf, block);
            kill(0, SIGUSR1);
        }
        else {
#ifdef DEBUG
            printf("can't parse the cmd from manager\n");
#endif
            dzlog_info("can't parse the cmd from manager");
        }
    }
}

int main(int argc, char* argv[])
{
    pthread_t id[2];

    handle=pcap_open_live(argv[1], 1600, 1, 0, NULL);

    if(argc != 2) {
        printf("such as: 'tipsnetblock eth2'\n");
        exit(1);
    }

    if ( pthread_create(&id[0], 0, start_capture, NULL) != 0 ) {
        printf("create pthread stop_loop failed\n");
        dzlog_info("create pthread capture failed");
        exit(1);
    }

    if ( pthread_create(&id[1], 0, stop_capture, NULL) != 0 ) {
        printf("create pthread stop_capture failed\n");
        dzlog_info("create pthread capture failed");
        exit(1);
    }

    init_sock(); 
    
    pthread_join(id[0], NULL);
    pthread_join(id[1], NULL);
    
    pcap_close(handle);
    return 0;
}

#if 0
int main(int argc, char* argv[])
{
    pthread_t id[2];

     if(dzlog_init("/usr/local/tipschecker/etc/log.conf", "netblock")) {
         printf("dzlog init failed\n");
     }
     WtPid();//add by lipeiting 2014-01-12-9:08
     
     signal(SIGUSR1, handle_func);

    if (pthread_create(&id[0], 0, capture_packet, argv[1]) != 0) {
        printf("create pthread capture failed\n");
        exit(1);
    }
    if (pthread_create(&id[1], 0, parsecmd,0) != 0) {
        printf("create pthread policy failed\n");
        exit(1);
    }
    
    pthread_join(id[0], NULL);
    pthread_join(id[1], NULL);
}
#endif
