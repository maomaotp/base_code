#include "monitorNetwork.h"
extern char eth0[];
void getmonitorinfo(Monitor_t &monitor)
{
   getMonitorIP(monitor);
   getMonitorStatus(monitor);
   getMonitorConnectIP(monitor);
}

void getMonitorIP(Monitor_t &monitor)
{
    char order[1024]; 
		memset(order, 0, sizeof(order));
		if(!strlen(eth0))
						return ;
		snprintf(order, sizeof(order), "ifconfig %s | grep \"inet addr\" | awk 'BEGIN {FS=\":\"} {print $2}' | cut -d' ' -f1", eth0);
    FILE *file=popen(order,"r");
    memset(order,0,sizeof(order));
    fread(monitor.monitorip,1,32,file);
    pclose(file);
    monitor.monitorip[strlen(monitor.monitorip)-1]='\0';
}

void getMonitorStatus(Monitor_t &monitor)
{
   monitor.status=NORMAL;
}

void getMonitorConnectIP(Monitor_t &monitor)
{
   FILE *file=NULL;
   char order[]="netstat -tupn | grep 12300 | awk '{print $5}' | cut -d':' -f1 | uniq";
   
   memset(monitor.connectip,0,sizeof(monitor.connectip));
  
   file=popen(order,"r");
   if(!file)
   {
     return ;
   }
   else
   {
      fread(monitor.connectip,1,128,file);
      pclose(file);
      file=NULL;
      replacech(monitor);
   }
}

void replacech(Monitor_t &monitor)
{
   char *pos=NULL;

   while((bool)(pos=strchr(monitor.connectip,'\n')))
   {
       *pos=',';
   }

   monitor.connectip[strlen(monitor.connectip)-1]='\0';
}

