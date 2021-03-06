#include "admin_opt.h"
#include "managecp_ip.h"
#include "ui_manage_client.h"
#include <sys/types.h>
#include <signal.h>
#include <microhttpd.h>
#include <ctype.h>
#include <fcntl.h>
#include "arp.h" 
#include "sharemem.h"//add by lipeiting 2014-01-15

#include <sys/socket.h>
#include <netinet/in.h>
#define BUFFLEN 1024
#define SERVER_PORT 8008
#define KILL_PID "/usr/local/tipschecker/etc/tipscheck/pid"
#define MANAGE_ETH "eth0"
#define MANAGE_ETH0_FILE "/etc/sysconfig/network-scripts/ifcfg-eth0"
#define MANAGE_ETH1_FILE "/etc/sysconfig/network-scripts/ifcfg-eth1"
#define MANAGE_ETH3_FILE "/etc/sysconfig/network-scripts/ifcfg-eth2"
extern CManageCP_IP managecp_ip;
extern ShareMem obj;//add by lipeiting 2014-01-15
CUI_Manage_cli ui_cli;

CAdmin_opt::CAdmin_opt()
{

}

int CAdmin_opt::str_isdight(char *str)
{
	int len = strlen(str);
	for(int space = 0; space < len; space++){
		if(!isdigit(str[space]))
			return 1;
	}
  return 0;
}
int CAdmin_opt::is_netline(char *eth)//判断网线有没有插入
{

	char judge_state[100];
	memset(judge_state,0,sizeof(judge_state));
	sprintf(judge_state,"cat /sys/class/net/%s/carrier",eth);
	FILE *fp = popen (judge_state, "r");//镜像口1的连接状态
	if(fp == NULL)
	{
		pclose(fp);
		return 0;
	}
	fgets(judge_state,10,fp);
	pclose(fp);
	judge_state[strlen(judge_state)-1] = '\0';
	if(!strcmp(judge_state,"0"))
		return 0;
	else
		return 1;

}
int CAdmin_opt::str_isalpha(char *str)
{
	int len = strlen(str);
	for(int space = 0; space < len; space++){
		if(!isalpha(str[space]))
			return 1;
	}
  return 0;
}

CAdmin_opt::CAdmin_opt(credentials_t* scredentials, char *scmd, params_t* sparams, void *sopt_fun)
{
	credentials = scredentials;
	cmd = scmd;
	params = sparams;
	opt_fun = sopt_fun;
}

int CAdmin_opt::init_opt(credentials_t* scredentials, char *scmd, params_t* sparams, void* sopt_fun)
{
	credentials = scredentials;
	cmd = scmd;
	params = sparams;
	opt_fun = sopt_fun;
	return 0;
}
int CAdmin_opt::get_product_state(product_INFOSTATE *upgrade_infostate)//获取系统状态的接口
{
	char buf[CONFIG_MAXLINE];
	char *temp;
	g_mutex_lock(filemutex);
	FILE *fp = fopen(PRODUCT_FILE, "r+");
	if(fp == NULL)
	{
		g_mutex_unlock(filemutex);
    	return 1;
	}
	else
	{
		memset(buf, 0, CONFIG_MAXLINE);
		while(NULL != fgets(buf, CONFIG_MAXLINE, fp))
		{
			printf("buf================%s\n",buf);
			if(strlen(buf) >= CONFIG_MAXLINE-1)
			{
				fclose(fp);
				printf("can't open xml\n");
				g_mutex_unlock(filemutex);
				return 1;
			}
			if((temp = strstr(buf, "=")) != NULL)
			{
				int len = strlen(temp + 1);
				printf("temp==============%s\tlen=%d\n",temp,len);
				if(!strncmp(buf, "NAME", 4))
				{
					printf("len=%d\n",len);
					memcpy(upgrade_infostate->name,temp+1,((len < 50) ? len-1 : 49));
				}
				else if(!strncmp(buf, "MODE", 4))
				{
					memcpy(upgrade_infostate->mode,temp+1,((len < 50) ? len-1 : 49));
				}
				else if(!strncmp(buf, "CODE", 4))
				{
					memcpy(upgrade_infostate->code,temp+1,((len < 50) ? len-1 : 49));
				}
				else if(!strncmp(buf, "VERSION", 7))
				{
					memcpy(upgrade_infostate->version,temp+1,((len < 50) ? len-1 : 49));
				}
				else if(!strncmp(buf, "TROJAN_NEWTIME", 14))
				{
					memcpy(upgrade_infostate->trojan_newtime,temp+1,((len < 50) ? len-1 : 49));
				} else if(!strncmp(buf, "POLICY_NEWTIME", 14))
				{
					memcpy(upgrade_infostate->policy_newtime,temp+1,((len < 50) ? len-1 : 49));
				}
				else if(!strncmp(buf, "APPLY_TIME", 10))
				{
					memcpy(upgrade_infostate->apply_time,temp+1,((len < 50) ? len-1 : 49));
				}
				else if(!strncmp(buf, "END_TIME", 8))
				{
					memcpy(upgrade_infostate->end_time,temp+1,((len < 50) ? len-1 : 49));
				}
				else if(!strncmp(buf, "AUTHORIZATION_TYPE", 18))
				{
					memcpy(upgrade_infostate->authorization_type,temp+1,((len < 50) ? len-1 : 49));
				}
			}
			memset(buf, 0, CONFIG_MAXLINE);
		}
		fclose(fp);
		g_mutex_unlock(filemutex);
	}
	g_mutex_lock(filemutex);
	fp = fopen("/usr/local/tipschecker/etc/tipscheck/device.conf","r+");
	if(fp == NULL)
	{
		g_mutex_unlock(filemutex);
		fclose(fp);
    	return 1;
	}
	else
	{
		memset(buf, 0, CONFIG_MAXLINE);
		memset(upgrade_infostate->code,0,30);
		if(NULL != fgets(buf, CONFIG_MAXLINE, fp))
		{
			if((temp = strstr(buf, "=")) != NULL)
			{
				int len = strlen(temp + 1);
				if(!strncmp(buf, "DEVICE_ID", 9))
				{
					memcpy(upgrade_infostate->code,temp+1,((len < 50) ? len-1 : 49));
				}
				
			}
		}
		g_mutex_unlock(filemutex);
		fclose(fp);
	}
	return 0;
}
int CAdmin_opt::Upgrade(product_INFO *upgrade_info,int type)//type=0代表是检测，1代表是下载升级包，并且升级
{
	int s;
//hyj	int i = 0;
	struct sockaddr_in server;
	char buff[BUFFLEN];
	char pkt_name[BUFFLEN];
//hyj	char server_ip[50];
	char version[30];
	long pkt_size = 0;
	int data_len = 0;
	int w_len = 0;
	long w_size = 0;
	int fd = 0;
	FILE *fp = NULL;
	/*删除升级失败时候残留的文件*/
	char tar_file_name[100];
	memset(version,0,sizeof(version));
	memset(tar_file_name,0,sizeof(tar_file_name));
	sprintf(tar_file_name,"rm /usr/src/tips*-* -rvf");
	system(tar_file_name);
	/*获取服务器的ip*/
        fp = fopen("/usr/local/tipschecker/etc/tipscheck/up_server_ip", "r+");
	memset(tar_file_name,0,sizeof(tar_file_name));//此时存储服务器的IP地址
	fgets(tar_file_name,100,fp);
	fclose(fp);
	if(tar_file_name[strlen(tar_file_name)-1]=='\n')
		tar_file_name[strlen(tar_file_name)-1] ='\0';
	/*获取升级包的client程序*/
	s = socket(AF_INET, SOCK_STREAM, 0);
	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(tar_file_name);
	server.sin_port = htons(SERVER_PORT);
	int err = connect(s, (struct sockaddr*)&server, sizeof(server));
	if(-1 == err)
	{
		perror("connect failed");
		close(fd);
		close(s);
		return -1;
	}
	if(type ==0)//检测版本信息
	{
		send(s,"get_information",strlen("get_information"),0);
		//sleep(2);
		recv(s,upgrade_info, sizeof(product_INFO), 0);//接收版本信息
        	fp = fopen("/usr/local/tipschecker/etc/tipscheck/system_state", "r+");
		/*进行版本比较,如果为最新版本则返回-2*/
		while(fgets(version,100,fp))
		{
			if(!strncmp(version,"version",7))
			{
				if(version[strlen(version)-1] =='\n')
					version[strlen(version)-1] ='\0';
				char buff_version[30];
				int compire[3] = {0,0,0};
				int upgrade_version[3]= {0,0,0};
				memset(buff_version,0,sizeof(buff_version));
				char *p=strchr(version,'=');
				strcpy(buff_version,p+1);
				sscanf(buff_version,"%d.%d.%d",compire,compire+1,compire+2);
				sscanf(upgrade_info->version,"%d.%d.%d",upgrade_version,upgrade_version+1,upgrade_version+2);
				printf("upgrade_info====%s\n",upgrade_info->name);
				printf("upgrade_info====%s\n",upgrade_info->version);
				printf("upgrade_info====%s\n",upgrade_info->describe);
				printf("upgrade_info====%s\n",upgrade_info->upgrade_time);
				if(upgrade_version[2] <= compire[2])
				{
					close(s);
					fclose(fp);
					return -2;
				}
			}
		}
		fclose(fp);
		close(s);
	}
	else
	{
		send(s,"get_upgrade_file",strlen("get_upgrade_file"),0);
		sleep(2);
		recv(s, buff, BUFFLEN, 0);//接收文件名字,和文件大小
		memset(pkt_name, 0, BUFFLEN);
		sscanf(buff,"%s %ld", pkt_name, &pkt_size);//用sscanf切割字符串接收文件名字和文件大小
		printf("name=%s\n",pkt_name);
		printf("size=%ld\n",pkt_size);
		fd = open(pkt_name, O_RDWR|O_CREAT,0666);
		while(1)//接收文件
		{
			memset(buff, 0, BUFFLEN);
			data_len = recv(s, buff, BUFFLEN, 0);
			w_size = w_size+data_len;
			w_len = write(fd, buff, data_len);
			printf("bufflen===%d\n",data_len);
			printf("w_len===%d\n",w_len);
			if(data_len<=0)
			{
				printf("接收错误\n");
				exit(0);
				return -1;
			}
			//if(w_len > data_len)
			//{
			//	printf("file write failed\n");
			//	close(fd);
			//	close(s);
			//	return -1;
			//}
			//printf("w_szie=%ld\n",w_size);
			if(w_size >= pkt_size)
			{
				//sprintf(buff, "ok,received completely");
				//send(s, buff, strlen(buff), 0);
				printf("yes over\n");
				break;
				//exit(0);
				//
			}
		}
		close(fd);
		close(s);
		system("./upgrade.sh");
	}
	//sprintf(tar_file_name,"tar -xvf %s",pkt_name);
	//system(tar_file_name);
	//exit(0);
	return 0;
}
#if 0 
int CAdmin_opt::Upgrade()
{
	printf("fji99999999999999999999999999999999999999999999999999999999999999999999999\n");
	//	for(;;)
		int i = 0;
		unsigned short port = 8000;
		int s;
		struct sockaddr_in server;
		char buff[1024];
		char server_ip[50];
		int n = 0;
		s = socket(AF_INET, SOCK_STREAM, 0);
		memset(&server, 0, sizeof(server));
		memset(server_ip,0,sizeof(server_ip));
		strcpy(server_ip,"172.16.0.162");
		server.sin_family = AF_INET;
		server.sin_port = htons(port);
		inet_pton(AF_INET,server_ip,&server.sin_addr);
		int err = connect(s, (struct sockaddr*)&server, sizeof(server));
		if(err != 0)
		{
			printf("connect failed\n");
	//		continue;
		}
		memset(buff, 0, 1024);
		strcpy(buff, "upgrade:654321:2.1.9");
		//strcpy(buff, "upgrade:.1.9");
		send(s, buff, strlen(buff), 0);
		memset(buff, 0, 1024);
		while(1)
		{
			i++;
			n = recv(s,buff, 1024, 0);
			if(n>0)
			{
				printf("TIME%s\n",buff);
			}
			if(i == 1)
			{
				break;
				;//第一次进来判断命令，如果没有权限break；
			}
			if(i == 2)
			{
				;//接收文件名字是否合格
			}
			if(i > 2)
			{
				;//接收文件名字是否合格
			}
		}
		close(s);
		usleep(100000);
		return 0;
}
#endif
#if 0
void insertpro_handler(gpointer key, gpointer value ,gpointer tipsparam)
{
printf("============================================00\n");
	char *filecontent = (char *)params_value((params_t *)(((param_t *)tipsparam)->values), (char *)key);
	char *fileallname = NULL;
	fileallname = g_strdup_printf("%s%s", "/root/songruijie-tankuang/import", (char *)key);
	if((NULL != filecontent) && (NULL != fileallname) && strlen(fileallname))
	{
		FILE* fp = fopen(fileallname, "w");
		if(fp == NULL){
			printf("open error\n");
			exit(0);
		}
		int num = 0;
		int ret = 0;
		//int totlesize = strlen(filecontent);
		int totlesize = ((param_t *)tipsparam)->filelen;
		char *start = filecontent;
		while((num = fwrite(start, 1, totlesize, fp)) != totlesize)
		{
			if(num < 0){
				printf("init plugin error\n");
				exit(0);
			}
			totlesize -= num;
			start += num;
		}
		char chmod_buf[100];
		memset(chmod_buf,0,100);
		sprintf(chmod_buf,"%s %s",fileallname,(char *)"0111");
		system(chmod_buf);
		//ret = chmod(fileallname, 0111);	
		g_free(fileallname);
		fclose(fp);
	}
printf("============================================01\n");
}
#endif
char *CAdmin_opt::display_user_html(const char *errmsg, char *role)
{
	char *realxml;
	if(NULL == errmsg){
		realxml = (char *)g_strdup_printf("<get_tasks_response>%s<msg></msg></get_tasks_response>",tips_sqlite_handle.User_catch(role));
//		realxml = (char *)g_strdup_printf("<get_tasks_response>%s</get_tasks_response>");
	}
	else{
		realxml = (char *)g_strdup_printf("<get_tasks_response>%s<msg> %s</msg></get_tasks_response>", tips_sqlite_handle.User_catch(role),errmsg);
		//char *errg = (char *)errmsg;
		//g_free((char *)errg);
	}
	printf("realxml======================%s\n",realxml);
	char *html = ((admin_opt_fun *)opt_fun)(credentials, realxml);
	g_free(realxml);
	return html;
}

int CAdmin_opt::get_config_manage_xml(void *config)
{
        char buf[CONFIG_MAXLINE];
        char *temp;
        g_mutex_lock(filemutex);
        FILE *fp = fopen(CONFIG_FILE, "r+");
        if(fp == NULL){
                g_mutex_unlock(filemutex);
                 return 1;
        }
        else{
                memset(buf, 0, CONFIG_MAXLINE);
                while(NULL != fgets(buf, CONFIG_MAXLINE, fp)){
                printf("buf================%s\n",buf);
                if(strlen(buf) >= CONFIG_MAXLINE-1){
	                fclose(fp);
                        printf("can't open xml\n");
                        g_mutex_unlock(filemutex);
                               return 1;
                        }
                        if((temp = strstr(buf, "=")) != NULL){
                               int len = strlen(temp + 1);
                               /*判断每个配置项的内容格式*/
                               printf("temp==============%s\tlen=%d\n",temp,len);
                               if(!strncmp(buf, "MANAGE_IP", 9)){
                                        printf("len=%d\n",len);
                                        memcpy(((config_manage_t *)config)->manage_IP,temp+1,
                                                ((len < 50) ? len-1 : 49));
                                        printf("aaaa\n");
                                }
                                else if(!strncmp(buf, "MANAGE_PORT=", 12)){
                                        memcpy(((config_manage_t *)config)->manage_PORT,temp+1,
                                               len < 50 ? len-1 : 49);
                                }
                                else if(!strncmp(buf, "MANAGE_PORT1", 12)){
                                        memcpy(((config_manage_t *)config)->manage_PORT1,temp+1,
                                               len < 50 ? len-1 : 49);
                                }
                                else if(!strncmp(buf, "GATEWAY", 7)){
                                        memcpy(((config_manage_t *)config)->gateway,temp+1,
                                               len < 50 ? len-1 : 49);
                                }
                                else if(!strncmp(buf, "STATIC_ROUTE", 12)){
                                         memcpy(((config_manage_t *)config)->static_route,temp+1,
                                                 len < 50 ? len-1 : 49);
                                }
                                else if(!strncmp(buf, "SUPERVISOR", 10)){
                                        memcpy(((config_manage_t *)config)->supervisor,temp+1,
                                               len < 50 ? len-1 : 49);
                                }
                                else if(!strncmp(buf, "SERVE_ADDR", 10)){
                                        memcpy(((config_manage_t *)config)->serve_addr,temp+1,
                                               len < 50 ? len-1 : 49);
                                }
                                else if(!strncmp(buf, "SERVE_USER", 10)){
                                        memcpy(((config_manage_t *)config)->serve_user,temp+1,
                                               len < 50 ? len-1 : 49);
                                }
                                else if(!strncmp(buf, "SERVE_PASSWORD", 14)){
                                        memcpy(((config_manage_t *)config)->serve_password,temp+1,
                                                len < 50 ? len-1 : 49);
                                }
                                else if(!strncmp(buf, "ADMINSTRATION_IP=", 17)){
                                        memcpy(((config_manage_t *)config)->adminstration_IP,temp+1,
                                                len < 50 ? len-1 : 49);
                                }
                                else if(!strncmp(buf, "ADMINSTRATION_IP1", 17)){
                                        memcpy(((config_manage_t *)config)->adminstration_IP1,temp+1,
                                                len < 50 ? len-1 : 49);
                                }
                                else if(!strncmp(buf, "UNIT_NAME", 9)){
                                        memcpy(((config_manage_t *)config)->unit_name,temp+1,
                                                len < 256 ? len-1 : 255);
                                }
                                else if(!strncmp(buf, "NET_IP", 6)){
                                        memcpy(((config_manage_t *)config)->net_IP,temp+1,
                                               len < 256 ? len-1 : 255);
                                }
                        }
                        memset(buf, 0, CONFIG_MAXLINE);
                }
                fclose(fp);
                g_mutex_unlock(filemutex);
                return 0;
        }
 
}


void CAdmin_opt::MyTurnTime(const time_t clock, char *tips_clock, int clocksize)
{
        struct tm *tm;
        char *format=(char *)"%Y-%m-%d %H:%M:%S";
        tm = localtime(&clock);
        strftime(tips_clock,  clocksize, format,tm);
}


time_t CAdmin_opt::MyObtainTime(char* date)
{
        struct tm tm;
        int day = 0, month = 0, year = 0, hour = 0, min = 0, sec = 0, sum = 0, leap = 0,weekid = 0;
        int i = 2;
        char buf[1024];
        memset(buf, 0, sizeof (buf));
        strcpy(buf, date);
        char *token = NULL;
        token = strtok(buf, "-");

        year = atoi(token);
        tm.tm_year = year - 1900;
        while ((token = strtok(NULL, "-")) != NULL)
        {
                switch (i)
                {
                        case 2:
                                                 month = atoi(token);
                               tm.tm_mon = month - 1;
                                                 break;
                        case 3:
                                                 day = atoi(token);
                                                tm.tm_mday = day;
                                                break;
                        case 4:
                                                hour = atoi(token);
                               if (hour < 0 || hour > 23)
                               {
                                                        fprintf (stderr, "Error:%s\n", strerror(errno));
                                                        exit(0);
                                                        }
                                                        tm.tm_hour = hour;
                                                        break;
                        case 5:
                                                        min = atoi(token);
                                                if ((min < 0) || (min > 59))
                                                {
                                                                fprintf (stderr, "Error:%s\n", strerror(errno));
                                                                exit(0);
                                                        }
                                                tm.tm_min = min;
                                                break;
                        case 6:
                                                sec = atoi(token);
                                                if ((sec < 0) || (sec > 59))
                                                {
                                                                fprintf (stderr, "Error:%s\n", strerror(errno));
                                                                exit(0);
                                                }
                                                tm.tm_sec = sec;
                        default:
                                                break;
                }
                i++;
        }
        tm.tm_sec = sec;
        switch (month)
        {
                case 1:
                                sum = 0;
                                break;
                case 2:
                                sum = 31;
                                break;
                case 3:
                                sum = 59;
                                break;
                case 4:
                                sum = 90;
                                break;
                case 5:
                                sum = 120;
                                break;
                case 6:
                                sum = 151;
                                break;
                case 7:
                                sum = 181;
                                break;
                case 8:
                                sum = 212;
                                break;
                case 9:
                                sum = 243;
                                break;
                case 10:
                                sum = 273;
                                break;
                case 11:
                                sum = 304;
                                break;
                case 12:
                                sum = 334;
                                break;
                default:
                                fprintf(stdout, "data error\n");
                                break;
        }
        sum = sum + day;
        if (year % 400 == 0 || (year % 4 == 0 && year % 100 != 0))
                leap = 1;
        else
                leap = 0;
        if (leap == 1 && month > 2)
                sum ++;
        if (month == 2)
        {
                month += 12;
                year --;
        }
        weekid = (day + 2 * month + 3 * (month + 1) / 5 + year + year / 4 - year / 100 + year / 400) % 7 + 1;
        if (weekid == 7)
                weekid = 0;
        tm.tm_wday = weekid;
        tm.tm_yday = sum - 1;
        tm.tm_isdst = 0;
        time_t time = mktime(&tm);

        return time;
}

int CAdmin_opt::caculate_time(char date_time[][11])
{
	
	int year,month,day,last_year,last_month,year_flag = 0;
	int i = 1,j = 0;
	char dalte_buf[10];
	int day_temp = 0;
//	char date_time[5][11];
	struct tm *p;
	time_t date;
	char time_buff[30];
	char tim[20];
	time(&date);
	memset(time_buff,0,sizeof(time_buff));
	memset(tim,0,sizeof(tim));
//	for(j=0;j<5;j++)
//		memset(date_time[j],0,sizeof(date_time[j]));
	p = localtime(&date);
	sprintf(time_buff,"%d-%02d-%02d-%02d-%02d-%02d",p->tm_year+1900,p->tm_mon+1,p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);
	strncpy(tim,time_buff,10);
	puts(tim);
	puts(time_buff);
	sscanf(tim,"%d-%d-%d%s",&year,&month,&day,dalte_buf);
	printf("%d:::%d::::%d\n",year,month,day);
	day_temp = day;
	last_year = year;
	last_month= month;
	if((year%4==0 && year%100 !=0) || year%400 ==0)
		year_flag = 1;	
	printf("year_flag=================%d\n",year_flag);
//	day = 7;
	for(i=1;i<6;i++)	
	{
		if(day==1)
			break;
		day = day -1;
//		printf("day================%d\n",day);
	}
	printf("i=============%d\n",i);
	if(i <  5)
	{
		if(i==1)	
		{
			if(month==1)
				{
					last_year=year-1;
					last_month= 12;
					sprintf(date_time[0],"%d-%02d-%02d",last_year,last_month,28);
					sprintf(date_time[1],"%d-%02d-%02d",last_year,last_month,29);
					sprintf(date_time[2],"%d-%02d-%02d",last_year,last_month,30);
					sprintf(date_time[3],"%d-%02d-%02d",last_year,last_month,31);
					sprintf(date_time[4],"%d-%02d-%02d",year,month,day_temp);
				}
			else if(month==2 || month==4 || month==6 || month == 9 || month==11)
				{
					last_month = month-1;		
					sprintf(date_time[0],"%d-%02d-%02d",year,last_month,28);
					sprintf(date_time[1],"%d-%02d-%02d",year,last_month,29);
					sprintf(date_time[2],"%d-%02d-%02d",year,last_month,30);
					sprintf(date_time[3],"%d-%02d-%02d",year,last_month,31);
					sprintf(date_time[4],"%d-%02d-%02d",year,month,day_temp);
				}
			else if(month==3)
				{
					last_month = month-1;	
					if(year_flag)
					{
						sprintf(date_time[0],"%d-%02d-%02d",year,last_month,26);
						sprintf(date_time[1],"%d-%02d-%02d",year,last_month,27);
						sprintf(date_time[2],"%d-%02d-%02d",year,last_month,28);
						sprintf(date_time[3],"%d-%02d-%02d",year,last_month,29);
						sprintf(date_time[4],"%d-%02d-%02d",year,month,day_temp);
					}
					else
					{
						sprintf(date_time[0],"%d-%02d-%02d",year,last_month,25);
						sprintf(date_time[1],"%d-%02d-%02d",year,last_month,26);
						sprintf(date_time[2],"%d-%02d-%02d",year,last_month,27);
						sprintf(date_time[3],"%d-%02d-%02d",year,last_month,28);
						sprintf(date_time[4],"%d-%02d-%02d",year,month,day_temp);
					}
				}
			else 
				{
						last_month = month-1;
						sprintf(date_time[0],"%d-%02d-%02d",year,last_month,27);
						sprintf(date_time[1],"%d-%02d-%02d",year,last_month,28);
						sprintf(date_time[2],"%d-%02d-%02d",year,last_month,29);
						sprintf(date_time[3],"%d-%02d-%02d",year,last_month,30);
						sprintf(date_time[4],"%d-%02d-%02d",year,month,day_temp);
				}
		}
		else if(i==2)	
		{
				if(month==1)
				{
					last_year=year-1;
					last_month= 12;
					sprintf(date_time[0],"%d-%02d-%02d",last_year,last_month,29);
					sprintf(date_time[1],"%d-%02d-%02d",last_year,last_month,30);
					sprintf(date_time[2],"%d-%02d-%02d",last_year,last_month,31);
					sprintf(date_time[3],"%d-%02d-%02d",year,month,day_temp-1);
					sprintf(date_time[4],"%d-%02d-%02d",year,month,day_temp);
				}
			else if(month==2 || month==4 ||month==6 || month == 9 ||month==11)
				{
					last_month = month-1;		
					sprintf(date_time[0],"%d-%02d-%02d",year,last_month,29);
					sprintf(date_time[1],"%d-%02d-%02d",year,last_month,30);
					sprintf(date_time[2],"%d-%02d-%02d",year,last_month,31);
					sprintf(date_time[3],"%d-%02d-%02d",year,month,day_temp-1);
					sprintf(date_time[4],"%d-%02d-%02d",year,month,day_temp);
				}
			else if(month==3)
				{
					last_month = month-1;	
					if(year_flag)
					{
						sprintf(date_time[0],"%d-%02d-%02d",year,last_month,27);
						sprintf(date_time[1],"%d-%02d-%02d",year,last_month,28);
						sprintf(date_time[2],"%d-%02d-%02d",year,last_month,29);
						sprintf(date_time[3],"%d-%02d-%02d",year,month,day_temp-1);
						sprintf(date_time[4],"%d-%02d-%02d",year,month,day_temp);
					}
					else
					{
						sprintf(date_time[0],"%d-%02d-%02d",year,last_month,26);
						sprintf(date_time[1],"%d-%02d-%02d",year,last_month,27);
						sprintf(date_time[2],"%d-%02d-%02d",year,last_month,28);
						sprintf(date_time[3],"%d-%02d-%02d",year,month,day_temp-1);
						sprintf(date_time[4],"%d-%02d-%02d",year,month,day_temp);
					}
				}
			else 
				{
						last_month = month-1;
						sprintf(date_time[0],"%d-%02d-%02d",year,last_month,28);
						sprintf(date_time[1],"%d-%02d-%02d",year,last_month,29);
						sprintf(date_time[2],"%d-%02d-%02d",year,last_month,30);
						sprintf(date_time[3],"%d-%02d-%02d",year,month,day_temp-1);
						sprintf(date_time[4],"%d-%02d-%02d",year,month,day_temp);
				}
		}
		else if(i==3)	
		{
			if(month==1)
				{
					last_year=year-1;
					last_month= 12;
					sprintf(date_time[0],"%d-%02d-%02d",last_year,last_month,30);
					sprintf(date_time[1],"%d-%02d-%02d",last_year,last_month,31);
					sprintf(date_time[2],"%d-%02d-%02d",year,month,day_temp-2);
					sprintf(date_time[3],"%d-%02d-%02d",year,month,day_temp-1);
					sprintf(date_time[4],"%d-%02d-%02d",year,month,day_temp);
				}
			else if(month==2 || month==4 ||month==6 || month == 9 ||month==11)
				{
					last_month = month-1;		
					sprintf(date_time[0],"%d-%02d-%02d",year,last_month,30);
					sprintf(date_time[1],"%d-%02d-%02d",year,last_month,31);
					sprintf(date_time[2],"%d-%02d-%02d",year,month,day_temp-2);
					sprintf(date_time[3],"%d-%02d-%02d",year,month,day_temp-1);
					sprintf(date_time[4],"%d-%02d-%02d",year,month,day_temp);
				}
			else if(month==3)
				{
					last_month = month-1;	
					if(year_flag)
					{
						sprintf(date_time[0],"%d-%02d-%02d",year,last_month,28);
						sprintf(date_time[1],"%d-%02d-%02d",year,last_month,29);
						sprintf(date_time[2],"%d-%02d-%02d",year,month,day_temp-2);
						sprintf(date_time[3],"%d-%02d-%02d",year,month,day_temp-1);
						sprintf(date_time[4],"%d-%02d-%02d",year,month,day_temp);
					}
					else
					{
						sprintf(date_time[0],"%d-%02d-%02d",year,last_month,27);
						sprintf(date_time[1],"%d-%02d-%02d",year,last_month,28);
						sprintf(date_time[2],"%d-%02d-%02d",year,month,day_temp-2);
						sprintf(date_time[3],"%d-%02d-%02d",year,month,day_temp-1);
						sprintf(date_time[4],"%d-%02d-%02d",year,month,day_temp);
					}
				}
			else 
				{
						last_month = month-1;
						sprintf(date_time[0],"%d-%02d-%02d",year,last_month,29);
						sprintf(date_time[1],"%d-%02d-%02d",year,last_month,30);
						sprintf(date_time[2],"%d-%02d-%02d",year,month,day_temp-2);
						sprintf(date_time[3],"%d-%02d-%02d",year,month,day_temp-1);
						sprintf(date_time[4],"%d-%02d-%02d",year,month,day_temp);
				}

		}
		else if(i==4)
		{
				if(month==1)
				{
					last_year=year-1;
					last_month= 12;
					sprintf(date_time[0],"%d-%02d-%02d",last_year,last_month,31);
					sprintf(date_time[1],"%d-%02d-%02d",year,month,day_temp-3);
					sprintf(date_time[2],"%d-%02d-%02d",year,month,day_temp-2);
					sprintf(date_time[3],"%d-%02d-%02d",year,month,day_temp-1);
					sprintf(date_time[4],"%d-%02d-%02d",year,month,day_temp);
				}
			else if(month==2 || month==4 ||month==6 || month == 9 ||month==11)
				{
					last_month = month-1;		
					sprintf(date_time[0],"%d-%02d-%02d",year,last_month,31);
					sprintf(date_time[1],"%d-%02d-%02d",year,month,day_temp-3);
					sprintf(date_time[2],"%d-%02d-%02d",year,month,day_temp-2);
					sprintf(date_time[3],"%d-%02d-%02d",year,month,day_temp-1);
					sprintf(date_time[4],"%d-%02d-%02d",year,month,day_temp);
				}
			else if(month==3)
				{
					last_month = month-1;	
					if(year_flag)
					{
						sprintf(date_time[0],"%d-%02d-%02d",year,last_month,29);
						sprintf(date_time[1],"%d-%02d-%02d",year,month,day_temp-3);
						sprintf(date_time[2],"%d-%02d-%02d",year,month,day_temp-2);
						sprintf(date_time[3],"%d-%02d-%02d",year,month,day_temp-1);
						sprintf(date_time[4],"%d-%02d-%02d",year,month,day_temp);
					}
					else
					{
						sprintf(date_time[0],"%d-%02d-%02d",year,last_month,28);
						sprintf(date_time[1],"%d-%02d-%02d",year,month,day_temp-3);
						sprintf(date_time[2],"%d-%02d-%02d",year,month,day_temp-2);
						sprintf(date_time[3],"%d-%02d-%02d",year,month,day_temp-1);
						sprintf(date_time[4],"%d-%02d-%02d",year,month,day_temp);
					}
				}
			else 
				{
						last_month = month-1;
						sprintf(date_time[0],"%d-%02d-%02d",year,last_month,30);
						sprintf(date_time[1],"%d-%02d-%02d",year,month,day_temp-3);
						sprintf(date_time[2],"%d-%02d-%02d",year,month,day_temp-2);
						sprintf(date_time[3],"%d-%02d-%02d",year,month,day_temp-1);
						sprintf(date_time[4],"%d-%02d-%02d",year,month,day_temp);
				}

		}
	}
	else 
	{
		printf("dat_temp===========%d\n",day_temp);
		printf("dat_temp===========%d\n",year);
		printf("dat_temp===========%d\n",month);
		sprintf(date_time[0],"%d-%02d-%02d",year,month,day_temp-4);
		sprintf(date_time[1],"%d-%02d-%02d",year,month,day_temp-3);
		sprintf(date_time[2],"%d-%02d-%02d",year,month,day_temp-2);
		sprintf(date_time[3],"%d-%02d-%02d",year,month,day_temp-1);
		sprintf(date_time[4],"%d-%02d-%02d",year,month,day_temp);
	}
	for (j=0;j<5;j++)
		printf("%s\n",date_time[j]);
	return 0;
}

int CAdmin_opt::judge_time(const char *start_time,const char *end_time)
{
	int start_year=0,start_month = 0,start_day = 0;	
	int end_year=0,end_month = 0,end_day = 0;	
	char start_dalte_buf[10];
	char end_dalte_buf[10];
	memset(start_dalte_buf,0,sizeof(start_dalte_buf));
	memset(end_dalte_buf,0,sizeof(end_dalte_buf));
	sscanf(start_time,"%d-%d-%d%s",&start_year,&start_month,&start_day,start_dalte_buf);
	sscanf(end_time,"%d-%d-%d%s",&end_year,&end_month,&end_day,end_dalte_buf);
	printf("start_year=%d,start_month=%d,start_day=%d\n",start_year,start_month,start_day);
	printf("end_year=%d,end_month=%d,end_day=%d\n",end_year,end_month,end_day);
       if(start_year > end_year)
               return 1;
       else if(start_year < end_year)//1代表用户时间过期，0代表未过期
               return 0;
       else if(start_year == end_year)
       {
               if(start_month > end_month)
                       return 1;
               else if(start_month < end_month)
                       return 0;
               else if(start_month == end_month)
               {

                       if(start_day > end_day)
                               return 1;
                       else if(start_day == end_day)
                               return 0;
		       else
				return 0;
               }

       }
#if 0
	if(start_year > end_year)
		return 1;
	else
	{
		if(start_month > end_month)
			return 1;
		else
		{
			if(start_day > end_day)
				return 1;
		}

	}
#endif 
	return 0;
}
int CAdmin_opt::get_system_config_xml(void *config)
{
        char buf[CONFIG_MAXLINE];
        char *temp = NULL;
        g_mutex_lock(filemutex);
        FILE *fp = fopen(SYSTEM_CONFIG_FILE, "r+");
        if(fp == NULL){
                g_mutex_unlock(filemutex);
                 return 1;
        }
        else{
                memset(buf, 0, CONFIG_MAXLINE);
                while(NULL != fgets(buf, CONFIG_MAXLINE, fp)){
                printf("buf================%s\n",buf);
                if(strlen(buf) >= CONFIG_MAXLINE-1){
	                fclose(fp);
                        printf("can't open xml\n");
                        g_mutex_unlock(filemutex);
                               return 1;
                        }
                        if((temp = strstr(buf, "=")) != NULL){
                               int len = strlen(temp + 1);
                               /*判断每个配置项的内容格式*/
                               printf("temp==============%s\tlen=%d\n",temp,len);
                               if(!strncmp(buf, "CPU", 3)){
                                        printf("len=%d\n",len);
                                        memcpy(((system_config_t *)config)->cpu,temp+1,
                                                ((len < 50) ? len-1 : 49));
                                        printf("aaaa\n");
                                }
                                else if(!strncmp(buf, "MEM", 3)){
                                        memcpy(((system_config_t *)config)->mem,temp+1,
                                               len < 50 ? len-1 : 49);
                                }
                                else if(!strncmp(buf, "DISK", 4)){
                                         memcpy(((system_config_t *)config)->disk,temp+1,
                                                 len < 50 ? len-1 : 49);
                                }
                                else if(!strncmp(buf, "RUNTIME", 7)){
                                        memcpy(((system_config_t *)config)->runtime,temp+1,
                                               len < 50 ? len-1 : 49);
                                }
                                else if(!strncmp(buf, "ALARMTIME", 9)){
                                        memcpy(((system_config_t *)config)->alarmtime,temp+1,
                                               len < 256 ? len-1 : 255);
                                }
                                else if(!strncmp(buf, "SYNTIME", 7)){
                                        memcpy(((system_config_t *)config)->syntime,temp+1,
                                               len < 256 ? len-1 : 255);
                                }
                                else if(!strncmp(buf, "NETSTATETIME", 12)){
                                        memcpy(((system_config_t *)config)->netstatetime,temp+1,
                                               len < 256 ? len-1 : 255);
                                }
                        }
                        memset(buf, 0, CONFIG_MAXLINE);
                }
                fclose(fp);
                g_mutex_unlock(filemutex);
                return 0;
        }
 
}

int CAdmin_opt::init_one_config(const char *source, const char *descript, FILE *fp)
{
        printf("source:%s descript:%s\n",source, descript);
        if(strlen(source)){
	        gchar* xml = g_strdup_printf("%s=%s\n", descript, source);
                int len = fwrite(xml, 1, strlen(xml), fp);
               printf("init_one_config:%s  len=%d\n",xml, len);
                g_free(xml);
        }

//		else 
//		{
//	        gchar* xml = g_strdup_printf("%s=%s\n", descript, "0");
//                int len = fwrite(xml, 1, strlen(xml), fp);
//                printf("init_one_config:%s\tlen=%d\n",xml, len);
//                g_free(xml);
//		}
        return 0;
}

int CAdmin_opt::config_get_device(status_struct_t *devicesta)
{
	printf("in config_get_device ing \n");
		int fd = ui_cli.cli_connect((const char *)"/tmp/tips_ui_cli.domain",(const char *)"/tmp/UNIXUI.domain");
		ui_cli.cli_write(fd, 7);
		printf("in config_get_device ing \n");
		while(ui_cli.cli_read_string(fd, devicesta, sizeof(status_struct_t)) == -1){
			printf("catch device state error\n");
		};
		close(fd);
		return 0;
}

char* CAdmin_opt::config_manage_modify(config_manage_t* mconfig, credentials_t* credentials,int type)
{
	config_manage_t sconfig;
//hyj        gchar xml;
	char turnipbuf[100];
	int send_to_manger_flag = 0;
	memset(turnipbuf, 0, 100);
	memset(&sconfig, 0, sizeof(config_manage_t));
        /*是否需要判断返回值？int format =*/ get_config_manage_xml(&sconfig);
        g_mutex_lock(filemutex);
        FILE *fp = fopen(CONFIG_FILE, "w+");
        if(NULL == fp){
                g_mutex_unlock(filemutex);
                return NULL;
        }
	if(0 == type)//配置监控口(eth0)
	{
		if(strcmp(mconfig->manage_IP, sconfig.manage_IP))
		{
			init_one_config(mconfig->manage_IP, "MANAGE_IP", fp);
			snprintf(turnipbuf, 99, "ifconfig %s %s netmask %s",MANAGE_ETH,mconfig->manage_IP,mconfig->manage_mask);
			system(turnipbuf);
			mconfig->modify_flag = 1;
			Change_ip(mconfig->manage_IP, (char *)MANAGE_ETH0_FILE);//需要判断返回值；
			tips_sqlite_handle.catch_opt(credentials->username, (char *)"修改管理口IP成功");
			sleep(2);
			system((char *)"init 6");
		}
		else 
		{
			init_one_config(sconfig.manage_IP, "MANAGE_IP", fp);
		}
		init_one_config(sconfig.manage_PORT,"MANAGE_PORT",fp);
		init_one_config(sconfig.manage_PORT1,"MANAGE_PORT1",fp);
		init_one_config(sconfig.net_IP,"NET_IP",fp);
		init_one_config(sconfig.gateway,"GATEWAY",fp);
		init_one_config(sconfig.static_route,"STATIC_ROUTE",fp);
		init_one_config(sconfig.supervisor,"SUPERVISOR",fp);
		init_one_config(sconfig.serve_addr,"SERVE_ADDR",fp);
		init_one_config(sconfig.serve_user,"SERVE_USER",fp);
		init_one_config(sconfig.serve_password,"SERVE_PASSWORD",fp);
		init_one_config(sconfig.adminstration_IP,"ADMINSTRATION_IP",fp);
		init_one_config(sconfig.adminstration_IP1,"ADMINSTRATION_IP1",fp);
		init_one_config(sconfig.unit_name,"UNIT_NAME",fp);
	}
	else if(1 == type)//配置联网、通信口(eth1)
	{
		if(strcmp(mconfig->net_IP, sconfig.net_IP))//改变IP值(eth1)
		{
			if(!strlen(mconfig->net_IP))
			{
				init_one_config(sconfig.net_IP, "NET_IP", fp);
			}
			else
			{
				init_one_config(mconfig->net_IP, "NET_IP", fp);
				snprintf(turnipbuf, 99, "ifconfig %s %s netmask %s",(char *)"eth1",mconfig->net_IP,mconfig->mask);
				system(turnipbuf);
				Change_ip(mconfig->net_IP, (char *)MANAGE_ETH1_FILE);//需要判断返回值；
				mconfig->modify_flag = 1;
				send_to_manger_flag = 1;
				tips_sqlite_handle.catch_opt(credentials->username, (char *)"修改联网口IP成功");
			}
		}
		else 
		{
			init_one_config(sconfig.net_IP, "NET_IP", fp);
		}
		if(!strcmp(mconfig->gateway,(char *)"0.0.0.0"))
			init_one_config(sconfig.gateway, "GATEWAY", fp);
		else//改变eth1的网关
		{
			if(strcmp(mconfig->gateway, sconfig.gateway))
			{
				if(!strlen(mconfig->gateway))
				{
					init_one_config(sconfig.gateway, "GATEWAY", fp);
				}
				else
				{
	        			char *route_delete_down = (char *)"ip link set eth1 down";
                			char *route_delete_up = (char *)"ip link set eth1 up";
                			system(route_delete_down);
                			system(route_delete_up);
					init_one_config(mconfig->gateway, "GATEWAY", fp);
					mconfig->modify_flag = 1;
					snprintf(turnipbuf, 99, "route add default gw %s eth1",mconfig->gateway);
					system(turnipbuf);
					printf("gateway=%s\n",turnipbuf);
					change_route(mconfig->static_route, mconfig->gateway);
					Change_gateway(mconfig->gateway, (char *)"/etc/sysconfig/network");
					tips_sqlite_handle.catch_opt(credentials->username, (char *)"修改网关成功");
					send_to_manger_flag = 1;
				}
			}
			else 
				init_one_config(sconfig.gateway, "GATEWAY", fp);
		}
		if(!strcmp(mconfig->static_route,(char *)"0.0.0.0"))
			init_one_config(sconfig.gateway, "STATIC_ROUTE", fp);
		else
		{
			if(strcmp(mconfig->static_route, sconfig.static_route))
			{
				if(!strlen(mconfig->static_route))
				{
					init_one_config(sconfig.static_route, "STATIC_ROUTE", fp);
				}
				else
				{
					init_one_config(mconfig->static_route, "STATIC_ROUTE", fp);
					mconfig->modify_flag = 1;
					change_route(mconfig->static_route, mconfig->adminstration_IP);
					snprintf(turnipbuf, 99, "route add -host %s gw %s dev eth1",mconfig->adminstration_IP, mconfig->static_route);
					system(turnipbuf);
					printf("%s\n",turnipbuf);
					tips_sqlite_handle.catch_opt(credentials->username, (char *)"修改静态路由成功");
					send_to_manger_flag = 1;
				}
			}
			else 
			{
				init_one_config(sconfig.static_route, "STATIC_ROUTE", fp);
			}
		}
		init_one_config(sconfig.manage_IP,"MANAGE_IP",fp);
		init_one_config(sconfig.manage_PORT,"MANAGE_PORT",fp);
		init_one_config(sconfig.manage_PORT1,"MANAGE_PORT1",fp);
		init_one_config(sconfig.supervisor,"SUPERVISOR",fp);
		init_one_config(sconfig.serve_addr,"SERVE_ADDR",fp);
		init_one_config(sconfig.serve_user,"SERVE_USER",fp);
		init_one_config(sconfig.serve_password,"SERVE_PASSWORD",fp);
		init_one_config(sconfig.adminstration_IP,"ADMINSTRATION_IP",fp);
		init_one_config(sconfig.adminstration_IP1,"ADMINSTRATION_IP1",fp);
		init_one_config(sconfig.unit_name,"UNIT_NAME",fp);
	}
	else if(2 == type)
	{
		/*修改主管理中心IP*/
		if(strcmp(mconfig->adminstration_IP, sconfig.adminstration_IP))
		{
			if(!strlen(mconfig->adminstration_IP))
			{
				init_one_config(sconfig.adminstration_IP, "ADMINSTRATION_IP", fp);
			}
			else
			{
				init_one_config(mconfig->adminstration_IP, "ADMINSTRATION_IP", fp);
				mconfig->modify_flag = 1;
				send_to_manger_flag = 1;
				tips_sqlite_handle.catch_opt(credentials->username, (char *)"修改管理中心IP成功");
			}
		}
		else 
		{
			init_one_config(sconfig.adminstration_IP, "ADMINSTRATION_IP", fp);
		}
		/*修改主管理中心端口号*/
		if(strcmp(mconfig->manage_PORT, sconfig.manage_PORT))
		{
			if(!strlen(mconfig->manage_PORT))
			{
				init_one_config(sconfig.manage_PORT, "MANAGE_PORT", fp);
			}
			else
			{
				init_one_config(mconfig->manage_PORT, "MANAGE_PORT", fp);
				mconfig->modify_flag = 1;
				send_to_manger_flag = 1;
				tips_sqlite_handle.catch_opt(credentials->username, (char *)"修改管理中心PORT成功");
			}
		}
		else 
		{
			init_one_config(sconfig.manage_PORT, "MANAGE_PORT", fp);
		}
		/*修改副管理中心IP*/
		if(strcmp(mconfig->adminstration_IP1, sconfig.adminstration_IP1))
		{
			if(!strlen(mconfig->adminstration_IP1))
			{
				init_one_config(sconfig.adminstration_IP1, "ADMINSTRATION_IP1", fp);
			}
			else
			{
				init_one_config(mconfig->adminstration_IP1, "ADMINSTRATION_IP1", fp);
				mconfig->modify_flag = 1;
				send_to_manger_flag = 1;
				tips_sqlite_handle.catch_opt(credentials->username, (char *)"修改副管理中心IP成功");
			}
		}
		else 
		{
			init_one_config(sconfig.adminstration_IP1, "ADMINSTRATION_IP1", fp);
		}
		/*修改副管理中心端口号*/
		if(strcmp(mconfig->manage_PORT1, sconfig.manage_PORT1))
		{
			if(!strlen(mconfig->manage_PORT1))
			{
				init_one_config(sconfig.manage_PORT1, "MANAGE_PORT1", fp);
			}
			else
			{
				init_one_config(mconfig->manage_PORT1, "MANAGE_PORT1", fp);
				send_to_manger_flag = 1;
				mconfig->modify_flag = 1;
				tips_sqlite_handle.catch_opt(credentials->username, (char *)"修改副管理中心PORT成功");
			}
		}
		else 
		{
			init_one_config(sconfig.manage_PORT1, "MANAGE_PORT1", fp);
		}
		init_one_config(sconfig.manage_IP,"MANAGE_IP",fp);
		init_one_config(sconfig.net_IP,"NET_IP",fp);
		init_one_config(sconfig.gateway,"GATEWAY",fp);
		init_one_config(sconfig.static_route,"STATIC_ROUTE",fp);
		init_one_config(sconfig.supervisor,"SUPERVISOR",fp);
		init_one_config(sconfig.serve_addr,"SERVE_ADDR",fp);
		init_one_config(sconfig.serve_user,"SERVE_USER",fp);
		init_one_config(sconfig.serve_password,"SERVE_PASSWORD",fp);
		init_one_config(sconfig.unit_name,"UNIT_NAME",fp);
	}
	else if(3 == type)
	{
		if(strcmp(mconfig->serve_addr, sconfig.serve_addr))
		{
			if(!strlen(mconfig->serve_addr))
			{
				init_one_config(sconfig.serve_addr, "SERVE_ADDR", fp);
			}
			else
			{
				init_one_config(mconfig->serve_addr, "SERVE_ADDR", fp);
				mconfig->modify_flag = 1;
				tips_sqlite_handle.catch_opt(credentials->username, (char *)"修改代理服务器成功");
			}
		}
		else 
		{
			init_one_config(sconfig.serve_addr, "SERVE_ADDR", fp);
		}

		if(strcmp(mconfig->serve_user, sconfig.serve_user))
		{
			if(!strlen(mconfig->serve_user))
			{
				init_one_config(sconfig.serve_user, "SERVE_USER", fp);
			}
			else
			{
				init_one_config(mconfig->serve_user, "SERVE_USER", fp);
				mconfig->modify_flag = 1;
				tips_sqlite_handle.catch_opt(credentials->username, (char *)"修改代理服务器账号成功");
			}
		}
		else 
		{
			init_one_config(sconfig.serve_user, "SERVE_USER", fp);
		}
		
		if(strcmp(mconfig->serve_password, sconfig.serve_password))
		{
			if(!strlen(mconfig->serve_password))
			{
				init_one_config(sconfig.serve_password, "SERVE_PASSWORD", fp);
			}
			else
			{
				init_one_config(mconfig->serve_password, "SERVE_PASSWORD", fp);
				mconfig->modify_flag = 1;
				tips_sqlite_handle.catch_opt(credentials->username, (char *)"修改代理服务器口令成功");
			}
		}
		else 
		{
			init_one_config(sconfig.serve_password, "SERVE_PASSWORD", fp);
		}
		init_one_config(sconfig.manage_IP,"MANAGE_IP",fp);
		init_one_config(sconfig.manage_PORT,"MANAGE_PORT",fp);
		init_one_config(sconfig.manage_PORT1,"MANAGE_PORT1",fp);
		init_one_config(sconfig.net_IP,"NET_IP",fp);
		init_one_config(sconfig.gateway,"GATEWAY",fp);
		init_one_config(sconfig.static_route,"STATIC_ROUTE",fp);
		init_one_config(sconfig.supervisor,"SUPERVISOR",fp);
		init_one_config(sconfig.adminstration_IP,"ADMINSTRATION_IP",fp);
		init_one_config(sconfig.adminstration_IP1,"ADMINSTRATION_IP1",fp);
		init_one_config(sconfig.unit_name,"UNIT_NAME",fp);
		
	}
	else if(4 == type)
	{
		if(strcmp(mconfig->unit_name, sconfig.unit_name))
		{
			if(!strlen(mconfig->unit_name))
			{
				init_one_config(sconfig.unit_name, "UNIT_NAME", fp);
			}
			else
			{
				init_one_config(mconfig->unit_name, "UNIT_NAME", fp);
				mconfig->modify_flag = 1;
				send_to_manger_flag = 1;
				tips_sqlite_handle.catch_opt(credentials->username, (char *)"修改部属单位信息成功");
			}
		}
		else 
		{
			init_one_config(sconfig.unit_name, "UNIT_NAME", fp);
		}
		init_one_config(sconfig.manage_IP,"MANAGE_IP",fp);
		init_one_config(sconfig.manage_PORT,"MANAGE_PORT",fp);
		init_one_config(sconfig.manage_PORT1,"MANAGE_PORT1",fp);
		init_one_config(sconfig.net_IP,"NET_IP",fp);
		init_one_config(sconfig.gateway,"GATEWAY",fp);
		init_one_config(sconfig.static_route,"STATIC_ROUTE",fp);
		init_one_config(sconfig.supervisor,"SUPERVISOR",fp);
		init_one_config(sconfig.serve_addr,"SERVE_ADDR",fp);
		init_one_config(sconfig.serve_user,"SERVE_USER",fp);
		init_one_config(sconfig.serve_password,"SERVE_PASSWORD",fp);
		init_one_config(sconfig.adminstration_IP,"ADMINSTRATION_IP",fp);
		init_one_config(sconfig.adminstration_IP1,"ADMINSTRATION_IP1",fp);
		//init_one_config(mconfig->unit_name,"UNIT_NAME",fp);
	}
         if(send_to_manger_flag == 1)
         {
      		int fd = ui_cli.cli_connect((const char *)"/tmp/tips_ui_cli.domain",(const char *)"/tmp/UNIXUI.domain");
                ui_cli.cli_write(fd, 1);
                close(fd);
         }
	fclose(fp);
#if 0 
		if(0==type)
		{
				if(strcmp(mconfig->manage_IP, sconfig.manage_IP))
				{
					if(!strlen(mconfig->manage_IP))
					{
						init_one_config(sconfig.manage_IP, "MANAGE_IP", fp);
					}
					else
					{
						init_one_config(mconfig->manage_IP, "MANAGE_IP", fp);
						snprintf(turnipbuf, 99, "ifconfig %s %s netmask 255.255.255.0",MANAGE_ETH,mconfig->manage_IP);
						system(turnipbuf);
						mconfig->modify_flag = 1;
						Change_ip(mconfig->manage_IP, (char *)MANAGE_ETH0_FILE);//需要判断返回值；
						tips_sqlite_handle.catch_opt(credentials->username, (char *)"修改管理口IP成功");
						sleep(5);
						system((char *)"init 6");
						judge = 1;
					}
				}
				else 
				{
					init_one_config(sconfig.manage_IP, "MANAGE_IP", fp);
				}


				if(strcmp(mconfig->net_IP, sconfig.net_IP))
				{
				if(!strlen(mconfig->net_IP))
				{
					init_one_config(sconfig.net_IP, "NET_IP", fp);
				}
				else
					{
						init_one_config(mconfig->net_IP, "NET_IP", fp);
						snprintf(turnipbuf, 99, "ifconfig %s %s netmask 255.255.255.0",(char *)"eth1",mconfig->net_IP);
						system(turnipbuf);
						Change_ip(mconfig->net_IP, (char *)MANAGE_ETH1_FILE);//需要判断返回值；
						mconfig->modify_flag = 1;
						send_to_manger_flag[0] = 1;
						//int fd = ui_cli.cli_connect((const char *)"/tmp/tips_ui_cli.domain",(const char *)"/tmp/UNIXUI.domain");
						//ui_cli.cli_write(fd, 0);
						//close(fd);
						tips_sqlite_handle.catch_opt(credentials->username, (char *)"修改联网口IP成功");
					}
				}
				else 
				{
					init_one_config(sconfig.net_IP, "NET_IP", fp);
				}

				if(strcmp(mconfig->gateway, sconfig.gateway))
				{
					if(!strlen(mconfig->gateway))
					{
						init_one_config(sconfig.gateway, "GATEWAY", fp);
					}
					else
					{
	                                        char *route_delete_down = (char *)"ip link set eth1 down";
                                                char *route_delete_up = (char *)"ip link set eth1 up";
                                                system(route_delete_down);
                                                system(route_delete_up);
						init_one_config(mconfig->gateway, "GATEWAY", fp);
						mconfig->modify_flag = 1;
						snprintf(turnipbuf, 99, "route add default gw %s eth1",mconfig->gateway);
						system(turnipbuf);
						printf("gateway=%s\n",turnipbuf);
						change_route(mconfig->static_route, mconfig->gateway);
						Change_gateway(mconfig->gateway, (char *)"/etc/sysconfig/network");
						send_to_manger_flag[1] = 1;
						//int fd = ui_cli.cli_connect((const char *)"/tmp/tips_ui_cli.domain",(const char *)"/tmp/UNIXUI.domain");
						//ui_cli.cli_write(fd, 1);
						//close(fd);
						tips_sqlite_handle.catch_opt(credentials->username, (char *)"修改网关成功");
					}
				}
				else 
				{
					init_one_config(sconfig.gateway, "GATEWAY", fp);
				}


				if(strcmp(mconfig->static_route, sconfig.static_route))
				{
					if(!strlen(mconfig->static_route))
					{
						init_one_config(sconfig.static_route, "STATIC_ROUTE", fp);
					}
					else
					{
						init_one_config(mconfig->static_route, "STATIC_ROUTE", fp);
						mconfig->modify_flag = 1;
						change_route(mconfig->static_route, mconfig->adminstration_IP);
						snprintf(turnipbuf, 99, "route add -host %s gw %s dev eth1",mconfig->adminstration_IP, mconfig->static_route);
						system(turnipbuf);
						printf("%s\n",turnipbuf);
						tips_sqlite_handle.catch_opt(credentials->username, (char *)"修改静态路由成功");
					}
				}
				else 
				{
					init_one_config(sconfig.static_route, "STATIC_ROUTE", fp);
				}


				if(strcmp(mconfig->supervisor, sconfig.supervisor))
				{
				//	if(!strlen(mconfig->supervisor))
				//	{
				//		init_one_config(sconfig.supervisor, "SUPERVISOR", fp);
				//	}
				//	else
				//	{
						init_one_config(mconfig->supervisor, "SUPERVISOR", fp);
						managecp_ip.read_manage_ip();
						mconfig->modify_flag = 1;
						tips_sqlite_handle.catch_opt(credentials->username, (char *)"修改管理机IP成功");
				//	}
				}
				else 
				{
					init_one_config(sconfig.supervisor, "SUPERVISOR", fp);
				}


				if(strcmp(mconfig->unit_name, sconfig.unit_name))
				{
				//	if(!strlen(mconfig->unit_name))
				//	{
				//		init_one_config(sconfig.unit_name, "UNIT_NAME", fp);
				//	}
				//	else
				//	{
						init_one_config(mconfig->unit_name, "UNIT_NAME", fp);
						mconfig->modify_flag = 1;
					        send_to_manger_flag[7] = 1; 
						tips_sqlite_handle.catch_opt(credentials->username, (char *)"修改部属单位名称成功");
				//	}
				}
				else 
				{
					init_one_config(sconfig.unit_name, "UNIT_NAME", fp);
				}

				init_one_config(sconfig.manage_PORT      , "MANAGE_PORT", 			fp);
        		init_one_config(sconfig.manage_PORT1     , "MANAGE_PORT1",			fp);
        		init_one_config(sconfig.serve_addr       , "SERVE_ADDR", 			fp);
        		init_one_config(sconfig.serve_user       , "SERVE_USER", 			fp);
        		init_one_config(sconfig.serve_password   , "SERVE_PASSWORD", 	fp);
        		init_one_config(sconfig.adminstration_IP , "ADMINSTRATION_IP", fp);
        		init_one_config(sconfig.adminstration_IP1, "ADMINSTRATION_IP1",fp);
				fclose(fp);
				if(send_to_manger_flag[0] == 1)
				{
					int fd = ui_cli.cli_connect((const char *)"/tmp/tips_ui_cli.domain",(const char *)"/tmp/UNIXUI.domain");
					ui_cli.cli_write(fd, 0);
					close(fd);
				}
				if(send_to_manger_flag[1] == 1)
				{
					int fd = ui_cli.cli_connect((const char *)"/tmp/tips_ui_cli.domain",(const char *)"/tmp/UNIXUI.domain");
					ui_cli.cli_write(fd, 1);
					close(fd);
				}
				if(send_to_manger_flag[7] == 1)
				{
					system("./killmanager.sh");
				}
		}
		else
		{
				if(strcmp(mconfig->manage_PORT, sconfig.manage_PORT))
				{
			//		if(!strlen(mconfig->manage_PORT))
			//		{
			//			init_one_config(sconfig.manage_PORT, "MANAGE_PORT", fp);
			//		}
			//		else
			//		{
						init_one_config(mconfig->manage_PORT, "MANAGE_PORT", fp);
						mconfig->modify_flag = 1;
						send_to_manger_flag[2] = 1;
				//		int fd = ui_cli.cli_connect((const char *)"/tmp/tips_ui_cli.domain",(const char *)"/tmp/UNIXUI.domain");
				//		ui_cli.cli_write(fd, 3);
				//		close(fd);
						tips_sqlite_handle.catch_opt(credentials->username, (char *)"修改管理中心PORT成功");
			//		}
				}
				else 
				{
					init_one_config(sconfig.manage_PORT, "MANAGE_PORT", fp);
				}

				if(strcmp(mconfig->manage_PORT1, sconfig.manage_PORT1))
				{
				//	if(!strlen(mconfig->manage_PORT1))
				//	{
				//		init_one_config(sconfig.manage_PORT1, "MANAGE_PORT1", fp);
				//	}
				//	else
				//	{
						init_one_config(mconfig->manage_PORT1, "MANAGE_PORT1", fp);
						mconfig->modify_flag = 1;
						send_to_manger_flag[3] = 1;
				//		int fd = ui_cli.cli_connect((const char *)"/tmp/tips_ui_cli.domain",(const char *)"/tmp/UNIXUI.domain");
				//		ui_cli.cli_write(fd, 3);
				//		close(fd);
						tips_sqlite_handle.catch_opt(credentials->username, (char *)"修改副管理中心PORT成功");
				//	}
				}
				else 
				{
					init_one_config(sconfig.manage_PORT1, "MANAGE_PORT1", fp);
				}


				if(strcmp(mconfig->serve_addr, sconfig.serve_addr))
				{
				//	if(!strlen(mconfig->serve_addr))
				//	{
				//		init_one_config(sconfig.serve_addr, "SERVE_ADDR", fp);
				//	}
				//	else
				//	{
						init_one_config(mconfig->serve_addr, "SERVE_ADDR", fp);
						mconfig->modify_flag = 1;
						tips_sqlite_handle.catch_opt(credentials->username, (char *)"修改代理服务器成功");
				//	}
				}
				else 
				{
					init_one_config(sconfig.serve_addr, "SERVE_ADDR", fp);
				}

				if(strcmp(mconfig->serve_user, sconfig.serve_user))
				{
				//	if(!strlen(mconfig->serve_user))
				//	{
				//		init_one_config(sconfig.serve_user, "SERVE_USER", fp);
				//	}
				//	else
				//	{
						init_one_config(mconfig->serve_user, "SERVE_USER", fp);
						mconfig->modify_flag = 1;
						tips_sqlite_handle.catch_opt(credentials->username, (char *)"修改代理服务器账号成功");
				//	}
				}
				else 
				{
					init_one_config(sconfig.serve_user, "SERVE_USER", fp);
				//puts(mconfig->serve_user);
				}
				
				if(strcmp(mconfig->serve_password, sconfig.serve_password))
				{
				//	if(!strlen(mconfig->serve_password))
				//	{
				//		init_one_config(sconfig.serve_password, "SERVE_PASSWORD", fp);
				//	}
				//	else
				//	{
						init_one_config(mconfig->serve_password, "SERVE_PASSWORD", fp);
						send_to_manger_flag[4] = 1;
				//		int fd = ui_cli.cli_connect((const char *)"/tmp/tips_ui_cli.domain",(const char *)"/tmp/UNIXUI.domain");
				//		ui_cli.cli_write(fd, 2);
				//		close(fd);
						tips_sqlite_handle.catch_opt(credentials->username, (char *)"修改代理服务器口令成功");
				//	}
				}
				else 
				{
					init_one_config(sconfig.serve_password, "SERVE_PASSWORD", fp);
				}

				if(strcmp(mconfig->adminstration_IP, sconfig.adminstration_IP))
				{
				//	if(!strlen(mconfig->adminstration_IP))
				//	{
				//		init_one_config(sconfig.adminstration_IP, "ADMINSTRATION_IP", fp);
				//	}
				//	else
				//	{
						init_one_config(mconfig->adminstration_IP, "ADMINSTRATION_IP", fp);
						mconfig->modify_flag = 1;
						send_to_manger_flag[5] = 1;
						//int fd = ui_cli.cli_connect((const char *)"/tmp/tips_ui_cli.domain",(const char *)"/tmp/UNIXUI.domain");
						//ui_cli.cli_write(fd, 3);
						//close(fd);
						tips_sqlite_handle.catch_opt(credentials->username, (char *)"修改管理中心IP成功");
				//	}
				}
				else 
				{
					init_one_config(sconfig.adminstration_IP, "ADMINSTRATION_IP", fp);
				}

				if(strcmp(mconfig->adminstration_IP1, sconfig.adminstration_IP1))
				{
				//	if(!strlen(mconfig->adminstration_IP1))
				//	{
				//		init_one_config(sconfig.adminstration_IP1, "ADMINSTRATION_IP1", fp);
				//	}
				//	else
				//	{
						init_one_config(mconfig->adminstration_IP1, "ADMINSTRATION_IP1", fp);
						mconfig->modify_flag = 1;
						send_to_manger_flag[6] = 1;
					//	int fd = ui_cli.cli_connect((const char *)"/tmp/tips_ui_cli.domain",(const char *)"/tmp/UNIXUI.domain");
					//	ui_cli.cli_write(fd, 3);
					//	close(fd);
						tips_sqlite_handle.catch_opt(credentials->username, (char *)"修改副管理中心IP成功");
				//	}
				}
				else 
				{
					init_one_config(sconfig.adminstration_IP1, "ADMINSTRATION_IP1", fp);
				}

				init_one_config(sconfig.unit_name, "UNIT_NAME", fp);
				init_one_config(sconfig.supervisor, "SUPERVISOR", fp);
				init_one_config(sconfig.static_route, "STATIC_ROUTE", fp);
				init_one_config(sconfig.gateway, "GATEWAY", fp);
				init_one_config(sconfig.manage_IP, "MANAGE_IP", fp);
				init_one_config(sconfig.net_IP, "NET_IP", fp);
				fclose(fp);
				if(send_to_manger_flag[2] == 1)
				{
					int fd = ui_cli.cli_connect((const char *)"/tmp/tips_ui_cli.domain",(const char *)"/tmp/UNIXUI.domain");
					ui_cli.cli_write(fd, 3);
					close(fd);
					system("./killmanager.sh");
				}

				if(send_to_manger_flag[3] == 1)
				{
					int fd = ui_cli.cli_connect((const char *)"/tmp/tips_ui_cli.domain",(const char *)"/tmp/UNIXUI.domain");
					ui_cli.cli_write(fd, 3);
					close(fd);
					system("./killmanager.sh");
				}

				if(send_to_manger_flag[4] == 1)
				{
					int fd = ui_cli.cli_connect((const char *)"/tmp/tips_ui_cli.domain",(const char *)"/tmp/UNIXUI.domain");
					ui_cli.cli_write(fd, 2);
					close(fd);
				}
				
				if(send_to_manger_flag[5] == 1)
				{
					int fd = ui_cli.cli_connect((const char *)"/tmp/tips_ui_cli.domain",(const char *)"/tmp/UNIXUI.domain");
					ui_cli.cli_write(fd, 3);
					close(fd);
					system("./killmanager.sh");
				}

				if(send_to_manger_flag[6] == 1)
				{
					int fd = ui_cli.cli_connect((const char *)"/tmp/tips_ui_cli.domain",(const char *)"/tmp/UNIXUI.domain");
					ui_cli.cli_write(fd, 3);
					close(fd);
					system("./killmanager.sh");
				}
		}
#endif
        g_mutex_unlock(filemutex);
#if 0 
	if(strcmp(mconfig->manage_IP, sconfig.manage_IP)){
		/*realdisplay = g_strdup_printf("%s 管理口IP", display);
		g_free(display);
		display = realdisplay;*/
		mconfig->modify_flag = 1;
		snprintf(turnipbuf, 99, "ifconfig %s %s netmask 255.255.255.0",MANAGE_ETH,mconfig->manage_IP);
		system(turnipbuf);
		Change_ip(mconfig->manage_IP, (char *)MANAGE_ETH0_FILE);//需要判断返回值；
		tips_sqlite_handle.catch_opt(credentials->username, (char *)"修改管理口IP成功");
		judge = 1;
		//execl("./a.out","./a.out",NULL);
		printf("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n");
	}
	if(strcmp(mconfig->manage_PORT, sconfig.manage_PORT)){
		/*realdisplay = g_strdup_printf("%s 管理中心PORT", display);
		g_free(display);
		display = realdisplay;*/
		mconfig->modify_flag = 1;
		int fd = ui_cli.cli_connect((const char *)"/tmp/tips_ui_cli.domain",(const char *)"/tmp/UNIXUI.domain");
		ui_cli.cli_write(fd, 3);
		close(fd);
		tips_sqlite_handle.catch_opt(credentials->username, (char *)"修改管理中心PORT成功");
	}
	if(strcmp(mconfig->manage_PORT1, sconfig.manage_PORT1)){
		/*realdisplay = g_strdup_printf("%s 管理中心PORT", display);
		g_free(display);
		display = realdisplay;*/
		mconfig->modify_flag = 1;
		int fd = ui_cli.cli_connect((const char *)"/tmp/tips_ui_cli.domain",(const char *)"/tmp/UNIXUI.domain");
		ui_cli.cli_write(fd, 3);
		close(fd);
		tips_sqlite_handle.catch_opt(credentials->username, (char *)"修改副管理中心PORT成功");
	}
	if(strcmp(mconfig->gateway, sconfig.gateway)){
		/*realdisplay = g_strdup_printf("%s 网关", display);
		g_free(display);
		display = realdisplay;*/
		mconfig->modify_flag = 1;
		snprintf(turnipbuf, 99, "route add default gw %s",mconfig->gateway);
		system(turnipbuf);
		printf("gateway=%s\n",turnipbuf);
		change_route(mconfig->static_route, mconfig->gateway);
		Change_gateway(mconfig->gateway, (char *)"/etc/sysconfig/network");
		int fd = ui_cli.cli_connect((const char *)"/tmp/tips_ui_cli.domain",(const char *)"/tmp/UNIXUI.domain");
		ui_cli.cli_write(fd, 1);
		close(fd);
		tips_sqlite_handle.catch_opt(credentials->username, (char *)"修改网关成功");
	}
	if(strcmp(mconfig->static_route, sconfig.static_route)){
		/*realdisplay = g_strdup_printf("%s 静态路由", display);
		g_free(display);
		display = realdisplay;*/
		mconfig->modify_flag = 1;
		change_route(mconfig->static_route, mconfig->adminstration_IP);
		snprintf(turnipbuf, 99, "route add -host %s gw %s dev eth1",mconfig->adminstration_IP, mconfig->static_route);
		system(turnipbuf);
		printf("%s\n",turnipbuf);
		tips_sqlite_handle.catch_opt(credentials->username, (char *)"修改静态路由成功");
		
	}
	if(strcmp(mconfig->supervisor, sconfig.supervisor)){
		/*realdisplay = g_strdup_printf("%s 管理机IP", display);
		g_free(display);
		display = realdisplay;*/
		managecp_ip.read_manage_ip();
		mconfig->modify_flag = 1;
		tips_sqlite_handle.catch_opt(credentials->username, (char *)"修改管理机IP成功");
	}
	if(strcmp(mconfig->serve_addr, sconfig.serve_addr)){
		/*realdisplay = g_strdup_printf("%s 代理服务器地址", display);
		g_free(display);
		display = realdisplay;*/
		mconfig->modify_flag = 1;
		tips_sqlite_handle.catch_opt(credentials->username, (char *)"修改代理服务器成功");
	}
	if(strcmp(mconfig->serve_user, sconfig.serve_user)){
		/*realdisplay = g_strdup_printf("%s 代理服务器账号", display);
		g_free(display);
		display = realdisplay;*/
		mconfig->modify_flag = 1;
		tips_sqlite_handle.catch_opt(credentials->username, (char *)"修改代理服务器账号成功");
	}
	if(strcmp(mconfig->serve_password, sconfig.serve_password)){
		/*realdisplay = g_strdup_printf("%s 代理服务器口令", display);
		g_free(display);
		display = realdisplay;
		mconfig->modify_flag = 1;*/
		int fd = ui_cli.cli_connect((const char *)"/tmp/tips_ui_cli.domain",(const char *)"/tmp/UNIXUI.domain");
		ui_cli.cli_write(fd, 2);
		close(fd);
		tips_sqlite_handle.catch_opt(credentials->username, (char *)"修改代理服务器口令成功");
	}
	if(strcmp(mconfig->adminstration_IP, sconfig.adminstration_IP)){
		/*realdisplay = g_strdup_printf("%s 管理中心IP", display);
		g_free(display);
		display = realdisplay;*/
		mconfig->modify_flag = 1;
		int fd = ui_cli.cli_connect((const char *)"/tmp/tips_ui_cli.domain",(const char *)"/tmp/UNIXUI.domain");
		ui_cli.cli_write(fd, 3);
		close(fd);
		tips_sqlite_handle.catch_opt(credentials->username, (char *)"修改管理中心IP成功");
	}
	if(strcmp(mconfig->adminstration_IP1, sconfig.adminstration_IP1)){
		/*realdisplay = g_strdup_printf("%s 管理中心IP", display);
		g_free(display);
		display = realdisplay;*/
		mconfig->modify_flag = 1;
		int fd = ui_cli.cli_connect((const char *)"/tmp/tips_ui_cli.domain",(const char *)"/tmp/UNIXUI.domain");
		ui_cli.cli_write(fd, 3);
		close(fd);
		tips_sqlite_handle.catch_opt(credentials->username, (char *)"修改副管理中心IP成功");
	}
	if(strcmp(mconfig->unit_name, sconfig.unit_name)){
		/*realdisplay = g_strdup_printf("%s 部属单位名称", display);
		g_free(display);
		display = realdisplay;*/
		mconfig->modify_flag = 1;
		tips_sqlite_handle.catch_opt(credentials->username, (char *)"修改部属单位名称成功");
	}
	if(strcmp(mconfig->net_IP, sconfig.net_IP)){
		/*realdisplay = g_strdup_printf("%s 联网口IP", display);
		g_free(display);
		display = realdisplay;*/
		snprintf(turnipbuf, 99, "ifconfig %s %s netmask 255.255.255.0",(char *)"eth1",mconfig->net_IP);
		system(turnipbuf);
		Change_ip(mconfig->net_IP, (char *)MANAGE_ETH1_FILE);//需要判断返回值；
		mconfig->modify_flag = 1;
		int fd = ui_cli.cli_connect((const char *)"/tmp/tips_ui_cli.domain",(const char *)"/tmp/UNIXUI.domain");
		ui_cli.cli_write(fd, 0);
		close(fd);
		tips_sqlite_handle.catch_opt(credentials->username, (char *)"修改联网口IP成功");
	}
#endif
//	printf("judge=%d\n",judge);
//	if(judge != 0){
//		printf("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\n");
//		execl("./tipsuser","./tipsuser",NULL);
//	}
	/*realdisplay = g_strdup_printf("%s 成功", display);
	g_free(display);*/
  return NULL;
}


char* CAdmin_opt::system_config_modify(system_config_t* mconfig)
{
				system_config_t sconfig;
				memset(&sconfig, 0, sizeof(system_config_t));
        /*是否需要判断返回值？int format =*/ get_system_config_xml(&sconfig);
        /*if(format){
               return 1;
        }*/
        g_mutex_lock(filemutex);
        FILE *fp = fopen(SYSTEM_CONFIG_FILE, "w+");
        //fclose(fp);
        //fp = fopen(CONFIG_FILE, "a");
        if(NULL == fp){
                g_mutex_unlock(filemutex);
                return NULL;
        }
        g_mutex_unlock(filemutex);

				char *display = strdup("修改");
				char *realdisplay = display;
				if(strcmp(mconfig->cpu, sconfig.cpu)){
					if(!strlen(mconfig->cpu))
					{
						printf("mconfig->cpu1111::::%s  sconfig.cpu1111:::::%s\n",mconfig->cpu, sconfig.cpu);
						init_one_config(sconfig.cpu, "CPU", fp);
					}
					else
					{
						printf("mconfig->cpu2222::::%s  sconfig.cpu2222:::::%s\n",mconfig->cpu, sconfig.cpu);
						init_one_config(mconfig->cpu, "CPU", fp);
						realdisplay = g_strdup_printf("%s CPU利用率", display);
						g_free(display);
						display = realdisplay;
						mconfig->modify_flag = 1;
					}
				}
				else 
				{
					printf("mconfig->cpu3333::::%s  sconfig.cpu3333:::::%s\n",mconfig->cpu, sconfig.cpu);
					init_one_config(sconfig.cpu, "CPU", fp);
				}
				if(strcmp(mconfig->mem, sconfig.mem)){
					if(!strlen(mconfig->mem))
					{
						printf("mconfig->mem1111::::%s  sconfig.mem1111:::::%s\n",mconfig->mem, sconfig.mem);
						init_one_config(sconfig.mem, "MEM", fp);
					}
					else
					{
						printf("mconfig->mem2222::::%s  sconfig.mem2222:::::%s\n",mconfig->mem, sconfig.mem);
						init_one_config(mconfig->mem, "MEM", fp);
						realdisplay = g_strdup_printf("%s 内存利用率", display);
						g_free(display);
						display = realdisplay;
						mconfig->modify_flag = 1;
					}
				}
				else 
				{
					printf("mconfig->mem3333::::%s  sconfig.mem3333:::::%s\n",mconfig->mem, sconfig.mem);
					init_one_config(sconfig.mem, "MEM", fp);
				}
				if(strcmp(mconfig->disk, sconfig.disk)){
					if(!strlen(mconfig->disk))
					{
						printf("mconfig->disk1111::::%s  sconfig.disk1111:::::%s\n",mconfig->disk, sconfig.disk);
						init_one_config(sconfig.disk, "DISK", fp);
					}
					else
					{
						printf("mconfig->disk2222::::%s  sconfig.disk2222:::::%s\n",mconfig->disk, sconfig.disk);
						init_one_config(mconfig->disk, "DISK", fp);
						realdisplay = g_strdup_printf("%s 磁盘剩余空间", display);
						g_free(display);
						display = realdisplay;
						mconfig->modify_flag = 1;
					}
				}
				else
				{
					printf("mconfig->disk3333::::%s  sconfig.disk3333:::::%s\n",mconfig->disk, sconfig.disk);
					init_one_config(sconfig.disk, "DISK", fp);
				}
				if(strcmp(mconfig->runtime, sconfig.runtime)){
					if(!strlen(mconfig->runtime))
					{
						printf("mconfig->runtime1111::::%s  sconfig.runtime1111:::::%s\n",mconfig->runtime, sconfig.runtime);
						init_one_config(sconfig.runtime, "RUNTIME", fp);
						printf("wwwww::::%s\t%s\n",mconfig->runtime, sconfig.runtime);
					}
					else 
					{
						printf("mconfig->runtime2222::::%s  sconfig.runtime2222:::::%s\n",mconfig->runtime, sconfig.runtime);
						init_one_config(mconfig->runtime, "RUNTIME", fp);
						realdisplay = g_strdup_printf("%s 上报运行状态间隔时间", display);
						g_free(display);
						display = realdisplay;
						mconfig->modify_flag = 1;
						int fd = ui_cli.cli_connect((const char *)"/tmp/tips_ui_cli.domain",(const char *)"/tmp/UNIXUI.domain");
						ui_cli.cli_write(fd, 5);
						close(fd);
					}
				}
				else 
				{
					printf("mconfig->runtime3333::::%s  sconfig.runtime3333:::::%s\n",mconfig->runtime, sconfig.runtime);
					init_one_config(sconfig.runtime, "RUNTIME", fp);
				}
				if(strcmp(mconfig->alarmtime, sconfig.alarmtime)){
					if(!strlen(mconfig->alarmtime))
					{
						printf("mconfig->alarmtime1111::::%s  sconfig.alarmtime1111:::::%s\n",mconfig->alarmtime, sconfig.alarmtime);
						init_one_config(sconfig.alarmtime, "ALARMTIME", fp);
					}
					else
					{
						printf("mconfig->alarmtime2222::::%s  sconfig.alarmtime2222:::::%s\n",mconfig->alarmtime, sconfig.alarmtime);
						init_one_config(mconfig->alarmtime, "ALARMTIME", fp);
						realdisplay = g_strdup_printf("%s 上报报警信息间隔时间", display);
						g_free(display);
						display = realdisplay;
						mconfig->modify_flag = 1;
					}
					//int fd = ui_cli.cli_connect((const char *)"/tmp/tips_ui_cli.domain",(const char *)"/tmp/UNIXUI.domain");
					//ui_cli.cli_write(fd, 6);
					//close(fd);
				}
				else
				{
					printf("mconfig->alarmtime3333::::%s  sconfig.alarmtime3333:::::%s\n",mconfig->alarmtime, sconfig.alarmtime);
    			    init_one_config(sconfig.alarmtime, "ALARMTIME", fp);
				}
         	if(strcmp(mconfig->syntime, sconfig.syntime)){
         	        if(!strlen(mconfig->syntime))
         	                init_one_config(sconfig.syntime, "SYNTIME", fp);
         	        else
         	        {
         	                init_one_config(mconfig->syntime, "SYNTIME", fp);
         	                realdisplay = g_strdup_printf("%s 上报时间同步间隔时间", display);
         	                g_free(display);
         	                display = realdisplay;
         	                mconfig->modify_flag = 1;
         	        	int fd = ui_cli.cli_connect((const char *)"/tmp/tips_ui_cli.domain",(const char *)"/tmp/UNIXUI.domain");
     	            		ui_cli.cli_write(fd, 6);
     	            		close(fd);
         	        }
     	    }
     	    else
     	             init_one_config(sconfig.syntime, "SYNTIME", fp);
         if(strcmp(mconfig->netstatetime, sconfig.netstatetime)){
                 if(!strlen(mconfig->netstatetime))
                         init_one_config(sconfig.netstatetime, "NETSTATETIME", fp);
                 else
                 {
                         init_one_config(mconfig->netstatetime, "NETSTATETIME", fp);
                         realdisplay = g_strdup_printf("%s 上报网络状态间隔时间", display);
                         g_free(display);
                         display = realdisplay;
                         mconfig->modify_flag = 1;
                 }
         }
         else
                 init_one_config(sconfig.netstatetime, "NETSTATETIME", fp);
				realdisplay = g_strdup_printf("%s 成功", display);
				puts(realdisplay);
				g_free(display);
  
        fclose(fp);
        return realdisplay;
}

char* CAdmin_opt::cmd_opt_get()
{
	const char *login = params_value(params, "login");
	printf("cmd in admin *******************************%s\n", cmd);
	if(!strcmp(cmd, "get_block_info")){/*通信阻断审计*/
                return ((admin_opt_fun *)opt_fun)(credentials, (char *)"<block></block>");

        }
        else if(!strcmp(cmd, "get_event_info")){/*涉密标志事件*/
                return ((admin_opt_fun *)opt_fun)(credentials, (char *)"<autorefresh interval='0'></autorefresh><secret><isfind>0</isfind></secret>");
        }
        else if(!strcmp(cmd, "get_Alarm_info")){/*攻击窃密警报*/
		printf("111111111111111111\n");
		char page_number[20];
		uint32_t total_number=0;
		char bug_pagenum[10];
		memset(bug_pagenum,0,sizeof(bug_pagenum));
		int sort = 0;
		char* sortstrtemp = (char *)params_value(params, "tips_sort");
		char* sortis =  (char *)params_value(params, "tips_sortis");
		if(sortstrtemp != NULL){
			sort = atoi(sortstrtemp);
		}

		if(sortis != NULL){
			sort = ((sort + 1) % 2);
		}
		char *realxml = NULL;
		memset(page_number,0,sizeof(page_number));
	       	CTips_alarm tips_alarm_handle;
		alarm_stut_t alarm_info;
		alarm_info.Trojan_id = (char *)"";
		alarm_info.Sip = (char *)"";
		alarm_info.Dip = (char *)"";
		alarm_info.Dmac = (char *)"";
		alarm_info.Alarm_time = (char *)"";
		alarm_info.Alarm_end_time = (char *)"";
		alarm_info.Type = (char *)"";
		alarm_info.Risk = (char *)"";
		alarm_info.Scc_flag = (char *)"";
		char *interval = (char *)"0";
            	//tips_alarm_params(params, &alarm_info);
		uint32_t nowpage = 1;
		char *isfindtemp = (char *)params_value(params, "isfind");
		char *every_pagenum = (char *)params_value(params, "every_pagenum");
		printf("222222222\n");
		FILE *fp;
		if(every_pagenum == NULL || strlen(every_pagenum) == 0)
		{
			fp = fopen((char *)"/usr/local/tipschecker/etc/tipscheck/every_pagenum","r+");
			fread(bug_pagenum,sizeof(bug_pagenum),1,fp);
			every_pagenum = bug_pagenum;
			fclose(fp);
		}
		else
		{
			if(NULL == (fp = fopen((char *)"/usr/local/tipschecker/etc/tipscheck/every_pagenum","w+")))
			{
				perror("fopen");
				exit(0);
			}
			fwrite(every_pagenum,strlen(every_pagenum),1,fp);
			fclose(fp);
		}
		if(isfindtemp != NULL){
			int isfind = atoi(isfindtemp);
			if(isfind == 0){
				nowpage = 1;
		}
		}
            	tips_alarm_handle.Count_init_sql(&alarm_info, nowpage);
		printf("33333333333333333333\n");
            	uint32_t lastpage = tips_alarm_handle.Count_exec_sql(&total_number);
		CTips_Page tips_page;
		char *pagenum = tips_page.init_pagenum(nowpage, lastpage,total_number);
		tips_page.init_urlpart_alarm(&alarm_info, credentials->token, interval);
		printf("666666666666666666\n");
		char *tips_url = tips_page.init_url(nowpage,lastpage, sort,every_pagenum);
            	tips_alarm_handle.Alarm_init_sql(&alarm_info, nowpage, sort+1);
		printf("444444444444444444444444\n");
            	char *xml = tips_alarm_handle.Alarm_exec_sql();
		printf("555555555555555555555\n");
		if(strlen(alarm_info.Alarm_time) && strlen(alarm_info.Alarm_end_time))
		{
			int judge_time_size = judge_time(alarm_info.Alarm_time,alarm_info.Alarm_end_time);
			if(judge_time_size)
				realxml = (char *)g_strdup_printf("<autorefresh interval='%s'></autorefresh><attack Type='%s' Risk='%s' Scc_flag='%s'><msg>开始时间 不能大于 结束时间</msg><isfind>1</isfind><OTrogan_id>%s</OTrogan_id><OSip>%s</OSip><ODip>%s</ODip><ODmac>%s</ODmac><OAlarm_time>%s</OAlarm_time><OAlarm_end_time>%s</OAlarm_end_time><tips_sort>%d</tips_sort>%s<page Count='%s'>%s%s</page></attack>",interval, alarm_info.Type, alarm_info.Risk, alarm_info.Scc_flag, alarm_info.Trojan_id, alarm_info.Sip, alarm_info.Dip, alarm_info.Dmac, alarm_info.Alarm_time,alarm_info.Alarm_end_time, sort,xml,every_pagenum,pagenum,tips_url);
			else
				realxml = (char *)g_strdup_printf("<autorefresh interval='%s'></autorefresh><attack Type='%s' Risk='%s' Scc_flag='%s'><isfind>1</isfind><OTrogan_id>%s</OTrogan_id><OSip>%s</OSip><ODip>%s</ODip><ODmac>%s</ODmac><OAlarm_time>%s</OAlarm_time><OAlarm_end_time>%s</OAlarm_end_time><tips_sort>%d</tips_sort>%s<page Count='%s'>%s%s</page></attack>",interval, alarm_info.Type, alarm_info.Risk, alarm_info.Scc_flag, alarm_info.Trojan_id, alarm_info.Sip, alarm_info.Dip, alarm_info.Dmac, alarm_info.Alarm_time,alarm_info.Alarm_end_time, sort,xml,every_pagenum,pagenum,tips_url);
					
		}
		else
			realxml = (char *)g_strdup_printf("<autorefresh interval='%s'></autorefresh><attack Type='%s' Risk='%s' Scc_flag='%s'><isfind>1</isfind><OTrogan_id>%s</OTrogan_id><OSip>%s</OSip><ODip>%s</ODip><ODmac>%s</ODmac><OAlarm_time>%s</OAlarm_time><OAlarm_end_time>%s</OAlarm_end_time><tips_sort>%d</tips_sort>%s<page Count='%s'>%s%s</page></attack>",interval, alarm_info.Type, alarm_info.Risk, alarm_info.Scc_flag, alarm_info.Trojan_id, alarm_info.Sip, alarm_info.Dip, alarm_info.Dmac, alarm_info.Alarm_time,alarm_info.Alarm_end_time, sort,xml,every_pagenum,pagenum,tips_url);

		tips_page.Page_free();
            	g_free(xml);
            	char *response = ((admin_opt_fun *)opt_fun)(credentials, realxml);
            	g_free(realxml);
            	return response;
                //return ((admin_opt_fun *)opt_fun)(credentials, (char *)"<autorefresh interval='0'></autorefresh><attack><isfind>0</isfind><tips_sort>0</tips_sort></attack>");
        }
        else if(!strcmp(cmd, "get_Alert_info")){/*可疑行为报警*/
                return ((admin_opt_fun *)opt_fun)(credentials, (char *)"<shadiness></shadiness>");
        }
        else if(!strcmp(cmd, "get_netlog_info")){/*通信会话审计*/
		char page_number[20];
		uint32_t total_number = 0;
		int sort = 0;
		char* sortstrtemp = (char *)params_value(params, "tips_sort");
		char* sortis =  (char *)params_value(params, "tips_sortis");
		printf("sortstrtemp ===================================%s\n",sortstrtemp);
		printf("sortis=========================================%s\n",sortis);
		if(sortstrtemp != NULL){
			sort = atoi(sortstrtemp);
			printf("sort================================================%d\n",sort);
		}
		if(sortis != NULL){
			printf("before sort================================================%d\n",sort);
			sort = ((sort + 1) % 2);
			printf("after sort================================================%d\n",sort);
		}
		char bug_pagenum[10];
		char *realxml = NULL;
		memset(page_number,0,sizeof(page_number));
		memset(bug_pagenum,0,sizeof(bug_pagenum));
		CTips_netlog tips_netlog_handle;
		netlog_stut_t netlog_info;
		netlog_info.Dip = (char *)"";
		netlog_info.Dport = (char *)"";
		netlog_info.Sip = (char *)"";
		netlog_info.Sport = (char *)"";
		netlog_info.Protocol = (char *)"";
		netlog_info.Start_time = (char *)"";
		netlog_info.End_time = (char *)"";
		netlog_info.Load = (char *)"";
		netlog_info.Upload = (char *)"";
		
		//tips_netlog_params(params, &netlog_info);
		char *interval = (char *)"0";
		uint32_t nowpage = 1;
		char *isfindtemp = (char *)params_value(params, "isfind");
		char *every_pagenum = (char *)params_value(params, "every_pagenum");
		FILE *fp;
		if(every_pagenum == NULL || strlen(every_pagenum) == 0)
		{
			fp = fopen((char *)"/usr/local/tipschecker/etc/tipscheck/every_pagenum","r+");
			fread(bug_pagenum,sizeof(bug_pagenum),1,fp);
			/*
			//if(NULL == (fp = fopen((char *)"/usr/local/tipschecker/etc/tipscheck/every_pagenum","w+")))
			//{
			//	perror("fopen");
			//	exit(0);
			//}
			////every_pagenum = bug_pagenum;
			//printf("wo\n");
			////fread(page_number,sizeof(page_number),1,fp);
			////every_pagenum = page_number;
			//fwrite((char *)"50",strlen((char *)"50"),1,fp);
			*/
			every_pagenum = bug_pagenum;
			fclose(fp);
		}
		else
		{
			if(NULL == (fp = fopen((char *)"/usr/local/tipschecker/etc/tipscheck/every_pagenum","w+")))
			{
				perror("fopen");
				exit(0);
			}
			fwrite(every_pagenum,strlen(every_pagenum),1,fp);
			fclose(fp);
		}
		//sleep(30);
		if(isfindtemp != NULL)
		{
			int isfind = atoi(isfindtemp);
			if(isfind == 0)
				nowpage = 1;
		}
		tips_netlog_handle.Count_init_sql(&netlog_info, nowpage);
		uint32_t lastpage = tips_netlog_handle.Count_exec_sql(&total_number);
		CTips_Page tips_page;
		char *pagenum = tips_page.init_pagenum(nowpage, lastpage,total_number);
		tips_page.init_urlpart_netlog(&netlog_info, credentials->token, interval);
		char *tips_url = tips_page.init_url(nowpage,lastpage,sort,netlog_info.Every_pagenum);
		tips_netlog_handle.Netlog_init_sql(&netlog_info, nowpage,sort+1);
		char *xml = tips_netlog_handle.Netlog_exec_sql();
		printf("xml=======%s\n",xml);
		if(strlen(netlog_info.Start_time) &&/*hyj ,*/ strlen(netlog_info.End_time))
		{
		int judge = judge_time(netlog_info.Start_time,netlog_info.End_time);
		if(judge)
		realxml = (char *)g_strdup_printf("<autorefresh interval='%s'></autorefresh><tips_dialog><isfind>1</isfind><msg>开始时间 不能大于 结束时间</msg><ODip>%s</ODip><OSip>%s</OSip><OSport>%s</OSport><OProtocol>%s</OProtocol><OStart_time>%s</OStart_time><OEnd_time>%s</OEnd_time><OLoad>%s</OLoad><OUpload>%s</OUpload><tips_sort>%d</tips_sort>%s<page Count='%s'>%s%s</page></tips_dialog>", interval, netlog_info.Dip, netlog_info.Sip, netlog_info.Sport, netlog_info.Protocol, netlog_info.Start_time, netlog_info.End_time, netlog_info.Load, netlog_info.Upload,sort, xml, every_pagenum,pagenum, tips_url);
		else
		realxml = (char *)g_strdup_printf("<autorefresh interval='%s'></autorefresh><tips_dialog><isfind>1</isfind><ODip>%s</ODip><OSip>%s</OSip><OSport>%s</OSport><OProtocol>%s</OProtocol><OStart_time>%s</OStart_time><OEnd_time>%s</OEnd_time><OLoad>%s</OLoad><OUpload>%s</OUpload><tips_sort>%d</tips_sort>%s<page Count='%s'>%s%s</page></tips_dialog>", interval, netlog_info.Dip, netlog_info.Sip, netlog_info.Sport, netlog_info.Protocol, netlog_info.Start_time, netlog_info.End_time, netlog_info.Load, netlog_info.Upload,sort, xml, every_pagenum,pagenum, tips_url);
		}
		else
		realxml = (char *)g_strdup_printf("<autorefresh interval='%s'></autorefresh><tips_dialog><isfind>1</isfind><ODip>%s</ODip><OSip>%s</OSip><OSport>%s</OSport><OProtocol>%s</OProtocol><OStart_time>%s</OStart_time><OEnd_time>%s</OEnd_time><OLoad>%s</OLoad><OUpload>%s</OUpload><tips_sort>%d</tips_sort>%s<page Count='%s'>%s%s</page></tips_dialog>", interval, netlog_info.Dip, netlog_info.Sip, netlog_info.Sport, netlog_info.Protocol, netlog_info.Start_time, netlog_info.End_time, netlog_info.Load, netlog_info.Upload,sort, xml, every_pagenum,pagenum, tips_url);
		tips_page.Page_free();
		g_free(xml);
		char *response = ((admin_opt_fun *)opt_fun)(credentials, realxml);
		printf("realxml***************%s\n",realxml);
		g_free(realxml);
		return response;
                //return ((admin_opt_fun *)opt_fun)(credentials, (char *)"<autorefresh interval='0'></autorefresh><tips_dialog><isfind>0</isfind></tips_dialog>");
        }
        else if(!strcmp(cmd, "get_data")){/*通信会话审计*/
                return ((admin_opt_fun *)opt_fun)(credentials, (char *)"<autorefresh interval='0'></autorefresh><data><isfind>0</isfind></data>");
        //char *realxml = (char *)g_strdup_printf("<data>%s<page>%s%s</page></data>",xml, pagenum, tips_url);
		}
	else if(!strcmp(cmd, "tips_alarm_display")){
		char page_number[20];
		uint32_t total_number=0;
		char bug_pagenum[10];
		memset(bug_pagenum,0,sizeof(bug_pagenum));
		int sort = 0;
		char* sortstrtemp = (char *)params_value(params, "tips_sort");
		char* sortis =  (char *)params_value(params, "tips_sortis");
		if(sortstrtemp != NULL){
			sort = atoi(sortstrtemp);
		}

		if(sortis != NULL){
			sort = ((sort + 1) % 2);
		}
		char *realxml = NULL;
		memset(page_number,0,sizeof(page_number));
	       	CTips_alarm tips_alarm_handle;
		alarm_stut_t alarm_info;
		char *interval = (char *)params_value(params, "refresh_interval");
            	tips_alarm_params(params, &alarm_info);
		uint32_t nowpage = atoll(params_value(params, "pagenum"));
		char *isfindtemp = (char *)params_value(params, "isfind");
		char *every_pagenum = (char *)params_value(params, "every_pagenum");
		FILE *fp;
		if(every_pagenum == NULL || strlen(every_pagenum) == 0)
		{
			fp = fopen((char *)"/usr/local/tipschecker/etc/tipscheck/every_pagenum","r+");
			fread(bug_pagenum,sizeof(bug_pagenum),1,fp);
			every_pagenum = bug_pagenum;
			fclose(fp);
		}
		else
		{
			if(NULL == (fp = fopen((char *)"/usr/local/tipschecker/etc/tipscheck/every_pagenum","w+")))
			{
				perror("fopen");
				exit(0);
			}
			fwrite(every_pagenum,strlen(every_pagenum),1,fp);
			fclose(fp);
		}
		if(isfindtemp != NULL){
			int isfind = atoi(isfindtemp);
			if(isfind == 0){
				nowpage = 1;
		}
		}
            	tips_alarm_handle.Count_init_sql(&alarm_info, nowpage);
            	uint32_t lastpage = tips_alarm_handle.Count_exec_sql(&total_number);
		CTips_Page tips_page;
		char *pagenum = tips_page.init_pagenum(nowpage, lastpage,total_number);
		tips_page.init_urlpart_alarm(&alarm_info, credentials->token, interval);
		char *tips_url = tips_page.init_url(nowpage,lastpage, sort,every_pagenum);
            	tips_alarm_handle.Alarm_init_sql(&alarm_info, nowpage, sort+1);
            	char *xml = tips_alarm_handle.Alarm_exec_sql();
		if(strlen(alarm_info.Alarm_time) && strlen(alarm_info.Alarm_end_time))
		{
			int judge_time_size = judge_time(alarm_info.Alarm_time,alarm_info.Alarm_end_time);
			if(judge_time_size)
				realxml = (char *)g_strdup_printf("<autorefresh interval='%s'></autorefresh><attack Type='%s' Risk='%s' Scc_flag='%s'><msg>开始时间 不能大于 结束时间</msg><isfind>1</isfind><OTrogan_id>%s</OTrogan_id><OSip>%s</OSip><ODip>%s</ODip><ODmac>%s</ODmac><OAlarm_time>%s</OAlarm_time><OAlarm_end_time>%s</OAlarm_end_time><tips_sort>%d</tips_sort>%s<page Count='%s'>%s%s</page></attack>",interval, alarm_info.Type, alarm_info.Risk, alarm_info.Scc_flag, alarm_info.Trojan_id, alarm_info.Sip, alarm_info.Dip, alarm_info.Dmac, alarm_info.Alarm_time,alarm_info.Alarm_end_time, sort,xml,every_pagenum,pagenum,tips_url);
			else
				realxml = (char *)g_strdup_printf("<autorefresh interval='%s'></autorefresh><attack Type='%s' Risk='%s' Scc_flag='%s'><isfind>1</isfind><OTrogan_id>%s</OTrogan_id><OSip>%s</OSip><ODip>%s</ODip><ODmac>%s</ODmac><OAlarm_time>%s</OAlarm_time><OAlarm_end_time>%s</OAlarm_end_time><tips_sort>%d</tips_sort>%s<page Count='%s'>%s%s</page></attack>",interval, alarm_info.Type, alarm_info.Risk, alarm_info.Scc_flag, alarm_info.Trojan_id, alarm_info.Sip, alarm_info.Dip, alarm_info.Dmac, alarm_info.Alarm_time,alarm_info.Alarm_end_time, sort,xml,every_pagenum,pagenum,tips_url);
					
		}
		else
			realxml = (char *)g_strdup_printf("<autorefresh interval='%s'></autorefresh><attack Type='%s' Risk='%s' Scc_flag='%s'><isfind>1</isfind><OTrogan_id>%s</OTrogan_id><OSip>%s</OSip><ODip>%s</ODip><ODmac>%s</ODmac><OAlarm_time>%s</OAlarm_time><OAlarm_end_time>%s</OAlarm_end_time><tips_sort>%d</tips_sort>%s<page Count='%s'>%s%s</page></attack>",interval, alarm_info.Type, alarm_info.Risk, alarm_info.Scc_flag, alarm_info.Trojan_id, alarm_info.Sip, alarm_info.Dip, alarm_info.Dmac, alarm_info.Alarm_time,alarm_info.Alarm_end_time, sort,xml,every_pagenum,pagenum,tips_url);

		tips_page.Page_free();
            	g_free(xml);
            	char *response = ((admin_opt_fun *)opt_fun)(credentials, realxml);
            	g_free(realxml);
            	return response;
	}
	else if(!strcmp(cmd, "tips_copy_alarm")){
						char *sortstrtemp = (char *)(params_value(params, "tips_sort"));
						int sort   = 0;
						if(sortstrtemp != NULL){
								sort = atoi(sortstrtemp);
						}
						char *realxml = NULL;
						int isfind = atoi(params_value(params, "isfind"));
						if(isfind != 0){
	      			CTips_alarm tips_alarm_handle;
            	alarm_stut_t alarm_info;
		char *interval = (char *)params_value(params, "refresh_interval");
            	tips_alarm_params(params, &alarm_info);
		uint32_t nowpage = atoll(params_value(params, "pagenum"));
            	tips_alarm_handle.Count_init_sql(&alarm_info, nowpage);
            	uint32_t lastpage = tips_alarm_handle.Count_exec_sql();
            	tips_alarm_handle.Copy_init_sql(&alarm_info,nowpage);
            	tips_alarm_handle.Copy_exec_sql();
		CTips_Page tips_page;
		char *pagenum = tips_page.init_pagenum(nowpage, lastpage);
		tips_page.init_urlpart_alarm(&alarm_info, credentials->token, interval);
		char *tips_url = tips_page.init_url(nowpage,lastpage, sort);
            	tips_alarm_handle.Alarm_init_sql(&alarm_info,nowpage, sort + 1);
            	char *xml = tips_alarm_handle.Alarm_exec_sql();
            	printf("xml=======%s\n",xml);
		realxml = (char *)g_strdup_printf("<autorefresh interval='%s'></autorefresh><attack Type='%s' Risk='%s' Scc_flag='%s'><isfind>1</isfind><msg>备份成功</msg><OTrogan_id>%s</OTrogan_id><OSip>%s</OSip><ODip>%s</ODip><ODmac>%s</ODmac><OAlarm_time>%s</OAlarm_time><OAlarm_end_time>%s</OAlarm_end_time><tips_sort>%d</tips_sort>%s<page>%s%s</page></attack>",interval, alarm_info.Type, alarm_info.Risk, alarm_info.Scc_flag, alarm_info.Trojan_id, alarm_info.Sip, alarm_info.Dip, alarm_info.Dmac, alarm_info.Alarm_time,alarm_info.Alarm_end_time, sort, xml, pagenum, tips_url);

							tips_sqlite_handle.catch_opt(credentials->username, (char *)"备份攻击窃密警报信息成功");
							tips_page.Page_free();
            	g_free(xml);
						}
						else{
								realxml=g_strdup("<attack><msg>备份失败，请先查询</msg><isfind>0</isfind><tips_sort>0</tips_sort></attack>");
								tips_sqlite_handle.catch_opt(credentials->username, (char *)"备份攻击窃密警报信息失败");
							}
                char *response = ((admin_opt_fun *)opt_fun)(credentials, realxml);
                g_free(realxml);
                return response;
  }
	else if(!strcmp(cmd, "tips_copy_alarmf")){
						char *sortstrtemp = (char *)(params_value(params, "tips_sort"));
						int sort   = 0;
						if(sortstrtemp != NULL){
								sort = atoi(sortstrtemp);
						}
							char *realxml = NULL;
	      			CTips_alarm tips_alarm_handle;
            	alarm_stut_t alarm_info;
							char *interval = (char *)params_value(params, "refresh_interval");
            	tips_alarm_params(params, &alarm_info);
							char* nowchar = (char *)params_value(params, "pagenum");
							uint32_t nowpage = 1;
							if((NULL != nowchar) && (!strlen(nowchar)))
								nowpage = atoll(nowchar);
            	tips_alarm_handle.Count_init_sql(&alarm_info, nowpage);
            	uint32_t lastpage = tips_alarm_handle.Count_exec_sql();
            	tips_alarm_handle.Copy_init_sql(&alarm_info,nowpage);
            	tips_alarm_handle.Copy_exec_sql(1);
							CTips_Page tips_page;
							char *pagenum = tips_page.init_pagenum(nowpage, lastpage);
							tips_page.init_urlpart_alarm(&alarm_info, credentials->token, interval);
							char *tips_url = tips_page.init_url(nowpage,lastpage, sort);
            	tips_alarm_handle.Alarm_init_sql(&alarm_info,nowpage);
            	char *xml = tips_alarm_handle.Alarm_exec_sql();
            	printf("xml=======%s\n",xml);
		realxml = (char *)g_strdup_printf("<autorefresh interval='%s'></autorefresh><attack Type='%s' Risk='%s' Scc_flag='%s'><isfind>1</isfind><msg>备份成功</msg><OTrogan_id>%s</OTrogan_id><OSip>%s</OSip><ODip>%s</ODip><ODmac>%s</ODmac><OAlarm_time>%s</OAlarm_time><OAlarm_end_time>%s</OAlarm_end_time><tips_sort>%d</tips_sort>%s<page>%s%s</page></attack>",interval, alarm_info.Type, alarm_info.Risk, alarm_info.Scc_flag, alarm_info.Trojan_id, alarm_info.Sip, alarm_info.Dip, alarm_info.Dmac, alarm_info.Alarm_time,alarm_info.Alarm_end_time, sort, xml, pagenum, tips_url);

							tips_sqlite_handle.catch_opt(credentials->username, (char *)"备份全部攻击窃密警报信息成功");
							tips_page.Page_free();
            	g_free(xml);
              char *response = ((admin_opt_fun *)opt_fun)(credentials, realxml);
              g_free(realxml);
              return response;
  }
	else if(!strcmp(cmd, "tips_delete_alarm")){
			char *realxml = NULL;
			int isfind = atoi(params_value(params, "isfind"));
			if(isfind != 0){
	        			CTips_alarm tips_alarm_handle;
                alarm_stut_t alarm_info;
								char *interval = (char *)params_value(params, "refresh_interval");
                tips_alarm_params(params, &alarm_info);
								uint32_t nowpage = atoll(params_value(params, "pagenum"));
                tips_alarm_handle.Count_init_sql(&alarm_info, nowpage);
                uint32_t lastpage = tips_alarm_handle.Count_exec_sql();
								printf("pagenum=%d\t%d\n",nowpage, lastpage);
                tips_alarm_handle.Delete_init_sql(&alarm_info,nowpage);
                tips_alarm_handle.Delete_exec_sql();
								CTips_Page tips_page;
								if(nowpage == lastpage)
									nowpage--;
								lastpage--;
								char *pagenum = tips_page.init_pagenum(nowpage, lastpage);
								tips_page.init_urlpart_alarm(&alarm_info, credentials->token, interval);
								char *tips_url = tips_page.init_url(nowpage,lastpage);
                tips_alarm_handle.Alarm_init_sql(&alarm_info,nowpage);
                char *xml = tips_alarm_handle.Alarm_exec_sql();
                printf("xml=======%s\n",xml);
		realxml = (char *)g_strdup_printf("<autorefresh interval='%s'></autorefresh><attack Type='%s' Risk='%s' Scc_flag='%s'><msg>删除成功</msg><isfind>1</isfind><OTrogan_id>%s</OTrogan_id><OSip>%s</OSip><ODip>%s</ODip><ODmac>%s</ODmac><OAlarm_time>%s</OAlarm_time><OAlarm_end_time>%s</OAlarm_end_time>%s<page>%s%s</page></attack>",interval, alarm_info.Type, alarm_info.Risk,alarm_info.Scc_flag, alarm_info.Trojan_id, alarm_info.Sip, alarm_info.Dip, alarm_info.Dmac, alarm_info.Alarm_time,alarm_info.Alarm_end_time, xml, pagenum, tips_url);
			tips_sqlite_handle.catch_opt(credentials->username, (char *)"删除攻击窃密警报信息成功");
								tips_page.Page_free();
                g_free(xml);
		}
		else{
			realxml=g_strdup("<attack><msg>删除失败，请先查询</msg><isfind>0</isfind></attack>");
			tips_sqlite_handle.catch_opt(credentials->username, (char *)"删除攻击窃密警报信息失败");
		}
    char *response = ((admin_opt_fun *)opt_fun)(credentials, realxml);
    g_free(realxml);
    return response;
  }
	else if(!strcmp(cmd, "tips_delete_alarmf")){
			char *realxml = NULL;
	    CTips_alarm tips_alarm_handle;
      alarm_stut_t alarm_info;
			char *interval = (char *)params_value(params, "refresh_interval");
      tips_alarm_params(params, &alarm_info);
			char* nowchar = (char *)params_value(params, "pagenum");
			uint32_t nowpage = 1;
			if((NULL != nowchar) && (!strlen(nowchar)))
				nowpage = atoll(nowchar);
			//uint32_t nowpage = atoll(params_value(params, "pagenum"));
      tips_alarm_handle.Count_init_sql(&alarm_info, nowpage);
      uint32_t lastpage = tips_alarm_handle.Count_exec_sql();
			printf("pagenum=%d\t%d\n",nowpage, lastpage);
      tips_alarm_handle.Delete_init_sql(&alarm_info,nowpage);
      tips_alarm_handle.Delete_exec_sql(1);
			CTips_Page tips_page;
			nowpage=1;
			lastpage=1;
			char *pagenum = tips_page.init_pagenum(nowpage, lastpage);
			tips_page.init_urlpart_alarm(&alarm_info, credentials->token, interval);
			char *tips_url = tips_page.init_url(nowpage,lastpage);
      tips_alarm_handle.Alarm_init_sql(&alarm_info,nowpage);
      char *xml = tips_alarm_handle.Alarm_exec_sql();
      printf("xml=======%s\n",xml);
realxml = (char *)g_strdup_printf("<autorefresh interval='%s'></autorefresh><attack Type='%s' Risk='%s' Scc_flag='%s'><msg>删除成功</msg><isfind>1</isfind><OTrogan_id>%s</OTrogan_id><OSip>%s</OSip><ODip>%s</ODip><ODmac>%s</ODmac><OAlarm_time>%s</OAlarm_time><OAlarm_end_time>%s</OAlarm_end_time>%s<page>%s%s</page></attack>",interval, alarm_info.Type, alarm_info.Risk, alarm_info.Scc_flag, alarm_info.Trojan_id, alarm_info.Sip, alarm_info.Dip, alarm_info.Dmac, alarm_info.Alarm_time,alarm_info.Alarm_end_time, xml, pagenum, tips_url);
			tips_sqlite_handle.catch_opt(credentials->username, (char *)"删除攻击窃密警报信息成功");
			tips_page.Page_free();
      g_free(xml);
    	char *response = ((admin_opt_fun *)opt_fun)(credentials, realxml);
    	g_free(realxml);
    	return response;
  }
	else if(!strcmp(cmd, "tips_event_display")){
		int sort = 0;
		char* sortstrtemp = (char *)params_value(params, "tips_sort");
		char* sortis =  (char *)params_value(params, "tips_sortis");
		printf("sortstrtemp ===================================%s\n",sortstrtemp);
		printf("sortis=========================================%s\n",sortis);
		if(sortstrtemp != NULL){
			sort = atoi(sortstrtemp);
			printf("sort================================================%d\n",sort);
		}

		if(sortis != NULL){
			printf("before sort================================================%d\n",sort);
			sort = ((sort + 1) % 2);
			printf("after sort================================================%d\n",sort);
		}
		char *realxml = NULL;
	  CTips_Event tips_Event_handle;
    event_stut_t event_info;
    tips_event_params(params, &event_info);
		uint32_t nowpage = atoll(params_value(params, "pagenum"));
    tips_Event_handle.Count_init_sql(&event_info, nowpage);
    uint32_t lastpage = tips_Event_handle.Count_exec_sql();
		CTips_Page tips_page;
		char *interval = (char *)params_value(params, "refresh_interval");
		char *pagenum = tips_page.init_pagenum(nowpage, lastpage);
		char *isfindtemp = (char *)params_value(params, "isfind");
		if(isfindtemp != NULL){
				int isfind = atoi(isfindtemp);
				if(isfind == 0){
					nowpage = 1;
				}
		}
		tips_page.init_urlpart_event(&event_info, credentials->token, interval);
		char *tips_url = tips_page.init_url(nowpage,lastpage,sort);
    tips_Event_handle.Event_init_sql(&event_info, nowpage,sort+1);
    char *xml = tips_Event_handle.Event_exec_sql();
    printf("xml=======%s\n",xml);
    realxml = (char *)g_strdup_printf("<autorefresh interval='%s'></autorefresh><secret type='%s'><isfind>1</isfind><OEvent_time>%s</OEvent_time><OSip>%s</OSip><OSport>%s</OSport><ODip>%s</ODip><OSender>%s</OSender><OReceiver>%s</OReceiver><OSen_msg>%s</OSen_msg><tips_sort>%d</tips_sort>%s<page>%s%s</page></secret>", interval, event_info.Event_class, event_info.Event_time, event_info.Sip, event_info.Sport, event_info.Dip, event_info.Sender, event_info.Receiver, event_info.Sen_msg,sort,xml, pagenum, tips_url);
		tips_page.Page_free();
    g_free(xml);
    char *response = ((admin_opt_fun *)opt_fun)(credentials, realxml);
    g_free(realxml);
    return response;
 }	
else if(!strcmp(cmd, "tips_copy_event")){
		char *realxml = NULL;
		int isfind = atoi(params_value(params, "isfind"));
		if(isfind != 0){
	   	     	CTips_Event tips_Event_handle;
           	event_stut_t event_info;
           	tips_event_params(params, &event_info);
						uint32_t nowpage = atoll(params_value(params, "pagenum"));
            tips_Event_handle.Count_init_sql(&event_info, nowpage);
            uint32_t lastpage = tips_Event_handle.Count_exec_sql();
						char *interval = (char *)params_value(params, "refresh_interval");
            tips_Event_handle.Copy_init_sql(&event_info, nowpage);
            tips_Event_handle.Copy_exec_sql();
						CTips_Page tips_page;
						char *pagenum = tips_page.init_pagenum(nowpage, lastpage);
						tips_page.init_urlpart_event(&event_info, credentials->token, interval);
						char *tips_url = tips_page.init_url(nowpage,lastpage);
            tips_Event_handle.Event_init_sql(&event_info, nowpage);
            char *xml = tips_Event_handle.Event_exec_sql();
            printf("xml=======%s\n",xml);
            realxml = (char *)g_strdup_printf("<autorefresh interval='%s'></autorefresh><secret type='%s'><msg>备份成功</msg><isfind>1</isfind><OEvent_time>%s</OEvent_time><OSip>%s</OSip><OSport>%s</OSport><ODip>%s</ODip><OSender>%s</OSender><OReceiver>%s</OReceiver><OSen_msg>%s</OSen_msg>%s<page>%s%s</page></secret>", interval, event_info.Event_class, event_info.Event_time, event_info.Sip, event_info.Sport, event_info.Dip, event_info.Sender, event_info.Receiver, event_info.Sen_msg,xml, pagenum, tips_url);
						tips_sqlite_handle.catch_opt(credentials->username, (char *)"备份涉密标志事件成功");
						tips_page.Page_free();
            g_free(xml);
		}
		else{
			realxml=g_strdup("<secret><msg>备份失败，请先查询</msg><isfind>0</isfind></secret>");
			tips_sqlite_handle.catch_opt(credentials->username, (char *)"备份涉密标志事件失败");
		}
    char *response = ((admin_opt_fun *)opt_fun)(credentials, realxml);
    g_free(realxml);
    return response;
}
else if(!strcmp(cmd, "tips_copy_eventf")){
				char *realxml = NULL;
				CTips_Event tips_Event_handle;
				event_stut_t event_info;
				tips_event_params(params, &event_info);
				char* nowchar = (char *)params_value(params, "pagenum");
				uint32_t nowpage = 1;
				if((NULL != nowchar) && (!strlen(nowchar)))
					nowpage = atoll(nowchar);
				//uint32_t nowpage = atoll(params_value(params, "pagenum"));
				tips_Event_handle.Count_init_sql(&event_info, nowpage);
				uint32_t lastpage = tips_Event_handle.Count_exec_sql();
				char *interval = (char *)params_value(params, "refresh_interval");
				tips_Event_handle.Copy_init_sql(&event_info, nowpage);
				tips_Event_handle.Copy_exec_sql(1);
				CTips_Page tips_page;
				char *pagenum = tips_page.init_pagenum(nowpage, lastpage);
				tips_page.init_urlpart_event(&event_info, credentials->token, interval);
				char *tips_url = tips_page.init_url(nowpage,lastpage);
				tips_Event_handle.Event_init_sql(&event_info, nowpage);
				char *xml = tips_Event_handle.Event_exec_sql();
				printf("xml=======%s\n",xml);
				realxml = (char *)g_strdup_printf("<autorefresh interval='%s'></autorefresh><secret type='%s'><msg>备份成功</msg><isfind>1</isfind><OEvent_time>%s</OEvent_time><OSip>%s</OSip><OSport>%s</OSport><ODip>%s</ODip><OSender>%s</OSender><OReceiver>%s</OReceiver><OSen_msg>%s</OSen_msg>%s<page>%s%s</page></secret>", interval, event_info.Event_class, event_info.Event_time, event_info.Sip, event_info.Sport, event_info.Dip, event_info.Sender, event_info.Receiver, event_info.Sen_msg,xml, pagenum, tips_url);
				tips_sqlite_handle.catch_opt(credentials->username, (char *)"备份涉密标志事件成功");
				tips_page.Page_free();
				g_free(xml);
				char *response = ((admin_opt_fun *)opt_fun)(credentials, realxml);
				g_free(realxml);
				return response;
}
else if(!strcmp(cmd, "tips_delete_event")){
		char *realxml = NULL;
		int isfind = atoi(params_value(params, "isfind"));
		if(isfind != 0){
	     CTips_Event tips_Event_handle;
       event_stut_t event_info;
       tips_event_params(params, &event_info);
			 uint32_t nowpage = atoll(params_value(params, "pagenum"));
       tips_Event_handle.Count_init_sql(&event_info, nowpage);
       uint32_t lastpage = tips_Event_handle.Count_exec_sql();
			 char *interval = (char *)params_value(params, "refresh_interval");
       tips_Event_handle.Delete_init_sql(&event_info, nowpage);
       tips_Event_handle.Delete_exec_sql();
			 CTips_Page tips_page;
			 if(nowpage == lastpage)
				 nowpage--;
			 lastpage--;
			 char *pagenum = tips_page.init_pagenum(nowpage, lastpage);
			 tips_page.init_urlpart_event(&event_info, credentials->token, interval);
			 char *tips_url = tips_page.init_url(nowpage,lastpage);
       tips_Event_handle.Event_init_sql(&event_info, nowpage);
       char *xml = tips_Event_handle.Event_exec_sql();
       printf("xml=======%s\n",xml);
       realxml = (char *)g_strdup_printf("<autorefresh interval='%s'></autorefresh><secret type='%s'><msg>删除成功</msg><isfind>1</isfind><OEvent_time>%s</OEvent_time><OSip>%s</OSip><OSport>%s</OSport><ODip>%s</ODip><OSender>%s</OSender><OReceiver>%s</OReceiver><OSen_msg>%s</OSen_msg>%s<page>%s%s</page></secret>", interval, event_info.Event_class, event_info.Event_time, event_info.Sip, event_info.Sport, event_info.Dip, event_info.Sender, event_info.Receiver, event_info.Sen_msg,xml,pagenum,tips_url);
				tips_sqlite_handle.catch_opt(credentials->username, (char *)"删除涉密标志事件成功");
				tips_page.Page_free();
        g_free(xml);
		}
		else{
			realxml=g_strdup("<secret><msg>删除失败，请先查询</msg><isfind>0</isfind></secret>");
			tips_sqlite_handle.catch_opt(credentials->username, (char *)"删除涉密标志事件失败");
		}
                char *response = ((admin_opt_fun *)opt_fun)(credentials, realxml);
                g_free(realxml);
                return response;
}
else if(!strcmp(cmd, "tips_delete_eventf")){
		char *realxml = NULL;
	  CTips_Event tips_Event_handle;
    event_stut_t event_info;
    tips_event_params(params, &event_info);
		char* nowchar = (char *)params_value(params, "pagenum");
		uint32_t nowpage = 1;
		if((NULL != nowchar) && (!strlen(nowchar)))
			nowpage = atoll(nowchar);
		//uint32_t nowpage = atoll(params_value(params, "pagenum"));
    tips_Event_handle.Count_init_sql(&event_info, nowpage);
    uint32_t lastpage = tips_Event_handle.Count_exec_sql();
		char *interval = (char *)params_value(params, "refresh_interval");
    tips_Event_handle.Delete_init_sql(&event_info, nowpage);
    tips_Event_handle.Delete_exec_sql(1);
		CTips_Page tips_page;
		nowpage = 1;
		lastpage = 1;
		char *pagenum = tips_page.init_pagenum(nowpage, lastpage);
		tips_page.init_urlpart_event(&event_info, credentials->token, interval);
		char *tips_url = tips_page.init_url(nowpage,lastpage);
    tips_Event_handle.Event_init_sql(&event_info, nowpage);
    char *xml = tips_Event_handle.Event_exec_sql();
    printf("xml=======%s\n",xml);
    realxml = (char *)g_strdup_printf("<autorefresh interval='%s'></autorefresh><secret type='%s'><msg>删除成功</msg><isfind>1</isfind><OEvent_time>%s</OEvent_time><OSip>%s</OSip><OSport>%s</OSport><ODip>%s</ODip><OSender>%s</OSender><OReceiver>%s</OReceiver><OSen_msg>%s</OSen_msg>%s<page>%s%s</page></secret>", interval, event_info.Event_class, event_info.Event_time, event_info.Sip, event_info.Sport, event_info.Dip, event_info.Sender, event_info.Receiver, event_info.Sen_msg,xml,pagenum,tips_url);
		tips_sqlite_handle.catch_opt(credentials->username, (char *)"删除涉密标志事件成功");
		tips_page.Page_free();
    g_free(xml);
    char *response = ((admin_opt_fun *)opt_fun)(credentials, realxml);
    g_free(realxml);
    return response;
}
else if(!strcmp(cmd, "tips_netlog_display")){
	char page_number[20];
	uint32_t total_number = 0;
	int sort = 0;
	char* sortstrtemp = (char *)params_value(params, "tips_sort");
	char* sortis =  (char *)params_value(params, "tips_sortis");
	printf("sortstrtemp ===================================%s\n",sortstrtemp);
	printf("sortis=========================================%s\n",sortis);
	if(sortstrtemp != NULL){
		sort = atoi(sortstrtemp);
		printf("sort================================================%d\n",sort);
	}
	if(sortis != NULL){
		printf("before sort================================================%d\n",sort);
		sort = ((sort + 1) % 2);
		printf("after sort================================================%d\n",sort);
	}
	char bug_pagenum[10];
	char *realxml = NULL;
	memset(page_number,0,sizeof(page_number));
	memset(bug_pagenum,0,sizeof(bug_pagenum));
	CTips_netlog tips_netlog_handle;
	netlog_stut_t netlog_info;
	tips_netlog_params(params, &netlog_info);
	char *interval = (char *)params_value(params, "refresh_interval");
	uint32_t nowpage = atoll(params_value(params, "pagenum"));
	char *isfindtemp = (char *)params_value(params, "isfind");
	char *every_pagenum = (char *)params_value(params, "every_pagenum");
	FILE *fp;
	if(every_pagenum == NULL || strlen(every_pagenum) == 0)
	{
		fp = fopen((char *)"/usr/local/tipschecker/etc/tipscheck/every_pagenum","r+");
		fread(bug_pagenum,sizeof(bug_pagenum),1,fp);
		/*
		//if(NULL == (fp = fopen((char *)"/usr/local/tipschecker/etc/tipscheck/every_pagenum","w+")))
		//{
		//	perror("fopen");
		//	exit(0);
		//}
		////every_pagenum = bug_pagenum;
		//printf("wo\n");
		////fread(page_number,sizeof(page_number),1,fp);
		////every_pagenum = page_number;
		//fwrite((char *)"50",strlen((char *)"50"),1,fp);
		*/
		every_pagenum = bug_pagenum;
		fclose(fp);
	}
	else
	{
		if(NULL == (fp = fopen((char *)"/usr/local/tipschecker/etc/tipscheck/every_pagenum","w+")))
		{
			perror("fopen");
			exit(0);
		}
		fwrite(every_pagenum,strlen(every_pagenum),1,fp);
		fclose(fp);
	}
	//sleep(30);
	if(isfindtemp != NULL)
	{
		int isfind = atoi(isfindtemp);
		if(isfind == 0)
			nowpage = 1;
	}
	tips_netlog_handle.Count_init_sql(&netlog_info, nowpage);
	uint32_t lastpage = tips_netlog_handle.Count_exec_sql(&total_number);
	CTips_Page tips_page;
	char *pagenum = tips_page.init_pagenum(nowpage, lastpage,total_number);
	tips_page.init_urlpart_netlog(&netlog_info, credentials->token, interval);
	char *tips_url = tips_page.init_url(nowpage,lastpage,sort,netlog_info.Every_pagenum);
	puts("wowowowowowowowowowoowowwoowowwo33333333");
	tips_netlog_handle.Netlog_init_sql(&netlog_info, nowpage,sort+1);
	char *xml = tips_netlog_handle.Netlog_exec_sql();
	printf("xml=======%s\n",xml);
	if(strlen(netlog_info.Start_time)/*hyj ,*/ && strlen(netlog_info.End_time))
	{
	int judge = judge_time(netlog_info.Start_time,netlog_info.End_time);
	if(judge)
	realxml = (char *)g_strdup_printf("<autorefresh interval='%s'></autorefresh><tips_dialog><isfind>1</isfind><msg>开始时间 不能大于 结束时间</msg><ODip>%s</ODip><OSip>%s</OSip><OSport>%s</OSport><ODport>%s</ODport><OProtocol>%s</OProtocol><OStart_time>%s</OStart_time><OEnd_time>%s</OEnd_time><OLoad>%s</OLoad><OUpload>%s</OUpload><tips_sort>%d</tips_sort>%s<page Count='%s'>%s%s</page></tips_dialog>", interval, netlog_info.Dip, netlog_info.Sip, netlog_info.Sport, netlog_info.Dport, netlog_info.Protocol, netlog_info.Start_time, netlog_info.End_time, netlog_info.Load, netlog_info.Upload,sort, xml, every_pagenum,pagenum, tips_url);
	else
	realxml = (char *)g_strdup_printf("<autorefresh interval='%s'></autorefresh><tips_dialog><isfind>1</isfind><ODip>%s</ODip><OSip>%s</OSip><OSport>%s</OSport><ODport>%s</ODport><OProtocol>%s</OProtocol><OStart_time>%s</OStart_time><OEnd_time>%s</OEnd_time><OLoad>%s</OLoad><OUpload>%s</OUpload><tips_sort>%d</tips_sort>%s<page Count='%s'>%s%s</page></tips_dialog>", interval, netlog_info.Dip, netlog_info.Sip, netlog_info.Sport, netlog_info.Dport, netlog_info.Protocol, netlog_info.Start_time, netlog_info.End_time, netlog_info.Load, netlog_info.Upload,sort, xml, every_pagenum,pagenum, tips_url);
	}
	else
	realxml = (char *)g_strdup_printf("<autorefresh interval='%s'></autorefresh><tips_dialog><isfind>1</isfind><ODip>%s</ODip><OSip>%s</OSip><OSport>%s</OSport><ODport>%s</ODport><OProtocol>%s</OProtocol><OStart_time>%s</OStart_time><OEnd_time>%s</OEnd_time><OLoad>%s</OLoad><OUpload>%s</OUpload><tips_sort>%d</tips_sort>%s<page Count='%s'>%s%s</page></tips_dialog>", interval, netlog_info.Dip, netlog_info.Sip, netlog_info.Sport, netlog_info.Dport, netlog_info.Protocol, netlog_info.Start_time, netlog_info.End_time, netlog_info.Load, netlog_info.Upload,sort, xml, every_pagenum,pagenum, tips_url);
	tips_page.Page_free();
	g_free(xml);
	char *response = ((admin_opt_fun *)opt_fun)(credentials, realxml);
	printf("realxml***************%s\n",realxml);
	g_free(realxml);
	return response;
}	
	else if(!strcmp(cmd, "tips_copy_netlog")){
		char *realxml = NULL;
		int isfind = atoi(params_value(params, "isfind"));
		if(isfind != 0){
	  	CTips_netlog tips_netlog_handle;
    	netlog_stut_t netlog_info;
    	tips_netlog_params(params, &netlog_info);
			char *interval = (char *)params_value(params, "refresh_interval");
			uint32_t nowpage = atoll(params_value(params, "pagenum"));
    	tips_netlog_handle.Count_init_sql(&netlog_info, nowpage);
    	uint32_t lastpage = tips_netlog_handle.Count_exec_sql();
    	tips_netlog_handle.Copy_init_sql(&netlog_info, nowpage);
    	tips_netlog_handle.Copy_exec_sql();
			CTips_Page tips_page;
			char *pagenum = tips_page.init_pagenum(nowpage, lastpage);
			tips_page.init_urlpart_netlog(&netlog_info, credentials->token, interval);
			char *tips_url = tips_page.init_url(nowpage,lastpage);
    	tips_netlog_handle.Netlog_init_sql(&netlog_info, nowpage);
    	char *xml = tips_netlog_handle.Netlog_exec_sql();
    	printf("xml=======%s\n",xml);
    	realxml = (char *)g_strdup_printf("<autorefresh interval='%s'></autorefresh><tips_dialog><msg>备份成功</msg><isfind>1</isfind><ODip>%s</ODip><OSip>%s</OSip><OSport>%s</OSport><OProtocol>%s</OProtocol><OStart_time>%s</OStart_time><OEnd_time>%s</OEnd_time><OLoad>%s</OLoad><OUpload>%s</OUpload>%s<page>%s%s</page></tips_dialog>", interval, netlog_info.Dip, netlog_info.Sip, netlog_info.Sport, netlog_info.Protocol, netlog_info.Start_time, netlog_info.End_time, netlog_info.Load, netlog_info.Upload, xml, pagenum, tips_url);
			tips_sqlite_handle.catch_opt(credentials->username, (char *)"备份通信会话审计成功");
			tips_page.Page_free();
    	g_free(xml);
		}
		else{
			realxml=g_strdup("<tips_dialog><msg>备份失败，请先查询</msg><isfind>0</isfind></tips_dialog>");
			tips_sqlite_handle.catch_opt(credentials->username, (char *)"备份通信会话审计失败");
		}
    char *response = ((admin_opt_fun *)opt_fun)(credentials, realxml);
    printf("realxml***************%s\n",realxml);
    g_free(realxml);
    return response;
  }
	else if(!strcmp(cmd, "tips_copy_netlogf")){
		char *realxml = NULL;
	  CTips_netlog tips_netlog_handle;
    netlog_stut_t netlog_info;
    tips_netlog_params(params, &netlog_info);
		char *interval = (char *)params_value(params, "refresh_interval");
		//uint32_t nowpage = atoll(params_value(params, "pagenum"));
		char* nowchar = (char *)params_value(params, "pagenum");
		uint32_t nowpage = 1;
		if((NULL != nowchar) && (!strlen(nowchar)))
			nowpage = atoll(nowchar);
    tips_netlog_handle.Count_init_sql(&netlog_info, nowpage);
    uint32_t lastpage = tips_netlog_handle.Count_exec_sql();
    tips_netlog_handle.Copy_init_sql(&netlog_info, nowpage);
    tips_netlog_handle.Copy_exec_sql(1);
		CTips_Page tips_page;
		char *pagenum = tips_page.init_pagenum(nowpage, lastpage);
		tips_page.init_urlpart_netlog(&netlog_info, credentials->token, interval);
		char *tips_url = tips_page.init_url(nowpage,lastpage);
    tips_netlog_handle.Netlog_init_sql(&netlog_info, nowpage);
    char *xml = tips_netlog_handle.Netlog_exec_sql();
    printf("xml=======%s\n",xml);
    realxml = (char *)g_strdup_printf("<autorefresh interval='%s'></autorefresh><tips_dialog><msg>备份成功</msg><isfind>1</isfind><ODip>%s</ODip><OSip>%s</OSip><OSport>%s</OSport><OProtocol>%s</OProtocol><OStart_time>%s</OStart_time><OEnd_time>%s</OEnd_time><OLoad>%s</OLoad><OUpload>%s</OUpload>%s<page>%s%s</page></tips_dialog>", interval, netlog_info.Dip, netlog_info.Sip, netlog_info.Sport, netlog_info.Protocol, netlog_info.Start_time, netlog_info.End_time, netlog_info.Load, netlog_info.Upload, xml, pagenum, tips_url);
		tips_sqlite_handle.catch_opt(credentials->username, (char *)"备份通信会话审计成功");
		tips_page.Page_free();
    g_free(xml);
    char *response = ((admin_opt_fun *)opt_fun)(credentials, realxml);
    printf("realxml***************%s\n",realxml);
    g_free(realxml);
    return response;
  }
	else if(!strcmp(cmd, "tips_opt_copy"))
	{
		int isfind = atoi(params_value(params, "isfind"));
		if(isfind != 0)
		{
			char *realxml = NULL;
	  		CTips_opt tips_opt_handle;
    			opt_stut_t opt_info;
    			opt_info.username = (char *)params_value(params, "tips_username");
    			opt_info.Otime = (char *)params_value(params, "tips_Otime");
      			opt_info.Etime = (char *)params_value(params, "tips_Oetime");
      			opt_info.Optdsc = (char *)params_value(params, "tips_Optdsc");
			//uint32_t nowpage = atoll(params_value(params, "pagenum"));
			char* nowchar = (char *)params_value(params, "pagenum");
			uint32_t nowpage = 1;
			if((NULL != nowchar) && (!strlen(nowchar)))
				nowpage = atoll(nowchar);
    			tips_opt_handle.Copy_init_sql(&opt_info, nowpage);
    			tips_opt_handle.Copy_exec_sql(1);
      			tips_opt_handle.Count_init_sql(&opt_info, nowpage);
      			uint32_t lastpage = tips_opt_handle.Count_exec_sql();
			CTips_Page tips_page;
			char *pagenum = tips_page.init_pagenum(nowpage, lastpage);
			tips_page.init_urlpart_opt(&opt_info, credentials->token);
			char *tips_url = tips_page.init_url(nowpage,lastpage);
      			tips_opt_handle.Opt_init_sql(&opt_info, nowpage);
      			char *xml = tips_opt_handle.Opt_exec_sql();
      			realxml = (char *)g_strdup_printf("<audit><msg>备份成功</msg><Ousername><![CDATA[%s]]></Ousername><OOtime><![CDATA[%s]]></OOtime><OOetime><![CDATA[%s]]></OOetime><OOptdsc><![CDATA[%s]]></OOptdsc>%s<page>%s%s</page></audit>",opt_info.username, opt_info.Otime, opt_info.Etime, opt_info.Optdsc, xml, pagenum, tips_url);
			tips_page.Page_free();
      			g_free(xml);
      			char *html = ((admin_opt_fun *)opt_fun)(credentials, realxml);
      			g_free(realxml);
      			return html;
		}
		else
		{
			char *realxml = NULL;
			realxml=g_strdup("<audit><msg>备份失败，请先查询</msg><isfind>0</isfind></audit>");
      			char *html = ((admin_opt_fun *)opt_fun)(credentials, realxml);
			tips_sqlite_handle.catch_opt(credentials->username, (char *)"备份操作行为审计信息失败");
      			g_free(realxml);
      			return html;
			
		}
    }
	
	else if(!strcmp(cmd, "tips_delete_netlog")){
		char *realxml = NULL;
		int isfind = atoi(params_value(params, "isfind"));
		if(isfind != 0){
	   	CTips_netlog tips_netlog_handle;
     	netlog_stut_t netlog_info;
     	tips_netlog_params(params, &netlog_info);
			char *interval = (char *)params_value(params, "refresh_interval");
			uint32_t nowpage = atoll(params_value(params, "pagenum"));
      tips_netlog_handle.Count_init_sql(&netlog_info, nowpage);
      uint32_t lastpage = tips_netlog_handle.Count_exec_sql();
      tips_netlog_handle.Delete_init_sql(&netlog_info, nowpage);
      tips_netlog_handle.Delete_exec_sql();
			CTips_Page tips_page;
			if(nowpage == lastpage)
				nowpage--;
			lastpage--;
			char *pagenum = tips_page.init_pagenum(nowpage, lastpage);
			tips_page.init_urlpart_netlog(&netlog_info, credentials->token, interval);
			char *tips_url = tips_page.init_url(nowpage,lastpage);
      tips_netlog_handle.Netlog_init_sql(&netlog_info, nowpage);
      char *xml = tips_netlog_handle.Netlog_exec_sql();
      printf("xml=======%s\n",xml);
      realxml = (char *)g_strdup_printf("<autorefresh interval='%s'></autorefresh><tips_dialog><msg>删除成功</msg><isfind>1</isfind><ODip>%s</ODip><OSip>%s</OSip><OSport>%s</OSport><OProtocol>%s</OProtocol><OStart_time>%s</OStart_time><OEnd_time>%s</OEnd_time><OLoad>%s</OLoad><OUpload>%s</OUpload>%s<page>%s%s</page></tips_dialog>", interval, netlog_info.Dip, netlog_info.Sip, netlog_info.Sport, netlog_info.Protocol, netlog_info.Start_time, netlog_info.End_time, netlog_info.Load, netlog_info.Upload, xml, pagenum, tips_url);
			tips_sqlite_handle.catch_opt(credentials->username, (char *)"删除通信会话审计成功");
			tips_page.Page_free();
      g_free(xml);
		}
		else{
			realxml=g_strdup("<tips_dialog><msg>删除失败，请先查询</msg><isfind>0</isfind></tips_dialog>");
			tips_sqlite_handle.catch_opt(credentials->username, (char *)"删除通信会话审计失败");
		}
    char *response = ((admin_opt_fun *)opt_fun)(credentials, realxml);
    printf("realxml***************%s\n",realxml);
    g_free(realxml);
    return response;
  }
	else if(!strcmp(cmd, "tips_delete_netlogf")){
		char *realxml = NULL;
	  CTips_netlog tips_netlog_handle;
    netlog_stut_t netlog_info;
    tips_netlog_params(params, &netlog_info);
		char *interval = (char *)params_value(params, "refresh_interval");
		//int32_t nowpage = atoll(params_value(params, "pagenum"));
		char* nowchar = (char *)params_value(params, "pagenum");
		uint32_t nowpage = 1;
		if((NULL != nowchar) && (!strlen(nowchar)))
			nowpage = atoll(nowchar);
    tips_netlog_handle.Count_init_sql(&netlog_info, nowpage);
    uint32_t lastpage = tips_netlog_handle.Count_exec_sql();
    tips_netlog_handle.Delete_init_sql(&netlog_info, nowpage);
    tips_netlog_handle.Delete_exec_sql(1);
		CTips_Page tips_page;
		nowpage = 1;
		lastpage = 1;
		char *pagenum = tips_page.init_pagenum(nowpage, lastpage);
		tips_page.init_urlpart_netlog(&netlog_info, credentials->token, interval);
		char *tips_url = tips_page.init_url(nowpage,lastpage);
     tips_netlog_handle.Netlog_init_sql(&netlog_info, nowpage);
     char *xml = tips_netlog_handle.Netlog_exec_sql();
     printf("xml=======%s\n",xml);
     realxml = (char *)g_strdup_printf("<autorefresh interval='%s'></autorefresh><tips_dialog><msg>删除成功</msg><isfind>1</isfind><ODip>%s</ODip><OSip>%s</OSip><OSport>%s</OSport><OProtocol>%s</OProtocol><OStart_time>%s</OStart_time><OEnd_time>%s</OEnd_time><OLoad>%s</OLoad><OUpload>%s</OUpload>%s<page>%s%s</page></tips_dialog>", interval, netlog_info.Dip, netlog_info.Sip, netlog_info.Sport, netlog_info.Protocol, netlog_info.Start_time, netlog_info.End_time, netlog_info.Load, netlog_info.Upload, xml, pagenum, tips_url);
		tips_sqlite_handle.catch_opt(credentials->username, (char *)"删除通信会话审计成功");
		tips_page.Page_free();
    g_free(xml);
    char *response = ((admin_opt_fun *)opt_fun)(credentials, realxml);
    printf("realxml***************%s\n",realxml);
    g_free(realxml);
    return response;
  }

	else if((!strcmp(cmd, "user_modify_page")) && ((strcmp(credentials->role, "0")) || ((login != NULL) && (!strcmp(credentials->username, login))))){
		char *xml = NULL;
		char *role = (char *)params_value(params, "userrole");
		if((strcmp(credentials->role, "0"))){
			xml = (char *)g_strdup_printf("<get_tasks_response modify_user_page=\"1\"><gotocmd>get_users</gotocmd><tip_user><![CDATA[%s]]></tip_user><tip_password></tip_password><userrole>%s</userrole></get_tasks_response>",login,role);
		}
		else{
			xml = (char *)g_strdup_printf("<get_tasks_response modify_user_page=\"1\"><gotocmd>get_Alarm_info</gotocmd><tip_user><![CDATA[%s]]></tip_user><tip_password></tip_password><userrole>%s</userrole></get_tasks_response>",login, role);
		}
    char *html = ((admin_opt_fun *)opt_fun)(credentials, xml);
    g_free(xml);
    return html;
	}
	else if(strcmp(credentials->role, "0")){
		if(!strcmp(cmd, "get_users")){
			return display_user_html(NULL, credentials->role);
		}
		else if(!strcmp(cmd,"tips_stop_user")){
			char delete_msg[1024];
        		const char *delim=(char *)";";
        		const char *p; 
			char *tips_err = NULL;
			char *tmp;
			memset(delete_msg,0,sizeof(delete_msg));
			printf("user_delete_cmd:%s\n",cmd);
			char *login = (char *)params_value(params, "login");
			printf("delete users is %s\n",login);
			if((login != NULL) && (0 != strlen(login)))
			{
				tmp = g_strdup_printf("停用用户 %s 成功",login);
        			p=strtok(login,delim);
      				tips_sqlite_handle.User_stop(p);
				tips_err = g_strdup_printf("停用用户 %s 成功",p);
				tips_sqlite_handle.catch_opt(credentials->username, tips_err);
        			while((p=strtok(NULL,delim)))
				{
      					tips_sqlite_handle.User_stop(p);
					tips_err = g_strdup_printf("停用用户 %s 成功",p);
					tips_sqlite_handle.catch_opt(credentials->username, tips_err);
				}
				
			}
			else
			{
				tmp = g_strdup_printf("请选择要停用的用户");
			}
			strcpy(delete_msg,tmp);
			if(tips_err != NULL)
				g_free(tips_err);
			g_free(tmp);
      			return display_user_html(delete_msg, credentials->role);
		}
		else if(!strcmp(cmd,"tips_start_user")){
			char delete_msg[1024];
        		const char *delim=(char *)";";
        		const char *p; 
			char *tips_err = NULL;
			char *tmp_success;
			char *tmp_failed;
			char *start_success;
			char *start_failed;
			int success_flag = 0;
			int failed_flag = 0;
			memset(delete_msg,0,sizeof(delete_msg));
			printf("user_delete_cmd:%s\n",cmd);
			char *login = (char *)params_value(params, "login");
			printf("delete users is %s\n",login);
			if((login != NULL) && (0 != strlen(login)))
			{
				start_success = g_strdup_printf("启用用户");
				start_failed = g_strdup_printf("启用用户");
				tmp_success = start_success;
				tmp_failed = start_failed;
				//tmp = g_strdup_printf("启用用户 %s 成功",login);
        			p=strtok(login,delim);
      				if(!(tips_sqlite_handle.get_admit_time(p)))
				{
					success_flag = 1;
      				tips_sqlite_handle.User_start(p);
					tips_err = g_strdup_printf("启用用户 %s 成功",p);
					start_success = g_strdup_printf("%s %s;",tmp_success,p);
					tmp_success = start_success;
				}
				else
				{
					failed_flag = 1;
      				tips_sqlite_handle.User_start(p);
					start_failed = g_strdup_printf("%s %s;",tmp_failed,p);
					tmp_failed = start_failed;
					tips_err = g_strdup_printf("启用用户 %s 失败",p);
				}
				tips_sqlite_handle.catch_opt(credentials->username, tips_err);
        			while((p=strtok(NULL,delim)))
				{
      				if(!(tips_sqlite_handle.get_admit_time(p)))
					{
						success_flag = 1;
      					tips_sqlite_handle.User_start(p);
						tips_err = g_strdup_printf("启用用户 %s 成功",p);
						start_success = g_strdup_printf("%s %s;",tmp_success,p);
						tmp_success = start_success;
					}
					else
					{
						failed_flag = 1;
      					tips_sqlite_handle.User_start(p);
						start_failed = g_strdup_printf("%s %s;",tmp_failed,p);
						tmp_failed = start_failed;
						tips_err = g_strdup_printf("启用用户 %s 失败",p);
					}
					tips_sqlite_handle.catch_opt(credentials->username, tips_err);
				}
				if((success_flag == 1) && (failed_flag == 1))
					tmp_success = g_strdup_printf("%s成功  %s失败(原因:有效期到期)",start_success,start_failed);
				else if((success_flag == 1) && (failed_flag == 0))
					tmp_success = g_strdup_printf("%s成功",start_success);
				else if((success_flag == 0) && (failed_flag == 1))
					tmp_success = g_strdup_printf("%s失败(原因:有效期到期)",start_failed);
				else
					tmp_success = g_strdup_printf("请选择要启用的用户");
				
			}
			else
			{
				tmp_success = g_strdup_printf("请选择要启用的用户");
			}
			strcpy(delete_msg,tmp_success);
			if(tips_err != NULL)
				g_free(tips_err);
			g_free(tmp_success);
			g_free(start_success);
			g_free(start_failed);
      			return display_user_html(delete_msg, credentials->role);
		}
		else if(!strcmp(cmd, "user_delete")){
			char delete_msg[1024];
        		const char *delim=(char *)";";
        		const char *p; 
			char *tips_err = NULL;
			char *tmp;
			memset(delete_msg,0,sizeof(delete_msg));
			printf("user_delete_cmd:%s\n",cmd);
			char *login = (char *)params_value(params, "login");
			printf("delete users is %s\n",login);
			if((login != NULL) && (0 != strlen(login)))
			{
				tmp = g_strdup_printf("删除用户 %s 成功",login);
        			p=strtok(login,delim);
      				tips_sqlite_handle.User_delete(p);
				tips_err = g_strdup_printf("删除用户 %s 成功",p);
				tips_sqlite_handle.catch_opt(credentials->username, tips_err);
        			while((p=strtok(NULL,delim)))
				{
      					tips_sqlite_handle.User_delete(p);
					tips_err = g_strdup_printf("删除用户 %s 成功",p);
					tips_sqlite_handle.catch_opt(credentials->username, tips_err);
				}
				
			}
			else
			{
				tmp = g_strdup_printf("请选择要删除的用户");
			}
			strcpy(delete_msg,tmp);
			if(tips_err != NULL)
				g_free(tips_err);
			g_free(tmp);
      return display_user_html(delete_msg, credentials->role);
		}
		else if(!strcmp(cmd, "tips_opt_display")){
		char page_number[20];
		char bug_pagenum[10];
		uint32_t total_number=0;
		int sort = 0;
		char* sortstrtemp = (char *)params_value(params, "tips_sort");
		char* sortis =  (char *)params_value(params, "tips_sortis");
		memset(bug_pagenum,0,sizeof(bug_pagenum));
		printf("sortstrtemp ===================================%s\n",sortstrtemp);
		printf("sortis=========================================%s\n",sortis);
		if(sortstrtemp != NULL){
			sort = atoi(sortstrtemp);
			printf("sort================================================%d\n",sort);
		}

		if(sortis != NULL){
			printf("before sort================================================%d\n",sort);
			sort = ((sort + 1) % 2);
			printf("after sort================================================%d\n",sort);
		}
			char *realxml = NULL;
		memset(page_number,0,sizeof(page_number));
	  	CTips_opt tips_opt_handle;
    	opt_stut_t opt_info;
    	opt_info.username = (char *)params_value(params, "tips_username");
    	opt_info.Otime = (char *)params_value(params, "tips_Otime");
      opt_info.Etime = (char *)params_value(params, "tips_Oetime");
      opt_info.Optdsc = (char *)params_value(params, "tips_Optdsc");
		char *every_pagenum = (char *)params_value(params, "every_pagenum");
			uint32_t nowpage = atoll(params_value(params, "pagenum"));
#if 1 
		FILE *fp;
	if(every_pagenum == NULL || strlen(every_pagenum) == 0)
	{
		fp = fopen((char *)"/usr/local/tipschecker/etc/tipscheck/every_pagenum","r+");
		fread(bug_pagenum,sizeof(bug_pagenum),1,fp);
		/*
		//if(NULL == (fp = fopen((char *)"/usr/local/tipschecker/etc/tipscheck/every_pagenum","w+")))
		//{
		//	perror("fopen");
		//	exit(0);
		//}
		////every_pagenum = bug_pagenum;
		//printf("wo\n");
		////fread(page_number,sizeof(page_number),1,fp);
		////every_pagenum = page_number;
		//fwrite((char *)"50",strlen((char *)"50"),1,fp);
		*/
		every_pagenum = bug_pagenum;
		fclose(fp);
	}
	else
	{
		if(NULL == (fp = fopen((char *)"/usr/local/tipschecker/etc/tipscheck/every_pagenum","w+")))
		{
			perror("fopen");
			exit(0);
		}
		fwrite(every_pagenum,strlen(every_pagenum),1,fp);
		fclose(fp);
	}
		//if(NULL == (fp = fopen((char *)"/usr/local/tipschecker/etc/tipscheck/every_pagenum","w+")))
		//{
		//	perror("fopen");
		//	exit(0);
		//}
		//if(every_pagenum == NULL)
		//{
		////	fread(page_number,sizeof(page_number),1,fp);
		////	every_pagenum = page_number;
		//	fwrite((char *)"50",strlen((char *)"50"),1,fp);
		//	printf("every_pagenum is null");
		//}
		//else
		//		fwrite(every_pagenum,strlen(every_pagenum),1,fp);
		//fclose(fp);
#endif
      tips_opt_handle.Count_init_sql(&opt_info, nowpage);
      uint32_t lastpage = tips_opt_handle.Count_exec_sql(&total_number);
			CTips_Page tips_page;
			char *pagenum = tips_page.init_pagenum(nowpage, lastpage,total_number);
			tips_page.init_urlpart_opt(&opt_info, credentials->token);
			char *tips_url = tips_page.init_url(nowpage,lastpage,sort,every_pagenum);
      tips_opt_handle.Opt_init_sql(&opt_info, nowpage,sort+1);
      char *xml = tips_opt_handle.Opt_exec_sql();
	  if(strlen(opt_info.Otime) && strlen(opt_info.Etime))
	  {
			int judge = judge_time(opt_info.Otime,opt_info.Etime);
			if(judge)
				realxml = (char *)g_strdup_printf("<audit><Ousername><![CDATA[%s]]></Ousername><msg>开始时间 不能大于 结束时间</msg><OOtime><![CDATA[%s]]></OOtime><OOetime><![CDATA[%s]]></OOetime><OOptdsc><![CDATA[%s]]></OOptdsc><tips_sort>%d</tips_sort>%s<page Count='%s'>%s%s</page></audit>",opt_info.username, opt_info.Otime, opt_info.Etime, opt_info.Optdsc,sort, xml, every_pagenum,pagenum, tips_url);
			else
				 realxml = (char *)g_strdup_printf("<audit><Ousername><![CDATA[%s]]></Ousername><OOtime><![CDATA[%s]]></OOtime><OOetime><![CDATA[%s]]></OOetime><OOptdsc><![CDATA[%s]]></OOptdsc><tips_sort>%d</tips_sort>%s<page Count='%s'>%s%s</page></audit>",opt_info.username, opt_info.Otime, opt_info.Etime, opt_info.Optdsc,sort, xml, every_pagenum,pagenum, tips_url);
				printf("judge =============%d\n",judge);
	  }
	  else
			 realxml = (char *)g_strdup_printf("<audit><isfind>1</isfind><Ousername><![CDATA[%s]]></Ousername><OOtime><![CDATA[%s]]></OOtime><OOetime><![CDATA[%s]]></OOetime><OOptdsc><![CDATA[%s]]></OOptdsc><tips_sort>%d</tips_sort>%s<page Count='%s'>%s%s</page></audit>",opt_info.username, opt_info.Otime, opt_info.Etime, opt_info.Optdsc,sort, xml, every_pagenum,pagenum, tips_url);
			tips_page.Page_free();
      g_free(xml);
      printf("realxml***************%s\n",realxml);
      char *html = ((admin_opt_fun *)opt_fun)(credentials, realxml);
      g_free(realxml);
      return html;
    }
		else if(!strcmp(cmd, "get_config")){
			                  config_manage_t tips_config;
                        memset(&tips_config, 0, sizeof(tips_config));
                       /*是否需要判断返回值? int judge =*/ get_config_manage_xml(&tips_config);
												printf("MANAGE_IP=%s\n",tips_config.manage_IP);
                        char *xml = (char *)g_strdup_printf("<get_reports_response><report type=\"config\"><MANAGE_IP><![CDATA[%s]]></MANAGE_IP><MANAGE_PORT><![CDATA[%s]]></MANAGE_PORT><GATEWAY><![CDATA[%s]]></GATEWAY><STATIC_ROUTE><![CDATA[%s]]></STATIC_ROUTE><SUPERVISOR><![CDATA[%s]]></SUPERVISOR><SERVE_ADDR><![CDATA[%s]]></SERVE_ADDR><SERVE_USER><![CDATA[%s]]></SERVE_USER><SERVE_PASSWORD><![CDATA[%s]]></SERVE_PASSWORD><ADMINSTRATION_IP><![CDATA[%s]]></ADMINSTRATION_IP><UNIT_NAME><![CDATA[%s]]></UNIT_NAME><NET_IP><![CDATA[%s]]></NET_IP><MANAGE_PORT1><![CDATA[%s]]></MANAGE_PORT1><ADMINSTRATION_IP1><![CDATA[%s]]></ADMINSTRATION_IP1></report></get_reports_response>",tips_config.manage_IP, tips_config.manage_PORT,tips_config.gateway, tips_config.static_route, tips_config.supervisor, tips_config.serve_addr,tips_config.serve_user, tips_config.serve_password, tips_config.adminstration_IP, tips_config.unit_name, tips_config.net_IP, tips_config.manage_PORT1, tips_config.adminstration_IP1);
                        printf("xml in get_config:%s\n",xml);
                        char *html = ((admin_opt_fun *)opt_fun)(credentials, xml);
                        g_free(xml);
                        return html;
		}
		else if(!strcmp(cmd, "data_display")){
	      CTips_data tips_data_handle;
        data_stut_t data_info;
		memset(data_info.name,0,sizeof(data_info.name));
		memset(data_info.time,0,sizeof(data_info.time));
		if(NULL != params_value(params, "bak_name"))
			strcpy(data_info.name,params_value(params, "bak_name"));
		if(NULL != params_value(params, "bak_time"))
			strcpy(data_info.time,params_value(params, "bak_time"));
#if 0
			uint32_t nowpage = atoll(params_value(params, "pagenum"));
      tips_opt_handle.Count_init_sql(&opt_info, nowpage);
      uint32_t lastpage = tips_opt_handle.Count_exec_sql();
			CTips_Page tips_page;
			char *pagenum = tips_page.init_pagenum(nowpage, lastpage);
			tips_page.init_urlpart_opt(&opt_info, credentials->token);
			char *tips_url = tips_page.init_url(nowpage,lastpage,sort);
      tips_opt_handle.Opt_init_sql(&opt_info, nowpage,sort+1);
      char *xml = tips_opt_handle.Opt_exec_sql();
	  if(strlen(opt_info.Otime) && strlen(opt_info.Etime))
#endif
				uint32_t nowpage = atoll(params_value(params, "pagenum"));
        tips_data_handle.Count_init_sql(&data_info, nowpage);
        uint32_t lastpage = tips_data_handle.Count_exec_sql();
				CTips_Page tips_page;
				char *pagenum = tips_page.init_pagenum(nowpage, lastpage);
				tips_page.init_urlpart_data(&data_info, credentials->token);
				char *tips_url = tips_page.init_url(nowpage,lastpage);
        tips_data_handle.Data_init_sql(&data_info, nowpage);
        char *xml = tips_data_handle.Data_exec_sql();
        printf("xml=======%s\n",xml);
        char *realxml = (char *)g_strdup_printf("<data>%s<bakname>%s</bakname><baktime>%s</baktime><page>%s%s</page></data>",xml,data_info.name,data_info.time, pagenum, tips_url);
		tips_page.Page_free();
       	g_free(xml);
       	char *html = ((admin_opt_fun *)opt_fun)(credentials, realxml);
       	g_free(realxml);
      	return html;
    }
		else if(!strcmp(cmd, "data_delete")){
#if 0
param_t *param,*selects;
selects = (param_t *)params_value(params,"select:");

printf("wanle\n");
if(selects)
{
gchar *family;
params_iterator_t iter;
param_t *param,*selects;
selects = (param_t *)params_value(params,"select:");
params_iterator_init(&iter,(GHashTable *)selects);
while(params_iterator_next(&iter,&family,&param))
printf("haofe\n");
}
printf("weishenme afejifjeifjeifjeij\n");
exit(0);
#endif

        data_stut_t data_info;
		memset(data_info.name,0,sizeof(data_info.name));
		memset(data_info.time,0,sizeof(data_info.time));
		if(NULL != params_value(params, "bak_name"))
			strcpy(data_info.name,params_value(params, "bak_name"));
		if(NULL != params_value(params, "bak_time"))
			strcpy(data_info.time,params_value(params, "bak_time"));
puts("kaishifjieflefjefjekfle");
puts(params_value(params, "bak_time"));
puts(params_value(params, "bak_name"));
puts(data_info.time);
puts(data_info.name);
			unsigned long long int oid =  atoll(params_value(params, "BackID"));
			const char *tips_table = params_value(params, "tips_table");
			printf("tips_table===%s\n",tips_table);
			char *backname = tips_sqlite_handle.One_Data_catch(oid);
			char *dataopt = g_strdup_printf("备份%s删除成功",backname);
		  g_free(backname);
			tips_sqlite_handle.catch_opt(credentials->username, dataopt);
			tips_sqlite_handle.Data_delete(oid,(char *)tips_table);
	    CTips_data tips_data_handle;
			uint32_t nowpage = atoll(params_value(params, "pagenum"));
      tips_data_handle.Count_init_sql(&data_info, nowpage);
      uint32_t lastpage = tips_data_handle.Count_exec_sql();
			CTips_Page tips_page;
			if(nowpage > lastpage)
				nowpage = lastpage;
			char *pagenum = tips_page.init_pagenum(nowpage, lastpage);
			tips_page.init_urlpart_data(&data_info, credentials->token);
			char *tips_url = tips_page.init_url(nowpage,lastpage);
     	tips_data_handle.Data_init_sql(&data_info, nowpage);
     	char *xml = tips_data_handle.Data_exec_sql();
     	printf("xml=======%s\n",xml);
        char *realxml = (char *)g_strdup_printf("<data><msg_data>%s</msg_data>%s<bakname>%s</bakname><baktime>%s</baktime><page>%s%s</page></data>",dataopt,xml,data_info.name,data_info.time,pagenum, tips_url);
			tips_page.Page_free();
		g_free(dataopt);
      g_free(xml);
      char *html = ((admin_opt_fun *)opt_fun)(credentials, realxml);
      g_free(realxml);
      return html;
    }
		else if(!strcmp(cmd, "data_rebuild")){
			unsigned long long int oid =  atoll(params_value(params, "BackID"));
			char *backname = tips_sqlite_handle.One_Data_catch(oid);
			char *dataopt = g_strdup_printf("备份%s恢复成功",backname);
		  g_free(backname);
			tips_sqlite_handle.catch_opt(credentials->username, dataopt);
			const char *tips_table = params_value(params, "tips_table");
			if(!strcmp(tips_table, "0"))
				tips_sqlite_handle.Data_rebuild(oid,(char *)tips_table);
			else if(!strcmp(tips_table, "1"))
				tips_sqlite_handle.Data_rebuild_event(oid,(char *)tips_table);
			else if(!strcmp(tips_table, "2")){
				tips_sqlite_handle.Data_rebuild_netlog(oid,(char *)tips_table);
			}
			else if(!strcmp(tips_table, "3")){
				tips_sqlite_handle.Data_rebuild_opt(oid,(char *)tips_table);
			}
        data_stut_t data_info;
		memset(data_info.name,0,sizeof(data_info.name));
		memset(data_info.time,0,sizeof(data_info.time));
		if(NULL != params_value(params, "bak_name"))
			strcpy(data_info.name,params_value(params, "bak_name"));
		if(NULL != params_value(params, "bak_time"))
			strcpy(data_info.time,params_value(params, "bak_time"));
			
	        	CTips_data tips_data_handle;
		uint32_t nowpage = atoll(params_value(params, "pagenum"));
			printf("debugingginggijigji\n");
                tips_data_handle.Count_init_sql(&data_info, nowpage);
				printf("yes it is here\n");
                uint32_t lastpage = tips_data_handle.Count_exec_sql();
		CTips_Page tips_page;
		char *pagenum = tips_page.init_pagenum(nowpage, lastpage);
		tips_page.init_urlpart_data(&data_info, credentials->token);
		char *tips_url = tips_page.init_url(nowpage,lastpage);
                	tips_data_handle.Data_init_sql(&data_info, nowpage);
                	char *xml = tips_data_handle.Data_exec_sql();
                	printf("xml=======%s\n",xml);
        char *realxml = (char *)g_strdup_printf("<data><msg_data>%s</msg_data>%s<bakname>%s</bakname><baktime>%s</baktime><page>%s%s</page></data>",dataopt,xml,data_info.name,data_info.time,pagenum, tips_url);
		tips_page.Page_free();
					g_free(dataopt);
               		g_free(xml);
                	char *html = ((admin_opt_fun *)opt_fun)(credentials, realxml);
                	g_free(realxml);
               		return html;
        	}
        	else if(!strcmp(cmd, "get_audit")){
		char page_number[20];
		char bug_pagenum[10];
		uint32_t total_number=0;
		int sort = 0;
		char* sortstrtemp = (char *)params_value(params, "tips_sort");
		char* sortis =  (char *)params_value(params, "tips_sortis");
		memset(bug_pagenum,0,sizeof(bug_pagenum));
		printf("sortstrtemp ===================================%s\n",sortstrtemp);
		printf("sortis=========================================%s\n",sortis);
		if(sortstrtemp != NULL){
			sort = atoi(sortstrtemp);
			printf("sort================================================%d\n",sort);
		}

		if(sortis != NULL){
			printf("before sort================================================%d\n",sort);
			sort = ((sort + 1) % 2);
			printf("after sort================================================%d\n",sort);
		}
			char *realxml = NULL;
		memset(page_number,0,sizeof(page_number));
	  	CTips_opt tips_opt_handle;
    	opt_stut_t opt_info;
    	opt_info.username = (char *)"";
    	opt_info.Otime = (char *)"";
      opt_info.Etime = (char *)"";
      opt_info.Optdsc = (char *)"";
		char *every_pagenum = (char *)params_value(params, "every_pagenum");
			uint32_t nowpage = 1;
#if 1 
		FILE *fp;
	if(every_pagenum == NULL || strlen(every_pagenum) == 0)
	{
		fp = fopen((char *)"/usr/local/tipschecker/etc/tipscheck/every_pagenum","r+");
		fread(bug_pagenum,sizeof(bug_pagenum),1,fp);
		/*
		//if(NULL == (fp = fopen((char *)"/usr/local/tipschecker/etc/tipscheck/every_pagenum","w+")))
		//{
		//	perror("fopen");
		//	exit(0);
		//}
		////every_pagenum = bug_pagenum;
		//printf("wo\n");
		////fread(page_number,sizeof(page_number),1,fp);
		////every_pagenum = page_number;
		//fwrite((char *)"50",strlen((char *)"50"),1,fp);
		*/
		every_pagenum = bug_pagenum;
		fclose(fp);
	}
	else
	{
		if(NULL == (fp = fopen((char *)"/usr/local/tipschecker/etc/tipscheck/every_pagenum","w+")))
		{
			perror("fopen");
			exit(0);
		}
		fwrite(every_pagenum,strlen(every_pagenum),1,fp);
		fclose(fp);
	}
		//if(NULL == (fp = fopen((char *)"/usr/local/tipschecker/etc/tipscheck/every_pagenum","w+")))
		//{
		//	perror("fopen");
		//	exit(0);
		//}
		//if(every_pagenum == NULL)
		//{
		////	fread(page_number,sizeof(page_number),1,fp);
		////	every_pagenum = page_number;
		//	fwrite((char *)"50",strlen((char *)"50"),1,fp);
		//	printf("every_pagenum is null");
		//}
		//else
		//		fwrite(every_pagenum,strlen(every_pagenum),1,fp);
		//fclose(fp);
#endif
      tips_opt_handle.Count_init_sql(&opt_info, nowpage);
      uint32_t lastpage = tips_opt_handle.Count_exec_sql(&total_number);
			CTips_Page tips_page;
			char *pagenum = tips_page.init_pagenum(nowpage, lastpage,total_number);
			tips_page.init_urlpart_opt(&opt_info, credentials->token);
			char *tips_url = tips_page.init_url(nowpage,lastpage,sort,every_pagenum);
      tips_opt_handle.Opt_init_sql(&opt_info, nowpage,sort+1);
      char *xml = tips_opt_handle.Opt_exec_sql();
	  if(strlen(opt_info.Otime) && strlen(opt_info.Etime))
	  {
			int judge = judge_time(opt_info.Otime,opt_info.Etime);
			if(judge)
				realxml = (char *)g_strdup_printf("<audit><Ousername><![CDATA[%s]]></Ousername><msg>开始时间 不能大于 结束时间</msg><OOtime><![CDATA[%s]]></OOtime><OOetime><![CDATA[%s]]></OOetime><OOptdsc><![CDATA[%s]]></OOptdsc><tips_sort>%d</tips_sort>%s<page Count='%s'>%s%s</page></audit>",opt_info.username, opt_info.Otime, opt_info.Etime, opt_info.Optdsc,sort, xml, every_pagenum,pagenum, tips_url);
			else
				 realxml = (char *)g_strdup_printf("<audit><Ousername><![CDATA[%s]]></Ousername><OOtime><![CDATA[%s]]></OOtime><OOetime><![CDATA[%s]]></OOetime><OOptdsc><![CDATA[%s]]></OOptdsc><tips_sort>%d</tips_sort>%s<page Count='%s'>%s%s</page></audit>",opt_info.username, opt_info.Otime, opt_info.Etime, opt_info.Optdsc,sort, xml, every_pagenum,pagenum, tips_url);
				printf("judge =============%d\n",judge);
	  }
	  else
			 realxml = (char *)g_strdup_printf("<audit><isfind>1</isfind><Ousername><![CDATA[%s]]></Ousername><OOtime><![CDATA[%s]]></OOtime><OOetime><![CDATA[%s]]></OOetime><OOptdsc><![CDATA[%s]]></OOptdsc><tips_sort>%d</tips_sort>%s<page Count='%s'>%s%s</page></audit>",opt_info.username, opt_info.Otime, opt_info.Etime, opt_info.Optdsc,sort, xml, every_pagenum,pagenum, tips_url);
			tips_page.Page_free();
      g_free(xml);
      printf("realxml***************%s\n",realxml);
      char *html = ((admin_opt_fun *)opt_fun)(credentials, realxml);
      g_free(realxml);
      return html;
                	//char *xml = (char *)g_strdup_printf("<audit></audit>");
               		//char *html = ((admin_opt_fun *)opt_fun)(credentials, xml);
                	//g_free(xml);
                	//return html;
        	}
		//add by mjn 2013-08-29  统计信息--攻击窃密
        	else if(!strcmp(cmd, "get_information_attack")){
                        CTips_alarm tips_alarm_handle;
			char *xml = NULL;
                        int count[5] = {0,0,0,0,0};
                        int risk_count[4] = {0,0,0,0};
                        const char *start_time = params_value(params,"strTime");
                        const char *end_time = params_value(params,"endTime");
            if((NULL == start_time)||(NULL == end_time))
				{
#if 1 
						int i = 0;
						char picture_buf[5][11];
						for(i=0;i<5;i++)
							memset(picture_buf[i],0,sizeof(picture_buf[i]));
						caculate_time(picture_buf);
						for(i=0;i<5;i++)
                            tips_alarm_handle.Count_picture_sql(count+i,picture_buf[i]);
						for(i=0;i<4;i++)
						{
							tips_alarm_handle.Count_risk_sql(risk_count+i,i+1,picture_buf[0],picture_buf[4]);
						}
                				char *lineDataStr = (char *)g_strdup_printf("['%s',%d],['%s',%d],['%s',%d],['%s',%d],['%s',%d]",picture_buf[0],count[0],picture_buf[1],count[1],picture_buf[2],count[2],picture_buf[3],count[3],picture_buf[4],count[4]);
                				char *barDataStr = (char *)g_strdup_printf("['一般级',%d],['关注级',%d],['严重级',%d],['紧急级',%d]",risk_count[0],risk_count[1],risk_count[2],risk_count[3]);
						xml = (char *)g_strdup_printf("<information_attack><StrTime>%s</StrTime><EndTime>%s</EndTime><lineDataStr>%s</lineDataStr><barDataStr>%s</barDataStr></information_attack>",picture_buf[0],picture_buf[4],lineDataStr,barDataStr);
						g_free(lineDataStr);
						g_free(barDataStr);
#endif
				}
			else
				{
					if(strlen(start_time)<10 || strlen(end_time)<10)
					{
						char *lineDataStr=(char *)"['0',0],['0',0],['0',0],['0',0],['8-24',0]";//折线图
						char *barDataStr=(char *)"['一般级',0],['关注级',0],['严重级',0],['紧急级',0]";//柱状图
						xml = (char *)g_strdup_printf("<information_attack>><picture_msg>请输入或选择开始和结束时间,格式为:XX-XX-XX</picture_msg><lineDataStr>%s</lineDataStr><barDataStr>%s</barDataStr></information_attack>",lineDataStr,barDataStr);
					printf("**************************************************************************************************I am nothing do you know !\n");
					}	
					else
					{
						int judge = judge_time(start_time,end_time);
						if(judge)
						{
							char *lineDataStr=(char *)"['0',0],['0',0],['0',0],['0',0],['8-24',0]";//折线图
							char *barDataStr=(char *)"['一般级',0],['关注级',0],['严重级',0],['紧急级',0]";//柱状图
							xml = (char *)g_strdup_printf("<information_attack>><picture_msg>开始时间不能大于结束时间</picture_msg><StrTime>%s</StrTime><EndTime>%s</EndTime><lineDataStr>%s</lineDataStr><barDataStr>%s</barDataStr></information_attack>",start_time,end_time,lineDataStr,barDataStr);
						}
						else
						{
							char stime[30],etime[30],buf[50];
							char picture_buf[5][11];
							char timeip[5][20];
							int i = 0;
							int cmp[6]={0,0,0,0,0,0};
							memset(stime,0,sizeof(stime));
							memset(etime,0,sizeof(etime));
							memset(buf,0,sizeof(buf));
							for(i=0;i<5;i++)
								memset(picture_buf[i],0,sizeof(picture_buf[i]));
							sscanf(start_time,"%d-%d-%d%s",cmp,cmp+1,cmp+2,buf);
							sscanf(end_time,"%d-%d-%d%s",cmp+3,cmp+4,cmp+5,buf);
							memset(buf,0,sizeof(buf));
							sprintf(stime,"%d-%02d-%02d-00-00-00",cmp[0],cmp[1],cmp[2]);
							sprintf(etime,"%d-%02d-%02d-00-00-00",cmp[3],cmp[4],cmp[5]);
														
							time_t Stime = MyObtainTime(stime);
							time_t Etime = MyObtainTime(etime);
							time_t space = (Etime - Stime) / 4;
							for (i = 0; i < 5; i ++)
							{
							        memset(timeip, 0, sizeof (timeip[i]));
							        MyTurnTime(Stime + space * i, timeip[i], 20);
							        strncpy(picture_buf[i],timeip[i],10);
								printf("\n");
								puts(picture_buf[i]);
                        	     				tips_alarm_handle.Count_picture_sql(count+i,picture_buf[i]);
								printf("\n");
							        fprintf(stdout, "time[%d] = %s\n", i, timeip[i]);
							}

							for(i=0;i<4;i++)
							{
								tips_alarm_handle.Count_risk_sql(risk_count+i,i+1,picture_buf[0],picture_buf[4]);
							}
                					char *lineDataStr = (char *)g_strdup_printf("['%s',%d],['%s',%d],['%s',%d],['%s',%d],['%s',%d]",picture_buf[0],count[0],picture_buf[1],count[1],picture_buf[2],count[2],picture_buf[3],count[3],picture_buf[4],count[4]);
                					char *barDataStr = (char *)g_strdup_printf("['一般级',%d],['关注级',%d],['严重级',%d],['紧急级',%d]",risk_count[0],risk_count[1],risk_count[2],risk_count[3]);
							xml = (char *)g_strdup_printf("<information_attack><StrTime>%s</StrTime><EndTime>%s</EndTime><lineDataStr>%s</lineDataStr><barDataStr>%s</barDataStr></information_attack>",start_time,end_time,lineDataStr,barDataStr);
							g_free(lineDataStr);
							g_free(barDataStr);
						}
					 }
				}
               		char *html = ((admin_opt_fun *)opt_fun)(credentials, xml);
			printf("=========00%s\n",html);
                	g_free(xml);
			printf("=========01%s\n",html);
                        g_free((void *)start_time);
                        g_free((void *)end_time);
                	return html;
        	}
		//统计信息--涉密标识
        	else if(!strcmp(cmd, "get_information_secret")){
	  		CTips_Event tips_Event_handle;
			char *xml = NULL;
                        int count[5] = {0,0,0,0,0};
                        int risk_count[2] = {0,0};
                        const char *start_time = params_value(params,"strTime");
                        const char *end_time = params_value(params,"endTime");
                        if((NULL == start_time)||(NULL == end_time))
								{
									int i = 0;
									char picture_buf[5][11];
									for(i=0;i<5;i++)
										memset(picture_buf[i],0,sizeof(picture_buf[i]));
									caculate_time(picture_buf);
									for(i=0;i<5;i++)
										tips_Event_handle.Count_picture_sql(count+i,picture_buf[i]);
                                    for(i=0;i<2;i++)
                                     {
                                        tips_Event_handle.Count_risk_sql(risk_count+i,1002+i*9,picture_buf[0],picture_buf[4]);
                                     }
                                    char *lineDataStr = (char *)g_strdup_printf("['%s',%d],['%s',%d],['%s',%d],['%s',%d],['%s',%d]",picture_buf[0],count[0],picture_buf[1],count[1],picture_buf[2],count[2],picture_buf[3],count[3],picture_buf[4],count[4]);
                                    char *barDataStr = (char *)g_strdup_printf("['发现涉嫌违规传输含有恶意程序文件',%d],['',0],['',0],['发现涉嫌违规传输泄密信息',%d]",risk_count[0],risk_count[1]);
                                    xml = (char *)g_strdup_printf("<information_secret><StrTime>%s</StrTime><EndTime>%s</EndTime><lineDataStr>%s</lineDataStr><barDataStr>%s</barDataStr></information_secret>",picture_buf[0],picture_buf[4],lineDataStr,barDataStr);
                                    g_free(lineDataStr);
                                    g_free(barDataStr);
									printf("**************************************************************************************************I am nothing do you know !\n");
                                 }
                         else
                                 {
                                         if(strlen(start_time)<10 || strlen(end_time)<10)
                                         {
    					     char *lineDataStr=(char *)"['0',0],['0',0],['0',0],['0',0],['0',0]";//折线图
    				       	     char *barDataStr=(char *)"['发现涉嫌违规传输泄密信息',0],['',0],['',0],['发现涉嫌违规传输含有恶意程序文件',0]";//柱状图
                                                 xml = (char *)g_strdup_printf("<information_secret>><picture_msg>请输入或选择开始和结束时间,格式为:XX-XX-XX</picture_msg><lineDataStr>%s</lineDataStr><barDataStr>%s</barDataStr></information_secret>",lineDataStr,barDataStr);
                                         printf("**************************************************************************************************I am nothing do you know !\n");
                                         }
                                         else
                                         {
											int judge = judge_time(start_time,end_time);
											if(judge)
											{
												char *lineDataStr=(char *)"['0',0],['0',0],['0',0],['0',0],['0',0]";//折线图
												char *barDataStr=(char *)"['发现涉嫌违规传输泄密信息',0],['',0],['',0],['发现涉嫌违规传输含有恶意程序文件',0]";//柱状图
                                                xml = (char *)g_strdup_printf("<information_secret>><picture_msg>开始时间不能够大于结束时间</picture_msg><StrTime>%s</StrTime><EndTime>%s</EndTime><lineDataStr>%s</lineDataStr><barDataStr>%s</barDataStr></information_secret>",start_time,end_time,lineDataStr,barDataStr);
											}
											else
											{
                                                 char stime[30],etime[30],buf[50];
                                                 char picture_buf[5][11];
                                                 char timeip[5][20];
                                                 int i = 0;
                                                 int cmp[6]={0,0,0,0,0,0};
                                                 memset(stime,0,sizeof(stime));
                                                 memset(etime,0,sizeof(etime));
                                                 memset(buf,0,sizeof(buf));
                                                 for(i=0;i<5;i++)
                                                         memset(picture_buf[i],0,sizeof(picture_buf[i]));
                                                 sscanf(start_time,"%d-%d-%d%s",cmp,cmp+1,cmp+2,buf);
                                                 sscanf(end_time,"%d-%d-%d%s",cmp+3,cmp+4,cmp+5,buf);
                                                 memset(buf,0,sizeof(buf));
                                                 sprintf(stime,"%d-%02d-%02d-00-00-00",cmp[0],cmp[1],cmp[2]);
                                                 sprintf(etime,"%d-%02d-%02d-00-00-00",cmp[3],cmp[4],cmp[5]);
 
                                                 time_t Stime = MyObtainTime(stime);
                                                 time_t Etime = MyObtainTime(etime);
                                                 time_t space = (Etime - Stime) / 4;
                                                 for (i = 0; i < 5; i ++)
                                                 {
                                                         memset(timeip, 0, sizeof (timeip[i]));
                                                         MyTurnTime(Stime + space * i, timeip[i], 20);
                                                         strncpy(picture_buf[i],timeip[i],10);
                                                         printf("\n");
                                                         puts(picture_buf[i]);
                                                         tips_Event_handle.Count_picture_sql(count+i,picture_buf[i]);
                                                         printf("\n");
                                                         fprintf(stdout, "time[%d] = %s\n", i, timeip[i]);
                                                 }
 
                                                 for(i=0;i<2;i++)
                                                 {
                                                         tips_Event_handle.Count_risk_sql(risk_count+i,1002+i*9,picture_buf[0],picture_buf[4]);
                                                 }
                                                 char *lineDataStr = (char *)g_strdup_printf("['%s',%d],['%s',%d],['%s',%d],['%s',%d],['%s',%d]",picture_buf[0],count[0],picture_buf[1],count[1],picture_buf[2],count[2],picture_buf[3],count[3],picture_buf[4],count[4]);
                                                 char *barDataStr = (char *)g_strdup_printf("['发现涉嫌违规传输含有恶意程序文件',%d],['',0],['',0],['发现涉嫌违规传输泄密信息',%d]",risk_count[0],risk_count[1]);
                                                 xml = (char *)g_strdup_printf("<information_secret><StrTime>%s</StrTime><EndTime>%s</EndTime><lineDataStr>%s</lineDataStr><barDataStr>%s</barDataStr></information_secret>",start_time,end_time,lineDataStr,barDataStr);
                                                 g_free(lineDataStr);
                                                 g_free(barDataStr);
											}
                                          }
                                 }
                         char *html = ((admin_opt_fun *)opt_fun)(credentials, xml);
                         printf("=========00%s\n",html);
                         g_free(xml);
                         printf("=========01%s\n",html);
                         g_free((void *)start_time);
                         g_free((void *)end_time);
                         return html;
        	}

		//统计信息--行为审计
        	else if(!strcmp(cmd, "get_information_action")){
	 		 CTips_netlog tips_netlog_handle;
                         char *xml = NULL;
                         int count[5] = {0,0,0,0,0};
                         const char *start_time = params_value(params,"strTime");
                         const char *end_time = params_value(params,"endTime");
                         if((NULL == start_time)||(NULL == end_time))
                                 {
									int i = 0;
									char picture_buf[5][11];
									for(i=0;i<5;i++)
										memset(picture_buf[i],0,sizeof(picture_buf[i]));
									caculate_time(picture_buf);
									for(i=0;i<5;i++)
										tips_netlog_handle.Count_picture_sql(count+i,picture_buf[i]);
                                    char *lineDataStr = (char *)g_strdup_printf("['%s',%d],['%s',%d],['%s',%d],['%s',%d],['%s',%d]",picture_buf[0],count[0],picture_buf[1],count[1],picture_buf[2],count[2],picture_buf[3],count[3],picture_buf[4],count[4]);
                                    char *barDataStr = (char *)g_strdup_printf("['',0],['',0],['',0],['',0]");
                                    xml = (char *)g_strdup_printf("<information_action><StrTime>%s</StrTime><EndTime>%s</EndTime><lineDataStr>%s</lineDataStr><barDataStr>%s</barDataStr></information_action>",picture_buf[0],picture_buf[4],lineDataStr,barDataStr);
                                    g_free(lineDataStr);
                                    g_free(barDataStr);
                                 printf("**************************************************************************************************I am nothing do you know !\n");
                                  }
                          else
                                  {
                                          if(strlen(start_time)<10 || strlen(end_time)<10)
                                          {
                                              char *lineDataStr=(char *)"['0',0],['0',0],['0',0],['0',0],['0',0]";//折线图
                                              char *barDataStr=(char *)"['',0],['',0],['',0],['',0]";//柱状图
                                                  xml = (char *)g_strdup_printf("<information_action>><picture_msg>请输入或选择开始和结束时间,格式为:XX-XX-XX</picture_msg><lineDataStr>%s</lineDataStr><barDataStr>%s</barDataStr></information_action>",lineDataStr,barDataStr);
                                          printf("**************************************************************************************************I am nothing do you know !\n");
                                          }
                                          else
                                          {
											int judge = judge_time(start_time,end_time);
											if(judge)
											{
                                              char *lineDataStr=(char *)"['0',0],['0',0],['0',0],['0',0],['0',0]";//折线图
                                              char *barDataStr=(char *)"['',0],['',0],['',0],['',0]";//柱状图
                                              xml = (char *)g_strdup_printf("<information_action>><picture_msg>开始时间不能够大于结束时间</picture_msg><StrTime>%s</StrTime><EndTime>%s</EndTime><lineDataStr>%s</lineDataStr><barDataStr>%s</barDataStr></information_action>",start_time,end_time,lineDataStr,barDataStr);
											}
											else
											{
                                                  char stime[30],etime[30],buf[50];
                                                  char picture_buf[5][11];
                                                  char timeip[5][20];
                                                  int i = 0;
                                                  int cmp[6]={0,0,0,0,0,0};
                                                  memset(stime,0,sizeof(stime));
                                                  memset(etime,0,sizeof(etime));
                                                  memset(buf,0,sizeof(buf));
                                                  for(i=0;i<5;i++)
                                                          memset(picture_buf[i],0,sizeof(picture_buf[i]));
                                                  sscanf(start_time,"%d-%d-%d%s",cmp,cmp+1,cmp+2,buf);
                                                  sscanf(end_time,"%d-%d-%d%s",cmp+3,cmp+4,cmp+5,buf);
                                                  memset(buf,0,sizeof(buf));
                                                  sprintf(stime,"%d-%02d-%02d-00-00-00",cmp[0],cmp[1],cmp[2]);
                                                  sprintf(etime,"%d-%02d-%02d-00-00-00",cmp[3],cmp[4],cmp[5]);
 
                                                  time_t Stime = MyObtainTime(stime);
                                                  time_t Etime = MyObtainTime(etime);
                                                  time_t space = (Etime - Stime) / 4;
                                                  for (i = 0; i < 5; i ++)
                                                  {
                                                          memset(timeip, 0, sizeof (timeip[i]));
                                                          MyTurnTime(Stime + space * i, timeip[i], 20);
                                                          strncpy(picture_buf[i],timeip[i],10);
                                                          printf("\n");
                                                          puts(picture_buf[i]);
                                                          tips_netlog_handle.Count_picture_sql(count+i,picture_buf[i]);
                                                          printf("\n");
                                                          fprintf(stdout, "time[%d] = %s\n", i, timeip[i]);
                                                  }
 
                                                  char *lineDataStr = (char *)g_strdup_printf("['%s',%d],['%s',%d],['%s',%d],['%s',%d],['%s',%d]",picture_buf[0],count[0],picture_buf[1],count[1],picture_buf[2],count[2],picture_buf[3],count[3],picture_buf[4],count[4]);
                                                  char *barDataStr = (char *)g_strdup_printf("['',0],['',0],['',0],['',0]");
                                                  xml = (char *)g_strdup_printf("<information_action><StrTime>%s</StrTime><EndTime>%s</EndTime><lineDataStr>%s</lineDataStr><barDataStr>%s</barDataStr></information_action>",start_time,end_time,lineDataStr,barDataStr);
                                                  g_free(lineDataStr);
                                                  g_free(barDataStr);
											}
                                           }
                                  }
                          char *html = ((admin_opt_fun *)opt_fun)(credentials, xml);
                          printf("=========00%s\n",html);
                          g_free(xml);
                          printf("=========01%s\n",html);
                          g_free((void *)start_time);
                          g_free((void *)end_time);
                          return html;

        	}
                  //统计信息--操作审计
                  else if(!strcmp(cmd, "get_information_operation")){
                          CTips_opt tips_opt_handle;
                          char *xml = NULL;
                          int count[5] = {0,0,0,0,0};
                          const char *start_time = params_value(params,"strTime");
                          const char *end_time = params_value(params,"endTime");
                          if((NULL == start_time)||(NULL == end_time))
                                  {
									int i = 0;
									char picture_buf[5][11];
									for(i=0;i<5;i++)
										memset(picture_buf[i],0,sizeof(picture_buf[i]));
									caculate_time(picture_buf);
									for(i=0;i<5;i++)
										tips_opt_handle.Count_picture_sql(count+i,picture_buf[i]);
                                    char *lineDataStr = (char *)g_strdup_printf("['%s',%d],['%s',%d],['%s',%d],['%s',%d],['%s',%d]",picture_buf[0],count[0],picture_buf[1],count[1],picture_buf[2],count[2],picture_buf[3],count[3],picture_buf[4],count[4]);
                                    char *barDataStr = (char *)g_strdup_printf("['',0],['',0],['',0],['',0]");
                                    xml = (char *)g_strdup_printf("<information_operation><StrTime>%s</StrTime><EndTime>%s</EndTime><lineDataStr>%s</lineDataStr><barDataStr>%s</barDataStr></information_operation>",picture_buf[0],picture_buf[4],lineDataStr,barDataStr);
                                    g_free(lineDataStr);
                                    g_free(barDataStr);
                                  printf("**************************************************************************************************I am nothing do you know !\n");
                                   }
                           else
                                   {
                                           if(strlen(start_time)<10 || strlen(end_time)<10)
                                           {
                                               char *lineDataStr=(char *)"['0',0],['0',0],['0',0],['0',0],['0',0]";//折线图
                                               char *barDataStr=(char *)"['',0],['',0],['',0],['',0]";//柱状图
                                               xml = (char *)g_strdup_printf("<information_operation>><picture_msg>请输入或选择开始和结束时间,格式为:XX-XX-XX</picture_msg><lineDataStr>%s</lineDataStr><barDataStr>%s</barDataStr></information_operation>",lineDataStr,barDataStr);
                                           printf("**************************************************************************************************I am nothing do you know !\n");
                                           }
                                           else
                                           {
											int judge = judge_time(start_time,end_time);
											if(judge)
											{
                                               char *lineDataStr=(char *)"['0',0],['0',0],['0',0],['0',0],['0',0]";//折线图
                                               char *barDataStr=(char *)"['',0],['',0],['',0],['',0]";//柱状图
                                                   xml = (char *)g_strdup_printf("<information_operation>><picture_msg>开始时间不能够大于结束时间</picture_msg><StrTime>%s</StrTime><EndTime>%s</EndTime><lineDataStr>%s</lineDataStr><barDataStr>%s</barDataStr></information_operation>",start_time,end_time,lineDataStr,barDataStr);
											}
											else
											{
                                                   char stime[30],etime[30],buf[50];
                                                   char picture_buf[5][11];
                                                   char timeip[5][20];
                                                   int i = 0;
                                                   int cmp[6]={0,0,0,0,0,0};
                                                   memset(stime,0,sizeof(stime));
                                                   memset(etime,0,sizeof(etime));
                                                   memset(buf,0,sizeof(buf));
                                                   for(i=0;i<5;i++)
                                                           memset(picture_buf[i],0,sizeof(picture_buf[i]));
                                                   sscanf(start_time,"%d-%d-%d%s",cmp,cmp+1,cmp+2,buf);
                                                   sscanf(end_time,"%d-%d-%d%s",cmp+3,cmp+4,cmp+5,buf);
                                                   memset(buf,0,sizeof(buf));
                                                   sprintf(stime,"%d-%02d-%02d-00-00-00",cmp[0],cmp[1],cmp[2]);
                                                   sprintf(etime,"%d-%02d-%02d-00-00-00",cmp[3],cmp[4],cmp[5]);
 
                                                   time_t Stime = MyObtainTime(stime);
                                                   time_t Etime = MyObtainTime(etime);
                                                   time_t space = (Etime - Stime) / 4;
                                                   for (i = 0; i < 5; i ++)
                                                   {
                                                           memset(timeip, 0, sizeof (timeip[i]));
                                                           MyTurnTime(Stime + space * i, timeip[i], 20);
                                                           strncpy(picture_buf[i],timeip[i],10);
                                                           printf("\n");
                                                           puts(picture_buf[i]);
                                                           tips_opt_handle.Count_picture_sql(count+i,picture_buf[i]);
                                                           printf("\n");
                                                           fprintf(stdout, "time[%d] = %s\n", i, timeip[i]);
                                                   }
 
                                                   char *lineDataStr = (char *)g_strdup_printf("['%s',%d],['%s',%d],['%s',%d],['%s',%d],['%s',%d]",picture_buf[0],count[0],picture_buf[1],count[1],picture_buf[2],count[2],picture_buf[3],count[3],picture_buf[4],count[4]);
                                                   char *barDataStr = (char *)g_strdup_printf("['',0],['',0],['',0],['',0]");
                                                   xml = (char *)g_strdup_printf("<information_operation><StrTime>%s</StrTime><EndTime>%s</EndTime><lineDataStr>%s</lineDataStr><barDataStr>%s</barDataStr></information_operation>",start_time,end_time,lineDataStr,barDataStr);
                                                   g_free(lineDataStr);
                                                   g_free(barDataStr);
											}
                                            }
                                   }
                           char *html = ((admin_opt_fun *)opt_fun)(credentials, xml);
                           printf("=========00%s\n",html);
                           g_free(xml);
                           printf("=========01%s\n",html);
                           g_free((void *)start_time);
                           g_free((void *)end_time);
                           return html;
 
                 }
		//产品维护-设备状态
        	else if(!strcmp(cmd, "get_product")){
			char* devicestatue[4] = {(char *)"功能未定义", (char *)"功能运行正常", (char *)"功能运行不正常", (char *)"功能未运行"};
			char* devicemirror[2] = {(char *)"未连接", (char *)"已连接"};
			system_config_t tips_system_config;
			status_struct_t devicesta;
			memset(&tips_system_config,0,sizeof(tips_system_config));
			memset(&devicesta,0,sizeof(devicesta));
                	
#if 0
typedef struct status_struct{
char device_id[12];
char stime[21];
int mem;
int cpu;
double disk;
char mirror;//镜像口状态1是开启，0是关闭
int flow;//镜像口流量
int active_peer;
char module_state[9];
}status_struct_t;//设备状态信息

typedef struct system_config
{
	char cpu[50];	char mem[50];	char disk[50];	char runtime[50];	char alarmtime[50];	int modify_flag;
}system_config_t;
#endif
			get_system_config_xml(&tips_system_config);
			config_get_device(&devicesta);

			int cpucolor  = (atoi(tips_system_config.cpu) < devicesta.cpu  ? 1 : 0);
			int memcolor  = (atoi(tips_system_config.mem) < devicesta.mem  ? 1 : 0);
			int diskcolor = (atoi(tips_system_config.disk) <  devicesta.disk ? 1 : 0);
                        printf("111111111111111111xml in get_config:");
	                char *xml = (char *)g_strdup_printf("<product><cpucolor>%d</cpucolor><memcolor>%d</memcolor><diskcolor>%d</diskcolor><dstart>%s</dstart><dcpu>%d%%</dcpu><dmem>%d%%</dmem><ddisk>%0.2lfG</ddisk><dmirror>%s</dmirror><dflow>%dkbps</dflow><dattach>%s</dattach><dnetlog>%s</dnetlog><dsen>%s</dsen><dblock>%s</dblock></product>",cpucolor, memcolor, diskcolor,devicesta.stime,devicesta.cpu, devicesta.mem,devicesta.disk, devicemirror[(int)(devicesta.mirror)-48], devicesta.flow, devicestatue[(int)(devicesta.module_state[0])-48], devicestatue[(int)(devicesta.module_state[1])-48],devicestatue[(int)(devicesta.module_state[2])-48],devicestatue[(int)(devicesta.module_state[3])-48]);
 
//	                char *xml = (char *)g_strdup_printf("<product></product>");
                        printf("xml in get_config:%s\n",xml);
                        char *html = ((admin_opt_fun *)opt_fun)(credentials, xml);
                        g_free(xml);
                        return html;
        	}

		//产品维护-系统设置
	else if(!strcmp(cmd,"chenliting"))//软件升级
	{
		char *xml = (char *)g_strdup_printf("<sysSoftwareUpdate><list><product_name></product_name><version></version><describe></describe><upgrade_time></upgrade_time></list></sysSoftwareUpdate>");
               	char *html = ((admin_opt_fun *)opt_fun)(credentials, xml);			
                g_free(xml);
                return html;
	}
	else if(!strcmp(cmd,"chenliting_check"))//软件升级_检测
	{
		product_INFO upgrade_info;	
		char *xml=NULL;
                const char *upgrade_style = params_value(params,"upgrade");
		//printf("upgrade_style==%s\n",upgrade_style);
		if(upgrade_style != NULL && !strcmp(upgrade_style,"检测"))//检测模块
		{
			//int Judge_Number = 0;
			int Judge_Number=Upgrade(&upgrade_info);
			if(Judge_Number == 0)
				xml = (char *)g_strdup_printf("<sysSoftwareUpdate><list><product_name>%s</product_name><version>%s</version><describe>%s</describe><upgrade_time>%s</upgrade_time></list></sysSoftwareUpdate>",upgrade_info.name,upgrade_info.version,upgrade_info.describe,upgrade_info.upgrade_time);
			else if(Judge_Number == -2)
				xml = (char *)g_strdup_printf("<sysSoftwareUpdate><list></list><msg>当前版本即为最新版本</msg></sysSoftwareUpdate>");
			else
				xml = (char *)g_strdup_printf("<sysSoftwareUpdate><list></list><msg>连接服务器失败,检测未成功</msg></sysSoftwareUpdate>");
		}
		else//升级模块
		{
			int Judge_Number=Upgrade(&upgrade_info,1);
			const char *version = params_value(params,"version");
			if(Judge_Number == -1)
				xml = (char *)g_strdup_printf("<sysSoftwareUpdate><list></list><msg>传输文件出错,请重新检测,升级</msg></sysSoftwareUpdate>");
			else
			{
				//system("./upgrade.sh &");
				xml = (char *)g_strdup_printf("<sysSoftwareUpdate><list></list><msg>升级到最新版本%s</msg></sysSoftwareUpdate>",version);
			}
		}
               	char *html = ((admin_opt_fun *)opt_fun)(credentials, xml);			
                g_free(xml);
                return html;
	}
	else if(!strcmp(cmd,"strategyupdate"))//策略升级
	{
		char *xml = (char *)g_strdup_printf("<sysStrategyUpdate></sysStrategyUpdate>");
               	char *html = ((admin_opt_fun *)opt_fun)(credentials, xml);			
                g_free(xml);
                return html;
	}
	else if(!strcmp(cmd,"basicinfo"))//基本信息
	{
#if 0 
		unsigned char bufffff[6];
		unsigned char bufffff1[6];
		char buuu[100];
		char buuu1[100];
		memset(bufffff,0,sizeof(bufffff));
		memset(bufffff1,0,sizeof(bufffff1));
		memset(buuu1,0,100);
		memset(buuu1,0,100);
		get_local_mac((char *)"eth0",bufffff);
		get_local_mac((char *)"eth1",bufffff1);
		sprintf(buuu,"%02X:%02X:%02X:%02X:%02X:%02X",bufffff[0],bufffff[1],bufffff[2],bufffff[3],bufffff[4],bufffff[5]);
		sprintf(buuu1,"%02X:%02X:%02X:%02X:%02X:%02X",bufffff1[0],bufffff1[1],bufffff1[2],bufffff1[3],bufffff1[4],bufffff1[5]);
		printf("buuu==%s\n",buuu);
		printf("buuu1==%s\n",buuu1);
		exit(0);
		//printf("bufffff====%s\n",bufffff);
		sleep(5);
#endif
		config_manage_t sconfig;
		int int_mirror_state = 0,int_mirror_state1 = 0;
		int main_center_state = 0;
		int deputy_center_state = 0;
		int mirror1_mode=0,mirror2_mode=0;
		double mirror1_flow = 0,mirror2_flow = 0;
		int mirror1_active_peer = 0,mirror2_active_peer = 0,mirror1_session = 0,mirror2_session = 0;
		char manage_state[50];
		char mirror_state[50];
		char mirror_state1[50];
		char judge_state[10];
		char main_center[30];
		char deputy_center[30];
		char mirror2_mode_buf[30];
		unsigned char mac_c[6];
		unsigned char mac_c_net[6];
		char mac[40],net_mac[40];
		memset(mirror2_mode_buf,0,sizeof(mirror2_mode_buf));
		memset(main_center,0,sizeof(main_center));
		memset(deputy_center,0,sizeof(deputy_center));
		memset(mac,0,sizeof(mac));
		memset(net_mac,0,sizeof(net_mac));
		memset(sconfig.manage_IP,0,sizeof(sconfig.manage_IP));
		memset(&sconfig,0,sizeof(config_manage_t));
		memset(manage_state,0,sizeof(manage_state));
		memset(mirror_state,0,sizeof(mirror_state));
		memset(mirror_state1,0,sizeof(mirror_state1));
		memset(judge_state,0,sizeof(judge_state));
        	/*是否需要判断返回值?int format =*/ get_config_manage_xml(&sconfig); 
		//FILE *fp = popen ("cat /sys/class/net/eth1/carrier", "r");
        	//FILE *fp = fopen("/sys/class/net/eth1/carrier", "r+");
        	#if 0
		fgets(judge_state,10,fp);
		judge_state[strlen(judge_state)-1] = '\0';
		if(!strcmp(judge_state,"0"))
			strcpy(manage_state,"网线未连接");
		else
			strcpy(manage_state,"连接正常");

		memset(judge_state,0,sizeof(judge_state));
		#endif
#if 0
		FILE *fp = popen ("cat /sys/class/net/eth2/carrier", "r");//镜像口1的连接状态
		fgets(judge_state,10,fp);
		judge_state[strlen(judge_state)-1] = '\0';
		if(!strcmp(judge_state,"0"))
			strcpy(mirror_state,"网线未连接");
		else
			strcpy(mirror_state,"连接正常");

		memset(judge_state,0,sizeof(judge_state));
		fp = popen ("cat /sys/class/net/eth3/carrier", "r");//镜像口2的连接状态
		fgets(judge_state,10,fp);
		judge_state[strlen(judge_state)-1] = '\0';
		if(!strcmp(judge_state,"0"))
			strcpy(mirror_state1,"网线未连接");
		else
			strcpy(mirror_state1,"连接正常");
		pclose(fp);
#endif
		//printf("judge_state == %s\n",judge_state);
		//exit(0);
#if 0 
		printf("sconfig.manage_IP ===%s\n",sconfig.manage_IP);
		printf("sconfig.manage_PORT ===%s\n",sconfig.manage_PORT);
		printf("sconfig.manage_PORT1 ===%s\n",sconfig.manage_PORT1);
		printf("sconfig.net_IP ===%s\n",sconfig.net_IP);
		printf("sconfig.gateway ===%s\n",sconfig.gateway);
		printf("sconfig.static_route ===%s\n",sconfig.static_route);
		printf("sconfig.supervisor ===%s\n",sconfig.supervisor);
		printf("sconfig.serve_addr ===%s\n",sconfig.serve_addr);
		printf("sconfig.serve_user ===%s\n",sconfig.serve_user);
		printf("sconfig.serve_password ===%s\n",sconfig.serve_password);
		printf("sconfig.adminstration_IP ===%s\n",sconfig.adminstration_IP);
		printf("sconfig.adminstration_IP1 ===%s\n",sconfig.adminstration_IP1);
		printf("sconfig.unit_name ===%s\n",sconfig.unit_name);
		//exit(0);
#endif
		get_local_mac((char *)"eth0",mac_c);
		get_local_mac((char *)"eth1",mac_c_net);
		sprintf(mac,"%02X:%02X:%02X:%02X:%02X:%02X",mac_c[0],mac_c[1],mac_c[2],mac_c[3],mac_c[4],mac_c[5]);
		sprintf(net_mac,"%02X:%02X:%02X:%02X:%02X:%02X",mac_c_net[0],mac_c_net[1],mac_c_net[2],mac_c_net[3],mac_c_net[4],mac_c_net[5]);
		obj.Get_status_shm(6,&int_mirror_state,sizeof(int));//镜像口1的连接状态
		if(int_mirror_state == 1)
			strcpy(mirror_state,"连接正常");
		else
			strcpy(mirror_state,"未连接");
		obj.Get_status_shm(10,&int_mirror_state1,sizeof(int));//镜像口2的连接状态
		if(int_mirror_state1 == 1)
			strcpy(mirror_state1,"连接正常");
		else
			strcpy(mirror_state1,"未连接");
		obj.Get_status_shm(5,&mirror1_mode,sizeof(int));//镜像口的状态
		obj.Get_status_shm(7,&mirror1_flow,sizeof(double));
		obj.Get_status_shm(8,&mirror1_active_peer,sizeof(int));
		obj.Get_status_shm(9,&mirror1_session,sizeof(int));
		obj.Get_status_shm(5,&mirror2_mode,sizeof(int));
		if(mirror2_mode == 3)
			strcpy(mirror2_mode_buf,"独立模式");
		else if(mirror2_mode == 2)
			strcpy(mirror2_mode_buf,"保护模式");
		else
			strcpy(mirror2_mode_buf,"不工作");
		obj.Get_status_shm(11,&mirror2_flow,sizeof(double));
		obj.Get_status_shm(12,&mirror2_active_peer,sizeof(int));
		obj.Get_status_shm(13,&mirror2_session,sizeof(int));
		obj.Get_status_shm(17,&main_center_state,sizeof(int));//主中心连接状态
		if(main_center_state == 1)
			strcpy(main_center,"连接正常");
		else if(main_center_state == 2)
			strcpy(main_center,"连接不正常");
		else if(main_center_state == 3)
			strcpy(main_center,"网关不可达");
		else
			strcpy(main_center,"网线接入异常");
		obj.Get_status_shm(18,&deputy_center_state,sizeof(int));//副中心连接状态
		if(deputy_center_state == 1)
			strcpy(deputy_center,"连接正常");
		else if(deputy_center_state == 2)
			strcpy(deputy_center,"连接不正常");
		else if(deputy_center_state == 3)
			strcpy(deputy_center,"网关不可达");
		else
			strcpy(deputy_center,"网线接入异常");
		printf("wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww111111111\n");
		char *xml = (char *)g_strdup_printf("<sysBasicInfo><MANAGE_IP>%s</MANAGE_IP><MANAGE_IP_MAC>%s</MANAGE_IP_MAC><NET_MAC>%s</NET_MAC><SUPERVISOR>%s</SUPERVISOR><NET_IP>%s</NET_IP><ADMINSTRATION_IP>%s</ADMINSTRATION_IP><ADMINSTRATION_IP1>%s</ADMINSTRATION_IP1><ADMINSTRATION_SATE>%s</ADMINSTRATION_SATE><ADMINSTRATION_SATE1>%s</ADMINSTRATION_SATE1><MIRROR_STATE>%s</MIRROR_STATE><MIRROR_STATE1>%s</MIRROR_STATE1><MIRRROR1_MODE>%d</MIRRROR1_MODE><MIRRROR2_MODE>%s</MIRRROR2_MODE><MIRROR1_FLOW>%0.2f</MIRROR1_FLOW><MIRROR2_FLOW>%0.2f</MIRROR2_FLOW><MIRROR1_ACTIVE_PEER>%d</MIRROR1_ACTIVE_PEER><MIRROR2_ACTIVE_PEER>%d</MIRROR2_ACTIVE_PEER><MIRROR1_SESSION>%d</MIRROR1_SESSION><MIRROR2_SESSION>%d</MIRROR2_SESSION></sysBasicInfo>",sconfig.manage_IP,mac,net_mac,sconfig.supervisor,sconfig.net_IP,sconfig.adminstration_IP,sconfig.adminstration_IP1,main_center,deputy_center,mirror_state,mirror_state1,mirror1_mode,mirror2_mode_buf,mirror1_flow,mirror2_flow,mirror1_active_peer,mirror2_active_peer,mirror1_session,mirror2_session);
		printf("wwwwwwwwwwwwwwwwwwww222222222222222222222222222wwwwwwwwwwwwwwww111111111\n");
               	char *html = ((admin_opt_fun *)opt_fun)(credentials, xml);			
                g_free(xml);
                return html;
	}
	else if(!strcmp(cmd,"minitorsite"))//监控网络
	{
		//config_manage_t sconfig;
		struct sockaddr_in get_tips_address;
		char mac[40],buffer[100],mask[50];
       		char gateway[50];
		char eth0_ip[50];
		memset(eth0_ip,0,sizeof(eth0_ip));
       		memset(gateway,0,sizeof(gateway));
		memset(mac,0,sizeof(mac));
		memset(buffer,0,sizeof(buffer));
		memset(mask,0,sizeof(mask));
		//memset(sconfig.manage_IP,0,sizeof(sconfig.manage_IP));
		//memset(&sconfig,0,sizeof(config_manage_t));
        	get_tips_address.sin_addr.s_addr = get_local_ip((char *)"eth0");
		inet_ntop(AF_INET, &(get_tips_address.sin_addr.s_addr),eth0_ip,50);
		if(-1 == (int)get_tips_address.sin_addr.s_addr)		//update by hyj
		{
			strncpy(eth0_ip,"0.0.0.0",sizeof(eth0_ip));
       		        strncpy(mac, "00:00:00:00:00:00\n",sizeof (mac));
                	strncpy(mask, "0.0.0.0", sizeof (mask));
        	        sprintf(gateway,"%s","0.0.0.0");
		}
        	else
		{
			FILE *fp = popen ("ifconfig -a | grep eth0 | awk '{print $NF}'", "r");
       			 if(NULL == fp) 
       			 {   
       			         fprintf (stderr, "%s\n", strerror (errno));
       			         strncpy(mac, "00:00:00:00:00:00\n", sizeof (mac));
       			 }   
       			 else
       			 {   
       			         fgets (mac, sizeof(mac), fp);
       			         if(strlen(mac) == 0)
       			         {   
       			                 fprintf (stderr, "%s\n", strerror (errno));
       			                 strncpy(mac, "00:00:00:00:00:00\n", sizeof (mac));
       			         }   
       			 }   
       			 mac[strlen(mac) - 1] = '\0';
       			 pclose(fp);
			snprintf (buffer, sizeof (buffer), "ifconfig -a | grep %s | awk '{print $NF}'", eth0_ip);
			fp = popen (buffer, "r");
			if(NULL == fp)
			{
	        		fprintf (stderr, "%s\n", strerror (errno));
                		//memset (buffer, 0, sizeof (buffer));
                		strncpy(mask, "0.0.0.0", sizeof (mask));
			}
			else
			{
                		memset (buffer, 0, sizeof (buffer));
				fgets (buffer , sizeof(buffer), fp);
				if(strlen (buffer) == 0)
				{
					strncpy (mask, "0.0.0.0", sizeof (mask));
				}
				 else
                		{
                	       		buffer [strlen (buffer) - 1] = '\0';
                	        	char* p = strchr(buffer, ':');
                	        	strncpy (mask, p + 1, sizeof (mask));
				}
			}
			pclose(fp);
			//strcpy(gateway,(char *)"172.16.0.22");
#if 1 
        		fp = popen("route -n| grep eth0 | awk '{print $2}'","r");
        		if(fp == NULL)
        		{   
        		        printf("erro\n");
        		        sprintf(gateway,"%s","0.0.0.0");
        		}   
        		else
        		{   
        		        fgets(gateway,sizeof(gateway),fp);
        		        if(!strlen(gateway))
        		                sprintf(gateway,"%s","0.0.0.0");
        		        else
        		        {   
        		                gateway[strlen(gateway)-1] = '\0';
        		        }   
        		}   
        		if(!strcmp(gateway,"*"))
        		        sprintf(gateway,"%s","0.0.0.0");
        		//printf("gateway===%s\n",gateway);
        		pclose(fp);
		}
#endif
		//unsigned char mac[30];
		//config_manage_t sconfig;
		//memset(&sconfig,0,sizeof(&sconfig));
		//memset(mac,0,sizeof(mac));
		//get_local_ip((char *)"eth0",eth0_ip);
        	//int format = get_config_manage_xml(&sconfig); 
		//char *xml = (char *)g_strdup_printf("<sysMinitorSite><MANAGE_IP>%s</MANAGE_IP><MANAGE_IP_MAC>%s</MANAGE_IP_MAC></sysMinitorSite>",sconfig.manage_IP,mac);
		//char *xml = (char *)g_strdup_printf("<sysMinitorSite><MANAGE_IP>%s</MANAGE_IP><MANAGE_IP_MAC>%s</MANAGE_IP_MAC></sysMinitorSite>",sconfig.manage_IP,"为什么");
		char *xml = (char *)g_strdup_printf("<sysMinitorSite><MANAGE_IP>%s</MANAGE_IP><MANAGE_IP_MAC>%s</MANAGE_IP_MAC><MASK>%s</MASK><GATEWAY>%s</GATEWAY></sysMinitorSite>",eth0_ip,mac,mask,gateway);
               	char *html = ((admin_opt_fun *)opt_fun)(credentials, xml);			
                g_free(xml);
                return html;
	}
	else if(!strcmp(cmd,"minitorsite_cmd"))//监控网络
	{
	char *xml=NULL;
	//int ip_flag = 0;
         config_manage_t tips_config_manage;
	 memset(&tips_config_manage,0,sizeof(&tips_config_manage));
	tips_config_manage.modify_flag = 0;
#if 0
	 if(NULL != params_value(params, "manage_IP"))
		{
			if(strlen(params_value(params,"manage_IP")))
			{
				if(INADDR_NONE == inet_addr(params_value(params, "manage_IP")))
				{
					ip_flag = 1;
					strcpy(tips_config_manage.manage_IP,params_value(params, "manage_IP"));
				}
				else
					{
						char *node = NULL;
						node = strchr((char *)params_value(params, "manage_IP"),'.');
						if(NULL != node)
						strcpy(tips_config_manage.manage_IP,params_value(params, "manage_IP"));
						else
						{
							ip_flag =1;
							strcpy(tips_config_manage.manage_IP,params_value(params, "manage_IP"));
						}
					}
			}
			else
					strcpy(tips_config_manage.manage_IP,"");
		}
	else 
		strcpy(tips_config_manage.manage_IP,"");
#endif
		strcpy(tips_config_manage.manage_IP,params_value(params, "manage_IP"));
		//strcpy(tips_config_manage.manage_IP_MAC,params_value(params, "manage_IP_MAC"));
		strcpy(tips_config_manage.manage_mask,params_value(params, "manage_mask"));
		strcpy(tips_config_manage.manage_gateway,params_value(params, "manage_gateway"));
		config_manage_modify(&tips_config_manage, credentials,0);
			//xml = (char *)g_strdup_printf("<sysMinitorSite><MANAGE_IP>%s</MANAGE_IP><MANAGE_IP_MAC>%s</MANAGE_IP_MAC><MASK>%s</MASK><GATEWAY>%s</GATEWAY><msg>提交成功</msg></sysMinitorSite>",tips_config_manage.manage_IP,tips_config_manage.manage_IP_MAC,tips_config_manage.manage_mask,tips_config_manage.manage_gateway);
			//xml = (char *)g_strdup_printf("<sysMinitorSite><MANAGE_IP>%s</MANAGE_IP><MANAGE_IP_MAC>%s</MANAGE_IP_MAC><MASK>%s</MASK><GATEWAY>%s</GATEWAY><msg>提交失败(与前值设置相同)</msg></sysMinitorSite>",tips_config_manage.manage_IP,tips_config_manage.manage_IP_MAC,tips_config_manage.manage_mask,tips_config_manage.manage_gateway);
		if(tips_config_manage.modify_flag == 1)
			xml = (char *)g_strdup_printf("<sysMinitorSite><MANAGE_IP>%s</MANAGE_IP><MASK>%s</MASK><GATEWAY>%s</GATEWAY><msg>提交成功</msg></sysMinitorSite>",tips_config_manage.manage_IP,tips_config_manage.manage_mask,tips_config_manage.manage_gateway);
		else
			xml = (char *)g_strdup_printf("<sysMinitorSite><MANAGE_IP>%s</MANAGE_IP><MASK>%s</MASK><GATEWAY>%s</GATEWAY><msg>提交失败(与前值设置相同)</msg></sysMinitorSite>",tips_config_manage.manage_IP,tips_config_manage.manage_mask,tips_config_manage.manage_gateway);
               	char *html = ((admin_opt_fun *)opt_fun)(credentials, xml);			
                g_free(xml);
                return html;
	}
	else if(!strcmp(cmd,"communication"))//通信网络
	{
		config_manage_t sconfig;
		struct sockaddr_in get_tips_address;
		char mac[40],buffer[100],mask[50];
		char unit_name[100];
		char eth1_ip[50];
       		char gateway[50];
		memset(eth1_ip,0,sizeof(eth1_ip));
		memset(unit_name,0,sizeof(unit_name));
       		memset(gateway,0,sizeof(gateway));
		memset(mac,0,sizeof(mac));
		memset(buffer,0,sizeof(buffer));
		memset(mask,0,sizeof(mask));
		memset(sconfig.manage_IP,0,sizeof(sconfig.manage_IP));
		memset(&sconfig,0,sizeof(config_manage_t));
		printf("before get infosisiisisii\n");
        	/*是否需要判断返回值?int format =*/ get_config_manage_xml(&sconfig); 
		if(!strlen(sconfig.static_route))
			strcpy(sconfig.static_route,(char *)"0.0.0.0");
		printf("after get infosisiisisii\n");
        	get_tips_address.sin_addr.s_addr = get_local_ip((char *)"eth1");
		inet_ntop(AF_INET, &(get_tips_address.sin_addr.s_addr),eth1_ip,50);
		if(-1 == (int)get_tips_address.sin_addr.s_addr)							//update by hyj
		{
			strncpy(eth1_ip,"0.0.0.0",sizeof(eth1_ip));
       		        strncpy(mac, "00:00:00:00:00:00\n", sizeof (mac));
                	strncpy(mask, "0.0.0.0", sizeof (mask));
        	        sprintf(gateway,"%s","0.0.0.0");
		}
		else
		{
			FILE *fp = popen("ifconfig -a | grep eth1 | awk '{print $NF}'", "r");
       			 if(NULL == fp) 
       			 {   
       			         fprintf (stderr, "%s\n", strerror (errno));
       			         strncpy(mac, "00:00:00:00:00:00\n", sizeof (mac));
       			 }   
       			 else
       			 {   
       			         fgets (mac, sizeof(mac), fp);
       			         if(strlen(mac) == 0)
       			         {   
       			                 fprintf (stderr, "%s\n", strerror (errno));
       			                 strncpy(mac, "00:00:00:00:00:00\n", sizeof (mac));
       			         }   
       			 }   
       			 mac[strlen(mac) - 1] = '\0';
       			 pclose(fp);
			snprintf (buffer, sizeof (buffer), "ifconfig -a | grep %s | awk '{print $NF}'", eth1_ip);
			fp = popen (buffer, "r");
			if(NULL == fp)
			{
	        		fprintf (stderr, "%s\n", strerror (errno));
                		//memset (buffer, 0, sizeof (buffer));
                		strncpy(mask, "0.0.0.0", sizeof (mask));
			}
			else
			{
                		memset (buffer, 0, sizeof (buffer));
				fgets (buffer , sizeof(buffer), fp);
				if(strlen (buffer) == 0)
				{
					strncpy (mask, "0.0.0.0", sizeof (mask));
				}
				 else
                		{
                	       		buffer [strlen (buffer) - 1] = '\0';
                	        	char* p = strchr(buffer, ':');
                	        	strncpy (mask, p + 1, sizeof (mask));
				}
			}
			pclose(fp);
			//strcpy(gateway,(char *)"172.16.0.22");
#if 1 
        		fp = popen("route -n| grep eth1 | awk '{print $2}'","r");
        		if(fp == NULL)
        		{   
        		        printf("erro\n");
        		        sprintf(gateway,"%s","0.0.0.0");
        		}   
        		else
        		{   
        		        fgets(gateway,sizeof(gateway),fp);
        		        if(!strlen(gateway))
        		                sprintf(gateway,"%s","0.0.0.0");
        		        else
        		        {   
        		                gateway[strlen(gateway)-1] = '\0';
        		        }   
        		}   
        		if(!strcmp(gateway,"*"))
        		        sprintf(gateway,"%s","0.0.0.0");
        		//printf("gateway===%s\n",gateway);
        		pclose(fp);
#endif
		}
		FILE *fp_name = fopen("/usr/local/tipschecker/etc/tipscheck/unit_name","r+");
		//fread(page_number,sizeof(page_number),1,fp);
		fread(unit_name,sizeof(unit_name),1,fp_name);
		fclose(fp_name);
		if(unit_name[strlen(unit_name)-1] == '\n')
			unit_name[strlen(unit_name)-1]='\0';
		/*130000;130200;130204*/
		char *xml = (char *)g_strdup_printf("<sysCommunication><lipeiting>%s</lipeiting><NET_IP>%s</NET_IP><MASK>%s</MASK><STATIC_ROUTE>%s</STATIC_ROUTE><GATEWAY>%s</GATEWAY><SERVE_ADDR>%s</SERVE_ADDR><SERVE_USER>%s</SERVE_USER><SERVE_PASSWORD>%s</SERVE_PASSWORD><ADMINSTRATION_IP>%s</ADMINSTRATION_IP><ADMINSTRATION_IP1>%s</ADMINSTRATION_IP1><MANAGE_PORT>%s</MANAGE_PORT><MANAGE_PORT1>%s</MANAGE_PORT1></sysCommunication>",unit_name,eth1_ip,mask,sconfig.static_route,gateway,sconfig.serve_addr,sconfig.serve_user,sconfig.serve_password,sconfig.adminstration_IP,sconfig.adminstration_IP1,sconfig.manage_PORT,sconfig.manage_PORT1);
               	char *html = ((admin_opt_fun *)opt_fun)(credentials, xml);			
                g_free(xml);
                return html;
	}
	else if(!strcmp(cmd,"stragetymanage"))
	{
		char *xml = (char *)g_strdup_printf("<sysStragetyManage></sysStragetyManage>");
               	char *html = ((admin_opt_fun *)opt_fun)(credentials, xml);			
                g_free(xml);
                return html;
	}
	else if(!strcmp(cmd,"stragetymanage_cmd"))
	{
		int flag_vlan[4] = {0,0,0,0};
		FILE *fp = NULL;
		char *write_to_vlanfile = strdup("");
		char *vlan_num_name=(char *)params_value(params,"vlan_num_name");
		char *vlan_start_ip_name=(char *)params_value(params,"vlan_start_ip_name");
		char *vlan_end_ip_name=(char *)params_value(params,"vlan_end_ip_name");
		char *vlan_mask_name=(char *)params_value(params,"vlan_mask_name");
		printf("vlan_num_name==%s\n",vlan_num_name);
		printf("vlan_start_ip_name==%s\n",vlan_start_ip_name);
		printf("vlan_end_ip_name==%s\n",vlan_end_ip_name);
		printf("vlan_mask_name==%s\n",vlan_mask_name);
			//char *xmlparam=strdup("");
			//write_to_vlanfile = g_strdup_printf("%scheck -v %s ",write_to_vlanfile,vlan_num_name);
			//write_to_vlanfile= g_strdup_printf("%s-s %s,%s ",write_to_vlanfile,vlan_start_ip_name,vlan_end_ip_name);
			//write_to_vlanfile = g_strdup_printf("%s-m %s",write_to_vlanfile,vlan_mask_name);
			//if(vlan_num_name==NULL&&vlan_start_ip_name==NULL&&vlan_end_ip_name==NULL&&vlan_mask_name==NULL)
			//exit(0);
		if((strlen(vlan_num_name)) && (atoi(vlan_num_name)!=0))	
			flag_vlan[0] = 1;
		if((strlen(vlan_start_ip_name)))
			flag_vlan[1] = 1;
		if((strlen(vlan_end_ip_name)))
			flag_vlan[2] = 1;
		if(strlen(vlan_mask_name))
			flag_vlan[3] = 1;
		if(flag_vlan[0] == 0)
		{
			if(flag_vlan[1] && flag_vlan[2] && flag_vlan[3])
			{
				write_to_vlanfile=g_strdup_printf("check -s %s,%s -m %s",vlan_start_ip_name,vlan_end_ip_name,vlan_mask_name);
				fp = fopen("/usr/local/tipschecker/etc/policy.conf","w+");
				fwrite(write_to_vlanfile,strlen(write_to_vlanfile),1,fp);
				fclose(fp);
				printf("write_to_vlanfile===%s\n",write_to_vlanfile);
				sleep(5);
				char *xml = (char *)g_strdup_printf("<sysStragetyManage><msg_vlan>设置成功</msg_vlan></sysStragetyManage>");
               			char *html = ((admin_opt_fun *)opt_fun)(credentials, xml);			
                		g_free(xml);
                		g_free(write_to_vlanfile);
                		return html;
				/*全部为1*/
			}
			else if(flag_vlan[1] == 0 && flag_vlan[2] == 0 && flag_vlan[3] == 0)
			{
				char *xml = (char *)g_strdup_printf("<sysStragetyManage><msg_vlan>内容全部为空,未设置</msg_vlan></sysStragetyManage>");
               			char *html = ((admin_opt_fun *)opt_fun)(credentials, xml);			
                		g_free(xml);
                		g_free(write_to_vlanfile);
                		return html;
				/*全部为0*/	
			}
			else
			{
				char *xml = (char *)g_strdup_printf("<sysStragetyManage><msg_vlan>起始IP和终止IP和子网掩码都要填写</msg_vlan></sysStragetyManage>");
               			char *html = ((admin_opt_fun *)opt_fun)(credentials, xml);			
                		g_free(xml);
                		g_free(write_to_vlanfile);
                		return html;
				/*其它*/
			}
		}
		else
		{
			if(flag_vlan[1] && flag_vlan[2] && flag_vlan[3])
			{
				write_to_vlanfile=g_strdup_printf("check -v %s -s %s,%s -m %s",vlan_num_name,vlan_start_ip_name,vlan_end_ip_name,vlan_mask_name);
				fp = fopen("/usr/local/tipschecker/etc/policy.conf","w+");
				fwrite(write_to_vlanfile,strlen(write_to_vlanfile),1,fp);
				fclose(fp);
				printf("write_to_vlanfile===%s\n",write_to_vlanfile);
				sleep(5);
				char *xml = (char *)g_strdup_printf("<sysStragetyManage><msg_vlan>设置成功</msg_vlan></sysStragetyManage>");
               			char *html = ((admin_opt_fun *)opt_fun)(credentials, xml);			
                		g_free(xml);
                		g_free(write_to_vlanfile);
                		return html;
				/*全部为1*/
			}
			else if(flag_vlan[1] == 0 && flag_vlan[2] == 0 && flag_vlan[3] == 0)
			{
				write_to_vlanfile=g_strdup_printf("check -v %s",vlan_num_name);
				fp = fopen("/usr/local/tipschecker/etc/policy.conf","w+");
				fwrite(write_to_vlanfile,strlen(write_to_vlanfile),1,fp);
				fclose(fp);
				printf("write_to_vlanfile===%s\n",write_to_vlanfile);
				sleep(5);
				char *xml = (char *)g_strdup_printf("<sysStragetyManage><msg_vlan>设置成功</msg_vlan></sysStragetyManage>");
               			char *html = ((admin_opt_fun *)opt_fun)(credentials, xml);			
                		g_free(xml);
                		g_free(write_to_vlanfile);
                		return html;
				/*全部为0*/	
			}
			else
			{
				char *xml = (char *)g_strdup_printf("<sysStragetyManage><msg_vlan>起始IP和终止IP和子网掩码必须一一对应</msg_vlan></sysStragetyManage>");
               			char *html = ((admin_opt_fun *)opt_fun)(credentials, xml);			
                		g_free(xml);
                		g_free(write_to_vlanfile);
                		return html;
				/*其它*/
			}
		}
	}
	else if(!strcmp(cmd,"communication_netip"))
	{
#if 1 
		char *xml = NULL;
		config_manage_t sconfig;
         	config_manage_t tips_config_manage;
		char static_route[100];
		struct sockaddr_in get_tips_address;
		char mac[40],buffer[100],mask[50];
		char eth1_ip[50];
       		char gateway[50];
		memset(static_route,0,sizeof(static_route));
		memset(eth1_ip,0,sizeof(eth1_ip));
       		memset(gateway,0,sizeof(gateway));
		memset(mac,0,sizeof(mac));
		memset(buffer,0,sizeof(buffer));
		memset(mask,0,sizeof(mask));
		memset(sconfig.manage_IP,0,sizeof(sconfig.manage_IP));
		memset(&sconfig,0,sizeof(config_manage_t));
		memset(tips_config_manage.manage_IP,0,sizeof(tips_config_manage.manage_IP));
		memset(&tips_config_manage,0,sizeof(config_manage_t));
		tips_config_manage.modify_flag = 0;
		strcpy(tips_config_manage.net_IP,params_value(params, "net_IP"));
		strcpy(tips_config_manage.mask,params_value(params, "net_mask"));
		strcpy(tips_config_manage.gateway,params_value(params, "net_gateway"));
		strcpy(tips_config_manage.static_route,params_value(params, "static_route"));
		config_manage_modify(&tips_config_manage, credentials,1);
		//sleep(1);
		//config_manage_modify();
        	/*是否需要判断返回值?int format =*/ get_config_manage_xml(&sconfig); 
        	get_tips_address.sin_addr.s_addr = get_local_ip((char *)"eth1");
		inet_ntop(AF_INET, &(get_tips_address.sin_addr.s_addr),eth1_ip,50);
		if(-1 == (int)get_tips_address.sin_addr.s_addr)			//update by hyj
		{
			strncpy(eth1_ip,"0.0.0.0",sizeof(eth1_ip));
       		        strncpy(mac, "00:00:00:00:00:00\n", sizeof (mac));
                	strncpy(mask, "0.0.0.0", sizeof (mask));
        	        sprintf(gateway,"%s","0.0.0.0");
		}
		else
		{
			FILE *fp = popen("ifconfig -a | grep eth1 | awk '{print $NF}'", "r");
       			 if(NULL == fp) 
       			 {   
       			         fprintf (stderr, "%s\n", strerror (errno));
       			         strncpy(mac, "00:00:00:00:00:00\n", sizeof (mac));
       			 }   
       			 else
       			 {   
       			         fgets (mac, sizeof(mac), fp);
       			         if(strlen(mac) == 0)
       			         {   
       			                 fprintf (stderr, "%s\n", strerror (errno));
       			                 strncpy(mac, "00:00:00:00:00:00\n", sizeof (mac));
       			         }   
       			 }   
       			 mac[strlen(mac) - 1] = '\0';
       			 pclose(fp);
			snprintf (buffer, sizeof (buffer), "ifconfig -a | grep %s | awk '{print $NF}'", eth1_ip);
			fp = popen (buffer, "r");
			if(NULL == fp)
			{
	        		fprintf (stderr, "%s\n", strerror (errno));
                		//memset (buffer, 0, sizeof (buffer));
                		strncpy(mask, "0.0.0.0", sizeof (mask));
			}
			else
			{
                		memset (buffer, 0, sizeof (buffer));
				fgets (buffer , sizeof(buffer), fp);
				if(strlen (buffer) == 0)
				{
					strncpy (mask, "0.0.0.0", sizeof (mask));
				}
				 else
                		{
                	       		buffer [strlen (buffer) - 1] = '\0';
                	        	char* p = strchr(buffer, ':');
                	        	strncpy (mask, p + 1, sizeof (mask));
				}
			}
			pclose(fp);
        		fp = popen("route -n| grep eth1 | awk '{print $2}'","r");
        		if(fp == NULL)
        		{   
        		        printf("erro\n");
        		        sprintf(gateway,"%s","0.0.0.0");
        		}   
        		else
        		{   
        		        fgets(gateway,sizeof(gateway),fp);
        		        if(!strlen(gateway))
        		                sprintf(gateway,"%s","0.0.0.0");
        		        else
        		        {   
        		                gateway[strlen(gateway)-1] = '\0';
        		        }   
        		}   
        		if(!strcmp(gateway,"*"))
        		        sprintf(gateway,"%s","0.0.0.0");
        		//printf("gateway===%s\n",gateway);
        		pclose(fp);
		}
		if(tips_config_manage.modify_flag == 1)
			xml = (char *)g_strdup_printf("<sysCommunication><NET_IP>%s</NET_IP><MASK>%s</MASK><STATIC_ROUTE>%s</STATIC_ROUTE><GATEWAY>%s</GATEWAY><SERVE_ADDR>%s</SERVE_ADDR><SERVE_USER>%s</SERVE_USER><SERVE_PASSWORD>%s</SERVE_PASSWORD><ADMINSTRATION_IP>%s</ADMINSTRATION_IP><ADMINSTRATION_IP1>%s</ADMINSTRATION_IP1><MANAGE_PORT>%s</MANAGE_PORT><MANAGE_PORT1>%s</MANAGE_PORT1><msg>提交成功</msg></sysCommunication>",eth1_ip,mask,tips_config_manage.static_route,gateway,sconfig.serve_addr,sconfig.serve_user,sconfig.serve_password,sconfig.adminstration_IP,sconfig.adminstration_IP1,sconfig.manage_PORT,sconfig.manage_PORT1);
		else
			xml = (char *)g_strdup_printf("<sysCommunication><NET_IP>%s</NET_IP><MASK>%s</MASK><STATIC_ROUTE>%s</STATIC_ROUTE><GATEWAY>%s</GATEWAY><SERVE_ADDR>%s</SERVE_ADDR><SERVE_USER>%s</SERVE_USER><SERVE_PASSWORD>%s</SERVE_PASSWORD><ADMINSTRATION_IP>%s</ADMINSTRATION_IP><ADMINSTRATION_IP1>%s</ADMINSTRATION_IP1><MANAGE_PORT>%s</MANAGE_PORT><MANAGE_PORT1>%s</MANAGE_PORT1><msg>提交失败(与前设置相同)</msg></sysCommunication>",eth1_ip,mask,tips_config_manage.static_route,gateway,sconfig.serve_addr,sconfig.serve_user,sconfig.serve_password,sconfig.adminstration_IP,sconfig.adminstration_IP1,sconfig.manage_PORT,sconfig.manage_PORT1);
#endif
		//char *xml = (char *)g_strdup_printf("<sysCommunication></sysCommunication>");
               	char *html = ((admin_opt_fun *)opt_fun)(credentials, xml);			
                g_free(xml);
                return html;
	}
	else if(!strcmp(cmd,"communication_centerip"))
	{
#if 1 
		char *xml = NULL;
		config_manage_t sconfig;
         	config_manage_t tips_config_manage;
		struct sockaddr_in get_tips_address;
		char mac[40],buffer[100],mask[50];
		char eth1_ip[50];
       		char gateway[50];
		memset(eth1_ip,0,sizeof(eth1_ip));
       		memset(gateway,0,sizeof(gateway));
		memset(mac,0,sizeof(mac));
		memset(buffer,0,sizeof(buffer));
		memset(mask,0,sizeof(mask));
		memset(sconfig.manage_IP,0,sizeof(sconfig.manage_IP));
		memset(&sconfig,0,sizeof(config_manage_t));
		memset(tips_config_manage.manage_IP,0,sizeof(tips_config_manage.manage_IP));
		memset(&tips_config_manage,0,sizeof(config_manage_t));
		tips_config_manage.modify_flag = 0;	
		strcpy(tips_config_manage.adminstration_IP,params_value(params, "administration_IP"));
		strcpy(tips_config_manage.manage_PORT,params_value(params, "manage_PORT"));
		strcpy(tips_config_manage.manage_PORT1,params_value(params, "manage_PORT1"));
		strcpy(tips_config_manage.adminstration_IP1,params_value(params, "administration_IP1"));
		//printf("tips_config_manage.adminstration_IP===%s\n",tips_config_manage.adminstration_IP);
		//printf("tips_config_manage.adminstration_IP1===%s\n",tips_config_manage.adminstration_IP1);
		//printf("tips_config_manage.manage_PORT===%s\n",tips_config_manage.manage_PORT);
		//printf("tips_config_manage.manage_PORT1===%s\n",tips_config_manage.manage_PORT1);
		//exit(0);
		config_manage_modify(&tips_config_manage, credentials,2);
		//sleep(1);
		//config_manage_modify();
        	/*是否需要判断返回值?int format =*/ get_config_manage_xml(&sconfig); 
		if(!strlen(sconfig.static_route))
			strcpy(sconfig.static_route,(char *)"0.0.0.0");
        	get_tips_address.sin_addr.s_addr = get_local_ip((char *)"eth1");
		inet_ntop(AF_INET, &(get_tips_address.sin_addr.s_addr),eth1_ip,50);
		printf("威慑么么么么22222:\n");
		if(-1 == (int)get_tips_address.sin_addr.s_addr)				//update by hyj
		{
			strncpy(eth1_ip,"0.0.0.0",sizeof(eth1_ip));
       		        strncpy(mac, "00:00:00:00:00:00\n", sizeof (mac));
                	strncpy(mask, "0.0.0.0", sizeof (mask));
        	        sprintf(gateway,"%s","0.0.0.0");
		}
		else
		{
			FILE *fp = popen("ifconfig -a | grep eth1 | awk '{print $NF}'", "r");
       			 if(NULL == fp) 
       			 {   
       			         fprintf (stderr, "%s\n", strerror (errno));
       			         strncpy(mac, "00:00:00:00:00:00\n", sizeof (mac));
       			 }   
       			 else
       			 {   
       			         fgets (mac, sizeof(mac), fp);
       			         if(strlen(mac) == 0)
       			         {   
       			                 fprintf (stderr, "%s\n", strerror (errno));
       			                 strncpy(mac, "00:00:00:00:00:00\n", sizeof (mac));
       			         }   
       			 }   
       			 mac[strlen(mac) - 1] = '\0';
       			 pclose(fp);
			snprintf (buffer, sizeof (buffer), "ifconfig -a | grep %s | awk '{print $NF}'", eth1_ip);
			fp = popen (buffer, "r");
			if(NULL == fp)
			{
	        		fprintf (stderr, "%s\n", strerror (errno));
                		//memset (buffer, 0, sizeof (buffer));
                		strncpy(mask, "0.0.0.0", sizeof (mask));
			}
			else
			{
                		memset (buffer, 0, sizeof (buffer));
				fgets (buffer , sizeof(buffer), fp);
				if(strlen (buffer) == 0)
				{
					strncpy (mask, "0.0.0.0", sizeof (mask));
				}
				 else
                		{
                	       		buffer [strlen (buffer) - 1] = '\0';
                	        	char* p = strchr(buffer, ':');
                	        	strncpy (mask, p + 1, sizeof (mask));
				}
			}
			pclose(fp);
        		fp = popen("route -n| grep eth1 | awk '{print $2}'","r");
        		if(fp == NULL)
        		{   
        		        printf("erro\n");
        		        sprintf(gateway,"%s","0.0.0.0");
        		}   
        		else
        		{   
        		        fgets(gateway,sizeof(gateway),fp);
        		        if(!strlen(gateway))
        		                sprintf(gateway,"%s","0.0.0.0");
        		        else
        		        {   
        		                gateway[strlen(gateway)-1] = '\0';
        		        }   
        		}   
        		if(!strcmp(gateway,"*"))
        		        sprintf(gateway,"%s","0.0.0.0");
        		//printf("gateway===%s\n",gateway);
        		pclose(fp);
		}
		if(tips_config_manage.modify_flag == 1)
			xml = (char *)g_strdup_printf("<sysCommunication><NET_IP>%s</NET_IP><MASK>%s</MASK><STATIC_ROUTE>%s</STATIC_ROUTE><GATEWAY>%s</GATEWAY><SERVE_ADDR>%s</SERVE_ADDR><SERVE_USER>%s</SERVE_USER><SERVE_PASSWORD>%s</SERVE_PASSWORD><ADMINSTRATION_IP>%s</ADMINSTRATION_IP><ADMINSTRATION_IP1>%s</ADMINSTRATION_IP1><MANAGE_PORT>%s</MANAGE_PORT><MANAGE_PORT1>%s</MANAGE_PORT1><msg_center>提交成功</msg_center></sysCommunication>",eth1_ip,mask,sconfig.static_route,gateway,sconfig.serve_addr,sconfig.serve_user,sconfig.serve_password,sconfig.adminstration_IP,sconfig.adminstration_IP1,sconfig.manage_PORT,sconfig.manage_PORT1);
		else
			xml = (char *)g_strdup_printf("<sysCommunication><NET_IP>%s</NET_IP><MASK>%s</MASK><STATIC_ROUTE>%s</STATIC_ROUTE><GATEWAY>%s</GATEWAY><SERVE_ADDR>%s</SERVE_ADDR><SERVE_USER>%s</SERVE_USER><SERVE_PASSWORD>%s</SERVE_PASSWORD><ADMINSTRATION_IP>%s</ADMINSTRATION_IP><ADMINSTRATION_IP1>%s</ADMINSTRATION_IP1><MANAGE_PORT>%s</MANAGE_PORT><MANAGE_PORT1>%s</MANAGE_PORT1><msg_center>提交失败(与前设置相同)</msg_center></sysCommunication>",eth1_ip,mask,sconfig.static_route,gateway,sconfig.serve_addr,sconfig.serve_user,sconfig.serve_password,sconfig.adminstration_IP,sconfig.adminstration_IP1,sconfig.manage_PORT,sconfig.manage_PORT1);
#endif
		//printf("威慑么么么么:%s\n",xml);
		//exit(0);
		//char *xml = (char *)g_strdup_printf("<sysCommunication></sysCommunication>");
               	char *html = ((admin_opt_fun *)opt_fun)(credentials, xml);			
                g_free(xml);
                return html;
	}
	else if(!strcmp(cmd,"communication_serverip"))
	{
#if 1 
		char *xml = NULL;
		config_manage_t sconfig;
         	config_manage_t tips_config_manage;
		struct sockaddr_in get_tips_address;
		char mac[40],buffer[100],mask[50];
		char eth1_ip[50];
       		char gateway[50];
		memset(eth1_ip,0,sizeof(eth1_ip));
       		memset(gateway,0,sizeof(gateway));
		memset(mac,0,sizeof(mac));
		memset(buffer,0,sizeof(buffer));
		memset(mask,0,sizeof(mask));
		memset(sconfig.manage_IP,0,sizeof(sconfig.manage_IP));
		memset(&sconfig,0,sizeof(config_manage_t));
		memset(tips_config_manage.manage_IP,0,sizeof(tips_config_manage.manage_IP));
		memset(&tips_config_manage,0,sizeof(config_manage_t));
		tips_config_manage.modify_flag = 0;
		strcpy(tips_config_manage.serve_addr,params_value(params, "serve_addr"));
		strcpy(tips_config_manage.serve_user,params_value(params, "serve_user"));
		strcpy(tips_config_manage.serve_password,params_value(params, "serve_password"));
		//printf("tips_config_manage.adminstration_IP===%s\n",tips_config_manage.adminstration_IP);
		//printf("tips_config_manage.adminstration_IP1===%s\n",tips_config_manage.adminstration_IP1);
		//printf("tips_config_manage.manage_PORT===%s\n",tips_config_manage.manage_PORT);
		//printf("tips_config_manage.manage_PORT1===%s\n",tips_config_manage.manage_PORT1);
		//exit(0);
		config_manage_modify(&tips_config_manage, credentials,3);
		//sleep(1);
		//config_manage_modify();
        	/*是否需要判断返回值?int format =*/ get_config_manage_xml(&sconfig); 
		if(!strlen(sconfig.static_route))
			strcpy(sconfig.static_route,(char *)"0.0.0.0");
        	get_tips_address.sin_addr.s_addr = get_local_ip((char *)"eth1");
		inet_ntop(AF_INET, &(get_tips_address.sin_addr.s_addr),eth1_ip,50);
		if(-1 == (int)get_tips_address.sin_addr.s_addr)				//update by hyj
		{
			strncpy(eth1_ip,"0.0.0.0",sizeof(eth1_ip));
       		        strncpy(mac, "00:00:00:00:00:00\n", sizeof (mac));
                	strncpy(mask, "0.0.0.0", sizeof (mask));
        	        sprintf(gateway,"%s","0.0.0.0");
		}
		else
		{
			FILE *fp = popen("ifconfig -a | grep eth1 | awk '{print $NF}'", "r");
       			 if(NULL == fp) 
       			 {   
       			         fprintf (stderr, "%s\n", strerror (errno));
       			         strncpy(mac, "00:00:00:00:00:00\n", sizeof (mac));
       			 }   
       			 else
       			 {   
       			         fgets (mac, sizeof(mac), fp);
       			         if(strlen(mac) == 0)
       			         {   
       			                 fprintf (stderr, "%s\n", strerror (errno));
       			                 strncpy(mac, "00:00:00:00:00:00\n", sizeof (mac));
       			         }   
       			 }   
       			 mac[strlen(mac) - 1] = '\0';
       			 pclose(fp);
			snprintf (buffer, sizeof (buffer), "ifconfig -a | grep %s | awk '{print $NF}'", eth1_ip);
			fp = popen (buffer, "r");
			if(NULL == fp)
			{
	        		fprintf (stderr, "%s\n", strerror (errno));
                		//memset (buffer, 0, sizeof (buffer));
                		strncpy(mask, "0.0.0.0", sizeof (mask));
			}
			else
			{
                		memset (buffer, 0, sizeof (buffer));
				fgets (buffer , sizeof(buffer), fp);
				if(strlen (buffer) == 0)
				{
					strncpy (mask, "0.0.0.0", sizeof (mask));
				}
				 else
                		{
                	       		buffer [strlen (buffer) - 1] = '\0';
                	        	char* p = strchr(buffer, ':');
                	        	strncpy (mask, p + 1, sizeof (mask));
				}
			}
			pclose(fp);
        		fp = popen("route -n| grep eth1 | awk '{print $2}'","r");
        		if(fp == NULL)
        		{   
        		        printf("erro\n");
        		        sprintf(gateway,"%s","0.0.0.0");
        		}   
        		else
        		{   
        		        fgets(gateway,sizeof(gateway),fp);
        		        if(!strlen(gateway))
        		                sprintf(gateway,"%s","0.0.0.0");
        		        else
        		        {   
        		                gateway[strlen(gateway)-1] = '\0';
        		        }   
        		}   
        		if(!strcmp(gateway,"*"))
        		        sprintf(gateway,"%s","0.0.0.0");
        		//printf("gateway===%s\n",gateway);
        		pclose(fp);
		}
		if(tips_config_manage.modify_flag == 1)
			xml = (char *)g_strdup_printf("<sysCommunication><NET_IP>%s</NET_IP><MASK>%s</MASK><STATIC_ROUTE>%s</STATIC_ROUTE><GATEWAY>%s</GATEWAY><SERVE_ADDR>%s</SERVE_ADDR><SERVE_USER>%s</SERVE_USER><SERVE_PASSWORD>%s</SERVE_PASSWORD><ADMINSTRATION_IP>%s</ADMINSTRATION_IP><ADMINSTRATION_IP1>%s</ADMINSTRATION_IP1><MANAGE_PORT>%s</MANAGE_PORT><MANAGE_PORT1>%s</MANAGE_PORT1><msg_server>提交成功</msg_server></sysCommunication>",eth1_ip,mask,sconfig.static_route,gateway,sconfig.serve_addr,sconfig.serve_user,sconfig.serve_password,sconfig.adminstration_IP,sconfig.adminstration_IP1,sconfig.manage_PORT,sconfig.manage_PORT1);
		else
			xml = (char *)g_strdup_printf("<sysCommunication><NET_IP>%s</NET_IP><MASK>%s</MASK><STATIC_ROUTE>%s</STATIC_ROUTE><GATEWAY>%s</GATEWAY><SERVE_ADDR>%s</SERVE_ADDR><SERVE_USER>%s</SERVE_USER><SERVE_PASSWORD>%s</SERVE_PASSWORD><ADMINSTRATION_IP>%s</ADMINSTRATION_IP><ADMINSTRATION_IP1>%s</ADMINSTRATION_IP1><MANAGE_PORT>%s</MANAGE_PORT><MANAGE_PORT1>%s</MANAGE_PORT1><msg_server>提交失败(与前设置相同)</msg_server></sysCommunication>",eth1_ip,mask,sconfig.static_route,gateway,sconfig.serve_addr,sconfig.serve_user,sconfig.serve_password,sconfig.adminstration_IP,sconfig.adminstration_IP1,sconfig.manage_PORT,sconfig.manage_PORT1);
#endif
		//printf("威慑么么么么:%s\n",xml);
		//exit(0);
		//char *xml = (char *)g_strdup_printf("<sysCommunication></sysCommunication>");
               	char *html = ((admin_opt_fun *)opt_fun)(credentials, xml);			
                g_free(xml);
                return html;
	}
	else if(!strcmp(cmd,"communication_name"))//镜像网络
	{
		char *xml = NULL;
		config_manage_t sconfig;
         	config_manage_t tips_config_manage;
		struct sockaddr_in get_tips_address;
		char buffer[100],mask[50];
		char eth1_ip[50];
       		char gateway[50];
		memset(eth1_ip,0,sizeof(eth1_ip));
       		memset(gateway,0,sizeof(gateway));
		memset(buffer,0,sizeof(buffer));
		memset(mask,0,sizeof(mask));
		memset(sconfig.manage_IP,0,sizeof(sconfig.manage_IP));
		memset(&sconfig,0,sizeof(config_manage_t));
		memset(&tips_config_manage,0,sizeof(config_manage_t));
        	/*是否需要判断返回值？int format =*/ get_config_manage_xml(&sconfig); 
		if(!strlen(sconfig.static_route))
			strcpy(sconfig.static_route,(char *)"0.0.0.0");
        	get_tips_address.sin_addr.s_addr = get_local_ip((char *)"eth1");
		inet_ntop(AF_INET, &(get_tips_address.sin_addr.s_addr),eth1_ip,50);
		if(-1 == (int)get_tips_address.sin_addr.s_addr)			//update by hyj
		{
			strncpy(eth1_ip,"0.0.0.0",sizeof(eth1_ip));
		}
			snprintf (buffer, sizeof (buffer), "ifconfig -a | grep %s | awk '{print $NF}'", eth1_ip);
			FILE *fp = popen (buffer, "r");
			if(NULL == fp)
			{
	        		fprintf (stderr, "%s\n", strerror (errno));
                		//memset (buffer, 0, sizeof (buffer));
                		strncpy(mask, "0.0.0.0", sizeof (mask));
			}
			else
			{
                		memset (buffer, 0, sizeof (buffer));
				fgets (buffer , sizeof(buffer), fp);
				if(strlen (buffer) == 0)
				{
					strncpy (mask, "0.0.0.0", sizeof (mask));
				}
				 else
                		{
                	       		buffer [strlen (buffer) - 1] = '\0';
                	        	char* p = strchr(buffer, ':');
                	        	strncpy (mask, p + 1, sizeof (mask));
				}
			}
			pclose(fp);
			//strcpy(gateway,(char *)"172.16.0.22");
        		fp = popen("route -n| grep eth1 | awk '{print $2}'","r");
        		if(fp == NULL)
        		{   
        		        printf("erro\n");
        		        sprintf(gateway,"%s","0.0.0.0");
        		}   
        		else
        		{   
        		        fgets(gateway,sizeof(gateway),fp);
        		        if(!strlen(gateway))
        		                sprintf(gateway,"%s","0.0.0.0");
        		        else
        		        {   
        		                gateway[strlen(gateway)-1] = '\0';
        		        }   
        		}   
        		if(!strcmp(gateway,"*"))
        		        sprintf(gateway,"%s","0.0.0.0");
        		//printf("gateway===%s\n",gateway);
        		pclose(fp);
		memset(buffer,0,sizeof(buffer));
		char *chenliting = (char *)params_value(params,"chenliting");
		char *temp=strrchr(chenliting,';');
		strcpy(buffer,temp+1);
		printf("buffer=%s\n",buffer);
		strcpy(tips_config_manage.unit_name,buffer);
		config_manage_modify(&tips_config_manage, credentials,4);
		printf("chenliting:%s\n",tips_config_manage.unit_name);
		//exit(0);
		//printf("chenliting:%s\n",chenliting);
		//printf("lipeiting:%s\n",lipeiting);

		
		if(tips_config_manage.modify_flag == 1)
		{
			FILE *fp_name = fopen("/usr/local/tipschecker/etc/tipscheck/unit_name","w+");
			//fwrite("HA=1",strlen("HA=1"),1,fp);
			fwrite(chenliting,strlen(chenliting),1,fp_name);
			fclose(fp_name);
			xml = (char *)g_strdup_printf("<sysCommunication><lipeiting>%s</lipeiting><NET_IP>%s</NET_IP><MASK>%s</MASK><STATIC_ROUTE>%s</STATIC_ROUTE><GATEWAY>%s</GATEWAY><SERVE_ADDR>%s</SERVE_ADDR><SERVE_USER>%s</SERVE_USER><SERVE_PASSWORD>%s</SERVE_PASSWORD><ADMINSTRATION_IP>%s</ADMINSTRATION_IP><ADMINSTRATION_IP1>%s</ADMINSTRATION_IP1><MANAGE_PORT>%s</MANAGE_PORT><MANAGE_PORT1>%s</MANAGE_PORT1><msg_unit_name>提交成功</msg_unit_name></sysCommunication>",chenliting,eth1_ip,mask,sconfig.static_route,gateway,sconfig.serve_addr,sconfig.serve_user,sconfig.serve_password,sconfig.adminstration_IP,sconfig.adminstration_IP1,sconfig.manage_PORT,sconfig.manage_PORT1);
		}
		else
			xml = (char *)g_strdup_printf("<sysCommunication><lipeiting>%s</lipeiting><NET_IP>%s</NET_IP><MASK>%s</MASK><STATIC_ROUTE>%s</STATIC_ROUTE><GATEWAY>%s</GATEWAY><SERVE_ADDR>%s</SERVE_ADDR><SERVE_USER>%s</SERVE_USER><SERVE_PASSWORD>%s</SERVE_PASSWORD><ADMINSTRATION_IP>%s</ADMINSTRATION_IP><ADMINSTRATION_IP1>%s</ADMINSTRATION_IP1><MANAGE_PORT>%s</MANAGE_PORT><MANAGE_PORT1>%s</MANAGE_PORT1><msg_unit_name>提交失败,单位信息未改变</msg_unit_name></sysCommunication>",chenliting,eth1_ip,mask,sconfig.static_route,gateway,sconfig.serve_addr,sconfig.serve_user,sconfig.serve_password,sconfig.adminstration_IP,sconfig.adminstration_IP1,sconfig.manage_PORT,sconfig.manage_PORT1);
		//char *xml = (char *)g_strdup_printf("<sysCommunication><lipeiting>%s</lipeiting></sysCommunication>",chenliting);
               	char *html = ((admin_opt_fun *)opt_fun)(credentials, xml);			
                g_free(xml);
                return html;
	}
	else if(!strcmp(cmd,"mirrornetwork"))//镜像网络
	{
		//DefaultNICList_t DefaultNicStu;
		//int mirro_flag = 0;
		char mirro_buf[10];
		//int eth_flag[8] = {0,0,0,0,0,0,0,0};
		memset(mirro_buf,0,10);
		//memset(&DefaultNicStu,0,sizeof(DefaultNICList_t));
		//GetNicInfo(&DefaultNicStu);
		//eth_flag[2] = is_netline(DefaultNicStu.E_image_1);
		//eth_flag[3] = is_netline(DefaultNicStu.E_image_2);
		//eth_flag[6] = is_netline(DefaultNicStu.O_image_1);
		//eth_flag[7] = is_netline(DefaultNicStu.O_image_2);
		//if(eth_flag[2] == 1 && eth_flag[3] == 1)
			//mirro_flag = 1;
		FILE *fp = fopen("/usr/local/tipschecker/etc/ModeByUI","r+");
		fgets(mirro_buf,100,fp);
		if(mirro_buf[strlen(mirro_buf)-1] == '\n')
			mirro_buf[strlen(mirro_buf)-1] = '\0';
		fclose(fp);
		char *xml = (char *)g_strdup_printf("<sysMirrorNetwork><start_mirror_flag>1</start_mirror_flag><choosed>%s</choosed></sysMirrorNetwork>",mirro_buf);
               	char *html = ((admin_opt_fun *)opt_fun)(credentials, xml);			
                g_free(xml);
                return html;
	}
	else if(!strcmp(cmd,"mirrornetwork_cmd"))//镜像网络
	{
		FILE *fp;
		//DefaultNICList_t DefaultNicStu;
		//int mirro_flag = 0;
		//int eth_flag[8] = {0,0,0,0,0,0,0,0};
		char mirro_buf[10];
		char msg_buf[100];
		memset(mirro_buf,0,10);
		memset(msg_buf,0,100);
		//memset(&DefaultNicStu,0,sizeof(DefaultNICList_t));
		//GetNicInfo(&DefaultNicStu);
		//eth_flag[2] = is_netline(DefaultNicStu.E_image_1);
		//eth_flag[3] = is_netline(DefaultNicStu.E_image_2);
		//eth_flag[6] = is_netline(DefaultNicStu.O_image_1);
		//eth_flag[7] = is_netline(DefaultNicStu.O_image_2);
		//if(eth_flag[2] == 1 && eth_flag[3] == 1)
			//mirro_flag = 1;
		char *independent = (char *)params_value(params,"independent");
		fp = fopen("/usr/local/tipschecker/etc/ModeByUI","r+");
		fgets(mirro_buf,100,fp);
		if(mirro_buf[strlen(mirro_buf)-1] == '\n')
			mirro_buf[strlen(mirro_buf)-1] = '\0';
		fclose(fp);
		if(!strcmp(independent,mirro_buf))
			strcpy(msg_buf,"设置失败,(原因：)工作模式与原来模式相同");
		else
		{
			fp = fopen("/usr/local/tipschecker/etc/ModeByUI","r+");
			fwrite(independent,strlen(independent),1,fp);
			fclose(fp);
			strcpy(msg_buf,"设置成功");
		}
		
		//printf("independent=====%s\n",independent);
		//exit(0);
		char *xml = (char *)g_strdup_printf("<sysMirrorNetwork><start_mirror_flag>1</start_mirror_flag><choosed>%s</choosed><msg>%s</msg></sysMirrorNetwork>",independent,msg_buf);
               	char *html = ((admin_opt_fun *)opt_fun)(credentials, xml);			
                g_free(xml);
                return html;
	}
	else if(!strcmp(cmd,"doubleengine"))//双机热备
	{
		int start_dobule_flag = 0;
		int master_slave = 0;
		FILE *fp;
		char *p = NULL,*xml=NULL;
		char master[50],slave[50];
		char buf[100],buf1[100];
		memset(buf,0,sizeof(100));
		memset(buf1,0,sizeof(100));
		memset(master,0,sizeof(master));
		memset(slave,0,sizeof(slave));
		fp = fopen("/usr/local/tipschecker/etc/master_slave","r+");
		fgets(buf,100,fp);
		if(buf[strlen(buf)-1] == '\n')
			buf[strlen(buf)-1] = '\0';
		if(1 == atoi(buf))
			master_slave = 1;	
		fclose(fp);
		memset(buf,0,sizeof(buf));
		fp = fopen("/usr/local/tipschecker/etc/workmodule.conf","r+");
		fgets(buf,100,fp);
		p =strchr(buf,'=');
		strcpy(buf1,p+1);
		if(buf1[strlen(buf1)-1] == '\n')	
			buf1[strlen(buf1)-1] = '\0';
		if(1 == atoi(buf1))
			start_dobule_flag = 1;
		fclose(fp);
		memset(buf,0,sizeof(buf));
		fp = fopen("/usr/local/tipschecker/etc/sync.conf","r+");
		while(fgets(buf,100,fp))
		{
			if(!strncmp(buf,"master",6))
			{
				p=strchr(buf,'=');
				strcpy(master,p+1);
				if(master[strlen(master)-1]=='\n')
					master[strlen(master)-1] = '\0';
			}
			if(!strncmp(buf,"slave",5))
			{
				p=strchr(buf,'=');
				strcpy(slave,p+1);
				if(slave[strlen(slave)-1] == '\n')
					slave[strlen(slave)-1] = '\0';
			}
			memset(buf,0,sizeof(buf));
		}
		fclose(fp);
		if(start_dobule_flag)
			xml = (char *)g_strdup_printf("<sysDoubleEngine><MACHINE_IP>%s</MACHINE_IP><DOUBLE_FLAG>%d</DOUBLE_FLAG><master_or_slave>%d</master_or_slave><kaiqi_flag>0</kaiqi_flag></sysDoubleEngine>",master,start_dobule_flag,master_slave);
		else
			xml = (char *)g_strdup_printf("<sysDoubleEngine><MACHINE_IP>%s</MACHINE_IP><DOUBLE_FLAG>%d</DOUBLE_FLAG><master_or_slave>%d</master_or_slave><kaiqi_flag>%d</kaiqi_flag></sysDoubleEngine>",master,start_dobule_flag,master_slave,!start_dobule_flag);
               	char *html = ((admin_opt_fun *)opt_fun)(credentials, xml);			
                g_free(xml);
                return html;
	}
	else if(!strcmp(cmd,"doubleengine_delete_cmd"))
	{
		FILE *fp = NULL;
//hyj		char *xml = NULL;
		system("/usr/local/tipschecker/bin/HA/cancelHA.sh");
		fp = fopen("/usr/local/tipschecker/etc/workmodule.conf","w+");
		fwrite("HA=1",strlen("HA=1"),1,fp);
		fclose(fp);

		fp = fopen("/usr/local/tipschecker/etc/isConfigured","w+");
		fwrite("0",1,1,fp);
		fclose(fp);

		fp = fopen("/usr/local/tipschecker/etc/master_slave","w+");
		fwrite("1",strlen((char *)"1"),1,fp);
		fclose(fp);
	
		//xml = (char *)g_strdup_printf("<sysDoubleEngine><MACHINE_IP>%s</MACHINE_IP><DOUBLE_FLAG>%d</DOUBLE_FLAG><master_or_slave>%d</master_or_slave></sysDoubleEngine>",master,start_dobule_flag,master_slave);
               	//char *html = ((admin_opt_fun *)opt_fun)(credentials, xml);			
                //g_free(xml);
                //return html;
                return NULL;
		
	}
	
	else if(!strcmp(cmd,"doubleengine_cmd"))//双机热备
	{
		char *delete_start_button = (char *)params_value(params,"delete_start_button");
		printf("delete_start_button==%s\n",delete_start_button);
		if(NULL != delete_start_button)//取消双机热备工作模式
		{
			FILE *fp = NULL;
			char *xml = NULL;
			system("/usr/local/tipschecker/bin/HA/cancelHA.sh");
			fp = fopen("/usr/local/tipschecker/etc/workmodule.conf","w+");
			fwrite("HA=1",strlen("HA=1"),1,fp);
			fclose(fp);

			fp = fopen("/usr/local/tipschecker/etc/isConfigured","w+");
			fwrite("0",1,1,fp);
			fclose(fp);

			fp = fopen("/usr/local/tipschecker/etc/master_slave","w+");
			fwrite("1",strlen((char *)"1"),1,fp);
			fclose(fp);
			xml = (char *)g_strdup_printf("<sysDoubleEngine><DOUBLE_FLAG>0</DOUBLE_FLAG><msg_machine>取消成功</msg_machine></sysDoubleEngine>");
               		char *html = ((admin_opt_fun *)opt_fun)(credentials, xml);			
                	g_free(xml);
                	return html;
		}
		else//启动双机热备工作模式
		{
			FILE *fp;
			char *p = NULL;
			char *xml = NULL;
			char master[50],slave[50];
			char buf[100];
			int change_flag = 0,master_or_slave = 0;
			memset(buf,0,sizeof(100));
			memset(master,0,sizeof(master));
			memset(slave,0,sizeof(slave));
			char *double_device=(char *)params_value(params, "double_device");
			char *machine=(char *)params_value(params,"1");
			char *machine_ip=(char *)params_value(params,"machine_ip");

			if(double_device == NULL)//未选中启动双击热备工作模式的处理方式
			{
				if(!strcmp(machine,"main"))
					master_or_slave = 1;
				else	
					master_or_slave = 0;
				fp = fopen("/usr/local/tipschecker/etc/master_slave","w+");
				if(master_or_slave)//写入主从检测器判断的配置文件,以便后来点击双机热备是进行判断
					fwrite("1",strlen((char *)"1"),1,fp);
				else
					fwrite("0",strlen((char *)"0"),1,fp);
				fclose(fp);
				xml = (char *)g_strdup_printf("<sysDoubleEngine><DOUBLE_FLAG>0</DOUBLE_FLAG><master_or_slave>%d</master_or_slave><MACHINE_IP>%s</MACHINE_IP><msg_machine>设置失败,未选中启动双击热备工作模式</msg_machine></sysDoubleEngine>",master_or_slave,machine_ip);
               			char *html = ((admin_opt_fun *)opt_fun)(credentials, xml);			
                		g_free(xml);
                		return html;
			}
			else
			{
				char double_judge_isconf[50];
				struct sockaddr_in get_tips_address;
				char eth0_ip[50];
				char MASTER[50];
				char SLAVE[50];
				memset(eth0_ip,0,sizeof(eth0_ip));
				memset(MASTER,0,sizeof(MASTER));
				memset(SLAVE,0,sizeof(SLAVE));
				memset(double_judge_isconf,0,sizeof(double_judge_isconf));
        			get_tips_address.sin_addr.s_addr = get_local_ip((char *)"eth0");
				inet_ntop(AF_INET, &(get_tips_address.sin_addr.s_addr),eth0_ip,50);
				if(-1 == (int)get_tips_address.sin_addr.s_addr)			//update by hyj
				{
					strncpy(eth0_ip,"0.0.0.0",sizeof(eth0_ip));
				}
				fp = fopen("/usr/local/tipschecker/etc/sync.conf","r+");
				while(fgets(buf,100,fp))
				{
					if(!strncmp(buf,"master",6))
					{
						p=strchr(buf,'=');
						strcpy(master,p+1);
						master[strlen(master)-1] = '\0';
					}
					if(!strncmp(buf,"slave",5))
					{
						p=strchr(buf,'=');
						strcpy(slave,p+1);
						slave[strlen(slave)-1] = '\0';
					}
					memset(buf,0,sizeof(buf));
				}
				fclose(fp);

				fp = fopen("/usr/local/tipschecker/etc/isConfigured","r+");
				fgets(double_judge_isconf,50,fp);
				if(double_judge_isconf[strlen(double_judge_isconf)-1] == '\n')
					double_judge_isconf[strlen(double_judge_isconf)-1] = '\0';
				fclose(fp);

				if(atoi(double_judge_isconf) == 1)
				{
					if(!strcmp(machine,"main"))//本机作为主检测器
					{
						master_or_slave = 1;
						if(!strcmp(machine_ip,slave))
							change_flag = 1;
					}
					else//本机作为备用检测器
					{
						master_or_slave = 0;
						if(!strcmp(machine_ip,master))
							change_flag = 1;
					}

					fp = fopen("/usr/local/tipschecker/etc/master_slave","w+");
					if(master_or_slave)//写入主从检测器判断的配置文件,以便后来点击双机热备是进行判断
						fwrite("1",strlen((char *)"1"),1,fp);
					else
						fwrite("0",strlen((char *)"0"),1,fp);
					fclose(fp);

					if(change_flag)
					{
						xml = (char *)g_strdup_printf("<sysDoubleEngine><DOUBLE_FLAG>1</DOUBLE_FLAG><master_or_slave>%d</master_or_slave><MACHINE_IP>%s</MACHINE_IP><msg_machine>设置失败，IP未变化</msg_machine></sysDoubleEngine>",master_or_slave,machine_ip);
               					char *html = ((admin_opt_fun *)opt_fun)(credentials, xml);			
                				g_free(xml);
                				return html;
					}
					else
					{
						if(master_or_slave == 1)
						{
							fp = fopen("/usr/local/tipschecker/etc/sync.conf","w+");
							sprintf(MASTER,"master=%s\n",eth0_ip);
							sprintf(SLAVE,"slave=%s\n",machine_ip);
							fwrite(MASTER,strlen(MASTER),1,fp);
							fwrite(SLAVE,strlen(SLAVE),1,fp);
							fclose(fp);
						/*本机作为主检测器，则把master=====eth0,slave=machine_ip*/
						}
						else	
						{
							fp = fopen("/usr/local/tipschecker/etc/sync.conf","w+");
							sprintf(MASTER,"master=%s\n",machine_ip);
							sprintf(SLAVE,"slave=%s\n",eth0_ip);
							fwrite(MASTER,strlen(MASTER),1,fp);
							fwrite(SLAVE,strlen(SLAVE),1,fp);
							fclose(fp);
						/*本机作备用主检测器，则把master=====machine_ip,slave=eth0*/
						}
						fp = fopen("/usr/local/tipschecker/etc/workmodule.conf","w+");
						fwrite("HA=1",strlen("HA=1"),1,fp);
						fclose(fp);


						/*启动李宁杰的双机热备*/
						//system("/usr/local/tipschecker/bin/HA/SqlConfAlter/alter");
						system("/usr/local/tipschecker/bin/HA/SqlConfAlter/alter");

						xml = (char *)g_strdup_printf("<sysDoubleEngine><DOUBLE_FLAG>1</DOUBLE_FLAG><master_or_slave>%d</master_or_slave><MACHINE_IP>%s</MACHINE_IP><msg_machine>设置成功</msg_machine></sysDoubleEngine>",master_or_slave,machine_ip);
               					char *html = ((admin_opt_fun *)opt_fun)(credentials, xml);			
                				g_free(xml);
                				return html;
					}
				}
				else
				{
					if(!strcmp(machine,"main"))//本机作为主检测器
						master_or_slave = 1;
					else//本机作为备用检测器
						master_or_slave = 0;
					if(master_or_slave == 1)//本机作为主检测器写入IP配置文件
					{
						fp = fopen("/usr/local/tipschecker/etc/sync.conf","w+");
						sprintf(MASTER,"master=%s\n",eth0_ip);
						sprintf(SLAVE,"slave=%s\n",machine_ip);
						fwrite(MASTER,strlen(MASTER),1,fp);
						fwrite(SLAVE,strlen(SLAVE),1,fp);
						fclose(fp);
					/*本机作为主检测器，则把master=====eth0,slave=machine_ip*/
					}
					else	
					{
						fp = fopen("/usr/local/tipschecker/etc/sync.conf","w+");
						sprintf(MASTER,"master=%s\n",machine_ip);
						sprintf(SLAVE,"slave=%s\n",eth0_ip);
						fwrite(MASTER,strlen(MASTER),1,fp);
						fwrite(SLAVE,strlen(SLAVE),1,fp);
						fclose(fp);
					/*本机作备用主检测器，则把master=====machine_ip,slave=eth0*/
					}

					fp = fopen("/usr/local/tipschecker/etc/master_slave","w+");
					if(master_or_slave)//写入主从检测器判断的配置文件,以便后来点击双机热备是进行判断
						fwrite("1",strlen((char *)"1"),1,fp);
					else
						fwrite("0",strlen((char *)"0"),1,fp);
					fclose(fp);

					fp = fopen("/usr/local/tipschecker/etc/isConfigured","w+");
					fwrite("1",1,1,fp);
					fclose(fp);

					fp = fopen("/usr/local/tipschecker/etc/workmodule.conf","w+");
					fwrite("HA=1",strlen("HA=1"),1,fp);
					fclose(fp);


						/*启动李宁杰的双机热备*/
						system("/usr/local/tipschecker/bin/HA/Require/GetCompanion");
						

					xml = (char *)g_strdup_printf("<sysDoubleEngine><DOUBLE_FLAG>1</DOUBLE_FLAG><master_or_slave>%d</master_or_slave><MACHINE_IP>%s</MACHINE_IP><msg_machine>设置成功</msg_machine></sysDoubleEngine>",master_or_slave,machine_ip);
               				char *html = ((admin_opt_fun *)opt_fun)(credentials, xml);			
                			g_free(xml);
                			return html;
				}
			}

#if 0 
		fp = fopen("/usr/local/tipschecker/etc/sync.conf","r+");
		while(fgets(buf,100,fp))
		{
			if(!strncmp(buf,"master",6))
			{
				p=strchr(buf,'=');
				strcpy(master,p+1);
				master[strlen(master)-1] = '\0';
			}
			if(!strncmp(buf,"slave",5))
			{
				p=strchr(buf,'=');
				strcpy(slave,p+1);
				slave[strlen(slave)-1] = '\0';
			}
			memset(buf,0,sizeof(buf));
		}
		fclose(fp);
		if(double_device == NULL)//未选中启动双击热备工作模式的处理方式
		{
			//fp = fopen("/usr/local/tipschecker/etc/workmodule.conf","w+");
			//fwrite("HA=0",strlen("HA=0"),1,fp);
			//fclose(fp);
			if(!strcmp(machine,"main"))
				master_or_slave = 1;
			else	
				master_or_slave = 0;
			fp = fopen("/usr/local/tipschecker/etc/master_slave","w+");
			if(master_or_slave)//写入主从检测器判断的配置文件,以便后来点击双机热备是进行判断
				fwrite("1",strlen((char *)"1"),1,fp);
			else
				fwrite("0",strlen((char *)"0"),1,fp);
			fclose(fp);
			xml = (char *)g_strdup_printf("<sysDoubleEngine><DOUBLE_FLAG>0</DOUBLE_FLAG><master_or_slave>%d</master_or_slave><MACHINE_IP>%s</MACHINE_IP><msg_machine>设置失败,未选中启动双击热备工作模式</msg_machine></sysDoubleEngine>",master_or_slave,machine_ip);
               		char *html = ((admin_opt_fun *)opt_fun)(credentials, xml);			
                	g_free(xml);
                	return html;
		}
		else
		{
			char double_judge_HA[50];
			char double_judge_HA_write[50];
			memset(double_judge_HA,0,sizeof(double_judge_HA));
			memset(double_judge_HA_write,0,sizeof(double_judge_HA_write));
			if(!strcmp(machine,"main"))
			{
				master_or_slave = 1;
				if(!strcmp(machine_ip,slave))
					change_flag = 1;
			}
			else//本机作为备用检测器
			{
				master_or_slave = 0;
				if(!strcmp(machine_ip,master))
					change_flag = 1;
			}
			fp = fopen("/usr/local/tipschecker/etc/master_slave","w+");
			if(master_or_slave)//写入主从检测器判断的配置文件,以便后来点击双机热备是进行判断
				fwrite("1",strlen((char *)"1"),1,fp);
			else
				fwrite("0",strlen((char *)"0"),1,fp);
			fclose(fp);

			fp = fopen("/usr/local/tipschecker/etc/workmodule.conf","r+");
			fgets(double_judge_HA,50,fp);
			p=strchr(double_judge_HA,'=');
			strcpy(double_judge_HA_write,p+1);
			if(double_judge_HA_write[strlen(double_judge_HA_write)-1] == '\n')
				double_judge_HA_write[strlen(double_judge_HA_write)-1] = '\0';
			fclose(fp);

			if(atoi(double_judge_HA_write) == 0)
			fwrite("HA=1",strlen("HA=1"),1,fp);
			fclose(fp);
			printf("444444444444444444444444444444444444444\n");
			if(change_flag)
			{
				xml = (char *)g_strdup_printf("<sysDoubleEngine><DOUBLE_FLAG>1</DOUBLE_FLAG><master_or_slave>%d</master_or_slave><MACHINE_IP>%s</MACHINE_IP><msg_machine>设置失败，IP未变化</msg_machine></sysDoubleEngine>",master_or_slave,machine_ip);
			}
			else
			{
			printf("5555555555555555555555555555\n");
				struct sockaddr_in get_tips_address;
				char eth0_ip[50];
				char MASTER[50];
				char SLAVE[50];
				memset(eth0_ip,0,sizeof(eth0_ip));
				memset(MASTER,0,sizeof(MASTER));
				memset(SLAVE,0,sizeof(SLAVE));
        			get_tips_address.sin_addr.s_addr = get_local_ip((char *)"eth0");
				inet_ntop(AF_INET, &(get_tips_address.sin_addr.s_addr),eth0_ip,50);
				if(-1 == get_tips_address.sin_addr.s_addr)
				{
					strncpy(eth0_ip,"0.0.0.0",sizeof(eth0_ip));
				}
				fp = fopen("/usr/local/tipschecker/etc/sync.conf","w+");
			printf("6666666666666666666\n");
				if(master_or_slave == 1)
				{
			printf("888888888888888888888\n");
					sprintf(MASTER,"master=%s\n",eth0_ip);
					sprintf(SLAVE,"slave=%s\n",machine_ip);
					fwrite(MASTER,strlen(MASTER),1,fp);
					fwrite(SLAVE,strlen(SLAVE),1,fp);
					fclose(fp);
				/*本机作为主检测器，则把master=====eth0,slave=machine_ip*/
				}
				else	
				{
			printf("9999999999999999999\n");
					sprintf(MASTER,"master=%s\n",machine_ip);
					sprintf(SLAVE,"slave=%s\n",eth0_ip);
					fwrite(MASTER,strlen(MASTER),1,fp);
					fwrite(SLAVE,strlen(SLAVE),1,fp);
					fclose(fp);
				/*本机作备用主检测器，则把master=====machine_ip,slave=eth0*/
				}
				/*启动李宁杰的双机热备*/
				//system("/usr/local/tipschecker/bin/HA/SqlConfAlter/alter");
			printf("7777777777777777777\n");
				xml = (char *)g_strdup_printf("<sysDoubleEngine><DOUBLE_FLAG>1</DOUBLE_FLAG><master_or_slave>%d</master_or_slave><MACHINE_IP>%s</MACHINE_IP><msg_machine>设置成功</msg_machine></sysDoubleEngine>",master_or_slave,machine_ip);
			}
               		char *html = ((admin_opt_fun *)opt_fun)(credentials, xml);			
                	g_free(xml);
                	return html;
		}
#endif
		}
	}
	else if(!strcmp(cmd,"sitestatus"))//系统监控-网络状态
	{
		char mirror_state[50];
		char mirror_state1[50];
		int int_mirror_state = 0,int_mirror_state1 = 0;
		char judge_state[10];
		int main_center_state = 0;
		int deputy_center_state = 0;
		char main_center[30];
		char deputy_center[30];
		char slave_state[76];//备用检测器的状态信息
		int mirror1_mode=0,mirror2_mode=0;
		char mirror2_mode_buf[30];
		double mirror1_flow = 0,mirror2_flow = 0;
		int mirror1_active_peer = 0,mirror2_active_peer = 0,mirror1_session = 0,mirror2_session = 0;
		config_manage_t sconfig;
//hyj		struct sockaddr_in get_tips_address;
		char eth0_ip[50];
		char eth1_ip[50];
		double max_flow = 0;
		int max_user = 0,max_link = 0;
		memset(main_center,0,sizeof(main_center));
		memset(deputy_center,0,sizeof(deputy_center));
		memset(mirror2_mode_buf,0,sizeof(mirror2_mode_buf));
		memset(eth0_ip,0,sizeof(eth0_ip));
		memset(eth1_ip,0,sizeof(eth1_ip));
		memset(slave_state,0,sizeof(slave_state));
		memset(&sconfig,0,sizeof(config_manage_t));
		memset(mirror_state,0,sizeof(mirror_state));
		memset(mirror_state1,0,sizeof(mirror_state1));
		memset(judge_state,0,sizeof(judge_state));
        	/*是否需要判断返回值？int format =*/ get_config_manage_xml(&sconfig); 
#if 0
        	get_tips_address.sin_addr.s_addr = get_local_ip((char *)"eth0");
		inet_ntop(AF_INET, &(get_tips_address.sin_addr.s_addr),eth0_ip,50);
		if(-1 == get_tips_address.sin_addr.s_addr)
		{
			strncpy(eth0_ip,"0.0.0.0",sizeof(eth0_ip));
		}
        	get_tips_address.sin_addr.s_addr = get_local_ip((char *)"eth1");
		inet_ntop(AF_INET, &(get_tips_address.sin_addr.s_addr),eth1_ip,50);
		if(-1 == get_tips_address.sin_addr.s_addr)
		{
			strncpy(eth1_ip,"0.0.0.0",sizeof(eth1_ip));
		}
		FILE *fp = popen ("cat /sys/class/net/eth2/carrier", "r");
		fgets(judge_state,10,fp);
		judge_state[strlen(judge_state)-1] = '\0';
		if(!strcmp(judge_state,"0"))
			strcpy(mirror_state,"网线未连接");
		else
			strcpy(mirror_state,"连接正常");

		memset(judge_state,0,sizeof(judge_state));
		fp = popen ("cat /sys/class/net/eth3/carrier", "r");
		fgets(judge_state,10,fp);
		judge_state[strlen(judge_state)-1] = '\0';
		if(!strcmp(judge_state,"0"))
			strcpy(mirror_state1,"网线未连接");
		else
			strcpy(mirror_state1,"连接正常");
		pclose(fp);
#endif
		//obj.Get_status_shm(6,&int_mirror_state,sizeof(int));//镜像口1的连接状态
		//if(int_mirror_state == 1)
		//	strcpy(mirror_state,"连接正常");
		//else
		//	strcpy(mirror_state,"未连接");
		//obj.Get_status_shm(10,&int_mirror_state1,sizeof(int));//镜像口2的连接状态
		//if(int_mirror_state1 == 1)
		//	strcpy(mirror_state1,"连接正常");
		//else
		//	strcpy(mirror_state1,"未连接");
		//obj.Get_status_shm(5,&mirror1_mode,sizeof(int));//镜像口的状态
		//obj.Get_status_shm(7,&mirror1_flow,sizeof(double));
		//obj.Get_status_shm(8,&mirror1_active_peer,sizeof(int));
		//obj.Get_status_shm(9,&mirror1_session,sizeof(int));
		//obj.Get_status_shm(5,&mirror2_mode,sizeof(int));
		//if(mirror2_mode == 3)
		//	strcpy(mirror2_mode_buf,"独立模式");
		//else
		//	strcpy(mirror2_mode_buf,"保护模式");
		//obj.Get_status_shm(11,&mirror2_flow,sizeof(double));
		//obj.Get_status_shm(12,&mirror2_active_peer,sizeof(int));
		//obj.Get_status_shm(13,&mirror2_session,sizeof(int));
		//obj.Get_status_shm(17,&main_center_state,sizeof(int));//主中心连接状态
		//if(main_center_state == 1)
		//	strcpy(main_center,"连接正常");
		//else if(main_center_state == 2)
		//	strcpy(main_center,"连接不正常");
		//else if(main_center_state == 3)
		//	strcpy(main_center,"网关不可达");
		//else
		//	strcpy(main_center,"网线接入异常");
		//obj.Get_status_shm(18,&deputy_center_state,sizeof(int));//副中心连接状态
		//if(deputy_center_state == 1)
		//	strcpy(deputy_center,"连接正常");
		//else if(deputy_center_state == 2)
		//	strcpy(deputy_center,"连接不正常");
		//else if(deputy_center_state == 3)
		//	strcpy(deputy_center,"网关不可达");
		//else
		//	strcpy(deputy_center,"网线接入异常");

		obj.Get_status_shm(5,&mirror1_mode,sizeof(int));
		obj.Get_status_shm(7,&mirror1_flow,sizeof(double));
		obj.Get_status_shm(8,&mirror1_active_peer,sizeof(int));
		obj.Get_status_shm(45,&max_flow,sizeof(double));
		obj.Get_status_shm(46,&max_user,sizeof(int));
		obj.Get_status_shm(47,&max_link,sizeof(int));
		obj.Get_status_shm(9,&mirror1_session,sizeof(int));
		obj.Get_status_shm(6,&int_mirror_state,sizeof(int));//镜像口1的连接状态
		if(int_mirror_state == 1)
			strcpy(mirror_state,"连接正常");
		else
			strcpy(mirror_state,"未连接");
		obj.Get_status_shm(10,&int_mirror_state1,sizeof(int));//镜像口2的连接状态
		if(int_mirror_state1 == 1)
			strcpy(mirror_state1,"连接正常");
		else
			strcpy(mirror_state1,"未连接");
		obj.Get_status_shm(5,&mirror2_mode,sizeof(int));
		if(mirror2_mode == 3)
			strcpy(mirror2_mode_buf,"独立模式");
		else if(mirror2_mode == 2)
			strcpy(mirror2_mode_buf,"保护模式");
		else
			strcpy(mirror2_mode_buf,"不工作");
		obj.Get_status_shm(11,&mirror2_flow,sizeof(double));
		obj.Get_status_shm(12,&mirror2_active_peer,sizeof(int));
		obj.Get_status_shm(13,&mirror2_session,sizeof(int));
		obj.Get_status_shm(17,&main_center_state,sizeof(int));//主中心连接状态
		if(main_center_state == 1)
			strcpy(main_center,"连接正常");
		else if(main_center_state == 2)
			strcpy(main_center,"连接不正常");
		else if(main_center_state == 3)
			strcpy(main_center,"网关不可达");
		else
			strcpy(main_center,"网线接入异常");
		obj.Get_status_shm(18,&deputy_center_state,sizeof(int));//副中心连接状态
		if(deputy_center_state == 1)
			strcpy(deputy_center,"连接正常");
		else if(deputy_center_state == 2)
			strcpy(deputy_center,"连接不正常");
		else if(deputy_center_state == 3)
			strcpy(deputy_center,"网关不可达");
		else
			strcpy(deputy_center,"网线接入异常");
		//obj.Get_status_shm(17,&main_center_state,1);//主中心连接状态
		//obj.Get_status_shm(18,&deputy_center_state,1);//副中心连接状态


/*调用李宁杰的可执行程序，往共享内存里边写备用检测器的状态，之后获取共享内存数据并进行强制转换*/
	//	obj.Get_status_shm(43,&slave_state,76);//备用检测器连接状态
		//struct slave_status *Slave_Stat11 = (struct slave_status *)slave_state;
		//struct slave_status Slave_Stat2;
		//struct slave_status *Slave_Stat = &Slave_Stat2;
		


		//char *xml = (char *)g_strdup_printf("<sysSiteStatus><MIRRROR1_MODE>%c</MIRRROR1_MODE><MIRRROR2_MODE>%c</MIRRROR2_MODE><MIRROR1_FLOW>%0.2f</MIRROR1_FLOW><MIRROR2_FLOW>%0.2f</MIRROR2_FLOW><MIRROR1_ACTIVE_PEER>%d</MIRROR1_ACTIVE_PEER><MIRROR2_ACTIVE_PEER>%d</MIRROR2_ACTIVE_PEER><MIRROR1_SESSION>%d</MIRROR1_SESSION><MIRROR2_SESSION>%d</MIRROR2_SESSION><MIRROR_STATE>%s</MIRROR_STATE><MIRROR_STATE1>%s</MIRROR_STATE1><MANAGE_IP>%s</MANAGE_IP><NET_IP>%s</NET_IP><ADMINSTRATION_IP>%s</ADMINSTRATION_IP><ADMINSTRATION_IP1>%s</ADMINSTRATION_IP1><SUPERVISOR>%s</SUPERVISOR><managerPortIp>%s</managerPortIp><managerPort>%c</managerPort><ConnectPortIp>%s</ConnectPortIp><connectPortStatus>%c</connectPortStatus><mirrorPortStatus>%c</mirrorPortStatus><datasize>%d</datasize><connectNumer>%d</connectNumer></sysSiteStatus>",mirror1_mode,mirror2_mode,mirror1_flow,mirror2_flow,mirror1_active_peer,mirror2_active_peer,mirror1_session,mirror2_session,mirror_state,mirror_state1,eth0_ip,eth1_ip,sconfig.adminstration_IP,sconfig.adminstration_IP1,sconfig.supervisor,Slave_Stat->managerPortIp,Slave_Stat->managerPort,Slave_Stat->ConnectPortIp,Slave_Stat->connectPortStatus,Slave_Stat->mirrorPortStatus,Slave_Stat->datasize,Slave_Stat->connectNumer);
		char *xml = (char *)g_strdup_printf("<sysSiteStatus><MIRRROR1_MODE>%d</MIRRROR1_MODE><MIRRROR2_MODE>%s</MIRRROR2_MODE><MIRROR1_FLOW>%0.2f</MIRROR1_FLOW><MIRROR2_FLOW>%0.2f</MIRROR2_FLOW><MIRROR1_ACTIVE_PEER>%d</MIRROR1_ACTIVE_PEER><MIRROR2_ACTIVE_PEER>%d</MIRROR2_ACTIVE_PEER><MIRROR1_SESSION>%d</MIRROR1_SESSION><MIRROR2_SESSION>%d</MIRROR2_SESSION><MIRROR_STATE>%s</MIRROR_STATE><MIRROR_STATE1>%s</MIRROR_STATE1><MANAGE_IP>%s</MANAGE_IP><NET_IP>%s</NET_IP><ADMINSTRATION_IP>%s</ADMINSTRATION_IP><ADMINSTRATION_IP1>%s</ADMINSTRATION_IP1><SUPERVISOR>%s</SUPERVISOR><managerPortIp>李宁杰待写入共享内存</managerPortIp><managerPort>0</managerPort><ConnectPortIp>李宁杰待写入共享内存</ConnectPortIp><connectPortStatus>0</connectPortStatus><mirrorPortStatus>0</mirrorPortStatus><datasize>0</datasize><connectNumer>0</connectNumer><main_center_state>%s</main_center_state><deputy_center_state>%s</deputy_center_state><max_flow>%0.2f</max_flow><max_user>%d</max_user><max_link>%d</max_link></sysSiteStatus>",mirror1_mode,mirror2_mode_buf,mirror1_flow,mirror2_flow,mirror1_active_peer,mirror2_active_peer,mirror1_session,mirror2_session,mirror_state,mirror_state1,sconfig.manage_IP,sconfig.net_IP,sconfig.adminstration_IP,sconfig.adminstration_IP1,sconfig.supervisor,main_center,deputy_center,max_flow,max_user,max_link);
               	char *html = ((admin_opt_fun *)opt_fun)(credentials, xml);			
                g_free(xml);
                return html;
	}
	else if(!strcmp(cmd,"modestatus"))//系统监控-模块
	{
		printf("woowwoocacacaocao\n");
		char alarm_run_buf[10],audit_run_buf[10],senst_run_buf[10],block_run_buf[10];
		char alarm_run = '0',audit_run = '0',senst_run = '0',block_run = '0';
		memset(alarm_run_buf,0,sizeof(alarm_run_buf));
		memset(audit_run_buf,0,sizeof(audit_run_buf));
		memset(senst_run_buf,0,sizeof(senst_run_buf));
		memset(block_run_buf,0,sizeof(block_run_buf));
		obj.Get_status_shm(1,&alarm_run,sizeof(char));//木马模块运行状态
		if(alarm_run == '1')
			strcpy(alarm_run_buf,"正常");
		else
			strcpy(alarm_run_buf,"故障");
		obj.Get_status_shm(2,&audit_run,sizeof(char));//网络行为审计模块运行状态
		if(audit_run == '1')
			strcpy(audit_run_buf,"正常");
		else
			strcpy(audit_run_buf,"故障");
		obj.Get_status_shm(3,&senst_run,sizeof(char));//网络行为审计模块运行状态
		if(senst_run == '1')
			strcpy(senst_run_buf,"正常");
		else
			strcpy(senst_run_buf,"故障");
		obj.Get_status_shm(4,&block_run,sizeof(char));//窃密阻断模块
		if(block_run == '1')
			strcpy(block_run_buf,"正常");
		else
			strcpy(block_run_buf,"故障");
		char *xml = (char *)g_strdup_printf("<sysModeStatus><ALARM_RUN>%s</ALARM_RUN><AUDIT_RUN>%s</AUDIT_RUN><SENST_RUN>%s</SENST_RUN><BLOCK_RUN>%s</BLOCK_RUN></sysModeStatus>",alarm_run_buf,audit_run_buf,senst_run_buf,block_run_buf);
		printf("111111111111woowwoocacacaocao\n");
               	char *html = ((admin_opt_fun *)opt_fun)(credentials, xml);			
		printf("111111111111woowwoocacacaocao\n");
                g_free(xml);
                return html;
	}
	else if(!strcmp(cmd,"enginestatus"))//系统监控-设备
	{
       		 struct status_struct status;
		 system_config_t tips_system_config;
       		 TipsManager tips;
		 memset(&tips_system_config, 0, sizeof(tips_system_config));
		 /*是否需要判断返回值int judge =*/ get_system_config_xml(&tips_system_config);
       		 tips.status_Init(&status);
		 if(status.cpu == 0)
			status.cpu = 2;
		 int cpucolor  = (atoi(tips_system_config.cpu)  < status.cpu  ? 1 : 0);
                 int memcolor  = (atoi(tips_system_config.mem)  < status.mem  ? 1 : 0);
                 int diskcolor = (atoi(tips_system_config.disk) > status.disk ? 1 : 0);
                 char *xml = (char *)g_strdup_printf("<sysEngineStatus><cpu>%d</cpu><mem>%d</mem><disk>%0.2f</disk><cpucolor>%d</cpucolor><memcolor>%d</memcolor><diskcolor>%d</diskcolor></sysEngineStatus>",status.cpu,status.mem,status.disk,cpucolor,memcolor,diskcolor);
		//char *xml = (char *)g_strdup_printf("<sysEngineStatus></sysEngineStatus>");
               	char *html = ((admin_opt_fun *)opt_fun)(credentials, xml);			
                g_free(xml);
                return html;
	}
	else if(!strcmp(cmd,"systemstatus"))//系统监控-系统状态
	{
#if 0
 		char timebuf[100];
        	memset(timebuf,0,100);
        	struct tm *tm;
        	time_t clock;
        	clock = time(NULL);
		char *format=(char *)"%Y-%m-%d %H:%M:%S";
		strftime(timebuf,100,format,tm);
		printf("timebuf ==%s\n",timebuf);
#endif
		product_INFOSTATE product_state;
		char *xml = NULL;
 		char timebuf[100];
		char temp_nowtimebuf[100];
		char temp_starttimebuf[100];
		char start_device_time[21];
		char uptime_buf[50];
       	time_t clock;
		memset(&product_state,0,sizeof(product_INFOSTATE));
       	memset(timebuf,0,100);
		memset(uptime_buf,0,sizeof(uptime_buf));
		memset(temp_nowtimebuf,0,sizeof(temp_nowtimebuf));
		memset(temp_starttimebuf,0,sizeof(temp_starttimebuf));
       	memset(start_device_time,0,sizeof(start_device_time));
		obj.Get_status_shm(14,&start_device_time,21);
       	clock = time(NULL);
							//sprintf(stime,"%d-%02d-%02d-00-00-00",cmp[0],cmp[1],cmp[2]);
							//sprintf(etime,"%d-%02d-%02d-00-00-00",cmp[3],cmp[4],cmp[5]);
							//							
							//time_t Stime = MyObtainTime(stime);
							//time_t Etime = MyObtainTime(etime);
        //struct tm *tm;
        //char *format=(char *)"%Y-%m-%d %H:%M:%S";
        //tm = localtime(&clock);
        //strftime(timebuf, 100, format,tm);
		MyTurnTime(clock,timebuf,100);
		get_product_state(&product_state);
		if(strlen(start_device_time) == 0 || start_device_time == NULL)
			xml = (char *)g_strdup_printf("<sysSystemStatus><NAME>%s</NAME><MODE>%s</MODE><CODE>%s</CODE><VERSION>%s</VERSION><TROJAN_NEWTIME>%s</TROJAN_NEWTIME><POLICY_NEWTIME>%s</POLICY_NEWTIME><APPLY_TIME>%s</APPLY_TIME><END_TIME>%s</END_TIME><AUTHORIZATION_TYPE>%s</AUTHORIZATION_TYPE><NOW_TIME>%s</NOW_TIME><START_DEVICE_TIME>%s</START_DEVICE_TIME><RUN_TIME>0天0时0分</RUN_TIME></sysSystemStatus>",product_state.name,product_state.mode,product_state.code,product_state.version,product_state.trojan_newtime,product_state.policy_newtime,product_state.apply_time,product_state.end_time,product_state.authorization_type,timebuf,timebuf);
		else
		{
			int updays,uphours,upminutes;
			int start_year=0,start_month = 0,start_day = 0, start_hour = 0,start_min = 0,start_sec = 0;
          		int end_year=0,end_month = 0,end_day = 0,end_hour = 0,end_min = 0,end_sec = 0;
			strcpy(temp_nowtimebuf,timebuf);
			strcpy(temp_starttimebuf,start_device_time);
			sscanf(temp_starttimebuf,"%d-%d-%d %d:%d:%d",&start_year,&start_month,&start_day,&start_hour,&start_min,&start_sec);
			sscanf(temp_nowtimebuf,"%d-%d-%d %d:%d:%d",&end_year,&end_month,&end_day,&end_hour,&end_min,&end_sec);

			memset(temp_nowtimebuf,0,sizeof(temp_nowtimebuf));
			memset(temp_starttimebuf,0,sizeof(temp_starttimebuf));
			sprintf(temp_starttimebuf,"%d-%02d-%02d-%02d-%02d-%02d",start_year,start_month,start_day,start_hour,start_min,start_sec);
			sprintf(temp_nowtimebuf,"%d-%02d-%02d-%02d-%02d-%02d",end_year,end_month,end_day,end_hour,end_min,end_sec);
			time_t Etime = MyObtainTime(temp_starttimebuf);
			time_t Stime = MyObtainTime(temp_nowtimebuf);//当前时间
			time_t uptime_sec = Stime - Etime;
			if(uptime_sec > 0)
			{
				updays = (int)uptime_sec / (60*60*24);
  				upminutes = (int) uptime_sec / 60;
  				uphours = upminutes / 60;
  				uphours = uphours % 24;
  				upminutes = upminutes % 60;
				sprintf(uptime_buf,"%d天%d时%d分",updays,uphours,upminutes);
			}
			else
				sprintf(uptime_buf,"%d天%d时%d分",0,0,0);
			xml = (char *)g_strdup_printf("<sysSystemStatus><NAME>%s</NAME><MODE>%s</MODE><CODE>%s</CODE><VERSION>%s</VERSION><TROJAN_NEWTIME>%s</TROJAN_NEWTIME><POLICY_NEWTIME>%s</POLICY_NEWTIME><APPLY_TIME>%s</APPLY_TIME><END_TIME>%s</END_TIME><AUTHORIZATION_TYPE>%s</AUTHORIZATION_TYPE><NOW_TIME>%s</NOW_TIME><START_DEVICE_TIME>%s</START_DEVICE_TIME><RUN_TIME>%s</RUN_TIME></sysSystemStatus>",product_state.name,product_state.mode,product_state.code,product_state.version,product_state.trojan_newtime,product_state.policy_newtime,product_state.apply_time,product_state.end_time,product_state.authorization_type,timebuf,start_device_time,uptime_buf);
		}
		//char *xml = (char *)g_strdup_printf("<sysSystemStatus></sysSystemStatus>");
               	char *html = ((admin_opt_fun *)opt_fun)(credentials, xml);			
                g_free(xml);
                return html;
	}
        	else if(!strcmp(cmd, "get_sysSetting")){
#if 1
                                  system_config_t tips_system_config;
                                  memset(&tips_system_config, 0, sizeof(tips_system_config));
					get_system_config_xml(&tips_system_config);
			
	   //             char *xml = (char *)g_strdup_printf("<sysSetting><cpucolor>%d</cpucolor><memcolor>%d</memcolor><diskcolor>%d</diskcolor><cpu><![CDATA[%s]]></cpu><mem><![CDATA[%s]]></mem><disk><![CDATA[%s]]></disk><runtime><![CDATA[%s]]></runtime><alarmtime><![CDATA[%s]]></alarmtime><dstart>%s</dstart><dcpu>%d\%</dcpu><dmem>%d\%</dmem><ddisk>%0.2lfG</ddisk><dmem>%d</dmem><dmirror>%s</dmirror><dflow>%dkbps</dflow><dattach>%s</dattach><dnetlog>%s</dnetlog><dsen>%s</dsen><dblock>%s</dblock></sysSetting>",cpucolor, memcolor, diskcolor, tips_system_config.cpu, tips_system_config.mem,tips_system_config.disk, tips_system_config.runtime, tips_system_config.alarmtime,devicesta.stime,devicesta.cpu, devicesta.mem,devicesta.disk, devicesta.mem,devicemirror[(int)(devicesta.mirror)-48], devicesta.flow, devicestatue[(int)(devicesta.module_state[0])-48], devicestatue[(int)(devicesta.module_state[1])-48],devicestatue[(int)(devicesta.module_state[2])-48],devicestatue[(int)(devicesta.module_state[3])-48]);
                char *xml = (char *)g_strdup_printf("<sysSetting><cpu>%s</cpu><mem>%s</mem><disk>%s</disk><runtime>%s</runtime><alarmtime>%s</alarmtime></sysSetting>",tips_system_config.cpu,tips_system_config.mem,tips_system_config.disk,tips_system_config.runtime,tips_system_config.syntime);
            //    char *xml = (char *)g_strdup_printf("<sysSetting></sysSetting>");
#endif
			//puts(xml);
			//exit(0);
               		char *html = ((admin_opt_fun *)opt_fun)(credentials, xml);			
                	g_free(xml);
			printf("=========01%s\n",html);
                	return html;
        	}
                 else if(!strcmp(cmd, "debuging")){
                                  char* devicestatue[4] = {(char *)"功能未定义", (char *)"功能运行正常", (char *)"功能运行不正常", (char *)"功能未运行"};
                                  char* devicemirror[2] = {(char *)"未连接", (char *)"已连接"};
								  char *xml = NULL;
                                  system_config_t tips_system_config;
 
 
                                  status_struct_t devicesta;
                                  memset(&devicesta, 0, sizeof(devicesta));
                                  config_get_device(&devicesta);
								  get_system_config_xml(&tips_system_config);
                                  int cpucolor  = (atoi(tips_system_config.cpu)  < devicesta.cpu  ? 1 : 0);
                                  int memcolor  = (atoi(tips_system_config.mem)  < devicesta.mem  ? 1 : 0);
                                  int diskcolor = (atoi(tips_system_config.disk) < devicesta.disk ? 1 : 0);
								  int ret = system((const char *)("./sshShell.sh"));
								  if(-1 == ret)
									xml = (char *)g_strdup_printf("<product><debug>远程调试失败</debug><cpucolor>%d</cpucolor><memcolor>%d</memcolor><diskcolor>%d</diskcolor><dstart>%s</dstart><dcpu>%d%%</dcpu><dmem>%d%%</dmem><ddisk>%0.2lfG</ddisk><dmirror>%s</dmirror><dflow>%dkbps</dflow><dattach>%s</dattach><dnetlog>%s</dnetlog><dsen>%s</dsen><dblock>%s</dblock></product>",cpucolor, memcolor, diskcolor,devicesta.stime,devicesta.cpu, devicesta.mem,devicesta.disk, devicemirror[(int)(devicesta.mirror)-48], devicesta.flow, devicestatue[(int)(devicesta.module_state[0])-48], devicestatue[(int)(devicesta.module_state[1])-48],devicestatue[(int)(devicesta.module_state[2])-48],devicestatue[(int)(devicesta.module_state[3])-48]);
								  else
									xml = (char *)g_strdup_printf("<product><debug>远程调试中**************</debug><cpucolor>%d</cpucolor><memcolor>%d</memcolor><diskcolor>%d</diskcolor><dstart>%s</dstart><dcpu>%d%%</dcpu><dmem>%d%%</dmem><ddisk>%0.2lfG</ddisk><dmirror>%s</dmirror><dflow>%dkbps</dflow><dattach>%s</dattach><dnetlog>%s</dnetlog><dsen>%s</dsen><dblock>%s</dblock></product>",cpucolor, memcolor, diskcolor,devicesta.stime,devicesta.cpu, devicesta.mem,devicesta.disk, devicemirror[(int)(devicesta.mirror)-48], devicesta.flow, devicestatue[(int)(devicesta.module_state[0])-48], devicestatue[(int)(devicesta.module_state[1])-48],devicestatue[(int)(devicesta.module_state[2])-48],devicestatue[(int)(devicesta.module_state[3])-48]);
	               // char *xml = (char *)g_strdup_printf("<product></product>");
					//			char *xml = (char *)g_strdup_printf("<product><debug>1</debug><cpucolor>%d</cpucolor><memcolor>%d</memcolor><diskcolor>%d</diskcolor><cpu><![CDATA[%s]]></cpu><mem><![CDATA[%s]]></mem><disk><![CDATA[%s]]></disk><runtime><![CDATA[%s]]></runtime><alarmtime><![CDATA[%s]]></alarmtime><dstart>%s</dstart><dcpu>%d\%</dcpu><dmem>%d\%</dmem><ddisk>%0.2lfG</ddisk><dmem>%d</dmem><dmirror>%s</dmirror><dflow>%dkbps</dflow><dattach>%s</dattach><dnetlog>%s</dnetlog><dsen>%s</dsen><dblock>%s</dblock></product>",cpucolor, memcolor, diskcolor, tips_system_config.cpu, tips_system_config.mem,tips_system_config.disk, tips_system_config.runtime, tips_system_config.alarmtime,devicesta.stime,devicesta.cpu, devicesta.mem,devicesta.disk, devicesta.mem,devicemirror[(int)(devicesta.mirror)-48], devicesta.flow, devicestatue[(int)(devicesta.module_state[0])-48], devicestatue[(int)(devicesta.module_state[1])-48],devicestatue[(int)(devicesta.module_state[2])-48],devicestatue[(int)(devicesta.module_state[3])-48]);
                                  printf("xml in get_config:%s\n",xml);
                                  char *html = ((admin_opt_fun *)opt_fun)(credentials, xml);
                                  g_free(xml);
                                  return html;
                  }
                 else if(!strcmp(cmd, "debuging_1")){
 
                                  system_config_t tips_system_config;
                                  memset(&tips_system_config, 0, sizeof(tips_system_config));
								  char *xml = NULL;
					get_system_config_xml(&tips_system_config);
								  int ret = system((const char *)("./sshShell.sh"));
								  if(-1 == ret)
									xml = (char *)g_strdup_printf("<sysSetting><debug>远程调试失败</debug><cpu>%s</cpu><mem>%s</mem><disk>%s</disk><runtime>%s</runtime><alarmtime>%s</alarmtime></sysSetting>",tips_system_config.cpu,tips_system_config.mem,tips_system_config.disk,tips_system_config.runtime,tips_system_config.alarmtime);
								  else
                					xml = (char *)g_strdup_printf("<sysSetting><debug>远程调试中**************</debug><cpu>%s</cpu><mem>%s</mem><disk>%s</disk><runtime>%s</runtime><alarmtime>%s</alarmtime></sysSetting>",tips_system_config.cpu,tips_system_config.mem,tips_system_config.disk,tips_system_config.runtime,tips_system_config.alarmtime);
            //    char *xml = (char *)g_strdup_printf("<sysSetting><debug>远程调试中**************</debug></sysSetting>");
	               // char *xml = (char *)g_strdup_printf("<sysSetting><debug>1</debug><cpucolor>%d</cpucolor><memcolor>%d</memcolor><diskcolor>%d</diskcolor><cpu><![CDATA[%s]]></cpu><mem><![CDATA[%s]]></mem><disk><![CDATA[%s]]></disk><runtime><![CDATA[%s]]></runtime><alarmtime><![CDATA[%s]]></alarmtime><dstart>%s</dstart><dcpu>%d\%</dcpu><dmem>%d\%</dmem><ddisk>%0.2lfG</ddisk><dmem>%d</dmem><dmirror>%s</dmirror><dflow>%dkbps</dflow><dattach>%s</dattach><dnetlog>%s</dnetlog><dsen>%s</dsen><dblock>%s</dblock></sysSetting>",cpucolor, memcolor, diskcolor, tips_system_config.cpu, tips_system_config.mem,tips_system_config.disk, tips_system_config.runtime, tips_system_config.alarmtime,devicesta.stime,devicesta.cpu, devicesta.mem,devicesta.disk, devicesta.mem,devicemirror[(int)(devicesta.mirror)-48], devicesta.flow, devicestatue[(int)(devicesta.module_state[0])-48], devicestatue[(int)(devicesta.module_state[1])-48],devicestatue[(int)(devicesta.module_state[2])-48],devicestatue[(int)(devicesta.module_state[3])-48]);
                   //      char *xml = (char *)g_strdup_printf("<sysSetting><debug>1</debug><cpucolor>%d</cpucolor><memcolor>%d</memcolor><diskcolor>%d</diskcolor><cpu><![CDATA[%s]]></cpu><mem><![CDATA[%s]]></mem><disk><![CDATA[%s]]></disk><runtime><![CDATA[%s]]></runtime><alarmtime><![CDATA[%s]]></alarmtime><dstart>%s</dstart><dcpu>%d\%</dcpu><dmem>%d\%</dmem><ddisk>%0.2lfG</ddisk><dmem>%d</dmem><dmirror>%s</dmirror><dflow>%dkbps</dflow><dattach>%s</dattach><dnetlog>%s</dnetlog><dsen>%s</dsen><dblock>%s</dblock></sysSetting>",cpucolor, memcolor, diskcolor, tips_system_config.cpu, tips_system_config.mem, tips_system_config.disk, tips_system_config.runtime, tips_system_config.alarmtime,devicesta.stime, devicesta.cpu, devicesta.mem,devicesta.disk, devicesta.mem,devicemirror[(int)(devicesta.mirror)-48], devicesta.flow, devicestatue[(int)(devicesta.module_state[0])-48], devicestatue[(int)(devicesta.module_state[1])-48], devicestatue[(int)(devicesta.module_state[2])-48],devicestatue[(int)(devicesta.module_state[3])-48]);
                         char *html = ((admin_opt_fun *)opt_fun)(credentials, xml);
                         g_free(xml);
                         printf("=========01%s\n",html);
                         return html;
                  }

        	else if(!strcmp(cmd, "close")){
						pid_t pid;
						if(!Getpid(&pid)){
							kill(pid, SIGUSR1);
						}
						sleep(5);
						tips_sqlite_handle.catch_opt(credentials->username, (char *)"关机成功");
						system("init 0");
					}
        	else if(!strcmp(cmd, "reboot")){
						pid_t pid=0;
						if(!Getpid(&pid)){
							printf("pid==%d\n",pid);
							kill(pid, SIGUSR1);
						}
						printf("pid3==%d\n",pid);
						sleep(5);
						tips_sqlite_handle.catch_opt(credentials->username, (char *)"重启成功");
						system("reboot");
					}
        	else if(!strcmp(cmd, "upgrade")){
#if 0 
                                                int fd = ui_cli.cli_connect((const char *)"/tmp/tips_ui_cli.domain",(const char *)"/tmp/UNIXUI.domain");
                                                ui_cli.cli_write(fd, 4);
                                                close(fd);
                                                system_config_t tips_system_config;
                                                memset(&tips_system_config, 0, sizeof(tips_system_config));
                                                int judge = get_system_config_xml(&tips_system_config);
                                                char *xml = (char *)g_strdup_printf("<product><cpu><![CDATA[%s]]></cpu><mem><![CDATA[%s]]></mem><disk><![CDATA[%s]]></disk><runtime><![CDATA[%s]]></runtime><alarmtime><![CDATA[%s]]></alarmtime></product>",tips_system_config.cpu, tips_system_config.mem, tips_system_config.disk, tips_system_config.runtime, tips_system_config.alarmtime);
                                                char *html = ((admin_opt_fun *)opt_fun)(credentials, xml);
                                                g_free(xml);
                                                return html;
#endif
								char *xml = NULL;
                                 char* devicestatue[4] = {(char *)"功能未定义", (char *)"功能运行正常", (char *)"功能运行不正常", (char *)"功能未运行"};
                                 char* devicemirror[2] = {(char *)"未连接", (char *)"已连接"};
                                 system_config_t tips_system_config;


                                 status_struct_t devicesta;
                                 memset(&devicesta, 0, sizeof(devicesta));
                                 config_get_device(&devicesta);
								 get_system_config_xml(&tips_system_config);
                                 int cpucolor  = (atoi(tips_system_config.cpu)  < devicesta.cpu  ? 1 : 0);
                                 int memcolor  = (atoi(tips_system_config.mem)  < devicesta.mem  ? 1 : 0);
                                 int diskcolor = (atoi(tips_system_config.disk) < devicesta.disk ? 1 : 0);
			       	 int judge = Upgrade();
					 if(0 == judge)
	                xml = (char *)g_strdup_printf("<product><debug>系统升级成功</debug><cpucolor>%d</cpucolor><memcolor>%d</memcolor><diskcolor>%d</diskcolor><dstart>%s</dstart><dcpu>%d%%</dcpu><dmem>%d%%</dmem><ddisk>%0.2lfG</ddisk><dmirror>%s</dmirror><dflow>%dkbps</dflow><dattach>%s</dattach><dnetlog>%s</dnetlog><dsen>%s</dsen><dblock>%s</dblock></product>",cpucolor, memcolor, diskcolor,devicesta.stime,devicesta.cpu, devicesta.mem,devicesta.disk, devicemirror[(int)(devicesta.mirror)-48], devicesta.flow, devicestatue[(int)(devicesta.module_state[0])-48], devicestatue[(int)(devicesta.module_state[1])-48],devicestatue[(int)(devicesta.module_state[2])-48],devicestatue[(int)(devicesta.module_state[3])-48]);
					 else 
	                xml = (char *)g_strdup_printf("<product><debug>系统升级失败</debug><cpucolor>%d</cpucolor><memcolor>%d</memcolor><diskcolor>%d</diskcolor><dstart>%s</dstart><dcpu>%d%%</dcpu><dmem>%d%%</dmem><ddisk>%0.2lfG</ddisk><dmirror>%s</dmirror><dflow>%dkbps</dflow><dattach>%s</dattach><dnetlog>%s</dnetlog><dsen>%s</dsen><dblock>%s</dblock></product>",cpucolor, memcolor, diskcolor,devicesta.stime,devicesta.cpu, devicesta.mem,devicesta.disk, devicemirror[(int)(devicesta.mirror)-48], devicesta.flow, devicestatue[(int)(devicesta.module_state[0])-48], devicestatue[(int)(devicesta.module_state[1])-48],devicestatue[(int)(devicesta.module_state[2])-48],devicestatue[(int)(devicesta.module_state[3])-48]);

//	                char *xml = (char *)g_strdup_printf("<product></product>");
				//				char *xml = (char *)g_strdup_printf("<product><upgrade>1</upgrade><cpucolor>%d</cpucolor><memcolor>%d</memcolor><diskcolor>%d</diskcolor><cpu><![CDATA[%s]]></cpu><mem><![CDATA[%s]]></mem><disk><![CDATA[%s]]></disk><runtime><![CDATA[%s]]></runtime><alarmtime><![CDATA[%s]]></alarmtime><dstart>%s</dstart><dcpu>%d\%</dcpu><dmem>%d\%</dmem><ddisk>%0.2lfG</ddisk><dmem>%d</dmem><dmirror>%s</dmirror><dflow>%dkbps</dflow><dattach>%s</dattach><dnetlog>%s</dnetlog><dsen>%s</dsen><dblock>%s</dblock></product>",cpucolor, memcolor, diskcolor, tips_system_config.cpu, tips_system_config.mem,tips_system_config.disk, tips_system_config.runtime, tips_system_config.alarmtime,devicesta.stime,devicesta.cpu, devicesta.mem,devicesta.disk, devicesta.mem,devicemirror[(int)(devicesta.mirror)-48], devicesta.flow, devicestatue[(int)(devicesta.module_state[0])-48], devicestatue[(int)(devicesta.module_state[1])-48],devicestatue[(int)(devicesta.module_state[2])-48],devicestatue[(int)(devicesta.module_state[3])-48]);
                                //char *xml = (char *)g_strdup_printf("<product><upgrade>1</upgrade><cpucolor>%d</cpucolor><memcolor>%d</memcolor><diskcolor>%d</diskcolor><cpu><![CDATA[%s]]></cpu><mem><![CDATA[%s]]></mem><disk><![CDATA[%s]]></disk><runtime><![CDATA[%s]]></runtime><alarmtime><![CDATA[%s]]></alarmtime><dstart>%s</dstart><dcpu>%d\%</dcpu><dmem>%d\%</dmem><ddisk>%0.2lfG</ddisk><dmem>%d</dmem><dmirror>%s</dmirror><dflow>%dkbps</dflow><dattach>%s</dattach><dnetlog>%s</dnetlog><dsen>%s</dsen><dblock>%s</dblock></product>",cpucolor, memcolor, diskcolor, tips_system_config.cpu, tips_system_config.mem, tips_system_config.disk, tips_system_config.runtime, tips_system_config.alarmtime,devicesta.stime, devicesta.cpu, devicesta.mem,devicesta.disk, devicesta.mem,devicemirror[(int)(devicesta.mirror)-48], devicesta.flow, devicestatue[(int)(devicesta.module_state[0])-48], devicestatue[(int)(devicesta.module_state[1])-48], devicestatue[(int)(devicesta.module_state[2])-48],devicestatue[(int)(devicesta.module_state[3])-48]);
                                 //char *xml = (char *)g_strdup_printf("<product><cpu><![CDATA[%s]]></cpu><mem><![CDATA[%s]]></mem><disk><![CDATA[%s]]></disk><ru     ntime><![CDATA[%s]]></runt     ime><alarmtime><![CDATA[%s]]></alarmtime></product>",tips_system_config.cpu, tips_system_config.mem, tips_system_config.disk, tips_system_config.runtime, tips_system_     config.alarmtime);
                                 printf("xml in get_config:%s\n",xml);
                                 char *html = ((admin_opt_fun *)opt_fun)(credentials, xml);
                                 g_free(xml);
                                 return html;
				}
                else if(!strcmp(cmd, "upgrade_1")){
#if 0 
                                                int fd = ui_cli.cli_connect((const char *)"/tmp/tips_ui_cli.domain",(const char *)"/tmp/UNIXUI.domain");
                                                ui_cli.cli_write(fd, 4);
                                                close(fd);
                                                system_config_t tips_system_config;
                                                memset(&tips_system_config, 0, sizeof(tips_system_config));
                                                int judge = get_system_config_xml(&tips_system_config);
                                                char *xml = (char *)g_strdup_printf("<product><cpu><![CDATA[%s]]></cpu><mem><![CDATA[%s]]></mem><disk><![CDATA[%s]]></disk><runtime><![CDATA[%s]]></runtime><alarmtime><![CDATA[%s]]></alarmtime></product>",tips_system_config.cpu, tips_system_config.mem, tips_system_config.disk, tips_system_config.runtime, tips_system_config.alarmtime);
                                                char *html = ((admin_opt_fun *)opt_fun)(credentials, xml);
                                                g_free(xml);
                                                return html;
#endif
										char *xml = NULL;
                                        printf("aaaaaaaaaaaaaaaaaaaaaaaaaaaaammmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm\n");
			       	 int judge = Upgrade();
					 if(0 == judge)
                xml = (char *)g_strdup_printf("<sysSetting><debug>远程调试成功</debug></sysSetting>");
					 else 
                xml = (char *)g_strdup_printf("<sysSetting><debug>远程调试失败</debug></sysSetting>");
				//				char *xml = (char *)g_strdup_printf("<product><upgrade>1</upgrade><cpucolor>%d</cpucolor><memcolor>%d</memcolor><diskcolor>%d</diskcolor><cpu><![CDATA[%s]]></cpu><mem><![CDATA[%s]]></mem><disk><![CDATA[%s]]></disk><runtime><![CDATA[%s]]></runtime><alarmtime><![CDATA[%s]]></alarmtime><dstart>%s</dstart><dcpu>%d\%</dcpu><dmem>%d\%</dmem><ddisk>%0.2lfG</ddisk><dmem>%d</dmem><dmirror>%s</dmirror><dflow>%dkbps</dflow><dattach>%s</dattach><dnetlog>%s</dnetlog><dsen>%s</dsen><dblock>%s</dblock></product>",cpucolor, memcolor, diskcolor, tips_system_config.cpu, tips_system_config.mem,tips_system_config.disk, tips_system_config.runtime, tips_system_config.alarmtime,devicesta.stime,devicesta.cpu, devicesta.mem,devicesta.disk, devicesta.mem,devicemirror[(int)(devicesta.mirror)-48], devicesta.flow, devicestatue[(int)(devicesta.module_state[0])-48], devicestatue[(int)(devicesta.module_state[1])-48],devicestatue[(int)(devicesta.module_state[2])-48],devicestatue[(int)(devicesta.module_state[3])-48]);
				//						char *xml = (char *)g_strdup_printf("<sysSetting><upgrade>1</upgrade><cpucolor>%d</cpucolor><memcolor>%d</memcolor><diskcolor>%d</diskcolor><cpu><![CDATA[%s]]></cpu><mem><![CDATA[%s]]></mem><disk><![CDATA[%s]]></disk><runtime><![CDATA[%s]]></runtime><alarmtime><![CDATA[%s]]></alarmtime><dstart>%s</dstart><dcpu>%d\%</dcpu><dmem>%d\%</dmem><ddisk>%0.2lfG</ddisk><dmem>%d</dmem><dmirror>%s</dmirror><dflow>%dkbps</dflow><dattach>%s</dattach><dnetlog>%s</dnetlog><dsen>%s</dsen><dblock>%s</dblock></sysSetting>",cpucolor, memcolor, diskcolor, tips_system_config.cpu, tips_system_config.mem,tips_system_config.disk, tips_system_config.runtime, tips_system_config.alarmtime,devicesta.stime,devicesta.cpu, devicesta.mem,devicesta.disk, devicesta.mem,devicemirror[(int)(devicesta.mirror)-48], devicesta.flow, devicestatue[(int)(devicesta.module_state[0])-48], devicestatue[(int)(devicesta.module_state[1])-48],devicestatue[(int)(devicesta.module_state[2])-48],devicestatue[(int)(devicesta.module_state[3])-48]);
                                       // char *xml = (char *)g_strdup_printf("<sysSetting><upgrade>1</upgrade><cpucolor>%d</cpucolor><memcolor>%d</memcolor><diskcolor>%d</diskcolor><cpu><![CDATA[%s]]></cpu><mem><![CDATA[%s]]></mem><disk><![CDATA[%s]]></disk><runtime><![CDATA[%s]]></runtime><alarmtime><![CDATA[%s]]></alarmtime><dstart>%s</dstart><dcpu>%d\%</dcpu><dmem>%d\%</dmem><ddisk>%0.2lfG</ddisk><dmem>%d</dmem><dmirror>%s</dmirror><dflow>%dkbps</dflow><dattach>%s</dattach><dnetlog>%s</dnetlog><dsen>%s</dsen><dblock>%s</dblock></sysSetting>",cpucolor, memcolor, diskcolor, tips_system_config.cpu, tips_system_config.mem, tips_system_config.disk, tips_system_config.runtime, tips_system_config.alarmtime,devicesta.stime, devicesta.cpu, devicesta.mem,devicesta.disk, devicesta.mem,devicemirror[(int)(devicesta.mirror)-48], devicesta.flow, devicestatue[(int)(devicesta.module_state[0])-48], devicestatue[(int)(devicesta.module_state[1])-48], devicestatue[(int)(devicesta.module_state[2])-48],devicestatue[(int)(devicesta.module_state[3])-48]);
                                        char *html = ((admin_opt_fun *)opt_fun)(credentials, xml);
                                        g_free(xml);
                                        printf("=========01%s\n",html);
                                        return html;
                       	 }

        	else if(!strcmp(cmd, "restore")){
						pid_t pid;
						system("source /usr/local/tipschecker/etc/tipscheck/restore.sh");
						if(!Getpid(&pid)){
							kill(pid, SIGUSR1);
						}
						sleep(5);
						tips_sqlite_handle.catch_opt(credentials->username, (char *)"恢复出厂设置成功");
						system("reboot");
					}
	}
        else{
                char *xml = (char *)g_strdup_printf("<msg>无操作模块权限</msg>");
                char *html = ((admin_opt_fun *)opt_fun)(credentials, xml);
                g_free(xml);
                return html;
        }
/*返回值?*/return NULL;
}

char* CAdmin_opt::cmd_opt_post()
{
	char *html=NULL;
	char *realxml=NULL;
	sqlite_user_t tips_user;
	tips_user.username = params_value(params, "username");
	if(!strcmp(cmd, "tips_block_display")){
	        CTips_block tips_block_handle;
                block_stut_t block_info;
                tips_block_params(params, &block_info);
                tips_block_handle.Block_init_sql(&block_info);
                char *xml = tips_block_handle.Block_exec_sql();
                realxml = (char *)g_strdup_printf("<block><ODip>%s</ODip><OSip>%s</OSip><OSport>%s</OSport><OStart_time>%s</OStart_time><OExpired_time>%s</OExpired_time>%s</block>",block_info.dip,block_info.sip,  block_info.Sport, block_info.Start_time, block_info.Expired_time,xml);
                g_free(xml);
                char *response = ((admin_opt_fun *)opt_fun)(credentials, realxml);
                g_free(realxml);
                return response;
        }
	else if(!strcmp(cmd,"user_modify_myself"))
	{
		tips_user.password = params_value(params, "password");
    		const char *password = params_value(params, "checkpassword");
		printf("tipsusername ==%s\n",tips_user.username);
		printf("tipsuser.password ==%s\n",tips_user.password);
    		if(strlen(tips_user.password) < 9)
		{
			char *tips_err = g_strdup_printf("修改用户%s信息失败，原因:密码长度小于9",tips_user.username);
    			char *xml = (char *)g_strdup_printf("<get_tasks_response><msg>%s</msg></get_tasks_response>",tips_err);
			tips_sqlite_handle.catch_opt(credentials->username, tips_err);
			g_free(tips_err);
     			html = ((admin_opt_fun *)opt_fun)(credentials, xml);
     			g_free(xml);
     			return html;
    		}
    		if(!str_isalpha((char *)tips_user.password)){
			char *tips_err = g_strdup_printf("修改用户%s信息失败，原因:密码为纯字母",tips_user.username);
    			char *xml = (char *)g_strdup_printf("<get_tasks_response><msg>%s</msg></get_tasks_response>",tips_err);
			tips_sqlite_handle.catch_opt(credentials->username, tips_err);
			g_free(tips_err);
      			html = ((admin_opt_fun *)opt_fun)(credentials, xml);
      			g_free(xml);
      			return html;
     		}
     		if(!str_isdight((char *)tips_user.password)){
				char *tips_err = g_strdup_printf("修改用户%s密码失败，原因:密码为纯数字",tips_user.username);
    				char *xml = (char *)g_strdup_printf("<get_tasks_response><msg>%s</msg></get_tasks_response>",tips_err);
				tips_sqlite_handle.catch_opt(credentials->username, tips_err);
				g_free(tips_err);
       				html = ((admin_opt_fun *)opt_fun)(credentials, xml);
       				g_free(xml);
       				return html;
      		}
      		if(tips_user.password && password && strcmp(tips_user.password, password)){
			char *tips_err = g_strdup_printf("修改用户%s密码失败，原因:密码与确认密码不一致",tips_user.username);
    			char *xml = (char *)g_strdup_printf("<get_tasks_response><msg>%s</msg></get_tasks_response>",tips_err);
			tips_sqlite_handle.catch_opt(credentials->username, tips_err);
			g_free(tips_err);
       			html = ((admin_opt_fun *)opt_fun)(credentials, xml);
       			g_free(xml);
       			return html;
       		}
		char *tips_err = g_strdup_printf("修改用户%s密码成功,请用新密码登陆",tips_user.username);
		tips_sqlite_handle.User_Update_myself(&tips_user);
		char *xml = g_strdup_printf("<login_page><message>%s</message></login_page>",tips_err);
    		//char *xml = (char *)g_strdup_printf("<get_tasks_response><msg>修改用户%s密码成功</msg></get_tasks_response>",tips_user.username);
		tips_sqlite_handle.catch_opt(credentials->username, tips_err);
		g_free(tips_err);
        	html = ((admin_opt_fun *)opt_fun)(credentials, xml);
       			g_free(xml);
       			return html;
		//sleep(20);
	}
	else if((!strcmp(cmd, "user_modify")) && ((strcmp(credentials->role, "0")) ||((tips_user.username != NULL) && (!strcmp(credentials->username,tips_user.username))))){
		tips_user.password = params_value(params, "password");
		/*返回值没有被使用char *userrole =*/ (char *)params_value(params, "newrole");
    		const char *password = params_value(params, "checkpassword");
		if(strlen(tips_user.password)==0)
		{
			tips_user.admit_time = (char *)params_value(params,"tips_Alarm_time");//有效期
			tips_user.client_name = (char *)params_value(params,"txtUserName");//姓名
			tips_user.telephone = (char *)params_value(params,"txtPhone");//电话
			tips_user.unit_name = (char *)params_value(params,"txtDepartment");//部门
			tips_user.state = (char *)params_value(params,"userstate");//状态
			char *tips_err = g_strdup_printf("修改用户%s信息成功",tips_user.username);
	      		tips_sqlite_handle.User_Update(&tips_user);
			tips_sqlite_handle.catch_opt(credentials->username, tips_err);
    			//char *xml = (char *)g_strdup_printf("<get_tasks_response><msg>修改用户%s信息成功</msg></get_tasks_response>",tips_user.username);
			return display_user_html(tips_err, credentials->role);
			//g_free(tips_err);
        		//html = ((admin_opt_fun *)opt_fun)(credentials, xml);
		}
		else
		{
    			if(strlen(tips_user.password) < 9)
			{
				char *tips_err = g_strdup_printf("修改用户%s信息失败，原因:密码长度小于9",tips_user.username);
    				char *xml = (char *)g_strdup_printf("<get_tasks_response><msg>%s</msg></get_tasks_response>",tips_err);
				tips_sqlite_handle.catch_opt(credentials->username, tips_err);
				g_free(tips_err);
     				html = ((admin_opt_fun *)opt_fun)(credentials, xml);
     				g_free(xml);
     				return html;
    			}
    			if(!str_isalpha((char *)tips_user.password)){
				char *tips_err = g_strdup_printf("修改用户%s信息失败，原因:密码为纯字母",tips_user.username);
    				char *xml = (char *)g_strdup_printf("<get_tasks_response><msg>%s</msg></get_tasks_response>",tips_err);
				tips_sqlite_handle.catch_opt(credentials->username, tips_err);
				g_free(tips_err);
      				html = ((admin_opt_fun *)opt_fun)(credentials, xml);
      				g_free(xml);
      				return html;
     			}
     			if(!str_isdight((char *)tips_user.password)){
					char *tips_err = g_strdup_printf("修改用户%s密码失败，原因:密码为纯数字",tips_user.username);
    					char *xml = (char *)g_strdup_printf("<get_tasks_response><msg>%s</msg></get_tasks_response>",tips_err);
					tips_sqlite_handle.catch_opt(credentials->username, tips_err);
					g_free(tips_err);
       					html = ((admin_opt_fun *)opt_fun)(credentials, xml);
       					g_free(xml);
       					return html;
      			}
      			if(tips_user.password && password && strcmp(tips_user.password, password)){
				char *tips_err = g_strdup_printf("修改用户%s密码失败，原因:密码与确认密码不一致",tips_user.username);
    				char *xml = (char *)g_strdup_printf("<get_tasks_response><msg>%s</msg></get_tasks_response>",tips_err);
				tips_sqlite_handle.catch_opt(credentials->username, tips_err);
				g_free(tips_err);
       				html = ((admin_opt_fun *)opt_fun)(credentials, xml);
       				g_free(xml);
       				return html;
       			}
			tips_user.admit_time = (char *)params_value(params,"tips_Alarm_time");//有效期
			tips_user.client_name = (char *)params_value(params,"txtUserName");//姓名
			tips_user.telephone = (char *)params_value(params,"txtPhone");//电话
			tips_user.unit_name = (char *)params_value(params,"txtDepartment");//部门
			tips_user.state = (char *)params_value(params,"userstat");//状态
			char *tips_err = g_strdup_printf("修改用户%s信息成功",tips_user.username);
	      		tips_sqlite_handle.User_Update(&tips_user);
			tips_sqlite_handle.catch_opt(credentials->username, tips_err);
			return display_user_html(tips_err, credentials->role);
		}
#if 0
    		if(strlen(tips_user.password) < 9)
		{
			char *tips_err = g_strdup_printf("修改用户%s信息失败，原因:密码长度小于9",tips_user.username);
    			char *xml = (char *)g_strdup_printf("<get_tasks_response><msg>%s</msg></get_tasks_response>",tips_err);
			tips_sqlite_handle.catch_opt(credentials->username, tips_err);
			g_free(tips_err);
     			html = ((admin_opt_fun *)opt_fun)(credentials, xml);
     			g_free(xml);
     			return html;
    		}
    		if(!str_isalpha((char *)tips_user.password)){
			char *tips_err = g_strdup_printf("修改用户%s信息失败，原因:密码为纯字母",tips_user.username);
    			char *xml = (char *)g_strdup_printf("<get_tasks_response><msg>%s</msg></get_tasks_response>",tips_err);
			tips_sqlite_handle.catch_opt(credentials->username, tips_err);
			g_free(tips_err);
      			html = ((admin_opt_fun *)opt_fun)(credentials, xml);
      			g_free(xml);
      			return html;
     		}
     		if(!str_isdight((char *)tips_user.password)){
				char *tips_err = g_strdup_printf("修改用户%s密码失败，原因:密码为纯数字",tips_user.username);
    				char *xml = (char *)g_strdup_printf("<get_tasks_response><msg>%s</msg></get_tasks_response>",tips_err);
				tips_sqlite_handle.catch_opt(credentials->username, tips_err);
				g_free(tips_err);
       				html = ((admin_opt_fun *)opt_fun)(credentials, xml);
       				g_free(xml);
       				return html;
      		}
      		if(tips_user.password && password && strcmp(tips_user.password, password)){
			char *tips_err = g_strdup_printf("修改用户%s密码失败，原因:密码与确认密码不一致",tips_user.username);
    			char *xml = (char *)g_strdup_printf("<get_tasks_response><msg>%s</msg></get_tasks_response>",tips_err);
			tips_sqlite_handle.catch_opt(credentials->username, tips_err);
			g_free(tips_err);
       			html = ((admin_opt_fun *)opt_fun)(credentials, xml);
       			g_free(xml);
       			return html;
       		}
      		if(tips_user.password && strlen(tips_user.password)){
			  tips_user.admit_time = (char *)params_value(params,"tips_Alarm_time");
			  tips_user.client_name = (char *)params_value(params,"txtUserName");
			  tips_user.telephone = (char *)params_value(params,"txtPhone");
			  tips_user.unit_name = (char *)params_value(params,"txtDepartment");
			  char *tips_err = g_strdup_printf("修改用户%s信息成功",tips_user.username);
	      		  tips_sqlite_handle.User_Update(&tips_user);
    			  char *xml = (char *)g_strdup_printf("<get_tasks_response><msg>修改用户%s信息成功</msg></get_tasks_response>",tips_user.username);
			  tips_sqlite_handle.catch_opt(credentials->username, tips_err);
			  g_free(tips_err);
        		  html = ((admin_opt_fun *)opt_fun)(credentials, xml);
       		}
       		else{
    			char *xml = (char *)g_strdup_printf("<get_tasks_response><msg>密码不能够为空</msg></get_tasks_response>");
			char *tips_err = g_strdup_printf("修改用户%s信息失败，原因:密码不能为空",tips_user.username);
			tips_sqlite_handle.catch_opt(credentials->username, tips_err);
			g_free(tips_err);
        		html = ((admin_opt_fun *)opt_fun)(credentials, xml);
        		g_free(xml);
       		}
#endif 
	}
	else if(strcmp(credentials->role, "0")){
		if(!strcmp(cmd, "tips_create_user")){
	                			char *xml;
                        time_t now=time(NULL);
#if 0
			const char *submit_value_judge=params_value(params, "submit");
                        if(!strcmp(submit_value_judge,"搜索"))
                        {
                          tips_user.role = (char *)params_value(params, "newrole");
                           xml = tips_sqlite_handle.User_search(tips_user.username,tips_user.role);
                           realxml = (char *)g_strdup_printf("<get_tasks_response>%s</get_tasks_response>", xml);
                          tips_user.role = (char *)params_value(params, "newrole");
                           g_free(xml);
                           html = ((admin_opt_fun *)opt_fun)(credentials, realxml);
                           printf("html============================%s\n",html);
                           g_free(realxml);
                           g_free((void *)submit_value_judge);
                           return html;
 
                        }
#endif
                        tips_user.password = params_value(params, "password");
                        const char *password = params_value(params, "checkpassword");
                        printf("password:%s\t%s\n",tips_user.password, password);
                        if(strlen(tips_user.password) < 9){
							int user_flag = 0;
							int password_flag = 0; 
			//				tips_user.username = params_value(params, "username");
							tips_user.role = (char *)params_value(params, "newrole");
	                        xml = tips_sqlite_handle.User_catch();
							if(strlen(tips_user.username) == 0)
								user_flag = 1;
							if(strlen(tips_user.password) == 0)
								password_flag = 1;
							cout << "tips_user.role::::"<< tips_user.role << endl;
							cout << "tips_user.password::::"<< tips_user.password << endl;
							cout << "user_falg::::"<< user_flag << endl;
							cout << "password_falg::::"<< password_flag << endl;
							if(user_flag == 1 && password_flag == 1)
													 realxml = (char *)g_strdup_printf("<get_tasks_response>%s<msg>用户名和密码不能够为空</msg></get_tasks_response>", xml);
							else if(user_flag == 1 && password_flag == 0)
													 realxml = (char *)g_strdup_printf("<get_tasks_response>%s<msg>用户名不能够为空</msg></get_tasks_response>", xml);
							else if(user_flag == 0&& password_flag == 1)
													 realxml = (char *)g_strdup_printf("<get_tasks_response>%s<msg>密码不能够为空</msg></get_tasks_response>", xml);
							else
													 realxml = (char *)g_strdup_printf("<get_tasks_response>%s<msg>密阿长度小于9</msg></get_tasks_response>", xml);

													 char *tips_err = g_strdup_printf("创建用户%s失败，原因:密码太短",tips_user.username);
				   								 tips_sqlite_handle.catch_opt(credentials->username, tips_err);
					                 g_free(tips_err);
                           g_free(xml);
                           html = ((admin_opt_fun *)opt_fun)(credentials, realxml);
                           g_free(realxml);
                           return html;
                        }
                        if(!str_isalpha((char *)tips_user.password)){
	                         xml = tips_sqlite_handle.User_catch();
													 realxml = (char *)g_strdup_printf("<get_tasks_response>%s<msg>密码为纯字符</msg></get_tasks_response>", xml);
													 char *tips_err = g_strdup_printf("创建用户%s失败，原因:密码为纯字符",tips_user.username);
				   								 tips_sqlite_handle.catch_opt(credentials->username, tips_err);
					                 g_free(tips_err);
                           g_free(xml);
                           html = ((admin_opt_fun *)opt_fun)(credentials, realxml);
                           g_free(realxml);
                           return html;
                        }
                        if(!str_isdight((char *)tips_user.password)){
	                         xml = tips_sqlite_handle.User_catch();
													 realxml = (char *)g_strdup_printf("<get_tasks_response>%s<msg>密码为纯数字</msg></get_tasks_response>", xml);
													 char *tips_err = g_strdup_printf("创建用户%s失败，原因:密码为纯数字",tips_user.username);
				   								 tips_sqlite_handle.catch_opt(credentials->username, tips_err);
					                 g_free(tips_err);
                           g_free(xml);
                           html = ((admin_opt_fun *)opt_fun)(credentials, realxml);
                           g_free(realxml);
                           return html;
                        }
                        if(tips_user.username &&
                           tips_user.password &&
                           strlen(tips_user.username) &&
                           strlen(tips_user.password)){
                        	 if(tips_user.password && password && strcmp(tips_user.password, password)){
	                         xml = tips_sqlite_handle.User_catch();
													 realxml = (char *)g_strdup_printf("<get_tasks_response>%s<msg>密码与确认密码不符</msg></get_tasks_response>", xml);
													 char *tips_err = g_strdup_printf("创建用户%s失败，原因:密码与确认密码不一致",tips_user.username);
				   								 tips_sqlite_handle.catch_opt(credentials->username, tips_err);
					                 g_free(tips_err);
                           g_free(xml);
                           html = ((admin_opt_fun *)opt_fun)(credentials, realxml);
                           g_free(realxml);
                           return html;
                         	}
			  char tips_clock[20];
			  memset(tips_clock, 0, 20);
			  TurnTime(now, tips_clock, 20);
                          tips_user.createtime = g_strdup_printf("%s",tips_clock);
                          tips_user.role = (char *)params_value(params, "newrole");
			  tips_user.admit_time = (char *)params_value(params,"tips_Alarm_time");
			  tips_user.client_name = (char *)params_value(params,"txtUserName");
			  tips_user.telephone = (char *)params_value(params,"txtPhone");
			  tips_user.unit_name = (char *)params_value(params,"txtDepartment");
			  tips_user.state = (char *)params_value(params,"userstate");
                          printf("createtime:%s\n",tips_user.createtime);
                          int ret = tips_sqlite_handle.User_insert(&tips_user);
                          xml = tips_sqlite_handle.User_search(tips_user.username,tips_user.role);
                          g_free(tips_user.role);
                          g_free(tips_user.createtime);
                          printf("ret == %d\n",ret);
                          if(0 == ret){
	                  	 //realxml = (char *)g_strdup_printf("<get_tasks_response>%s<msg>用户 %s 已经存在</msg></get_tasks_response>", xml,tips_user.username);
				   			char *tips_err = g_strdup_printf("创建用户%s失败，原因:用户已经存在",tips_user.username);
				   			tips_sqlite_handle.catch_opt(credentials->username, tips_err);
						return display_user_html((const char*)tips_err, credentials->role);
				   						//						 g_free(tips_err);
                                   //g_free(xml);
                                   //html = ((admin_opt_fun *)opt_fun)(credentials, realxml);
                                   //g_free(realxml);
                           }
                           else{
	                           			//realxml = (char *)g_strdup_printf("<get_tasks_response>%s<msg>创建用户 %s 成功</msg></get_tasks_response>",xml,tips_user.username);
                                   //g_free(xml);
                                   //html = ((admin_opt_fun *)opt_fun)(credentials, realxml);
                                   //g_free(realxml);
				   												 char *tips_err = g_strdup_printf("创建用户%s成功",tips_user.username);
											return display_user_html(tips_err, credentials->role);
				   												 tips_sqlite_handle.catch_opt(credentials->username, tips_err);
				   												 g_free(tips_err);
                           }
                           return html;
                         }
                        else{
	                        xml = tips_sqlite_handle.User_catch();
                                realxml = (char *)g_strdup_printf("<get_tasks_response>%s<msg>密码或帐号不能为空</msg></get_tasks_response>",xml);
				   tips_sqlite_handle.catch_opt(credentials->username, (char *)"创建用户失败；原因：密码或帐号输入为空");
                                g_free(xml);
                                html = ((admin_opt_fun *)opt_fun)(credentials, realxml);
                                g_free(realxml);
                                return html;
                       }
                }
		else if(!strcmp(cmd, "configadmin")){
                         config_manage_t tips_config_manage;
                                         char *xml=NULL;
                                         const char *cmd_name_judge = params_value(params,"submit");
                                         printf("name is %s\n",cmd_name_judge);
										 int ip_flag = 0;
										 int isint_flag = 0;
                         memset(&tips_config_manage, 0, sizeof(tips_config_manage));
					if(!strcmp(cmd_name_judge,"提交"))
					{
#if 1						 
						 if(NULL != params_value(params, "manage_IP"))
							{
								if(strlen(params_value(params,"manage_IP")))
								{
									if(INADDR_NONE == inet_addr(params_value(params, "manage_IP")))
									{
										ip_flag = 1;
										strcpy(tips_config_manage.manage_IP,params_value(params, "manage_IP"));
									}
									else
										{
											char *node = NULL;
											node = strchr((char *)params_value(params, "manage_IP"),'.');
											if(NULL != node)
											strcpy(tips_config_manage.manage_IP,params_value(params, "manage_IP"));
											else
											{
												ip_flag =1;
												strcpy(tips_config_manage.manage_IP,params_value(params, "manage_IP"));
											}
										}
								}
								else
										strcpy(tips_config_manage.manage_IP,"");
							}
						else 
										strcpy(tips_config_manage.manage_IP,"");
						 if(NULL != params_value(params, "gateway"))
							{
								if(strlen(params_value(params,"gateway")))
								{
									if(INADDR_NONE == inet_addr(params_value(params, "gateway")))
									{
										ip_flag = 1;
										strcpy(tips_config_manage.gateway,params_value(params, "gateway"));
									}
									else
									{
											char *node = NULL;
											node = strchr((char *)params_value(params, "gateway"),'.');
											if(NULL != node)
												strcpy(tips_config_manage.gateway,params_value(params, "gateway"));
											else
											{
												ip_flag =1;
												strcpy(tips_config_manage.gateway,params_value(params, "gateway"));
											}
									}
								}
								else
										strcpy(tips_config_manage.gateway,"");
							}
						 else 
							strcpy(tips_config_manage.gateway,"");

						 if(NULL != params_value(params, "net_IP"))
							{
								if(strlen(params_value(params,"net_IP")))
								{
									if(INADDR_NONE == inet_addr(params_value(params, "net_IP")))
									{
										ip_flag = 1;
										strcpy(tips_config_manage.net_IP,params_value(params, "net_IP"));
									}
									else
									{
											char *node = NULL;
											node = strchr((char *)params_value(params, "net_IP"),'.');
											if(NULL != node)
												strcpy(tips_config_manage.net_IP,params_value(params, "net_IP"));
											else
											{
												ip_flag =1;
												strcpy(tips_config_manage.net_IP,params_value(params, "net_IP"));
											}
									}
								}
								else
										strcpy(tips_config_manage.net_IP,"");
							}
						 else
										strcpy(tips_config_manage.net_IP,"");
						 if(NULL != params_value(params, "static_route"))
							{
								if(strlen(params_value(params,"static_route")))
								{
									if(INADDR_NONE == inet_addr(params_value(params, "static_route")))
									{
										ip_flag = 1;
										strcpy(tips_config_manage.static_route,params_value(params, "static_route"));
									}
									else
									{
											char *node = NULL;
											node = strchr((char *)params_value(params, "static_route"),'.');
											if (NULL != node)
												strcpy(tips_config_manage.static_route, params_value(params, "static_route"));
											else
											{
												ip_flag =1;
												strcpy(tips_config_manage.static_route, params_value(params, "static_route"));
											}
									}
								}
								else
										strcpy(tips_config_manage.static_route,"");
							}
						 else
										strcpy(tips_config_manage.static_route,"");
						 if(NULL != params_value(params, "supervisor"))
							{
								if(strlen(params_value(params,"supervisor")))
								{
									if(INADDR_NONE == inet_addr(params_value(params, "supervisor")))
									{
										ip_flag = 1;
										strcpy(tips_config_manage.supervisor,params_value(params, "supervisor"));
									}
									else
									{
											char *node = NULL;
											node = strchr((char *)params_value(params, "supervisor"),'.');
											if(NULL != node)
												strcpy(tips_config_manage.supervisor,params_value(params, "supervisor"));
											else
											{
												ip_flag =1;
												strcpy(tips_config_manage.supervisor,params_value(params, "supervisor"));
											}
									}
								}
								else
										strcpy(tips_config_manage.supervisor,"");
							}
						 else
										strcpy(tips_config_manage.supervisor,"");
						 if(NULL != params_value(params, "unit_name"))
										strcpy(tips_config_manage.unit_name,params_value(params, "unit_name"));
						 else
										strcpy(tips_config_manage.unit_name,"");
						 if(NULL != params_value(params, "serve_addr"))
							strcpy(tips_config_manage.serve_addr,params_value(params, "serve_addr"));
						 if(NULL != params_value(params, "serve_user"))
							strcpy(tips_config_manage.serve_user,params_value(params, "serve_user"));
						 if(NULL != params_value(params, "serve_password"))
							strcpy(tips_config_manage.serve_password,params_value(params, "serve_password"));
						 if(NULL != params_value(params, "administration_IP"))
							strcpy(tips_config_manage.adminstration_IP,params_value(params, "administration_IP"));
						 if(NULL != params_value(params, "administration_IP1"))
							strcpy(tips_config_manage.adminstration_IP1,params_value(params, "administration_IP1"));
						 if(NULL != params_value(params, "manage_PORT"))
							strcpy(tips_config_manage.manage_PORT,params_value(params, "manage_PORT"));
						 if(NULL != params_value(params, "manage_PORT1"))
							strcpy(tips_config_manage.manage_PORT1,params_value(params, "manage_PORT1"));
						 if(!ip_flag)
						 {
//							 if(!strlen(params_value(params, "manage_IP")) && !strlen(params_value(params, "net_IP")) && !strlen(params_value(params, "gateway")) && !strlen(params_value(params, "static_route")) && !strlen(params_value(params, "supervisor")) && !strlen(params_value(params, "unit_name")))
//								 xml = (char *)g_strdup_printf("<get_reports_response><report type=\"config\"><MANAGE_IP><![CDATA[%s]]></MANAGE_IP><MANAGE_PORT><![CDATA[%s]]></MANAGE_PORT><GATEWAY><![CDATA[%s]]></GATEWAY><STATIC_ROUTE><![CDATA[%s]]></STATIC_ROUTE><SUPERVISOR><![CDATA[%s]]></SUPERVISOR><SERVE_ADDR><![CDATA[%s]]></SERVE_ADDR><SERVE_USER><![CDATA[%s]]></SERVE_USER><SERVE_PASSWORD><![CDATA[%s]]></SERVE_PASSWORD><ADMINSTRATION_IP><![CDATA[%s]]></ADMINSTRATION_IP><UNIT_NAME><![CDATA[%s]]></UNIT_NAME><NET_IP><![CDATA[%s]]></NET_IP><MANAGE_PORT1><![CDATA[%s]]></MANAGE_PORT1><ADMINSTRATION_IP1><![CDATA[%s]]></ADMINSTRATION_IP1><config_submit_msg>数 据 不 能 够 全 为 空, 提 交 失败</config_submit_msg></report></get_reports_response>",tips_config_manage.manage_IP,tips_config_manage.manage_PORT, tips_config_manage.gateway, tips_config_manage.static_route, tips_config_manage.supervisor, tips_config_manage.serve_addr, tips_config_manage.serve_user, tips_config_manage.serve_password, tips_config_manage.adminstration_IP, tips_config_manage.unit_name, tips_config_manage.net_IP, tips_config_manage.manage_PORT1, tips_config_manage.adminstration_IP1);
//							else
//							{
								config_manage_modify(&tips_config_manage, credentials,0);
                	        	xml = (char *)g_strdup_printf("<get_reports_response><report type=\"config\"><MANAGE_IP><![CDATA[%s]]></MANAGE_IP><MANAGE_PORT><![CDATA[%s]]></MANAGE_PORT><GATEWAY><![CDATA[%s]]></GATEWAY><STATIC_ROUTE><![CDATA[%s]]></STATIC_ROUTE><SUPERVISOR><![CDATA[%s]]></SUPERVISOR><SERVE_ADDR><![CDATA[%s]]></SERVE_ADDR><SERVE_USER><![CDATA[%s]]></SERVE_USER><SERVE_PASSWORD><![CDATA[%s]]></SERVE_PASSWORD><ADMINSTRATION_IP><![CDATA[%s]]></ADMINSTRATION_IP><UNIT_NAME><![CDATA[%s]]></UNIT_NAME><NET_IP><![CDATA[%s]]></NET_IP><MANAGE_PORT1><![CDATA[%s]]></MANAGE_PORT1><ADMINSTRATION_IP1><![CDATA[%s]]></ADMINSTRATION_IP1><config_submit_msg>提 交 成 功</config_submit_msg></report></get_reports_response>",tips_config_manage.manage_IP,tips_config_manage.manage_PORT, tips_config_manage.gateway, tips_config_manage.static_route, tips_config_manage.supervisor, tips_config_manage.serve_addr, tips_config_manage.serve_user, tips_config_manage.serve_password, tips_config_manage.adminstration_IP, tips_config_manage.unit_name, tips_config_manage.net_IP, tips_config_manage.manage_PORT1, tips_config_manage.adminstration_IP1);
//							}
						 }
						 else
                	        xml = (char *)g_strdup_printf("<get_reports_response><report type=\"config\"><MANAGE_IP><![CDATA[%s]]></MANAGE_IP><MANAGE_PORT><![CDATA[%s]]></MANAGE_PORT><GATEWAY><![CDATA[%s]]></GATEWAY><STATIC_ROUTE><![CDATA[%s]]></STATIC_ROUTE><SUPERVISOR><![CDATA[%s]]></SUPERVISOR><SERVE_ADDR><![CDATA[%s]]></SERVE_ADDR><SERVE_USER><![CDATA[%s]]></SERVE_USER><SERVE_PASSWORD><![CDATA[%s]]></SERVE_PASSWORD><ADMINSTRATION_IP><![CDATA[%s]]></ADMINSTRATION_IP><UNIT_NAME><![CDATA[%s]]></UNIT_NAME><NET_IP><![CDATA[%s]]></NET_IP><MANAGE_PORT1><![CDATA[%s]]></MANAGE_PORT1><ADMINSTRATION_IP1><![CDATA[%s]]></ADMINSTRATION_IP1><config_submit_msg>请 输 入 正 确 的 地 址 格 式</config_submit_msg></report></get_reports_response>",tips_config_manage.manage_IP,tips_config_manage.manage_PORT, tips_config_manage.gateway, tips_config_manage.static_route, tips_config_manage.supervisor, tips_config_manage.serve_addr, tips_config_manage.serve_user, tips_config_manage.serve_password, tips_config_manage.adminstration_IP, tips_config_manage.unit_name, tips_config_manage.net_IP, tips_config_manage.manage_PORT1, tips_config_manage.adminstration_IP1);
					}
//保存配置修改
					else 
					{
						 if(NULL != params_value(params, "serve_addr"))
							{
								if(strlen(params_value(params,"serve_addr")))
								{
									if(INADDR_NONE == inet_addr(params_value(params, "serve_addr")))
									{
										ip_flag = 1;
										strcpy(tips_config_manage.serve_addr,params_value(params, "serve_addr"));
									}
									else
									{
											char *node = NULL;
											node = strchr((char *)params_value(params, "serve_addr"),'.');
											if(NULL != node)
												strcpy(tips_config_manage.serve_addr,params_value(params, "serve_addr"));
											else
											{
												ip_flag =1;
												strcpy(tips_config_manage.serve_addr,params_value(params, "serve_addr"));
											}
									}
								}
								else
										strcpy(tips_config_manage.serve_addr,"");

							}
						 else
										strcpy(tips_config_manage.serve_addr,"");
						 if(NULL != params_value(params,"serve_user"))
										strcpy(tips_config_manage.serve_user,params_value(params, "serve_user"));
						 else
										strcpy(tips_config_manage.serve_user,"");
						 if(NULL != params_value(params,"serve_password"))
										strcpy(tips_config_manage.serve_password,params_value(params, "serve_password"));
						 else
										strcpy(tips_config_manage.serve_password,"");

						 if(NULL != params_value(params,"administration_IP"))
							{
								if(strlen(params_value(params,"administration_IP")))
								{
									if(INADDR_NONE == inet_addr(params_value(params, "administration_IP")))
									{
										ip_flag = 1;
										strcpy(tips_config_manage.adminstration_IP,params_value(params, "administration_IP"));
									}
									else
									{
											char *node = NULL;
											node = strchr((char *)params_value(params, "administration_IP"),'.');
											if(NULL != node)
												strcpy(tips_config_manage.adminstration_IP,params_value(params, "administration_IP"));
											else
											{
												ip_flag =1;
												strcpy(tips_config_manage.adminstration_IP,params_value(params, "administration_IP"));
											}
									}

								}
								else 
										strcpy(tips_config_manage.adminstration_IP,"");
							}
						 else
										strcpy(tips_config_manage.adminstration_IP,"");
						 if(NULL != params_value(params, "administration_IP1"))
							{
								if(strlen(params_value(params,"administration_IP1")))
								{
									if(INADDR_NONE == inet_addr(params_value(params, "administration_IP1")))
									{
										ip_flag = 1;
										strcpy(tips_config_manage.adminstration_IP1,params_value(params, "administration_IP1"));
									}
									else
									{
											char *node = NULL;
											node = strchr((char *)params_value(params, "administration_IP1"),'.');
											if(NULL != node)
												strcpy(tips_config_manage.adminstration_IP1,params_value(params, "administration_IP1"));
											else
											{
												ip_flag =1;
												strcpy(tips_config_manage.adminstration_IP1,params_value(params, "administration_IP1"));
											}
									}
								}
								else
										strcpy(tips_config_manage.adminstration_IP1,"");
							}
						 else
										strcpy(tips_config_manage.adminstration_IP1,"");
						if(NULL != params_value(params, "manage_PORT"))
						{
							if(strlen(params_value(params,"manage_PORT")))
							{
								int i = 0;
								strcpy(tips_config_manage.manage_PORT,params_value(params, "manage_PORT"));
								for(i = 0;tips_config_manage.manage_PORT[i] != 0;i++)		
								{
									if(!isdigit(tips_config_manage.manage_PORT[i]))
									{
										isint_flag = 1;
										break;
									}
								}
							}
							else
								strcpy(tips_config_manage.manage_PORT,"");
						}
						else 
								strcpy(tips_config_manage.manage_PORT,"");
						if(NULL != params_value(params, "manage_PORT1"))
						{
							if(strlen(params_value(params,"manage_PORT1")))
							{
								int i = 0;
								strcpy(tips_config_manage.manage_PORT1,params_value(params, "manage_PORT1"));
								for(i = 0;tips_config_manage.manage_PORT1[i] != 0;i++)		
								{
									if(!isdigit(tips_config_manage.manage_PORT1[i]))
									{
										isint_flag = 1;
										break;
									}
								}
							}
							else
								strcpy(tips_config_manage.manage_PORT1,"");
						}
						else 
								strcpy(tips_config_manage.manage_PORT1,"");
						 if(NULL != params_value(params, "manage_IP"))
							strcpy(tips_config_manage.manage_IP,params_value(params, "manage_IP"));
						 if(NULL != params_value(params, "gateway"))
							strcpy(tips_config_manage.gateway,params_value(params, "gateway"));
						 if(NULL != params_value(params, "net_IP"))
							strcpy(tips_config_manage.net_IP,params_value(params, "net_IP"));
						 if(NULL != params_value(params, "static_route"))
							strcpy(tips_config_manage.static_route,params_value(params, "static_route"));
						 if(NULL != params_value(params, "supervisor"))
							strcpy(tips_config_manage.supervisor,params_value(params, "supervisor"));
						 if(NULL != params_value(params, "unit_name"))
							strcpy(tips_config_manage.unit_name,params_value(params, "unit_name"));
						if(0==ip_flag && 0==isint_flag)
						{
							// if(!strlen(params_value(params, "serve_addr")) && !strlen(params_value(params, "serve_user")) && !strlen(params_value(params, "serve_password")) && !strlen(params_value(params, "administration_IP")) && !strlen(params_value(params, "administration_IP1")) && !strlen(params_value(params, "manage_PORT"))&& !strlen(params_value(params, "manage_PORT1")))
                	        //	xml = (char *)g_strdup_printf("<get_reports_response><report type=\"config\"><MANAGE_IP><![CDATA[%s]]></MANAGE_IP><MANAGE_PORT><![CDATA[%s]]></MANAGE_PORT><GATEWAY><![CDATA[%s]]></GATEWAY><STATIC_ROUTE><![CDATA[%s]]></STATIC_ROUTE><SUPERVISOR><![CDATA[%s]]></SUPERVISOR><SERVE_ADDR><![CDATA[%s]]></SERVE_ADDR><SERVE_USER><![CDATA[%s]]></SERVE_USER><SERVE_PASSWORD><![CDATA[%s]]></SERVE_PASSWORD><ADMINSTRATION_IP><![CDATA[%s]]></ADMINSTRATION_IP><UNIT_NAME><![CDATA[%s]]></UNIT_NAME><NET_IP><![CDATA[%s]]></NET_IP><MANAGE_PORT1><![CDATA[%s]]></MANAGE_PORT1><ADMINSTRATION_IP1><![CDATA[%s]]></ADMINSTRATION_IP1><config_msg>数据不能够全为空，保 存 失 败</config_msg></report></get_reports_response>",tips_config_manage.manage_IP, tips_config_manage.manage_PORT,tips_config_manage.gateway, tips_config_manage.static_route, tips_config_manage.supervisor, tips_config_manage.serve_addr, tips_config_manage.serve_user, tips_config_manage.serve_password, tips_config_manage.adminstration_IP, tips_config_manage.unit_name, tips_config_manage.net_IP, tips_config_manage.manage_PORT1, tips_config_manage.adminstration_IP1);
							// else
							// {
								config_manage_modify(&tips_config_manage, credentials,1);
                	        	xml = (char *)g_strdup_printf("<get_reports_response><report type=\"config\"><MANAGE_IP><![CDATA[%s]]></MANAGE_IP><MANAGE_PORT><![CDATA[%s]]></MANAGE_PORT><GATEWAY><![CDATA[%s]]></GATEWAY><STATIC_ROUTE><![CDATA[%s]]></STATIC_ROUTE><SUPERVISOR><![CDATA[%s]]></SUPERVISOR><SERVE_ADDR><![CDATA[%s]]></SERVE_ADDR><SERVE_USER><![CDATA[%s]]></SERVE_USER><SERVE_PASSWORD><![CDATA[%s]]></SERVE_PASSWORD><ADMINSTRATION_IP><![CDATA[%s]]></ADMINSTRATION_IP><UNIT_NAME><![CDATA[%s]]></UNIT_NAME><NET_IP><![CDATA[%s]]></NET_IP><MANAGE_PORT1><![CDATA[%s]]></MANAGE_PORT1><ADMINSTRATION_IP1><![CDATA[%s]]></ADMINSTRATION_IP1><config_msg>保 存 成 功</config_msg></report></get_reports_response>",tips_config_manage.manage_IP, tips_config_manage.manage_PORT,tips_config_manage.gateway, tips_config_manage.static_route, tips_config_manage.supervisor, tips_config_manage.serve_addr, tips_config_manage.serve_user, tips_config_manage.serve_password, tips_config_manage.adminstration_IP, tips_config_manage.unit_name, tips_config_manage.net_IP, tips_config_manage.manage_PORT1, tips_config_manage.adminstration_IP1);
							// }
						}
						else if(1==ip_flag && 0==isint_flag)
                	        	xml = (char *)g_strdup_printf("<get_reports_response><report type=\"config\"><MANAGE_IP><![CDATA[%s]]></MANAGE_IP><MANAGE_PORT><![CDATA[%s]]></MANAGE_PORT><GATEWAY><![CDATA[%s]]></GATEWAY><STATIC_ROUTE><![CDATA[%s]]></STATIC_ROUTE><SUPERVISOR><![CDATA[%s]]></SUPERVISOR><SERVE_ADDR><![CDATA[%s]]></SERVE_ADDR><SERVE_USER><![CDATA[%s]]></SERVE_USER><SERVE_PASSWORD><![CDATA[%s]]></SERVE_PASSWORD><ADMINSTRATION_IP><![CDATA[%s]]></ADMINSTRATION_IP><UNIT_NAME><![CDATA[%s]]></UNIT_NAME><NET_IP><![CDATA[%s]]></NET_IP><MANAGE_PORT1><![CDATA[%s]]></MANAGE_PORT1><ADMINSTRATION_IP1><![CDATA[%s]]></ADMINSTRATION_IP1><config_msg>请 输 入 正 确 的 地 址 格 式,保 存 失 败</config_msg></report></get_reports_response>",tips_config_manage.manage_IP, tips_config_manage.manage_PORT,tips_config_manage.gateway, tips_config_manage.static_route, tips_config_manage.supervisor, tips_config_manage.serve_addr, tips_config_manage.serve_user, tips_config_manage.serve_password, tips_config_manage.adminstration_IP, tips_config_manage.unit_name, tips_config_manage.net_IP, tips_config_manage.manage_PORT1, tips_config_manage.adminstration_IP1);
						else if(0==ip_flag && 1==isint_flag)
                	        	xml = (char *)g_strdup_printf("<get_reports_response><report type=\"config\"><MANAGE_IP><![CDATA[%s]]></MANAGE_IP><MANAGE_PORT><![CDATA[%s]]></MANAGE_PORT><GATEWAY><![CDATA[%s]]></GATEWAY><STATIC_ROUTE><![CDATA[%s]]></STATIC_ROUTE><SUPERVISOR><![CDATA[%s]]></SUPERVISOR><SERVE_ADDR><![CDATA[%s]]></SERVE_ADDR><SERVE_USER><![CDATA[%s]]></SERVE_USER><SERVE_PASSWORD><![CDATA[%s]]></SERVE_PASSWORD><ADMINSTRATION_IP><![CDATA[%s]]></ADMINSTRATION_IP><UNIT_NAME><![CDATA[%s]]></UNIT_NAME><NET_IP><![CDATA[%s]]></NET_IP><MANAGE_PORT1><![CDATA[%s]]></MANAGE_PORT1><ADMINSTRATION_IP1><![CDATA[%s]]></ADMINSTRATION_IP1><config_msg>请 输 入 正 确 的 端 口 号（正整数）,保 存 失 败</config_msg></report></get_reports_response>",tips_config_manage.manage_IP, tips_config_manage.manage_PORT,tips_config_manage.gateway, tips_config_manage.static_route, tips_config_manage.supervisor, tips_config_manage.serve_addr, tips_config_manage.serve_user, tips_config_manage.serve_password, tips_config_manage.adminstration_IP, tips_config_manage.unit_name, tips_config_manage.net_IP, tips_config_manage.manage_PORT1, tips_config_manage.adminstration_IP1);
						else 
                	        	xml = (char *)g_strdup_printf("<get_reports_response><report type=\"config\"><MANAGE_IP><![CDATA[%s]]></MANAGE_IP><MANAGE_PORT><![CDATA[%s]]></MANAGE_PORT><GATEWAY><![CDATA[%s]]></GATEWAY><STATIC_ROUTE><![CDATA[%s]]></STATIC_ROUTE><SUPERVISOR><![CDATA[%s]]></SUPERVISOR><SERVE_ADDR><![CDATA[%s]]></SERVE_ADDR><SERVE_USER><![CDATA[%s]]></SERVE_USER><SERVE_PASSWORD><![CDATA[%s]]></SERVE_PASSWORD><ADMINSTRATION_IP><![CDATA[%s]]></ADMINSTRATION_IP><UNIT_NAME><![CDATA[%s]]></UNIT_NAME><NET_IP><![CDATA[%s]]></NET_IP><MANAGE_PORT1><![CDATA[%s]]></MANAGE_PORT1><ADMINSTRATION_IP1><![CDATA[%s]]></ADMINSTRATION_IP1><config_msg>请 输 入 正 确 的 地 址 格 式、端 口 号（正整数）,保 存 失 败</config_msg></report></get_reports_response>",tips_config_manage.manage_IP, tips_config_manage.manage_PORT,tips_config_manage.gateway, tips_config_manage.static_route, tips_config_manage.supervisor, tips_config_manage.serve_addr, tips_config_manage.serve_user, tips_config_manage.serve_password, tips_config_manage.adminstration_IP, tips_config_manage.unit_name, tips_config_manage.net_IP, tips_config_manage.manage_PORT1, tips_config_manage.adminstration_IP1);
						 
					}
						 printf("ip_flag+++++++++++++++++++++++++++++++++++%d\n",ip_flag);
						 printf("isint_flag+++++++++++++++++++++++++++++++++++%d\n",isint_flag);
//                         config_manage_modify(&tips_config_manage, credentials);
#endif
#if 0
						if(!strcmp(cmd_name_judge,"保存"))
						{
							config_manage_modify(&tips_config_manage, credentials);
							if(0==ip_flag && 0==isint_flag)
							{
								config_manage_modify(&tips_config_manage, credentials);
                	        	xml = (char *)g_strdup_printf("<get_reports_response><report type=\"config\"><MANAGE_IP><![CDATA[%s]]></MANAGE_IP><MANAGE_PORT><![CDATA[%s]]></MANAGE_PORT><GATEWAY><![CDATA[%s]]></GATEWAY><STATIC_ROUTE><![CDATA[%s]]></STATIC_ROUTE><SUPERVISOR><![CDATA[%s]]></SUPERVISOR><SERVE_ADDR><![CDATA[%s]]></SERVE_ADDR><SERVE_USER><![CDATA[%s]]></SERVE_USER><SERVE_PASSWORD><![CDATA[%s]]></SERVE_PASSWORD><ADMINSTRATION_IP><![CDATA[%s]]></ADMINSTRATION_IP><UNIT_NAME><![CDATA[%s]]></UNIT_NAME><NET_IP><![CDATA[%s]]></NET_IP><MANAGE_PORT1><![CDATA[%s]]></MANAGE_PORT1><ADMINSTRATION_IP1><![CDATA[%s]]></ADMINSTRATION_IP1><config_msg>保存成功</config_msg></report></get_reports_response>",tips_config_manage.manage_IP, tips_config_manage.manage_PORT,tips_config_manage.gateway, tips_config_manage.static_route, tips_config_manage.supervisor, tips_config_manage.serve_addr, tips_config_manage.serve_user, tips_config_manage.serve_password, tips_config_manage.adminstration_IP, tips_config_manage.unit_name, tips_config_manage.net_IP, tips_config_manage.manage_PORT1, tips_config_manage.adminstration_IP1);
							}
							if(0==ip_flag && 1==isint_flag)
                	        	xml = (char *)g_strdup_printf("<get_reports_response><report type=\"config\"><MANAGE_IP><![CDATA[%s]]></MANAGE_IP><MANAGE_PORT><![CDATA[%s]]></MANAGE_PORT><GATEWAY><![CDATA[%s]]></GATEWAY><STATIC_ROUTE><![CDATA[%s]]></STATIC_ROUTE><SUPERVISOR><![CDATA[%s]]></SUPERVISOR><SERVE_ADDR><![CDATA[%s]]></SERVE_ADDR><SERVE_USER><![CDATA[%s]]></SERVE_USER><SERVE_PASSWORD><![CDATA[%s]]></SERVE_PASSWORD><ADMINSTRATION_IP><![CDATA[%s]]></ADMINSTRATION_IP><UNIT_NAME><![CDATA[%s]]></UNIT_NAME><NET_IP><![CDATA[%s]]></NET_IP><MANAGE_PORT1><![CDATA[%s]]></MANAGE_PORT1><ADMINSTRATION_IP1><![CDATA[%s]]></ADMINSTRATION_IP1><config_msg>保存成功</config_msg><config_submit_msg>端口号为正整数</config_submit_msg></report></get_reports_response>",tips_config_manage.manage_IP, tips_config_manage.manage_PORT,tips_config_manage.gateway, tips_config_manage.static_route, tips_config_manage.supervisor, tips_config_manage.serve_addr, tips_config_manage.serve_user, tips_config_manage.serve_password, tips_config_manage.adminstration_IP, tips_config_manage.unit_name, tips_config_manage.net_IP, tips_config_manage.manage_PORT1, tips_config_manage.adminstration_IP1);
							if(1==ip_flag && 0==isint_flag)
						}
			    		else
						{
							printf("在提交里边\n");
                	        xml = (char *)g_strdup_printf("<get_reports_response><report type=\"config\"><MANAGE_IP><![CDATA[%s]]></MANAGE_IP><MANAGE_PORT><![CDATA[%s]]></MANAGE_PORT><GATEWAY><![CDATA[%s]]></GATEWAY><STATIC_ROUTE><![CDATA[%s]]></STATIC_ROUTE><SUPERVISOR><![CDATA[%s]]></SUPERVISOR><SERVE_ADDR><![CDATA[%s]]></SERVE_ADDR><SERVE_USER><![CDATA[%s]]></SERVE_USER><SERVE_PASSWORD><![CDATA[%s]]></SERVE_PASSWORD><ADMINSTRATION_IP><![CDATA[%s]]></ADMINSTRATION_IP><UNIT_NAME><![CDATA[%s]]></UNIT_NAME><NET_IP><![CDATA[%s]]></NET_IP><MANAGE_PORT1><![CDATA[%s]]></MANAGE_PORT1><ADMINSTRATION_IP1><![CDATA[%s]]></ADMINSTRATION_IP1><config_submit_msg>提交成功</config_submit_msg></report></get_reports_response>",tips_config_manage.manage_IP,tips_config_manage.manage_PORT, tips_config_manage.gateway, tips_config_manage.static_route, tips_config_manage.supervisor, tips_config_manage.serve_addr, tips_config_manage.serve_user, tips_config_manage.serve_password, tips_config_manage.adminstration_IP, tips_config_manage.unit_name, tips_config_manage.net_IP, tips_config_manage.manage_PORT1, tips_config_manage.adminstration_IP1);
						}
#endif
                		    html = ((admin_opt_fun *)opt_fun)(credentials, xml);
                		    g_free(xml);
                		    g_free((void *)cmd_name_judge);
                		    return html;
                		}
		else if(!strcmp(cmd, "system_config_time")){

						char *display=NULL;
						char *xml=NULL;
						int i = 0,time_flag = 0;
						int null_time_flag = 0;
						int strlen_flag[2] = {0,0};
						int msg1_flag = 0;

	                			system_config_t tips_system_config;
	                			system_config_t tips_system_config_time;
                        			memset(&tips_system_config, 0, sizeof(tips_system_config));
                        			memset(&tips_system_config_time, 0, sizeof(tips_system_config_time));
						tips_system_config.modify_flag = 0;
        					get_system_config_xml(&tips_system_config_time);
						strcpy(tips_system_config.cpu,tips_system_config_time.cpu);
						strcpy(tips_system_config.disk,tips_system_config_time.disk);
						strcpy(tips_system_config.mem,tips_system_config_time.mem);
						strcpy(tips_system_config.alarmtime,tips_system_config_time.alarmtime);
						strcpy(tips_system_config.netstatetime,tips_system_config_time.netstatetime);
						if(NULL != params_value(params, "runtime"))
						{
							strcpy(tips_system_config.runtime, params_value(params, "runtime"));
							printf("jfffffffffffffffffffff%sffffffffffffffffffffffff\n",tips_system_config.runtime);
							for(i = 0;tips_system_config.runtime[i] != 0;i++)		
							{
								if(!isdigit(tips_system_config.runtime[i]))
									time_flag = 1;
							}
						}
						if(NULL != params_value(params, "alarmtime"))
						{
							strcpy(tips_system_config.syntime, params_value(params, "alarmtime"));
							printf("jfffffffffffffffffffff%sffffffffffffffffffffffff\n",tips_system_config.syntime);
							for(i = 0;tips_system_config.syntime[i] != 0;i++)		
							{
								if(!isdigit(tips_system_config.syntime[i]))
									time_flag = 1;
							}
						}
						if(time_flag == 1)
						{
	                		xml = (char *)g_strdup_printf("<sysSetting><cpu><![CDATA[%s]]></cpu><mem><![CDATA[%s]]></mem><disk><![CDATA[%s]]></disk><runtime><![CDATA[%s]]></runtime><alarmtime><![CDATA[%s]]></alarmtime><msg_product>请输入正确的整数</msg_product></sysSetting>",tips_system_config.cpu, tips_system_config.mem, tips_system_config.disk, tips_system_config.runtime, tips_system_config.syntime);
                            html = ((admin_opt_fun *)opt_fun)(credentials, xml);
                            g_free(xml);
                            return html;
						}
                        if((display=system_config_modify(&tips_system_config))){
							//char msg_buf[100];
							//memset(msg_buf,0,sizeof(100));
							char msg_buf1[100];
							memset(msg_buf1,0,sizeof(100));
							//strcpy(msg_buf,"提示");
							strcpy(msg_buf1,"提示");
							if(!strlen(tips_system_config.runtime))
							{
								strcat(msg_buf1,":上报运行状态间隔时间不能够为空");
								null_time_flag = 1;
								strlen_flag[0] = 1;
							}
							if(!strlen(tips_system_config.syntime))
							{
								strcat(msg_buf1,":上报报警信息间隔时间不能够为空");
								null_time_flag = 1;
								strlen_flag[1] = 1;
							}
							if(strlen_flag[0] == 1 && strlen_flag[1] == 1)
								msg1_flag = 1;
							if(null_time_flag==1)
							{
								//if(msg_flag == 1 && msg1_flag == 1)
									xml = (char *)g_strdup_printf("<sysSetting><cpu><![CDATA[%s]]></cpu><mem><![CDATA[%s]]></mem><disk><![CDATA[%s]]></disk><runtime><![CDATA[%s]]></runtime><alarmtime><![CDATA[%s]]></alarmtime><msg_product>%s</msg_product></sysSetting>",tips_system_config.cpu, tips_system_config.mem, tips_system_config.disk, tips_system_config.runtime, tips_system_config.syntime,msg_buf1);
							}
							else 
							{
								if(tips_system_config.modify_flag)
	                						xml = (char *)g_strdup_printf("<sysSetting><cpu><![CDATA[%s]]></cpu><mem><![CDATA[%s]]></mem><disk><![CDATA[%s]]></disk><runtime><![CDATA[%s]]></runtime><alarmtime><![CDATA[%s]]></alarmtime><msg_product>提交成功</msg_product></sysSetting>",tips_system_config.cpu, tips_system_config.mem, tips_system_config.disk, tips_system_config.runtime, tips_system_config.syntime);
								else
	                						xml = (char *)g_strdup_printf("<sysSetting><cpu><![CDATA[%s]]></cpu><mem><![CDATA[%s]]></mem><disk><![CDATA[%s]]></disk><runtime><![CDATA[%s]]></runtime><alarmtime><![CDATA[%s]]></alarmtime><msg_product>本次设置值与保存值相同</msg_product></sysSetting>",tips_system_config.cpu, tips_system_config.mem, tips_system_config.disk, tips_system_config.runtime, tips_system_config.syntime);
							}
				if(tips_system_config.modify_flag)
					tips_sqlite_handle.catch_opt(credentials->username, display);
				g_free(display);
                                html = ((admin_opt_fun *)opt_fun)(credentials, xml);
                                g_free(xml);
                                return html;
			}
		}
		else if(!strcmp(cmd, "system_config")){
#if 0
                                  system_config_t tips_system_config;
                                  memset(&tips_system_config, 0, sizeof(tips_system_config));
					get_system_config_xml(&tips_system_config);
printf("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaammmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm\n");
			
                char *xml = (char *)g_strdup_printf("<sysSetting><cpu>%s</cpu><mem>%s</mem><disk>%s</disk><runtime>%s</runtime><alarmtime>%s</alarmtime></sysSetting>",tips_system_config.cpu,tips_system_config.mem,tips_system_config.disk,tips_system_config.runtime,tips_system_config.alarmtime);
#endif
#if 1 
	
												char *display=NULL;
												char *xml=NULL;
												int i = 0,isint_flag = 0;
												int null_flag =0;
												int strlen_flag[3] = {0,0,0};
												int msg_flag = 0;

	                			system_config_t tips_system_config;
	                			system_config_t tips_system_config_time;
                        			memset(&tips_system_config, 0, sizeof(tips_system_config));
                        			memset(&tips_system_config_time, 0, sizeof(tips_system_config_time));
						tips_system_config.modify_flag = 0;
        					get_system_config_xml(&tips_system_config_time);
						strcpy(tips_system_config.alarmtime,tips_system_config_time.alarmtime);
						strcpy(tips_system_config.runtime,tips_system_config_time.runtime);
						if(NULL != params_value(params, "cpu"))
						{
							strcpy(tips_system_config.cpu, params_value(params, "cpu"));
							printf("jfffffffffffffffffffff%sffffffffffffffffffffffff\n",tips_system_config.cpu);
							for(i = 0;tips_system_config.cpu[i] != 0;i++)		
							{
								printf("isint_flag ===%d\n",isint_flag);
								if(!isdigit(tips_system_config.cpu[i]))
									isint_flag = 1;
							}
							int cpu_judge = atoi(tips_system_config.cpu); 	
							if(cpu_judge > 100)
								isint_flag = 1;
						}
						if(NULL != params_value(params, "mem"))
						{
							strcpy(tips_system_config.mem, params_value(params, "mem"));
							printf("jfffffffffffffffffffff%sffffffffffffffffffffffff\n",tips_system_config.mem);
							for(i = 0;tips_system_config.mem[i] != 0;i++)		
							{
								if(!isdigit(tips_system_config.mem[i]))
									isint_flag = 1;
							}
							int mem_judge = atoi(tips_system_config.mem); 	
							if(mem_judge > 100)
								isint_flag = 1;
						}
						if(NULL != params_value(params, "disk"))
						{
							strcpy(tips_system_config.disk, params_value(params, "disk"));
							printf("jfffffffffffffffffffff%sffffffffffffffffffffffff\n",tips_system_config.disk);
							for(i = 0;tips_system_config.disk[i] != 0;i++)		
							{
								if(!isdigit(tips_system_config.disk[i]))
									isint_flag = 1;
							}
								int disk_judge = atoi(tips_system_config.disk);
								if(disk_judge > 1024)
									isint_flag = 1;
						}
						if(isint_flag == 1)
						{
	                				xml = (char *)g_strdup_printf("<sysSetting><cpu><![CDATA[%s]]></cpu><mem><![CDATA[%s]]></mem><disk><![CDATA[%s]]></disk><runtime><![CDATA[%s]]></runtime><alarmtime><![CDATA[%s]]></alarmtime><msg>cpu，内存利用率不能够大于100,磁盘剩余空间不能够大于1024</msg></sysSetting>",tips_system_config.cpu, tips_system_config.mem, tips_system_config.disk, tips_system_config.runtime, tips_system_config.alarmtime);
                            				html = ((admin_opt_fun *)opt_fun)(credentials, xml);
                            				g_free(xml);
                            				return html;
						}
		
                        if((display=system_config_modify(&tips_system_config))){
							char msg_buf[100];
							memset(msg_buf,0,sizeof(100));
							//char msg_buf1[100];
							//memset(msg_buf1,0,sizeof(100));
							strcpy(msg_buf,"提示");
							//strcpy(msg_buf1,"提示");
							if(!strlen(tips_system_config.cpu))
							{
								strcat(msg_buf,":cpu利用率不能够为空");
								null_flag = 1;
								strlen_flag[0] = 1;
							}
							if(!strlen(tips_system_config.mem))
							{
								strcat(msg_buf,":内存利用率不能够为空");
								null_flag = 1;
								strlen_flag[1] = 1;
							}
							if(!strlen(tips_system_config.disk))
							{
								strcat(msg_buf,":磁盘剩余空间不能够为空");
								null_flag = 1;
								strlen_flag[2] = 1;
							}
							if(strlen_flag[0] == 1 && strlen_flag[1] == 1 && strlen_flag[2] == 1)
								msg_flag = 1;
							if(null_flag == 1)
							{
								if(msg_flag == 1)		
									xml = (char *)g_strdup_printf("<sysSetting><cpu><![CDATA[%s]]></cpu><mem><![CDATA[%s]]></mem><disk><![CDATA[%s]]></disk><runtime><![CDATA[%s]]></runtime><alarmtime><![CDATA[%s]]></alarmtime><msg>提示：设置不能够全为空</msg></sysSetting>",tips_system_config.cpu, tips_system_config.mem, tips_system_config.disk, tips_system_config.runtime, tips_system_config.alarmtime);
								else
									xml = (char *)g_strdup_printf("<sysSetting><cpu><![CDATA[%s]]></cpu><mem><![CDATA[%s]]></mem><disk><![CDATA[%s]]></disk><runtime><![CDATA[%s]]></runtime><alarmtime><![CDATA[%s]]></alarmtime><msg>%s</msg></sysSetting>",tips_system_config.cpu, tips_system_config.mem, tips_system_config.disk, tips_system_config.runtime, tips_system_config.alarmtime,msg_buf);
				
							}
							else 
							{	
								if(tips_system_config.modify_flag)
	                						xml = (char *)g_strdup_printf("<sysSetting><cpu><![CDATA[%s]]></cpu><mem><![CDATA[%s]]></mem><disk><![CDATA[%s]]></disk><runtime><![CDATA[%s]]></runtime><alarmtime><![CDATA[%s]]></alarmtime><msg>提交成功</msg></sysSetting>",tips_system_config.cpu, tips_system_config.mem, tips_system_config.disk, tips_system_config.runtime, tips_system_config.alarmtime);
								else
	                						xml = (char *)g_strdup_printf("<sysSetting><cpu><![CDATA[%s]]></cpu><mem><![CDATA[%s]]></mem><disk><![CDATA[%s]]></disk><runtime><![CDATA[%s]]></runtime><alarmtime><![CDATA[%s]]></alarmtime><msg>本次设置值与保存值相同</msg></sysSetting>",tips_system_config.cpu, tips_system_config.mem, tips_system_config.disk, tips_system_config.runtime, tips_system_config.alarmtime);
							}
				if(tips_system_config.modify_flag)
					tips_sqlite_handle.catch_opt(credentials->username, display);
				g_free(display);
                                html = ((admin_opt_fun *)opt_fun)(credentials, xml);
                                g_free(xml);
                                return html;
			}
#endif											
		}
	}
	else{
	        realxml = (char *)g_strdup_printf("<envelope><token>%s</token><msg>无操作模块权限</msg></envelope>",credentials->token);
                html = ((admin_opt_fun *)opt_fun)(credentials, realxml);
                g_free(realxml);
                return html;
        }
/*返回值？*/return NULL;
}
void CAdmin_opt::TurnTime(const time_t clock, char *tips_clock, int clocksize)
{
	struct tm *tm;
	char *format = (char *)"%Y-%m-%d %H:%M:%S";
	tm = localtime(&clock);
	strftime(tips_clock, clocksize, format, tm);
}

int CAdmin_opt::Getpid(pid_t *pid)
{
	FILE* fp = fopen(KILL_PID, "r");
	if(NULL == fp){
			printf("open error\n");
			return 1;
	}
	int num = fread(pid, 1, sizeof(pid_t), fp);
	if(num < 0){
		printf("num===%d\n",num);
		fclose(fp);
		return 1;
	}
	fclose(fp);
	return 0;
}

int CAdmin_opt::Change_ip(char *value, char *ethpath)
{
	char buf[1024];
	FILE *fp = fopen(ethpath, "r+");
	memset(buf, 0, 1024);
	char* result = g_strdup("");
	char* resulttemp = result;
	if(fp == NULL){
		goto nodata;
	}
	while(NULL != fgets(buf, 1024, fp)){
		if(!strncmp(buf, "IPADDR", 6)){
			continue;
		}
		else if(!strncmp(buf, "NETMASK", 7)){
			continue;
		}
		else{
			result = g_strdup_printf("%s%s",result,buf);
			g_free(resulttemp);
			resulttemp = result;
		}
	}
nodata:	result = g_strdup_printf("%sIPADDR=%s\nNETMASK=255.255.255.0",result, value);
	g_free(resulttemp);
	fclose(fp);
	if(NULL == (fp = fopen(ethpath, "w"))){
		return 1;
	}
	fwrite(result, 1, strlen(result), fp);
	g_free(result);
	fclose(fp);
	return 0;
}

int CAdmin_opt::Change_gateway(char *value, char *ethpath)
{
	char buf[1024];
	FILE *fp = fopen(ethpath, "r+");
	memset(buf, 0, 1024);
	char* result = g_strdup("");
	char* resulttemp = result;
	if(fp == NULL){
		goto nodata;
	}
	while(NULL != fgets(buf, 1024, fp)){
		if(!strncmp(buf, "GATEWAY", 7)){
			continue;
		}
		else{
			result = g_strdup_printf("%s%s",result,buf);
			g_free(resulttemp);
			resulttemp = result;
		}
	}
nodata:	result = g_strdup_printf("%sGATEWAY=%s\n",result, value);
	g_free(resulttemp);


	fclose(fp);
	if(NULL == (fp = fopen(ethpath, "w"))){
		return 1;
	}
	fwrite(result, 1, strlen(result), fp);
	printf("result====%s\n",result);
	g_free(result);
	fclose(fp);
	return 0;
}

/*gatewayip变成管理中心ip了*/
int CAdmin_opt::change_route(char *route_ip, char *gateway_ip)
{
	FILE *fp = fopen("/etc/sysconfig/network-scripts/route-eth1", "w");
	if(fp != NULL){
		char *result = g_strdup_printf("%s via %s dev eth1", gateway_ip, route_ip);
		fwrite(result, 1, strlen(result), fp);
		g_free(result);
		fclose(fp);
		return 0;
	}
	return 1;
}


/*
char *CAdmin_opt::cmd_to_display(char *cmd)
{
	
}




*/
