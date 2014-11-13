#include "check.h"

std::set<in_addr_t> ft_user;
std::set<in_addr_t> se_user;
std::set<message,compare> ft_comp;
std::set<message,compare> se_comp;

extern ShareMem obj;
extern  info_t  info;
extern  int flag;
unsigned long long eth2dataflow;
unsigned long long eth3dataflow;

void init_flag(int *flag)
{
				/*
   FILE *file=NULL;
   char order[]="cat /usr/local/tipschecker/etc/tipscheck/workmode.conf | grep WORKMODE | cut -d'=' -f2";

   file=popen(order,"r");
   memset(order,0,sizeof(order));
   fread(order,1,sizeof(order),file);
   pclose(file);
   file=NULL;
	 */
	 int mode = 0;
//	 obj.Set_status_shm(5, &mode, sizeof(mode));
	 obj.Get_status_shm(5, &mode, sizeof(mode));

   *flag = mode;
}

void packmd5(unsigned char *data,unsigned char *value)
{
   MD5_CTX ctx;
   MD5_Init(&ctx);
   MD5_Update(&ctx,data,strlen((const char *)data));
   MD5_Final(value,&ctx);
}

void geteth2dataflow(const struct pcap_pkthdr *hdr)
{
    eth2dataflow+=hdr->caplen;
}

bool search(const message &it)
{
   std::set<message,compare>::iterator pos;

   pos=ft_comp.find(it);

   if(pos != ft_comp.end())
   {
     return true;
   }

   return false;
}


bool find(const message &it)
{
   std::set<message,compare>::iterator pos;

   pos=se_comp.find(it);

   if(pos != se_comp.end())
   {
     return true;
   }

   return false;
}


void geteth3dataflow(const struct pcap_pkthdr *hdr)
{
   eth3dataflow+=hdr->caplen;
}

#if 0
void process(const int *flag)
{
  static unsigned long long max=0;
  static unsigned long  user_counter=0;
  static unsigned long  user_link=0;

  if(*flag == 1)
  {
    

    info.mirror_info.mode=*flag;
    getworkstatus(info.mirror_info);
    sprintf(info.mirror_info.eth0_flow,"%.2f",(double)eth2dataflow/1024/GETTIME);
    sprintf(info.mirror_info.eth1_flow,"%.2f",(double)eth3dataflow/1024/GETTIME);
    max=(eth2dataflow>max)?eth2dataflow:max;
    sprintf(info.mirror_info.max_flow,"%.2f",(double)max/1024/GETTIME);
    int counter=ft_user.size();
    sprintf(info.mirror_info.eth0_user_counter,"%d",counter/GETTIME);   
    sprintf(info.mirror_info.eth1_user_counter,"%d",se_user.size()/GETTIME);  
    user_counter=(counter>user_counter)?counter:user_counter;
    sprintf(info.mirror_info.max_user_counter,"%d",user_counter/GETTIME);
    counter=ft_comp.size();
    sprintf(info.mirror_info.eth0_link,"%d",counter/GETTIME);
    sprintf(info.mirror_info.eth1_link,"%d",se_comp.size()/GETTIME);
    user_link=(counter>user_link)?counter:user_link;
    sprintf(info.mirror_info.max_link,"%d",user_link/GETTIME);
  }
  else
  {
  
    info.mirror_info.mode=*flag;
    getworkstatus(info.mirror_info);
    sprintf(info.mirror_info.eth0_flow,"%.2f",(double)eth2dataflow/1024/GETTIME);
    sprintf(info.mirror_info.eth1_flow,"%.2f",(double)eth3dataflow/1024/GETTIME);
    max=((eth2dataflow+eth3dataflow)>max)?(eth2dataflow+eth3dataflow):max;
    sprintf(info.mirror_info.max_flow,"%.2f",(double)max/1024/GETTIME);
    int counter=ft_user.size();
    int temp=se_user.size();
    sprintf(info.mirror_info.eth0_user_counter,"%d",counter/GETTIME);   
    sprintf(info.mirror_info.eth1_user_counter,"%d",temp/GETTIME);   
    user_counter=((counter+temp)>user_counter)?(counter+temp):user_counter;
    sprintf(info.mirror_info.max_user_counter,"%d",user_counter/GETTIME);
    counter=ft_comp.size();
    temp=se_comp.size();
    sprintf(info.mirror_info.eth0_link,"%d",counter/GETTIME);
    sprintf(info.mirror_info.eth1_link,"%d",temp/GETTIME);
    user_link=((counter+temp)>user_link)?(counter+temp):user_link;
    sprintf(info.mirror_info.max_link,"%d",user_link/GETTIME);
  }

  eth2dataflow=0;
  eth3dataflow=0;
}

#endif
void process_time(const int *flag,time_t remained)
{
  static double max=0;
  static unsigned long  max_counter=0;
  static unsigned long  max_link=0;

  info.mirror_info.mode=*flag;
  getworkstatus(info.mirror_info);
  info.mirror_info.eth0_flow = (double)eth2dataflow/1024/remained;
  info.mirror_info.eth1_flow = (double)eth3dataflow/1024/remained;
  info.mirror_info.eth0_user_counter = ft_user.size();   
  info.mirror_info.eth1_user_counter = se_user.size();  
//	printf("ft_user.size()=%d\n", ft_user.size());
//	printf("se_user.size()=%d\n", se_user.size());
  info.mirror_info.eth0_link = ft_comp.size()/remained;
  info.mirror_info.eth1_link = se_comp.size()/remained;
	if(*flag == 0 ||  *flag == 1 || *flag == 3)
	{
		max=(info.mirror_info.eth0_flow>max)?info.mirror_info.eth0_flow:max;
		max=(info.mirror_info.eth1_flow>max)?info.mirror_info.eth1_flow:max;
    info.mirror_info.max_flow = max;
    max_counter=((unsigned int)info.mirror_info.eth0_user_counter>max_counter)?info.mirror_info.eth0_user_counter:max_counter;	
    max_counter=((unsigned int)info.mirror_info.eth1_user_counter>max_counter)?info.mirror_info.eth0_user_counter:max_counter;
    info.mirror_info.max_user_counter = max_counter;

    max_link=((unsigned int)info.mirror_info.eth0_link>max_link)?info.mirror_info.eth0_link:max_link;
    max_link=((unsigned int)info.mirror_info.eth1_link>max_link)?info.mirror_info.eth1_link:max_link;
    info.mirror_info.max_link = max_link;
	}
	else if(*flag == 2)
	{
		double double_sum = 0;
		int int_sum = 0;
		double_sum = info.mirror_info.eth0_flow + info.mirror_info.eth1_flow;
		max=(double_sum > max)?double_sum:max;
    info.mirror_info.max_flow = max;
		
		int_sum = info.mirror_info.eth0_user_counter + info.mirror_info.eth1_user_counter;
    max_counter=((unsigned int)int_sum > max_counter)?int_sum : max_counter;
    info.mirror_info.max_user_counter = max_counter;

		int_sum = info.mirror_info.eth0_link + info.mirror_info.eth1_link;
    max_link=((unsigned int)int_sum > max_link)?int_sum : max_link;
    info.mirror_info.max_link = max_link;
		
	}

  eth2dataflow=0;
  eth3dataflow=0;
}
