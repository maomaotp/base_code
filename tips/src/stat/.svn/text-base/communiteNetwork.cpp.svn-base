#include "communiteNetwork.h"

extern char eth0[];
extern char eth1[];
extern char eth2[];
extern char eth3[];
char manageip[32];
char manageip1[32];

void getcominfo(commucate_t &connect_struct)
{
   getCommunicatemasterInfo(connect_struct);
   getCommunicatePortIp(connect_struct);
}

void getCommunicatePortIp(commucate_t &connect_struct)
{
   FILE *file=NULL;
   char order[4096];
	 memset(order, 0, sizeof(order));
	 snprintf(order, sizeof(order), "ifconfig %s | grep \"inet addr\" | cut -d':' -f2 | cut -d' ' -f1", eth1);

   memset(connect_struct.connectIP,0,sizeof(connect_struct.connectIP));

	 if(!strlen(eth1))
					 return ;
   
   file=popen(order,"r");
   if(NULL == file)
   {
      return ;
   }
   else
   {
      fread(connect_struct.connectIP,1,32,file);
      connect_struct.connectIP[strlen(connect_struct.connectIP)-1]='\0';
			printf("connectIP=%s\n", connect_struct.connectIP);
      pclose(file);
      file=NULL;
   }
}

void getCenterConf(char * order, int size, commucate_t & connect_struct)
{
       //get the master and slave ip and port
       FILE *file=NULL;
       memset(order,0,4096);
       sprintf(order,"cat /usr/local/tipschecker/etc/tipscheck/config  | grep ADMINSTRATION_IP= | cut -d'=' -f2");
       file=popen(order,"r");
       memset(order,0,4096);
       fread(order,1,4096,file);
       pclose(file);
       file=NULL;
       order[strlen(order)-1]='\0';
       strncpy(connect_struct.masterIP,order,sizeof(connect_struct.masterIP));
    
       memset(order,0,4096);
       sprintf(order,"cat /usr/local/tipschecker/etc/tipscheck/config   | grep MANAGE_PORT= | cut -d'=' -f2");
       file=popen(order,"r");
       memset(order,0,4096);
       fread(order,1,4096,file);
       pclose(file);
       file=NULL;
       order[strlen(order)-1]='\0';
       strncpy(connect_struct.masterPORT,order,sizeof(connect_struct.masterPORT));
//			 printf("%s %s\n", connect_struct.masterIP, connect_struct.masterPORT);
	//		 printf("line=%d\n", __LINE__);
			 snprintf(manageip, sizeof(manageip), "%s:%s", connect_struct.masterIP, connect_struct.masterPORT);
//			 printf("%s\n", manageip);
//			 printf("line=%d\n", __LINE__);
       
       memset(order,0,4096);
       sprintf(order,"cat /usr/local/tipschecker/etc/tipscheck/config  | grep ADMINSTRATION_IP1 | cut -d'=' -f2");
       file=popen(order,"r");
       memset(order,0,4096);
       fread(order,1,4096,file);
       pclose(file);
       file=NULL;
       order[strlen(order)-1]='\0';
       strncpy(connect_struct.slaveIP,order,sizeof(connect_struct.slaveIP));

       memset(order,0,4096);
       sprintf(order,"cat /usr/local/tipschecker/etc/tipscheck/config  | grep MANAGE_PORT1 | cut -d'=' -f2");
       file=popen(order,"r");
       memset(order,0,4096);
       fread(order,1,4096,file);
       pclose(file);
       file=NULL;
       order[strlen(order)-1]='\0';
       strncpy(connect_struct.slavePORT,order,sizeof(connect_struct.slavePORT));
			 snprintf(manageip1, sizeof(manageip1), "%s:%s", connect_struct.slaveIP, connect_struct.slavePORT);
//			 printf("%s\n", manageip1);
//			 snprintf(manageip1, sizeof(manageip1), "202.108.241.132:443");

		size = size;		//hyj 保留参数
}

void getCommunicatemasterInfo(commucate_t &connect_struct)
{
   	char order[4096];
	  memset(order, 0, sizeof(order));
		
    memset(&connect_struct,0,sizeof(commucate_t));
    
		getCenterConf(order, 4096, connect_struct);

    if(strlen(eth1))
    {

       if(strlen(connect_struct.masterIP))
         connect_struct.masterstatus=testNetwork(0, connect_struct.masterIP,order);
       
       if(strlen(connect_struct.slaveIP))
        connect_struct.slavestatus=testNetwork(1, connect_struct.slaveIP,order);
    }
    else
    {
       connect_struct.masterstatus=NET_UNLIK;
       connect_struct.slavestatus=NET_UNLIK;
    }
}

char testNetwork(int servertype, const char *centerip,char *order)
{
    memset(order,0,4096);
#if 0
    FILE *result=NULL;
    sprintf(order,"route -n | awk '{ print $1 }' | grep 0.0.0.0");
    result=popen(order,"r");
    memset(order,0,4096);
    fread(order,1,4096,result);
    pclose(result);
    
    if(0 == strlen(order))
    return NET_UNGATE;
    
#endif 
		time_t old_time = 0, new_time = 0;
		if(0 == servertype)
		{
//			 printf("line=%d\n", __LINE__);
//			 printf("%s\n", manageip);
    	 int ret = GetTime(NULL, 0, &old_time, &new_time);
//			 printf("line=%d manageip=%s\n", __LINE__, manageip);
			if(0 == ret)
			{
				printf("communicate_0 ok\n");
    	  return NET_OK;
			}
			else
			{
				printf("communicate_0 failed\n");
        return NET_DOW;
			}
		}
		else if(1 == servertype)
		{
    	int ret = GetTime(NULL, 1, &old_time, &new_time);
			if(0 == ret)
			{
				printf("communicate_1 ok\n");
   	 		return NET_OK;
			}
			else
			{
				printf("communicate_1 failed\n");
        return NET_DOW;
			}
			
		}
	if(centerip){};		//hyj 保留参数

	return '\0';		//hyj 返回值？
}












