#include <iostream>
#include "check.h"
#include "zlog.h"
using namespace std;

char eth0[32] = "eth0";
char eth1[32] = "eth1";
char eth2[32] = "eth2";
char eth3[32] = "eth1";

time_t time_start;
time_t time_cur;

int flag=0;
info_t  info;
ShareMem obj;
static pthread_mutex_t ft_mutex;
static pthread_mutex_t se_mutex;

extern std::set<in_addr_t> ft_user;
extern std::set<in_addr_t> se_user;
extern std::set<message,compare> ft_comp;
extern std::set<message,compare> se_comp;

void mycallback1(u_char *user,const struct pcap_pkthdr *hdr,const u_char *bytes)
{
    geteth2dataflow(hdr); 
    
    struct ether_header *ether=(struct ether_header*)bytes;
    if(ntohs(ether->ether_type) == ETHERTYPE_IP)
    {
      static message md5;
      static message md5tp;
      static char buffer[64];

      struct iphdr *ip=(struct iphdr *)(bytes+sizeof(struct ether_header));
	  pthread_mutex_lock(&ft_mutex);
      if(ipmarkoff(&ip->saddr))
      {
        ft_user.insert(ip->saddr);
      }
      if(ipmarkoff(&ip->daddr))
      {
        ft_user.insert(ip->daddr);
      }
	  pthread_mutex_unlock(&ft_mutex);

      if(ip->protocol == IPPROTO_TCP )
      {
        struct tcphdr *tcp=(struct tcphdr*)(bytes+sizeof(ether_header)+ip->ihl*4);
	
		memset(buffer,0,64);
		sprintf(buffer,"%u%u%u%u",ip->saddr,tcp->source,ip->daddr,tcp->dest);
        memset(md5.mess,0,16);
		packmd5((unsigned char*)buffer,md5.mess);
	
		memset(buffer,0,64);
		sprintf(buffer,"%u%u%u%u",ip->saddr,tcp->source,ip->daddr,tcp->dest);
        memset(md5tp.mess,0,16);
        packmd5((unsigned char*)buffer,md5tp.mess); 

		pthread_mutex_lock(&ft_mutex);
        if(search(md5) || search(md5tp))
		{
		}
		else
		{
			ft_comp.insert(md5);  
		}
		pthread_mutex_unlock(&ft_mutex);
      }

      if(ip->protocol == IPPROTO_UDP )
      {
        struct udphdr *udp=(struct udphdr*)(bytes+sizeof(ether_header)+ip->ihl*4);
	
		memset(buffer,0,64);
		sprintf(buffer,"%u%u%u%u",ip->saddr,udp->source,ip->daddr,udp->dest);
        memset(md5.mess,0,16);
		packmd5((unsigned char*)buffer,md5.mess);
	
		memset(buffer,0,64);
		sprintf(buffer,"%u%u%u%u",ip->saddr,udp->source,ip->daddr,udp->dest);
        memset(md5tp.mess,0,16);
        packmd5((unsigned char*)buffer,md5tp.mess); 

		pthread_mutex_lock(&ft_mutex);
        if(search(md5) || search(md5tp))
		{
		}
		else
		{
			ft_comp.insert(md5);  
		}
		pthread_mutex_unlock(&ft_mutex);
      }

    }
	user = user;		//hyj 保留参数
}

void mycallback2(u_char *user,const struct pcap_pkthdr *hdr,const u_char *bytes)
{
   geteth3dataflow(hdr);

   struct ether_header *ether=(struct ether_header*)bytes;
   if(ntohs(ether->ether_type) == ETHERTYPE_IP)
   {
      static message md5;
      static message md5tp;
      static char buffer[64];

      struct iphdr *ip=(struct iphdr *)(bytes+sizeof(struct ether_header));
	  pthread_mutex_lock(&se_mutex);
      if(ipmarkoff(&ip->saddr))
      {
#if 0
				char ipstr[32]={0};
				struct in_addr s;
				s.s_addr = ip->saddr;
				inet_ntop(AF_INET,(void*)&s, ipstr,16);
				printf("saddr=%s\n", ipstr);
#endif
        se_user.insert(ip->saddr);
      }
      if(ipmarkoff(&ip->daddr))
      {
#if 0
				char ipstr[32]={0};
				struct in_addr s;
				s.s_addr = ip->daddr;
				inet_ntop(AF_INET,(void*)&s, ipstr,16);
				printf("daddr=%s\n", ipstr);
#endif
        se_user.insert(ip->daddr);
      }
	  pthread_mutex_unlock(&se_mutex);
       
      if(ip->protocol == IPPROTO_TCP)
      {
        struct tcphdr *tcp=(struct tcphdr*)(bytes+sizeof(ether_header)+ip->ihl*4);
	
		memset(buffer,0,64);
		sprintf(buffer,"%u%u%u%u",ip->saddr,tcp->source,ip->daddr,tcp->dest);
        memset(md5.mess,0,16);
		packmd5((unsigned char*)buffer,md5.mess);
	
		memset(buffer,0,64);
		sprintf(buffer,"%u%u%u%u",ip->saddr,tcp->source,ip->daddr,tcp->dest);
        memset(md5tp.mess,0,16);
        packmd5((unsigned char*)buffer,md5tp.mess); 

		pthread_mutex_lock(&se_mutex);
        if(find(md5) || find(md5tp))
		{
	
		}
		else
		{
			se_comp.insert(md5);  
		}
		pthread_mutex_unlock(&se_mutex);
      }
	  if(ip->protocol == IPPROTO_UDP )
      {
		struct udphdr *udp=(struct udphdr*)(bytes+sizeof(ether_header)+ip->ihl*4);
	
		memset(buffer,0,64);
		sprintf(buffer,"%u%u%u%u",ip->saddr,udp->source,ip->daddr,udp->dest);
        memset(md5.mess,0,16);
		packmd5((unsigned char*)buffer,md5.mess);
	
		memset(buffer,0,64);
		sprintf(buffer,"%u%u%u%u",ip->saddr,udp->source,ip->daddr,udp->dest);
        memset(md5tp.mess,0,16);
        packmd5((unsigned char*)buffer,md5tp.mess); 

		pthread_mutex_lock(&se_mutex);
        if(search(md5) || search(md5tp))
		{
		
		}
		else
		{
			ft_comp.insert(md5);  
		}
		pthread_mutex_unlock(&se_mutex);
      }
   }
	user = user;		//hyj 保留参数
}


void* func1(void* arg)
{
   pcap_loop((pcap_t*)arg,-1,mycallback1,NULL);

	return NULL;		//hyj 返回值?
}

void* func2(void* arg)
{
   pcap_loop((pcap_t*)arg,-1,mycallback2,NULL);

	return NULL;		//hyj 返回值？
}

int printinfo(info_t & info)
{
//	printf("masIP=%s\n", info.commu_info.masterIP);
	//dzlog_info("commu_info:\nconIP=%s  masIP=%s  masPORT=%s  masstat=%d slaIP=%s slaPORT=%s slastat=%d\n\n",info.commu_info.connectIP, info.commu_info.masterIP, info.commu_info.masterPORT, info.commu_info.masterstatus, info.commu_info.slaveIP, info.commu_info.slavePORT, info.commu_info.slavestatus);
	//dzlog_info("monitor_info:\nstat=%d monip=%s conip=%s\n\n", info.monitor_t.status, info.monitor_t.monitorip, info.monitor_t.connectip);
	//dzlog_info("mirror_info:\nmode=%d eth0stat=%d eth1stat=%d eth0flw=%.2f eth1flw=%.2f max_flw=%.2f ", info.mirror_info.mode, info.mirror_info.eth0_status, info.mirror_info.eth1_status, info.mirror_info.eth0_flow, info.mirror_info.eth1_flow, info.mirror_info.max_flow);
	//dzlog_info("eth0cnt=%d eth1cnt=%d maxcnt=%d eth0link=%d eth1link=%d maxlink=%d\n", info.mirror_info.eth0_user_counter, info.mirror_info.eth1_user_counter, info.mirror_info.max_user_counter, info.mirror_info.eth0_link, info.mirror_info.eth1_link, info.mirror_info.max_link);
	//dzlog_info("\n\n\n");
//	obj.Set_status_shm(5,&(info.mirror_info.mode),sizeof(int));
	obj.Set_status_shm(6,&(info.mirror_info.eth0_status),sizeof(int));
	obj.Set_status_shm(7,&(info.mirror_info.eth0_flow),sizeof(double));
	obj.Set_status_shm(8,&(info.mirror_info.eth0_user_counter),sizeof(int));
	obj.Set_status_shm(9,&(info.mirror_info.eth0_link),sizeof(int));
	obj.Set_status_shm(10,&(info.mirror_info.eth1_status),sizeof(int));
	obj.Set_status_shm(11,&(info.mirror_info.eth1_flow),sizeof(double));
	obj.Set_status_shm(12,&(info.mirror_info.eth1_user_counter),sizeof(int));
	obj.Set_status_shm(13,&(info.mirror_info.eth1_link),sizeof(int));
	obj.Set_status_shm(17,&(info.commu_info.masterstatus),sizeof(int));
	obj.Set_status_shm(18,&(info.commu_info.slavestatus),sizeof(int));
	obj.Set_status_shm(44,&(info.monitor_t.connectip),sizeof(info.monitor_t.connectip));
	obj.Set_status_shm(45,&(info.mirror_info.max_flow),sizeof(double));
	obj.Set_status_shm(46,&(info.mirror_info.max_user_counter),sizeof(int));
	obj.Set_status_shm(47,&(info.mirror_info.max_link),sizeof(int));

	return 0;		//hyj 返回值？
}

void getallinfo()
{
//	printf("flag=%d\n", flag);
//   process(&flag);
	pthread_mutex_lock(&ft_mutex);
	ft_user.clear();
	ft_comp.clear();
	pthread_mutex_unlock(&ft_mutex);
	pthread_mutex_lock(&se_mutex);
	se_user.clear();
	se_comp.clear();
	pthread_mutex_unlock(&se_mutex);
  
   getcominfo(info.commu_info);
   
   getmonitorinfo(info.monitor_t);
//   printf("%s  %s  %d\n",info.monitor_t.monitorip,info.monitor_t.connectip,info.monitor_t.status);
   memset(&info,0,sizeof(info));
//   alarm(GETTIME);
}

void getinfo(int signo)
{
	getcominfo(info.commu_info);
	
	getmonitorinfo(info.monitor_t);
	time_t  remain=0;
//   remain=alarm(GETTIME);
	time(&time_cur);
	remain = time_cur - time_start;
	if(remain == 0)
	{
		return ;
	}
	else
	{

		process_time(&flag,remain);
		pthread_mutex_lock(&ft_mutex);
		ft_user.clear();
		ft_comp.clear();
		pthread_mutex_unlock(&ft_mutex);
		pthread_mutex_lock(&se_mutex);
		se_user.clear();
		se_comp.clear();
		pthread_mutex_unlock(&se_mutex);
		time(&time_start);

		printinfo(info);
//   printf("%s  %s  %d  ======================\n",info.monitor_t.monitorip,info.monitor_t.connectip,info.monitor_t.status);
//   alarm(GETTIME);
	}
	signo = signo;		//hyj 保留参数
}

void WtPid()
{
	FILE *fp;
	char id_buf[10];
	pid_t pid;
	memset(id_buf,0,10);
	pid = getpid();
    dzlog_info("stat start now,pid==%d", pid);
	sprintf(id_buf,"%d",pid);
	if(NULL == (fp = fopen("/usr/local/tipschecker/etc/tipscheck/stat_id","w+")))
	{
		perror("fopen");
		return;
	}
	fwrite(id_buf,strlen(id_buf),1,fp);
	fclose(fp);
	return;
}

int main()
{
	pid_t  id=0;
    pcap_t *mirror0=NULL;
	pcap_t *mirror1=NULL;
	pthread_t pid1=0;
	pthread_t pid2=0;
	int thread1_status = 0;
	int thread2_status = 0;
	pthread_mutex_init(&ft_mutex, NULL);
	pthread_mutex_init(&se_mutex, NULL);
	memset(eth0, 0, sizeof(eth0));
	memset(eth1, 0, sizeof(eth1));
	memset(eth2, 0, sizeof(eth2));
	memset(eth3, 0, sizeof(eth3));
	id=getpid();

    //zlog init
    if(dzlog_init("/usr/local/tipschecker/etc/log.conf", "stat")) {
        printf("dzlog init failed\n");
    }

	WtPid();
	 
	int ret = obj.Init_shm();
	if(-1 == ret){
		dzlog_info("Init_shm failed\n");
		return -1; 
	}

//	obj.Set_status_shm(48, eth0, sizeof(eth0));
//  obj.Set_status_shm(49, eth1, sizeof(eth1));
//	obj.Set_status_shm(50, eth2, sizeof(eth2));
//	obj.Set_status_shm(51, eth3, sizeof(eth3));
	obj.Get_status_shm(48, eth0, sizeof(eth0));
	obj.Get_status_shm(49, eth1, sizeof(eth1));
	obj.Get_status_shm(50, eth2, sizeof(eth2));
	obj.Get_status_shm(51, eth3, sizeof(eth3));
//	printf("sizeof(eth0)=%d eth0=%s eth1=%s eth2=%s eth3=%s\n", sizeof(eth0),eth0, eth1, eth2, eth3);
	int seconds = 30;
	char cmd[1024];
	memset(cmd, 0, sizeof(cmd));
	sprintf(cmd,"cat /usr/local/tipschecker/etc/tipscheck/system_config  | grep NETSTATETIME | cut -d'=' -f2");
	FILE * fp = popen(cmd, "r");
	memset(cmd, 0, sizeof(cmd));
	fread(cmd, 1, 1024, fp);
	cmd[strlen(cmd)-1] = '\0';
//	printf("cmd=%s seconds=%d strlen(cmd)=%d\n", cmd, seconds, strlen(cmd));
	if(strlen(cmd))
	{
		seconds = atoi(cmd);
		//printf("cmd=%s seconds=%d\n", cmd, seconds);
	}

	init_flag(&flag);
	signal(SIGUSR1,getinfo);

	while(1)
	{
		init_flag(&flag);
		getworkstatus(info.mirror_info);
		time(&time_start);
 
		if(1 == info.mirror_info.eth0_status && thread1_status == 0)
		{
			mirror0=pcap_open_live(eth2,1600,1,0,NULL);
			pthread_create(&pid2,NULL,func1,mirror0);
			thread1_status = 1;
			// pcap_loop(eth2,-1,mycallback2,NULL);
		}
 
		if(1 == info.mirror_info.eth1_status && thread2_status == 0)
		{
			mirror1=pcap_open_live(eth3,1600,1,0,NULL);
			pthread_create(&pid1,NULL,func2,mirror1);
			thread2_status = 1;
		}
		sleep(seconds);
		getinfo(0);
		memset(&info,0,sizeof(info));
	}
	pthread_mutex_destroy(&ft_mutex);
	pthread_mutex_destroy(&se_mutex);
	return 0;
}
