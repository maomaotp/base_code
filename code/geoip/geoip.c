#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include"geoip.h"
#include "init.h"

struct ipaddr* ipfile;

void readIPfile()
{
     ipfile=(struct ipaddr *)calloc(38850,sizeof(struct ipaddr));
     if(NULL == ipfile)
     {
         dzlog_info("ipaddr calloc failed");
         abort();
         return;
     }
     char buffer[256] = {0};
     FILE *file = fopen("./geoip_2013.1.0.0.conf","r");
     if(NULL == file)
     {
         dzlog_info("open geoip file failed!!!");
         abort();
         return;
     }

     int index = 0;
     while(fgets(buffer,256,file))
     {
         char *ordination=(char*)buffer;
         strcpy(ipfile[index].startip, strsep(&ordination, "#"));
         strcpy(ipfile[index].endip, strsep(&ordination, "#"));
         strcpy(ipfile[index].region, strsep(&ordination, "#"));
         strcpy(ipfile[index].territory, strsep(&ordination, "#"));
         strcpy(&ipfile[index].flag, strsep(&ordination, "#"));
        
        //printf("ipfile->startip==%s, endip==%s, region==%s, territory==%s, flag==%s\n", ipfile[index].startip, ipfile[index].endip, ipfile[index].region, ipfile[index].territory, ipfile[index].flag);
         fflush(file);
         index++;
    }

    dzlog_debug("compile read line number index = %d", index);

    return;
}

//遍历法
void getAddr_ergodic(const unsigned int ip, char* geography, char* scc_flag)
{
    //long long ip_num = atoll(ip);

    int i = 0;
    int n = 38847;
    
    while(i <= n)
    {
        if(ip >= atoll(ipfile[i].startip) && ip <= atoll(ipfile[i].endip))
        {
           // printf("ip(**%u**) >= %lld && ip <= %lld******ipfile[%d].region== %s, ipfile[i].territory == %s\n", ip, atoll(ipfile[i].startip), atoll(ipfile[i].endip), i, ipfile[i].region, ipfile[i].territory);
            snprintf(geography, 31, "%s:%s", ipfile[i].region, ipfile[i].territory);
            *scc_flag = ipfile[i].flag; 
            return;
        }
        i++;
    }
    geography = "IP is unkown!";
    scc_flag = "5";
    dzlog_debug("ip == %u, ip is unkown!\n", ip);
}

//二分法
void getAddr_dichotomy(const unsigned int ip, char* geography, char* scc_flag)
{
    // long long ip_num = atoll(ip);
    int n = 38847;
    int low = 0, high = n-1, mid;

    while(low<=high)
    {
        mid = low+(high-low)/2;

        if(ip >= atoll(ipfile[mid].startip) && ip <= atoll(ipfile[mid].endip))
        {
            dzlog_debug("ip(**%u**) >= atol(ipfile[%d].startip)(**%lld**) && ip <= atol(ipfile[mid].endip(**%lld**))******ipfile[mid].region== %s, ipfile[mid].territory == %s\n",ip, mid, atoll(ipfile[mid].startip), atoll(ipfile[mid].endip), ipfile[mid].region, ipfile[mid].territory);
            
            sprintf(geography, "%s:%s", ipfile[mid].region, ipfile[mid].territory);
            *scc_flag = ipfile[mid].flag; 
            return;
        }

        if(ip < atoll(ipfile[mid].startip))
        {
            dzlog_debug("ip(**%u**)<ipfile[%d].startip(**%lld**)\n", ip, mid, atoll(ipfile[mid].startip));
            high = mid-1;
        }
        else if(ip > atoll(ipfile[mid].endip))
        {
            printf("ip(**%u**)>ipfile[%d].endip(**%lld**)\n", ip, mid, atoll(ipfile[mid].endip));
            low = mid+1;
        }
    }

    geography = "IP is unkown!";
    scc_flag = "5";
    dzlog_debug("IP is unkown!!!\n");
    return;
}
/*
int main(int argc, char* argv[])
{
    char geog[15] = {0};

    readfile();
    printf("read success!!!\n");

    //getAddr_dichotomy(argv[1]);
    getAddr_ergodic(argv[1], geog);
    printf("geog == %s\n", geog);

    return 0;
}
*/

