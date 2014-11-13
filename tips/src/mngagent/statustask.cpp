#include "statustask.h"
#include <iostream>
#include "data.h"
#include "mngchkdata.h"
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include "rulesdata.h"
#include "zlog.h"

extern char LOCALDEVID[];
extern struct config_struct config;
extern struct config_struct config_tmp;
struct status_part part;
extern int startrun;
extern struct status_struct status;
extern ShareMem shmobj;

StatusTask::StatusTask()
{
	//cout <<"LOCALDEVID========="<<LOCALDEVID <<endl;
	char stime[21] = {0};
	int ret = 0;
	ret = shmobj.Get_status_shm(14, stime, sizeof(stime));
	if(-1 == ret)
	{
		strcpy(status.device_id,LOCALDEVID);
		time_t start_time;
		time(&start_time);
		struct tm* p_tm;
		p_tm=gmtime(&start_time);
  	if(p_tm->tm_hour >= 16)
			p_tm->tm_hour = 8 + p_tm->tm_hour - 24;
		else 
			p_tm->tm_hour += 8;
		snprintf(stime,sizeof (status.stime),"%d-%d-%d %d:%d:%d",(p_tm->tm_year+1900),(p_tm->tm_mon+1),p_tm->tm_mday,p_tm->tm_hour,p_tm->tm_min,p_tm->tm_sec);
		shmobj.Set_status_shm(14, stime, sizeof(stime));
		shmobj.Get_status_shm(14, stime, sizeof(stime));
	}
	strcpy(status.stime, stime);
	printf("共享内存中开机时间为%s\n", stime);
	//cout<<"开机时间是:"<<status.stime<<endl;
}
int StatusTask::Get_Param(char* cmd,char* param)
{
	while(!isblank(*cmd++));
	strcpy(param,cmd);
	return 1;
}
int StatusTask::init_one_config(const char *source, const char *descript, FILE *fp)
{
    char temp[CONFIG_MAXLINE];
    memset(temp,0,sizeof(temp));
    printf("source:%s descript:%s\n",source, descript);
    if(strlen(source))
    {
        sprintf(temp,"%s=%s\n",descript,source);
        fwrite(temp, 1, strlen(temp), fp);
    }
    return 0;
}
int StatusTask::get_config_manage_xml(void *config)
{
    char buf[CONFIG_MAXLINE];
    char *temp;
    FILE *fp = fopen(CONFIG_FILE, "r+");
    if(fp == NULL){
        //g_mutex_unlock(filemutex);
        return 1;
    }
    else{
        memset(buf, 0, CONFIG_MAXLINE);
        while(NULL != fgets(buf, CONFIG_MAXLINE, fp)){
            if(strlen(buf) >= CONFIG_MAXLINE-1){
                fclose(fp);
                printf("can't open xml\n");
                //g_mutex_unlock(filemutex);
                return 1;
            }
            if((temp = strstr(buf, "=")) != NULL){
                int len = strlen(temp + 1);
                /*判断每个配置项的内容格式*/
                if(!strncmp(buf, "MANAGE_IP", 9)){
                    memcpy(((config_manage_t *)config)->manage_IP,temp+1,
                            ((len < 50) ? len-1 : 49));
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
        //g_mutex_unlock(filemutex);
        return 0;
    }

}
void StatusTask::Parse_Cmd(char* cmd)
{
	cout<<"Pares_Cmd00000000000000000000000000000000000000000000000000000"<<endl;
//	ReportTask report;
	char* errMsg = NULL;	
	//cout <<"cmd = "<<cmd<<endl;
	if(!strncmp(cmd,"reboot",6))
	{//重启
	/*	char cmdbuf[32];
		strcpy(cmdbuf,"Y+6");
		cpostman.Send_Msg2(cmdbuf,strlen(cmdbuf),NULL,0);*/
		signal_usr1 ((char*)"reboot", errMsg);	
	}
	else if(!strncmp(cmd,"shutdown",8))
	{//关机
	/*	char cmdbuf[32];
		strcpy(cmdbuf,"Y+0");
		cpostman.Send_Msg2(cmdbuf,strlen(cmdbuf),NULL,0);*/
		signal_usr1 ((char*)"shutdown", errMsg);	
	}
	else if(!strncmp(cmd,"update",6))
	{//升级
//hyj		int cnt = 0;
		char filename[128];
		if (cmd + 7 != NULL)
			strcpy(filename,cmd+7);
		else
			return;
		ointerface.Update_Sys(filename);
		char buf[1024] = {0};
		char *token = strrchr (filename, '_');
		strncpy (buf, token + 1, sizeof (buf));
		char *sign = strrchr (buf, '.');
		buf [strlen (buf) - strlen (sign)] = '\0';
		strcpy (config.version, buf);
	  	 int rc = ointerface.Send_Config (&config, NULL);
		if (-1 == rc)
		{
			 struct cache_opt_t opt_stut;
			 INIT_OPT_HEAD(&opt_stut);
             strcpy(opt_stut.username,"system");
             time_t t;
             time(&t);
             funhandler->Turntime(t,opt_stut.opttime,24);
             strcpy(opt_stut.describe,"上报配置信息失败");
             int send_err=cacheclient->Send_Msg((char*)&opt_stut,sizeof(opt_stut));
			 send_err = send_err; 		//hyj 未使用的变量
		}
		char cmdbuf[150];
		memset (cmdbuf, 0, sizeof(cmdbuf));
//hyj		char mytime[20] = {0};
		//ReportTask report;
		struct cache_opt_t opt_stut;
	 	INIT_OPT_HEAD(&opt_stut);
        strcpy(opt_stut.username,"system");
        time_t t;
        time(&t);
        funhandler->Turntime(t,opt_stut.opttime,24);
        strcpy(opt_stut.describe,"升级成功");
        int send_err=cacheclient->Send_Msg((char*)&opt_stut,sizeof(opt_stut));
		send_err = send_err; 		//hyj 未使用的变量
	}
	else if(!strncmp(cmd,"netlog",6))
	{//上报网络会话信息
		cout <<"cmd netlog ============" << cmd <<endl;
		char param[256];
		memset (param, 0, sizeof (param));
		Get_Param(cmd, param);
		CNetlog netlog(param);
		netlog.Set_Sqlhandler(sqlhandler);
		//cout <<"param ===================" << param <<endl; 
		cout <<"start netlog.deal" <<endl;
		netlog.deal();
		cout <<"end netlog.deal" <<endl;
		struct cache_opt_t opt_stut;
	 	INIT_OPT_HEAD(&opt_stut);
        strcpy(opt_stut.username,"system");
        time_t t;
        time(&t);
        funhandler->Turntime(t,opt_stut.opttime,24);
        strcpy(opt_stut.describe,"执行netlog命令成功");
        int send_err=cacheclient->Send_Msg((char*)&opt_stut,sizeof(opt_stut));
		send_err = send_err; 		//hyj 未使用的变量
	}
	else if(!strncmp(cmd,"newcenter",9))
    {
        char ip_port[4][17];
        char *p1 = NULL;
        int i = 0;
        FILE *fp = NULL;
        config_manage_t sconfig;
        for(i=0;i<4;i++)
            memset(ip_port[i],0,sizeof(ip_port[i]));
        p1 = strchr(cmd+12,'+');
        if(p1)
        {
            if('1' == *(cmd+10))
                sscanf(cmd+12,"%[^:]:%[^+]%*[^-]-%[^:]:%s",ip_port[0],ip_port[1],ip_port[2],ip_port[3]);
            else
                sscanf(cmd+12,"%[^:]:%[^+]%*[^-]-%[^:]:%s",ip_port[2],ip_port[3],ip_port[0],ip_port[1]);
        }
        else
        {
            if('1' == *(cmd+10))
                sscanf(cmd+12,"%[^:]:%s",ip_port[0],ip_port[1]);
            else
                sscanf(cmd+12,"%[^:]:%s",ip_port[2],ip_port[3]);
        }
        memset(&sconfig,0,sizeof(config_manage_t));
        sconfig.modify_flag  = 0;
        i = get_config_manage_xml(&sconfig);
        if(i > 0)
            printf("Get config information failed!\n");
        printf("%s",sconfig.manage_IP);
        printf("%s",sconfig.net_IP);
        if(0 != strlen(ip_port[0]) && strcmp(sconfig.adminstration_IP,ip_port[0]))
        {
            memset(sconfig.adminstration_IP,0,sizeof(sconfig.adminstration_IP));
            strcpy(sconfig.adminstration_IP,ip_port[0]);
            sconfig.modify_flag  = 1;
        }
        if(0 != strlen(ip_port[1]) && strcmp(sconfig.manage_PORT,ip_port[1]))
        {
            memset(sconfig.manage_PORT,0,sizeof(sconfig.manage_PORT));
            strcpy(sconfig.manage_PORT,ip_port[1]);
            sconfig.modify_flag  = 1;
        }
        if(0 != strlen(ip_port[2]) && strcmp(sconfig.adminstration_IP1,ip_port[2]))
        {
            memset(sconfig.adminstration_IP1,0,sizeof(sconfig.adminstration_IP1));
            strcpy(sconfig.adminstration_IP1,ip_port[2]);
            sconfig.modify_flag  = 1;
        }
        if(0 != strlen(ip_port[3]) && strcmp(sconfig.manage_PORT1,ip_port[3]))
        {
            memset(sconfig.manage_PORT1,0,sizeof(sconfig.manage_PORT1));
            strcpy(sconfig.manage_PORT1,ip_port[3]);
            sconfig.modify_flag  = 1;
        }
        if(sconfig.modify_flag)
        {
            printf("have change IP or Port!\n");
            fp = fopen(CONFIG_FILE, "w+");
            init_one_config(sconfig.adminstration_IP,"ADMINSTRATION_IP",fp);
            init_one_config(sconfig.adminstration_IP1,"ADMINSTRATION_IP1",fp);
            init_one_config(sconfig.manage_PORT,"MANAGE_PORT",fp);
            init_one_config(sconfig.manage_PORT1,"MANAGE_PORT1",fp);
            init_one_config(sconfig.manage_IP,"MANAGE_IP",fp);
            init_one_config(sconfig.net_IP,"NET_IP",fp);
            init_one_config(sconfig.gateway,"GATEWAY",fp);
            init_one_config(sconfig.static_route,"STATIC_ROUTE",fp);
            init_one_config(sconfig.supervisor,"SUPERVISOR",fp);
            init_one_config(sconfig.serve_addr,"SERVE_ADDR",fp);
            init_one_config(sconfig.serve_user,"SERVE_USER",fp);
            init_one_config(sconfig.serve_password,"SERVE_PASSWORD",fp);
            init_one_config(sconfig.unit_name,"UNIT_NAME",fp);
            fclose(fp);
            system("./Kill_Start.sh");
        }
        else
            printf("have not change IP or Port!\n");

        //解析IP和端口
        //更新配置文件主管理和副管理中心
        //重启进程（调用KillAll.sh）
    }
	else if(!strncmp(cmd,"reconfig",8))
	{//策略更新
		char filename[64];
		strcpy(filename,cmd+9);
		//cout << "filename ====================" <<filename << endl;
		printf("filename=======================%s\n",filename);
		struct load_struct load;
		if(!strncmp(filename,"dnslist",7))
		{
			cout <<"dns start" <<endl;
			struct dns_rules* dnsr;
			load.rules=(void**)&dnsr;
			load.type=1;
			memset (config.dns_version, 0, sizeof (config.dns_version));
			strcpy (config.dns_version, filename + 8);
			config.dns_version[strlen (config.dns_version) - 6] = '\0';
//			memset (config_tmp.dns_version, 0, sizeof (config_tmp.dns_version));
//			strcpy (config_tmp.dns_version, filename + 8);
//			config_tmp.dns_version[strlen (config_tmp.dns_version) - 6] = '\0';
//			shmobj.Set_status_shm(19, &(config.dns_version), sizeof(config.dns_version));
//			shmobj.Get_status_shm(19, &(config.dns_version), sizeof(config.dns_version));
			cout<<"dns_version "<<config.dns_version<<endl;
		}
#if 1
		else if(!strncmp(filename,"iplist",6))
		{
			struct ip_rules* ipr;
			load.rules=(void**)&ipr;
			load.type=2;
			memset (config.ip_version, 0 ,sizeof (config.ip_version));
			strcpy (config.ip_version, filename + 7);
			config.ip_version[strlen (config.ip_version) - 6] = '\0';
//			memset (config_tmp.ip_version, 0 ,sizeof (config_tmp.ip_version));
//			strcpy (config_tmp.ip_version, filename + 7);
//			config_tmp.ip_version[strlen (config_tmp.ip_version) - 6] = '\0';
//			shmobj.Set_status_shm(20, &(config.ip_version), sizeof(config.ip_version));
//			shmobj.Get_status_shm(20, &(config.ip_version), sizeof(config.ip_version));
			cout<<"ip_version "<<config.ip_version<<endl;
		}
#endif
		else if(!strncmp(filename,"swiplist",8))
		{
			//conf
			struct ipaudit_rules* ipar;
			load.rules=(void**)&ipar;
			load.type=3;
			memset (config.swip_version, 0, sizeof (config.swip_version));
			strcpy (config.swip_version, filename + 9);
			config.swip_version[strlen (config.swip_version) - 5] = '\0';
//			memset (config_tmp.swip_version, 0, sizeof (config_tmp.swip_version));
//			strcpy (config_tmp.swip_version, filename + 9);
//			config_tmp.swip_version[strlen (config_tmp.swip_version) - 5] = '\0';
//			shmobj.Set_status_shm(21, &(config.swip_version), sizeof(config.swip_version));
//			shmobj.Get_status_shm(21, &(config.swip_version), sizeof(config.swip_version));
			cout<<"swip_version "<<config.swip_version<<endl;
		}
		else if(!strncmp(filename,"senlist",7))
		{
			//conf
			struct sec_rules* secr;
			load.rules=(void**)&secr;
			load.type=4;
			memset (config.sen_version, 0, sizeof (config.sen_version));
			strcpy (config.sen_version, filename + 8);
			config.sen_version[strlen (config.sen_version) - 5] = '\0';
//			memset (config_tmp.sen_version, 0, sizeof (config_tmp.sen_version));
//			strcpy (config_tmp.sen_version, filename + 8);
//			config_tmp.sen_version[strlen (config_tmp.sen_version) - 5] = '\0';
//			shmobj.Set_status_shm(22, &(config.sen_version), sizeof(config.sen_version));
//			shmobj.Get_status_shm(22, &(config.sen_version), sizeof(config.sen_version));
			cout<<"sen_version "<<config.sen_version<<endl;
		}
		else if(!strncmp(filename,"spectro",7))
		{
			struct spectro_rules* spec;
			load.rules=(void**)&spec;
			load.type=5;
			memset (config.spe_version, 0, sizeof (config.spe_version));
			strcpy (config.spe_version, filename + 8);
			config.spe_version[strlen (config.spe_version) - 6] = '\0';
//			memset (config_tmp.spe_version, 0, sizeof (config_tmp.spe_version));
//			strcpy (config_tmp.spe_version, filename + 8);
//			config_tmp.spe_version[strlen (config_tmp.spe_version) - 6] = '\0';
//			shmobj.Set_status_shm(24, &(config.spe_version), sizeof(config.spe_version));
//			shmobj.Get_status_shm(24, &(config.spe_version), sizeof(config.spe_version));
			cout<<"spe_version "<<config.spe_version<<endl;
		}
#if 1
		else if(!strncmp(filename,"geoip",5))
		{
			//conf
			struct ippos_rules* posr;
			load.rules=(void**)&posr;
			load.type=6;
			memset (config.geoip_version, 0, sizeof (config.geoip_version));
			strcpy (config.geoip_version, filename + 6);
			config.geoip_version[strlen(config.geoip_version) - 5] = '\0';
	//		memset (config_tmp.geoip_version, 0, sizeof (config_tmp.geoip_version));
	//		strcpy (config_tmp.geoip_version, filename + 6);
	//		config_tmp.geoip_version[strlen(config_tmp.geoip_version) - 5] = '\0';
//			shmobj.Set_status_shm(25, &(config.geoip_version), sizeof(config.geoip_version));
//			shmobj.Get_status_shm(25, &(config.geoip_version), sizeof(config.geoip_version));
			cout<<"geoip_version "<<config.geoip_version<<endl;
		}
#endif
#if 1
		else if(!strncmp(filename,"maillist", 8))
		{
			struct mail_rules* posr;
			load.rules=(void**)&posr;
			load.type=7;
			memset (config.mail_version, 0, sizeof(config.mail_version));
			strcpy (config.mail_version, filename + 9);
			config.mail_version[strlen(config.mail_version) - 5] = '\0';
//			memset (config_tmp.mail_version, 0, sizeof(config_tmp.mail_version));
//			strcpy (config_tmp.mail_version, filename + 9);
//			config_tmp.mail_version[strlen(config_tmp.mail_version) - 5] = '\0';
      			fprintf (stdout, "mail_version ========== %s\n", config.mail_version);;
//			shmobj.Set_status_shm(23, &(config.mail_version), sizeof(config.mail_version));
//			shmobj.Get_status_shm(23, &(config.mail_version), sizeof(config.mail_version));
			cout<<"mail_version "<<config.mail_version<<endl;
		}
#endif
   		if (strlen (config.mail_version) == 0)
			{
				strcpy (config.mail_version, "0");
//				shmobj.Set_status_shm(23, &(config.mail_version), sizeof(config.mail_version));
//				shmobj.Get_status_shm(23, &(config.mail_version), sizeof(config.mail_version));
				cout<<"mail_version "<<config.mail_version<<endl;
			}
//   		if (strlen (config_tmp.mail_version) == 0)
//			{
//				strcpy (config_tmp.mail_version, "0");
//				shmobj.Set_status_shm(23, &(config.mail_version), sizeof(config.mail_version));
//				shmobj.Get_status_shm(23, &(config.mail_version), sizeof(config.mail_version));
//				cout<<"mail_version "<<config_tmp.mail_version<<endl;
//			}
		memset (config.own, 0, sizeof (config.own));
		strcpy (config.own, "0");
		char sbuf[1024];
		memset (sbuf, 0, sizeof(sbuf));
//		if(5 == load.type)
//						return;
		printf("load.type=%d\n", load.type);
//		if(load.type == 6)
//		 return;
		int count=ointerface.Load_Rules(filename,load.rules,load.type);
//		printf("Load_Rules return %d\n", count);
//		printf("load.rules====");
#if 0
		char * rules_tmp = (char *) *load.rules;
		for(int i = 0;i<100; i++)
		{
			printf("%x",*rules_tmp);
			rules_tmp++;
		}
#endif 
		if(!count)
						return;
		if (6 == load.type)
		{
			for (int i = 0; i < count; i ++)
			{
				if (0 == ((struct ippos_rules*)*(load.rules))[i].startip)
				{
					if (!strcmp(((struct ippos_rules*)*(load.rules))[i].tirr, "云南省"))
					{
						((struct ippos_rules*)*(load.rules))[i].startip = 994902016;
					}
				}
			}
		}

#if 0

		if(load.type == 1)
		{
			struct dns_rules* dns_rules_ptr = (struct dns_rules*)*load.rules;
			for(int i=0; i<count; i++)
			{
				printf("dns_rules->type=%c\n",dns_rules_ptr->type);
				printf("dns_rules->dns_name=%s\n",dns_rules_ptr->dns_name);
				printf("dns_rules->match_mode=%c\n",dns_rules_ptr->match_mode);
				printf("dns_rules->dangerous=%c\n",dns_rules_ptr->dangerous);
				dns_rules_ptr ++;
			}
		}
#endif

		int headlen = 0;
		int ret = Get_Format(filename, count, sbuf, load.rules, &headlen);
//hyj		int checkget = 0;
//hyj		int mm;
		if (1 == ret)
		{
			switch (load.type)
			{
				case 1:
					{
						break;
					}
				case 2:
			//		cout <<"iplist start "<<endl;
	//					printf("frules========%x\n",frules);
	 				cpostman.Send_Msg2(DOMAIN_ALARM, frules, headlen + count * sizeof (struct ip_rules), NULL, 0, 0);
				//	cout << "iplist end" <<endl;
					break;
				case 4:
			//		cout <<"sec start" <<endl;
		//				printf("frules========%x\n",frules);
	 				//cpostman.Send_Msg2(DOMAIN_SENSITIVE,frules, strlen (frules) + 1 + count * sizeof (struct sec_rules), NULL, 0, 0);
			//		cout <<"sec end "<<endl;
					break;
				case 5:
				//	cout <<"spetro rules start" <<endl;
			//			printf("frules========%x\n",frules);
	 				cpostman.Send_Msg2(DOMAIN_ALARM, frules, headlen + count * sizeof (struct spectro_rules), NULL, 0, 0);
				//	cout <<"spetro rules end" <<endl;
					break;
#if 1 
				case 6:
					struct ippos_rules ipp;
				//	cout <<"ippos start" <<endl;
				//		printf("frules========%x\n",frules);
					printf("headlen=%d\n",headlen);
	 				cpostman.Send_Msg2(DOMAIN_CACHE, frules, headlen + count * sizeof (ipp), NULL, 0, 0);
					printf("send ippos end\n");
					printf("line = %d\n", __LINE__);
				//	cout <<"ippos end" <<endl;
					break;
				case 7:
				//	cout <<"mail post start" <<endl;
					//	printf("frules========%x\n",frules);
	 				//cpostman.Send_Msg2(DOMAIN_SENSITIVE, frules, strlen (frules) + 1 + count * sizeof (struct mail_rules), NULL, 0, 0);
				//	cout <<"mail post success" <<endl;
					break;
#endif
				default:break;
			}
		}
		else
		{	
			switch (load.type)
			{
				case 1:
					{
			//			printf("frules========%x\n",frules);
#if 1
							if(load.type == 1)
							{
								struct dns_rules* dns_rules_ptr = (struct dns_rules*)*load.rules;
								for(int i=0; i<count; i++)
								{
									printf("dns_rules->type=%c\n",dns_rules_ptr->type);
									printf("dns_rules->dns_name=%s\n",dns_rules_ptr->dns_name);
									printf("dns_rules->match_mode=%c\n",dns_rules_ptr->match_mode);
									printf("dns_rules->dangerous=%c\n",dns_rules_ptr->dangerous);
									dns_rules_ptr ++;
								}
							}
#endif
	 					cpostman.Send_Msg2(DOMAIN_ALARM, frules, headlen + count * sizeof (struct dns_rules), NULL, 0, 0);
						break;
					}
				case 3:
					{
	 					cpostman.Send_Msg2(DOMAIN_NETLOG, frules, strlen (frules) + 1 + count * sizeof (struct ipaudit_rules), NULL, 0, 0);
						break;
					}
				default: break;
			}
		sleep(1);
		int flag_dns=0, flag_ip=0, flag_swip=0, flag_senst=0, flag_mail=0, flag_spectro=0, flag_geoip=0;
		shmobj.Get_status_shm(28, &flag_dns, 1);
		shmobj.Get_status_shm(29, &flag_ip, 1);
		shmobj.Get_status_shm(30, &flag_swip, 1);
		shmobj.Get_status_shm(31, &flag_senst, 1);
		shmobj.Get_status_shm(32, &flag_mail, 1);
		shmobj.Get_status_shm(33, &flag_spectro, 1);
		shmobj.Get_status_shm(34, &flag_geoip, 1);
		if( 1 == flag_dns)
		{
			shmobj.Set_status_shm(19, &(config_tmp.dns_version), sizeof(config_tmp.dns_version));
			shmobj.Get_status_shm(19, &(config.dns_version), sizeof(config.dns_version));
			flag_dns = 0;
			shmobj.Set_status_shm(28, &flag_dns, 1);
		}
		if( 1 == flag_ip)
		{
			shmobj.Set_status_shm(20, &(config_tmp.ip_version), sizeof(config_tmp.ip_version));
			shmobj.Get_status_shm(20, &(config.ip_version), sizeof(config.ip_version));
			flag_ip = 0;
			shmobj.Set_status_shm(29, &flag_ip, 1);
		}
		if( 1 == flag_swip)
		{
			shmobj.Set_status_shm(21, &(config_tmp.swip_version), sizeof(config_tmp.swip_version));
			shmobj.Get_status_shm(21, &(config.swip_version), sizeof(config.swip_version));
			flag_swip = 0;
			shmobj.Set_status_shm(30, &flag_swip, 1);
		}
		if( 1 == flag_senst)
		{
			shmobj.Set_status_shm(22, &(config_tmp.sen_version), sizeof(config_tmp.sen_version));
			shmobj.Get_status_shm(22, &(config.sen_version), sizeof(config.sen_version));
			flag_senst = 0;
			shmobj.Set_status_shm(31, &flag_senst, 1);
		}
		if( 1 == flag_mail)
		{
			shmobj.Set_status_shm(23, &(config_tmp.mail_version), sizeof(config_tmp.mail_version));
			shmobj.Get_status_shm(23, &(config.mail_version), sizeof(config.mail_version));
			flag_mail = 0;
			shmobj.Set_status_shm(32, &flag_mail, 1);
		}
		if( 1 == flag_spectro)
		{
			shmobj.Set_status_shm(24, &(config_tmp.spe_version), sizeof(config_tmp.spe_version));
			shmobj.Get_status_shm(24, &(config.spe_version), sizeof(config.spe_version));
			flag_spectro = 0;
			shmobj.Set_status_shm(33, &flag_spectro, 1);
		}
		if( 1 == flag_geoip)
		{
			shmobj.Set_status_shm(25, &(config_tmp.geoip_version), sizeof(config_tmp.geoip_version));
			shmobj.Get_status_shm(25, &(config.geoip_version), sizeof(config.geoip_version));
			flag_geoip = 0;
			shmobj.Set_status_shm(34, &flag_geoip, 1);
		}
 		int rc = ointerface.Send_Config(&config, NULL);
		if (-1 == rc)
		{
			 struct cache_opt_t opt_stut;
	 		 INIT_OPT_HEAD(&opt_stut);
                         strcpy(opt_stut.username,"system");
                         time_t t;
                         time(&t);
                         funhandler->Turntime(t,opt_stut.opttime,24);
                         strcpy(opt_stut.describe,"上报配置信息失败");
                         int send_err=cacheclient->Send_Msg((char*)&opt_stut,sizeof(opt_stut));
			 			 send_err = send_err; 		//hyj 未使用的变量
		}
		}
		if(frules != NULL)
			free (frules);
		if((*(load.rules)) != NULL)
			free(*(load.rules));
		frules = NULL;
	}
	else if(!strncmp(cmd,"startm",6))
	{//启动服务
		char param[256];
		Get_Param(cmd,param);
		if(!strncmp(param,"alarm",5))
		{//启动攻击窃密检测
			char sbuf[32];
			char cmd = '0';
			memset(sbuf, 0, sizeof(sbuf));
			strcpy(sbuf,"startm+alarm");	
			cpostman.Send_Msg2(DOMAIN_DAEMON, sbuf,strlen(sbuf),NULL,0, 0);
			shmobj.Set_status_shm(1, &cmd, sizeof(char));
		}
		else if(!strncmp(param,"netlog",6))
		{//启动网络行为审计
		
			char sbuf[32];
			char cmd = '0';
			memset(sbuf, 0, sizeof(sbuf));
			strcpy(sbuf,"startm+netlog");
			cpostman.Send_Msg2(DOMAIN_DAEMON, sbuf,strlen(sbuf),NULL,0, 0);
			shmobj.Set_status_shm(2, &cmd, sizeof(char));
		}
		else if(!strncmp(param,"sensitive",9))
		{//启动涉密标志检测
			char sbuf[32];
			char cmd = '1';
			memset(sbuf, 0, sizeof(sbuf));
			strcpy(sbuf,"startm+sensitive");
			cpostman.Send_Msg2(DOMAIN_DAEMON,sbuf,strlen(sbuf),NULL,0, 0);
			shmobj.Set_status_shm(3, &cmd, sizeof(char));
		}
		else if(!strncmp(param,"netblock",8))
		{//启动网络窃密阻断
			char sbuf[32];
			char cmd = '0';
			memset(sbuf, 0, sizeof(sbuf));
			strcpy(sbuf,"startm+netblock");
			cpostman.Send_Msg2(DOMAIN_DAEMON,sbuf,strlen(sbuf),NULL,0, 0);
			shmobj.Set_status_shm(4, &cmd, sizeof(char));
		}
	}
	else if(!strncmp(cmd,"stopm",5))
	{//停止服务
		char	param[256];
		Get_Param(cmd,param);
		if(!strncmp(param,"alarm",5))
		{//启动攻击窃密检测
			char sbuf[32];
			char cmd = '3';
			memset(sbuf, 0, sizeof(sbuf));
			strcpy(sbuf,"stopm+alarm");
			cpostman.Send_Msg2(DOMAIN_DAEMON,sbuf,strlen(sbuf),NULL,0, 0);
			shmobj.Set_status_shm(1, &cmd, sizeof(char));
			system("./kill_tipsalarm.sh");
		}
		else if(!strncmp(param,"netlog",6))
		{//启动网络行为审计
			char sbuf[32];
			char cmd = '3';
			memset(sbuf, 0, sizeof(sbuf));
			strcpy(sbuf,"stopm+netlog");
			cpostman.Send_Msg2(DOMAIN_DAEMON,sbuf,strlen(sbuf),NULL,0, 0);
			shmobj.Set_status_shm(2, &cmd, sizeof(char));
			system("./kill_tipsaudit.sh");
		}
		else if(!strncmp(param,"sensitive",9))
		{//启动涉密标志检测
			char sbuf[32];
			char cmd = '3';
			memset(sbuf, 0, sizeof(sbuf));
			strcpy(sbuf,"stopm+sensitive");
			cpostman.Send_Msg2(DOMAIN_DAEMON,sbuf,strlen(sbuf),NULL,0, 0);
			shmobj.Set_status_shm(3, &cmd, sizeof(char));
		}
		else if(!strncmp(param,"netblock",8))
		{//启动网络窃密阻断
			char sbuf[32];
			char cmd = '3';
			memset(sbuf, 0, sizeof(sbuf));
			strcpy(sbuf,"stopm+netblock");
			cpostman.Send_Msg2(DOMAIN_DAEMON,sbuf,strlen(sbuf),NULL,0, 0);
			shmobj.Set_status_shm(4, &cmd, sizeof(char));
			system("./kill_tipsblock.sh");
		}
	}
	else if(!strncmp(cmd,"netshark",8))
	{//内容审计
		char sbuf[512];
		memset(sbuf, 0, sizeof(sbuf));
		strcpy(sbuf,"C+11{> netshark");
		char* temp = cmd + 8;
		if(temp)
		{
			strncat(sbuf, temp, strlen(temp));
			cpostman.Send_Msg2(DOMAIN_NETLOG, sbuf,strlen(sbuf),NULL,0, 0);
		}
	}
	else if(!strncmp(cmd,"stopnetshark",12))
	{//停止内容审计
		char sbuf[512];
		memset(sbuf, 0, sizeof(sbuf));
		strcpy(sbuf,"C+12{> stopnetshark");
		cpostman.Send_Msg2(DOMAIN_NETLOG, sbuf,strlen(sbuf),NULL,0, 0);
	}
	else if(!strncmp(cmd,"netblock",8))
	{//阻断通信
		char sbuf[512];
		memset(sbuf, 0, sizeof(sbuf));
		strcpy(sbuf,"C+13{> netblock");
		char* temp = cmd + 8;
		if(temp)
		{
			strncat(sbuf, temp, strlen(temp));
			printf("sbuf===%s\n",sbuf);
			cpostman.Send_Msg2(DOMAIN_BLOCK, sbuf,strlen(sbuf),NULL,0, 0);
		}
	}
	else if(!strncmp(cmd,"stopnetblock",12))
	{//停止阻断
		char sbuf[512];
		memset(sbuf, 0, sizeof(sbuf));
		strcpy(sbuf,"C+14{> stopnetblock");
		printf("sbuf===%s\n",sbuf);
		cpostman.Send_Msg2(DOMAIN_BLOCK, sbuf,strlen(sbuf),NULL,0, 0);
	}
}
void StatusTask::Start_Task()
{
	//printf("**************************************StatusTask::Start_Task()*********************************\n");
	fflush(stdout);
	static int cmd_count = 0;
	//cout<<"开始执行上报状态定时任务"<<endl;
	//获得设备运行状态
	char msg_buff[1024];//cmdbuf[1024];
	char **cmdbuff = NULL;
	memset(msg_buff,0,1024);
	//获取设备状态信息组织
#if 0
	strcpy(msg_buff,"G");
	cpostman.Send_Msg2(DOMAIN_ALARM, msg_buff,strlen(msg_buff),(char*)&part,sizeof(part), 1);
#endif
	status_Init(&part);
#if 1
	int ret = 0;
	ret = SendStatus (&status, &cmdbuff, 0);
	if( 0 <= ret)
	{
        //printf("上报状态信息成功\n");
	}
	SendStatus (&status, &cmdbuff, 1);
	if(-1 == ret)
	{
		struct cache_opt_t opt_stut;
	 	INIT_OPT_HEAD(&opt_stut);
                strcpy(opt_stut.username,"system");
                time_t t;
                time(&t);
                funhandler->Turntime(t,opt_stut.opttime,24);
                strcpy(opt_stut.describe,"上报状态信息失败");
                int send_err=cacheclient->Send_Msg((char*)&opt_stut,sizeof(opt_stut));
				send_err = send_err; 		//hyj 未使用的变量
                dzlog_info("上报状态信息失败\n");

	}
	if (0 < ret)
	{	
		cmd_count++;
		for (int i = 0; i < ret; i++)
		{
			Parse_Cmd(cmdbuff[i]);
			//printf("cmdbuff===%s\n",cmdbuff[i]);
		}
	}
	else if (0 == ret)
	{
		dzlog_info("无管控命令返回\n");
	}

	for(int l = 0; l < ret; l ++)
	{
		if (cmdbuff[l])
			free (cmdbuff[l]);
	}
	//printf("cmd_count=%d\n", cmd_count);
#endif
}
void StatusTask::Init_Task()
{
	FILE* fp = fopen (RUNCONGFILE, "r");
	char buffer[1024];
	memset (buffer, 0, sizeof(buffer));
	while(fgets(buffer, sizeof(buffer), fp))
	{
		if(!strncmp(buffer, "RUNTIME", strlen("RUNTIME")))
		{
			char* p = strchr(buffer, '=');
			Second_Time= atoi(p + 1);
			printf("StatusTask time=%dsec\n", Second_Time);
		}
	}
	fclose(fp);
}

int StatusTask::Get_Mem()
{
	int i = 0;
	char buffer[1024];
	FILE* fp = popen ("awk '{print $2}' /proc/meminfo","r");
	memset (buffer, 0, sizeof (buffer));

	while (fgets(buffer, sizeof(buffer), fp))
	{
		++ i;
		if (1 == i)
		{
			Memtotal = new char[strlen(buffer) + 1];
			strcpy (Memtotal, buffer);
			Memtotal[strlen(Memtotal) - 1] = '\0';	
		}
		else if (2 == i)
		{
			Memfree = new char[strlen(buffer) + 1 ];
			strcpy (Memfree, buffer);
			Memfree[strlen(Memfree) - 1] = '\0';
		}
		else if (3 == i)
		{
			Buffers = new char[strlen(buffer) + 1];
			strcpy (Buffers, buffer);
			Buffers[strlen(Buffers) - 1] = '\0';
		}
		else if (4 == i)
		{
			Cached = new char[strlen(buffer) + 1];
			strcpy (Cached, buffer);
			Cached[strlen(Cached) - 1] = '\0';
		}
		if(4 == i)
			break;
	}
	total = atoi (Memtotal);
	cached = atoi (Cached);
	buffers = atoi (Buffers);
	memfree = atoi (Memfree);
	delete Memtotal;
	delete Memfree;
	delete Buffers;
	delete Cached;
	char temp[50];
	memset (temp, 0, sizeof(temp));
	pclose(fp);
	if (cached > total)
	{
		mem = 100 - memfree / total * 100;
		return mem;
	}
	else
	{
		mem = 100 - (memfree + cached + buffers) / total * 100;
		return mem;
	}
}
int StatusTask::Get_Cpu()
{
	char str[5][1024];
	char number[5][1024];
	int i = 0, count = 0;
	char buffer[1024];
	memset (buffer, 0, sizeof (buffer));
	for (i = 0; i < 5; i++)
	{
			memset (str[i], 0, sizeof (str[i]));
			memset (number[i], 0, sizeof (number[i]));		
	}
	i = 0;
	FILE* fp = popen ("vmstat", "r");

	while (fgets (buffer, sizeof(buffer), fp))
	{
		++i;
		if (1 == i)
				continue;
		char temp[1024];
		memset (temp, 0, sizeof (temp));
		strcpy (temp, buffer);
		char* token = strtok (temp, " ");
		int j = 0, num = 0, cnt = 0, sub = 0;
 		while((token = strtok(NULL, " ")))
		{
			if (strstr (token, "us"))
			{
				strcpy(str[j++], token);
				str[j - 1][strlen(str[j - 1])] = '\0';
 				while((token = strtok(NULL, " ")))
				{
					strcpy(str[j++], token);
					str[j - 1][strlen(str[j - 1])] = '\0';
				}
				count = cnt;
				break;
			}
			if(count == 0)
				cnt++;
			if(3 == i)
			{
				sub++;
				if(sub == count + 1)
				{
					strcpy (number[num++], token);
					number[num - 1][strlen(number[num -1])] = '\0';
 					while((token = strtok(NULL, " ")))
					{
						strcpy (number[num++], token);
						number[num - 1][strlen(number[num -1])] = '\0';
					}
				goto compute;
				}
			}		
		}
	}
compute:
		pclose(fp);
		double cpu[5];
		char buf[1024];
		memset (buf, 0, sizeof(buf));
		for(i = 0; i < 5; i++)
		{
			if(number[i] == NULL)
					cpu[i] = 0;
			else
					cpu[i] = (double)atoi(number[i]);
		}
		double tm = (cpu[0] + cpu[1] + cpu[3] + cpu[4]) / (cpu[0] + cpu[1] + cpu[2] + cpu[3] + cpu[4]) * 100;
		this->cpu = (int)tm;
		return this->cpu;
}
double StatusTask::Get_Disk()
{	
	int sign = 0;
	int ret;
	struct statfs st;
	double totalBlock, /*hyj totalSize,*/ freeSize;
	FILE* fp = popen ("df -hl", "r");
	char buf[1024];
	memset (buf, 0, sizeof (buf));
	if (NULL == fp)
		fprintf(stderr, "Error\n");
	while (fgets (buf, sizeof (buf), fp))
	{
		sign++;
		if (1 == sign)
			continue;
		buf[strlen(buf) - 1] = '\0';
		if (!strchr(buf, ' '))
			continue;
		char* p = strrchr (buf, ' ');
		ret = statfs (p + 1, &st);
		if (-1 == sign)
			fprintf (stderr, "Statfs error:%s\n", strerror(errno));
		totalBlock = st.f_bsize;
		freeSize += totalBlock * st.f_bfree;
	}
	pclose(fp);
	this->disk = freeSize / (1024 * 1024 * 1024);
	return this->disk;
}
int StatusTask::Get_Format(char* filename, int count, char* sbuf, void** load, int * headlen)
{
//	cout <<"Format start"<<endl;
	char buf[20];
	int type = 0;
	int tp = 0;
	memset (buf, 0, sizeof(buf));
	if(!strncmp(filename,"dnslist",7))
	{

		type = 1;
		tp = 14;
		int length = Add_Str(type, buf, count);
		*headlen = length;
//		frules = (char*)realloc(frules, length + 1 + count * sizeof (struct dns_rules));
		frules = (char*)realloc(frules, length + count * sizeof (struct dns_rules));
//		memset (frules + length + 1, 0, sizeof (struct dns_rules) * count);
		//memcpy ((struct dns_rules*)((char*)frules + length + 1), *load, count * sizeof (struct dns_rules));
//		printf("dns header length=%d\n", length);
		memcpy ((struct dns_rules*)((char*)frules + length), *load, count * sizeof (struct dns_rules));
		memset(buf, 0, sizeof(buf));
		snprintf (buf, sizeof (buf), "L+%d{%d}", tp, count);
		memset(buf+strlen(buf),' ',1) ;
//	cout <<"Format end"<<endl;
		return 2;
	}
	else if(!strncmp(filename,"iplist",6))
	{
		type = 2;
		int length = Add_Str(type, buf, count);
		*headlen = length;
		frules = (char*)realloc(frules, length + count * sizeof (struct ip_rules));
//		memset (frules + length + 1, 0, sizeof (struct ip_rules) * count);
		memcpy ((struct ip_rules*)((char*)frules + length), *load, count * sizeof (struct ip_rules));
//	cout <<"Format end"<<endl;
		return 1;
	}
	else if(!strncmp(filename,"swiplist",8))
	{
		type = 3;
		tp = 21;
		int length = Add_Str(type, buf, count);
		*headlen = length;
		frules = (char*)realloc(frules, length + count * sizeof (struct ipaudit_rules));
//		memset (frules + length, 0, sizeof (struct ipaudit_rules) * count);
		memcpy ((struct ipaudit_rules*)((char*)frules + length), *load, count * sizeof (struct ipaudit_rules));
//	cout <<"Format end"<<endl;
		memset(buf, 0, sizeof(buf));
		snprintf (buf, sizeof (buf), "L+%d{%d}", tp, count);
		memset(buf+strlen(buf),' ',1) ;
	//	buf[strlen(buf)] = '\0';
//	cout <<"Format end"<<endl;
		return 2;
	}
	else if(!strncmp(filename,"senlist",7))
	{
		type = 4;
		int length = Add_Str(type, buf, count);
		*headlen = length;
		frules = (char*)realloc(frules, length + count * sizeof (struct sec_rules));
//		memset (frules + length, 0, sizeof (struct sec_rules) * count);
		memcpy ((struct sec_rules*)((char*)frules + length), *load, count * sizeof (struct sec_rules));
//	cout <<"Format end"<<endl;
		return 1;
	}
	else if(!strncmp(filename,"spectro",7))
	{
		type = 5;
		int length = Add_Str(type, buf, count);
		*headlen = length;
		frules = (char*)realloc(frules, length + count * sizeof (struct spectro_rules));
//		memset (frules + length, 0, sizeof (struct spectro_rules) * count);
		memcpy ((struct spectro_rules*)((char*)frules + length), *load, count * sizeof (struct spectro_rules));
//	cout <<"Format end"<<endl;
		return 1;
	}
#if 1 
	else if(!strncmp(filename,"geoip",5))
	{
		struct ippos_rules ipp;
		type = 6;
		int length = Add_Str(type, buf, count);
		*headlen = length;
		frules = (char*)realloc(frules, length + count * sizeof (ipp));
//		printf("geoip frules len = %d\n", (length + count*sizeof(ipp)));
//		memset (frules + length, 0, sizeof (ipp) * count);
		memcpy ((struct ippos_rules*)((char*)frules + length), *load, count * sizeof (ipp));
//	cout <<"Format end"<<endl;
		return 1;
	}
	else if(!strncmp(filename,"maillist",8))
	{
		struct mail_rules ipp;
		type = 7;
		int length = Add_Str2(type, buf, count);
		*headlen = length;
		frules = (char*)realloc(frules, length + count * sizeof (ipp));
//		memset (frules + length, 0, sizeof (ipp) * count);
		memcpy ((struct mail_rules*)((char*)frules + length), *load, count * sizeof (ipp));
	//cout <<"Format end"<<endl;
		return 1;
	}
#endif
	sbuf = sbuf;		//hyj 保留参数
	return 0;			//hyj 返回值?
}
int StatusTask::Add_Str(int type, char* buf, int count)
{
	sprintf (buf, "L+%d{%d}", type, count);
	frules = (char*)malloc(strlen(buf) + 1);
	strcpy(frules, buf);
	int length =  strlen(frules);
//	frules[strlen(frules)] = '\0';
	memset(frules+strlen(buf),' ',1) ;
	length++;
	return length;
}
int StatusTask::Add_Str2(int type, char* buf, int count)
{
	sprintf (buf, "R+%d{%d}", type, count);
//	buf[strlen(buf)] = '\0';
	frules = (char*)malloc(strlen(buf) + 1);
	strcpy(frules, buf);
//	frules[strlen(frules)] = '\0';
	memset(frules+strlen(buf),' ',1) ;
	int length =  strlen(frules);
	return length;
}


void StatusTask::status_Init(struct status_part* part)
{
	Get_Mem();
	Get_Cpu();
	Get_Disk();
	status.mem = this->mem;
	status.cpu = this->cpu;
	cout<<"zzf------>cpu===="<<status.cpu<<endl;
	if(status.cpu>85)exit(0);
	status.disk = this->disk;
//	status.mirror = part->mirror;
	shmobj.Get_status_shm(15, &(status.device_id), 11);
	printf("status.device_id=%s\n",status.device_id);
	int c = 0;
//	shmobj.Set_status_shm(6, &c, 1);
	shmobj.Get_status_shm(6, &c, sizeof(c));
	if(1 != c)
	{
		shmobj.Get_status_shm(10, &c, sizeof(c));
	}
	status.mirror = '0'+ c;
	printf("status.mirror=%c\n",status.mirror);
//	status.flow = part->flow;
	double flow = 0;
	int peer = 0;
	int mirror_mode = 0;
	shmobj.Get_status_shm(5, &mirror_mode, sizeof(mirror_mode));
	if(0 == mirror_mode)
	{
		shmobj.Get_status_shm(7, &flow, sizeof(flow));
		shmobj.Get_status_shm(8, &peer, sizeof(peer));
		if(0 == flow)
		{
			shmobj.Get_status_shm(11, &flow, sizeof(flow));
			shmobj.Get_status_shm(12, &peer, sizeof(peer));
		}
		status.flow = (int) flow;
		status.active_peer = peer;
	}
	else if(1 == mirror_mode)
	{
		double flow2 = 0;
		int peer2 = 0;
		shmobj.Get_status_shm(7, &flow, sizeof(flow));
		shmobj.Get_status_shm(11, &flow2, sizeof(flow2));
		shmobj.Get_status_shm(8, &peer, sizeof(peer));
		shmobj.Get_status_shm(12, &peer2, sizeof(peer));
		status.flow = (int)(flow + flow2);
		status.active_peer = peer + peer2;
	}
	printf("status.flow=%d\n",status.flow);
//	status.flow = (part->flow) / (Second_Time * 2);
#if 0
	if ((status.flow > 700) && (status.flow <= 1100))
	{
		srand ((unsigned int) time (NULL));
		status.mem = rand()%(20-15)+15;
		status.cpu = rand()%(30-20)+20;
		status.flow *= rand()*1.0 / RAND_MAX + 1;
	}
	else if ((status.flow > 1100) && (status.flow <= 1500))
	{
		srand ((unsigned int) time (NULL));
		status.mem = rand()%(25-20)+20;
		status.cpu = rand()%(40-30)+30;
		status.flow *= rand()*1.0 / RAND_MAX + 2;
	}
	else if ((status.flow > 1500) && (status.flow < 2000))
	{
		srand ((unsigned int) time (NULL));
		status.mem = rand()%(30-25)+25;
		status.cpu = rand()%(50-40)+40;
		status.flow *= rand()*1.0 / RAND_MAX + 3;
	}
	else if ((status.flow > 2000) && (status.flow < 2500))
	{
		srand ((unsigned int) time (NULL));
		status.mem = rand()%(35-30)+30;
		status.cpu = rand()%(60-50)+50;
		status.flow *= rand()*1.0 / RAND_MAX + 4;
	}
	else if ((status.flow > 2500) && (status.flow < 3000))
	{
		srand ((unsigned int) time (NULL));
		status.mem = rand()%(40-35)+35;
		status.cpu = rand()%(70-60)+60;
		status.flow *= rand()*1.0 / RAND_MAX + 5;
	}
	else if ((status.flow > 3000) && (status.flow < 3500))
	{
		srand ((unsigned int) time (NULL));
		status.mem = rand()%(45-40)+40;
		status.cpu = rand()%(80-70)+70;
		status.flow *= rand()*1.0 / RAND_MAX + 6;
	}
#endif
//	status.flow = (part->flow) / (20 * 2);
//	status.active_peer = part->active_peer;
//	int active_peer = 15;
//	shmobj.Set_status_shm(8, &active_peer, sizeof(active_peer));
	printf("status.active_peer=%d\n",status.active_peer);
  //cout <<"status->mem ========== " <<status.mem <<endl;
  cout <<"status->cpu ===============" <<status.cpu <<endl;
//	strcpy (status.module_state, part->module);
	char module_state[9];
	memset(module_state, 0, sizeof(module_state));
	//shmobj.Set_status_shm(1,&ch,1);
	//shmobj.Set_status_shm(2,&ch,1);
	//shmobj.Set_status_shm(3,&ch,1);
	//shmobj.Set_status_shm(4,&ch,1);
	shmobj.Get_status_shm(1, &(status.module_state[0]),1);
	shmobj.Get_status_shm(2, &(status.module_state[1]),1);
	shmobj.Get_status_shm(3, &(status.module_state[2]),1);
	shmobj.Get_status_shm(4, &(status.module_state[3]),1);
	printf("status.module_state[2]=%c\n",status.module_state[2]);
	status.module_state[strlen (status.module_state)] = '\0';
	printf("status_Init completed\n");
//	shmobj.Set_status_shm();
//	shmobj.Get_status_shm();
//
	part = part;		//hyj 保留参数
}

void StatusTask::signal_usr1(char* str, char* errMsg)
{
	pid_t pid = 0;
	if(!Getpid(&pid, DAEMON_PID))
	{
		kill(pid, SIGUSR1);			
	}
	if(!Getpid(&pid, ALARM_PID))
	{
		kill(pid, SIGUSR1);			
	}
	if(!Getpid(&pid, AUDIT_PID))
	{
		kill(pid, SIGUSR1);			
	}
	if(!Getpid(&pid, BLOCK_PID))
	{
		kill(pid, SIGUSR1);			
	}
	if(!Getpid(&pid, SENST_PID))
	{
		kill(pid, SIGUSR1);			
	}
	if(!Getpid(&pid, CHECK_PID))
	{
		kill(pid, SIGUSR1);			
	}
	if(!Getpid(&pid, UI_PID))
	{
		kill(pid, SIGUSR1);			
	}
	if(!Getpid(&pid, CACHE_PID))
	{
		kill(pid, SIGUSR1);			
	}
	struct cache_opt_t opt_stut;
	INIT_OPT_HEAD(&opt_stut);
        strcpy(opt_stut.username,"system");
        time_t t;
        time(&t);
        funhandler->Turntime(t,opt_stut.opttime,24);
        sprintf(opt_stut.describe,"%s命令执行成功",str);
        int send_err=cacheclient->Send_Msg((char*)&opt_stut,sizeof(opt_stut));
		send_err = send_err;		//hyj 未使用的变量
	for(int i=0; i<30; i++)
	{
		sleep(1);
#if 1
		int flag1 = 0, flag2 = 0, flag3 = 0, flag4 = 0 , flag5 = 0, flag6 = 0, flag7 = 0;
		shmobj.Get_status_shm(35, &flag1, 1);
		shmobj.Get_status_shm(36, &flag2, 1);
		shmobj.Get_status_shm(37, &flag3, 1);
		shmobj.Get_status_shm(38, &flag4, 1);
		shmobj.Get_status_shm(40, &flag5, 1);
		shmobj.Get_status_shm(41, &flag6, 1);
		shmobj.Get_status_shm(42, &flag7, 1);
		if(!flag1 && !flag2 && !flag3 && !flag4 && !flag5 && !flag6 && !flag7)
			break;
#endif
	}
	if(!strcmp(str, "shutdown"))
	{
		char tmp[1024];
		memset (tmp, 0, sizeof(tmp));
		strcpy (tmp, "shutdown -h now");
		tmp[strlen(tmp)] = '\0';
		system(tmp);
	}
	else
		system(str);	
	errMsg = errMsg;		//hyj 保留参数
}

int StatusTask::Getpid(pid_t *pid ,const char* process_id)
{
   FILE* fp = fopen(process_id, "r");
   if(NULL == fp){
        fprintf(stderr, "open error\n");
        return 1;
   }
   int num = fread(pid, 1, sizeof(pid_t), fp);
   if(num < 0){
        // fprintf(stderr, "num===%d\n",num);
        fclose(fp);
        return 1;
   }
	fclose (fp);
	return 0;
}
