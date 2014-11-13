#include "tipsmanager.h"
#include "tipsalarm.h"
#include <string.h>
#include "publicdata.h"
#include <stdio.h>
#include <time.h>
#include <signal.h>
#include "zlog.h"

#include "cachestruct.h"
char LOCALDEVID[11];
char manageip[40];
char manageip1[40];
TipsAlarm thealarm;
ShareMem	shmobj;
struct config_struct config;
struct config_struct config_tmp;
extern struct status_part part;
struct status_struct status;
extern int startrun;
void alarm_hander(int param)
{//定时器回调函数
			cout<<"定时任务时间设置:"<<time(NULL)<<endl;
				thealarm.Run_Alarm_Task();
				thealarm.Set_An_Alarm();

	param = param;		//hyj 保留参数
}
void alarm_modify(int param)
{
  	char date_buff[30];
  	char cmd_buff[100];
	time_t old_time = 0;
	time_t new_time= 0;
  	memset(cmd_buff,0,sizeof(cmd_buff));
	printf("input date :\n");
	scanf("%s",date_buff);
  	sprintf(cmd_buff,"date -s \"%s\"",date_buff);
	old_time = time(NULL);
  	system(cmd_buff);
	new_time = time(NULL);
	thealarm.Get_Systime_Gap(old_time, new_time);
	thealarm.Modify_Task_Alarm();
	struct itimerval it;
	it.it_value.tv_sec=0;
	it.it_value.tv_usec=1;
	it.it_interval.tv_sec=0;
	it.it_interval.tv_usec=0;
	setitimer(ITIMER_REAL,&it,NULL);
//	thealarm.Set_An_Alarm();
//
	param = param;		//hyj 保留参数
}
bool TipsManager::ParseBuff(char *tmp,char *Buff)
{
				char *tmpp = 0;
				strtok(tmp,"=");		
				if((tmpp = strtok(NULL,"=")))		
				{
								while(!isdigit(*tmpp))
								{
											tmpp++;
								}
								if((*(tmpp + strlen(tmpp) - 1)) == '\n')
										(*(tmpp + strlen(tmpp) - 1)) = '\0';
								memcpy(Buff,tmpp,strlen(tmpp));								
								return true;
				}
				return false;
}

void TipsManager::Get_ManageIP(char* ip)
{
				//打开文件读取ip和端口
				FILE *fp = 0;	
				if(0 == (fp = fopen(MNGCONGFILE,"r")))
				{
								perror("MNGCONGFILE--fopen:");	
								fclose(fp);
								return ;
				}
				char buffer[BuffSize] = {0};
				char IPBuff[BuffSize] = {0};	
				char PORTBuff[BuffSize] = {0};	
				bool ipflag = false;
				bool portflag = false;
				char *tmp = 0;
				while((fgets(buffer,BuffSize,fp)))
				{
								if(!ipflag && (tmp = strstr(buffer,MANAGE_IP)))
								{
												if(ParseBuff(tmp,IPBuff))
												{
																ipflag = true;	
												}
												if(portflag)
												{
																break;
												}
								}
								else if(!portflag && (tmp = strstr(buffer,MANAGE_PORT)))
								{
												if(ParseBuff(tmp,PORTBuff))
												{
																portflag = true;
												}
												if(ipflag)
												{
																break;
												}
								}
								memset(buffer,0,sizeof(buffer));
				}
				sprintf(ip, "%s:%s",IPBuff,PORTBuff);
				cout <<"ip============="<<ip <<endl;
				//strcpy(ip,"172.16.0.139:8118");
				//cout<<"XXX|"<<ip<<"|"<<endl;
				fclose(fp);
				fp = 0;
}

void TipsManager::Get_ManageIP1(char* ip)
{
				//打开文件读取ip和端口
				FILE *fp = 0;	
				if(0 == (fp = fopen(MNGCONGFILE,"r")))
				{
								perror("MNGCONGFILE--fopen:");	
								fclose(fp);
								return ;
				}
				char buffer[BuffSize] = {0};
				char IPBuff[BuffSize] = {0};	
				char PORTBuff[BuffSize] = {0};	
				bool ipflag = false;
				bool portflag = false;
				char *tmp = NULL;
				while((fgets(buffer,BuffSize,fp)))
				{
								if(!ipflag && (tmp = strstr(buffer,MANAGE_IP1)))
								{
												if(ParseBuff(tmp,IPBuff))
												{
																ipflag = true;	
												}
												if(portflag)
												{
																break;
												}
								}
								else if(!portflag && (tmp = strstr(buffer,MANAGE_PORT1)))
								{
												if(ParseBuff(tmp,PORTBuff))
												{
																portflag = true;
												}
												if(ipflag)
												{
																break;
												}
								}
								memset(buffer,0,sizeof(buffer));
				}
				sprintf(ip, "%s:%s",IPBuff,PORTBuff);
				cout <<"ip11111111111============="<<ip <<endl;
				//strcpy(ip,"172.16.0.139:8118");
				//cout<<"XXX|"<<ip<<"|"<<endl;
				fclose(fp);
				fp = 0;
}

int TipsManager::Init_Module()
{
				
			
				//cout <<"11111111111111111111111111111111111"<<endl;
				int ret = shmobj.Init_shm();
				if(-1 == ret)
				{
					dzlog_info("Init shm failed\n");
					return -1;
				}
				WtDevID();
				//cout <<"11111111111111111111111111111111111"<<endl;
				ConfigInit();
				cout<<"init_config_info_completed!"<<endl;
//*******************************************************************

//********************************************************************
//初始化数据库
				bool init_err=sqlhandler.Init_Sql((char *)DATABASEADDR,(char *)DATABASEUSER,(char *)"123456",(char *)DATABASENAME,0,NULL,0);		//update by hyj
				if(!init_err)
				{
					dzlog_info("open database fialed!name====%s\n",DATABASENAME);
				}
				cout<<"init_database_completed!"<<endl;
//*********************************************************************
//初始化cacheclient
				ccacheclient.Init_Client((char *)CACHEHOSTIP);		//update by hyj
				cout<<"init_ccacheclient_completed!"<<endl;
//**********************************************************************
//初始化定时任务
				thealarm.Init_Alarm();
				memset(classid,0,CLASSCOUNT*4);
				memset(timetask,0,CLASSCOUNT*4);
				for(int i=0;i<CLASSCOUNT;++i)classid[i]=i+1;
				ObjectCreator objectcreator;
				objectcreator.CreatTask(classid,timetask);
				//	for(int i=0;i<1;++i)
				for(int i=0;i<CLASSCOUNT;++i)
				{
					timetask[i]->Set_Sqlhandler(&sqlhandler);
					timetask[i]->Set_Cacheclient(&ccacheclient);
					timetask[i]->Set_Funhandler(&funhandler);
				//	timetask[i]->Set_DBHandler(db);
					timetask[i]->Init_Task();
					thealarm.Register_Alarm(timetask[i]->Get_Time(),timetask[i],NULL);
				}
				cout<<"init_alarmtask_completed!"<<endl;
//******************************************************************************************
//sqlite3数据库操作优先级互斥部分初始化
	/*			tips_sqlite_sync.init_sync_mutex ();
				tips_sqlite_sync.sqlite_sync_cli_init ((char*)TIPS_DIR);
				pthread_t pid;
				tips_pthread.level = 0;
				tips_pthread.db = db;
				if (pthread_create(&pid, NULL, sqlite_sync, &tips_pthread) != 0)
				{
						perror ("create_pthread error\n");
				}
	*/
//******************************************************************************************
	return 1;		//hyj 返回值?
}
int TipsManager::Start_Module()
{
				OInterface ointerface;
				int rc = ointerface.Send_Config(&config, NULL);
				if (-1 == rc)
				{
					struct cache_opt_t opt_stut;
					INIT_OPT_HEAD(&opt_stut);
					strcpy(opt_stut.username,"system");
					time_t t;
					time(&t);
					funhandler.Turntime(t,opt_stut.opttime,24);
					strcpy(opt_stut.describe,"开机上报配置信息失败");
					int send_err=ccacheclient.Send_Msg((char*)&opt_stut,sizeof(opt_stut));	
					send_err = send_err;		//hyj 未使用的变量
				}
			
//***************************************************************
				thealarm.Set_An_Alarm();
				signal(SIGINT, alarm_modify);
//****************************************************************
//循环等待UI进程向manange所有信息
				sockfd=manageser.serv_listen(UI_UNIX_DOMAIN);
				while(1)
				{
								int acceptfd=manageser.serv_accept(sockfd);
							int result=manageser.serv_read(acceptfd);
							//	cout<<"result==========="<<result<<endl;
								switch (result)
								{
												case -1:
																{
																				break;
																}
												case 0:
																{
																	    	//联网口IP,上报设备配置信息
																	    	ConfigInit();
																				ointerface.Send_Config(&config, NULL);
																				break;
																}
												case 1:
																{
																				//网关,上报设备配置信息
																	    	ConfigInit();
																				ointerface.Send_Config(&config, NULL);
																				break;
																}
												case 2:
																{
																		    //代理服务
																				break;
																}
												case 3:
																{
																				//管理中心IP,上报设备配置信息
																	    	ConfigInit();
//																			Get_ManageIP(manageip);
//																			Get_ManageIP1(manageip1);
																			ointerface.Send_Config(&config, NULL);
//																			char buffer[15];
//																			memset (buffer, 0, sizeof(buffer));
//																			strncpy(buffer, "UPDATEMANGEIP", sizeof (buffer));
																			//cout<<"bufer=========="<<buffer<<endl;
																		//cout<<"length======="<<strlen(buffer)<<endl;
//																			MngChkPostman postman;
//																			postman.Init_Postman();
//																			postman.Send_Msg2(buffer, strlen(buffer), NULL, 0, 0);
																			break;
																}
												case 4:			
																{
																			//系统升级
																			break;
																}
												case 5:
																{
																				//修改
																			dzlog_info("状态时间\n");
																			dzlog_info("状态时间\n");
																			timetask[3]->Init_Task();
																			thealarm.UnRegister_Alarm(timetask[3]->flag);
																			thealarm.Register_Alarm(timetask[3]->Get_Time(),timetask[3],NULL);
																			break;
																}	
												case 6:{
																				//修改时间同步间隔
																			dzlog_info("同步时间\n");
																			dzlog_info("同步时间\n");
																			timetask[2]->Init_Task();
																			thealarm.UnRegister_Alarm(timetask[2]->flag);
																			thealarm.Register_Alarm(timetask[2]->Get_Time(),timetask[2],NULL);
																			break;
																}
												case 7:
																{
#if 0
																			char buffer[2];
																			strncpy (buffer, "G", sizeof (buffer));
																			MngChkPostman postman;
//																			postman.Init_Postman();
																			cout <<"POST MAN START" <<endl;
//																				postman.Send_Msg2(buffer, strlen(buffer), (char*)&part, sizeof(part), 1);
																			cout <<"POST MAN END" <<endl;
													//					exit (0);
																
																		cout << "send status start" <<endl;
																			 status_Init(&part);
																		cout <<"status ===========" <<status.module_state <<endl;
																		int result=manageser.serv_write_string(acceptfd, &status, sizeof (status));
																		cout << "send status endl" <<endl;	
#endif
																	break;
																}
												case 8:
																{
																			timetask[1]->Init_Task();
																			thealarm.UnRegister_Alarm(timetask[1]->flag);
																			thealarm.Register_Alarm(timetask[1]->Get_Time(),timetask[4],NULL);
																			break;		
																}
			     								default:
																		break;
								}
					cout<<"1111111"<<endl;
								close(acceptfd);
					cout<<"22222"<<endl;
	}
	return 0;		//hyj 返回值?
}
int TipsManager::Exit_Module()
{
				for(int i=0;i<CLASSCOUNT;++i)
								if(timetask)delete timetask[i];
	return 0;		//返回值?
}
int TipsManager::Restart_Module()
{
	return 0;		//hyj 返回值？
}

void TipsManager::ConfigInit()
{
				sign = 0;
				int cnt = 0;
				FILE* fp;
				memset (&config, 0, sizeof (struct config_struct));
				strncpy(config.device_id, LOCALDEVID, sizeof (config.device_id));
				strncpy(config.device_type, "1", sizeof (config.device_type));
				strncpy (config.version, "1", sizeof (config.version));
				strncpy (config.device_made, "03", sizeof (config.device_made));
				strncpy (config.own, "0", sizeof (config.own));
			//	cout <<"11111111111111111111111111111111111"<<endl;
				fp = fopen (MNGCONGFILE, "r");
				char buffer[1024];
				memset (buffer, 0, sizeof (buffer));
				while (fgets (buffer, sizeof (buffer), fp))
				{
								if(!strncmp (buffer ,"ADMINSTRATION_IP=", strlen ("ADMINSTRATION_IP=")))
								{
												buffer[strlen(buffer) - 1] = '\0';
												char* temp = buffer + strlen("ADMINSTRATION_IP=");
												strncpy (config.center_ip, temp, sizeof (config.center_ip));
												cnt ++;	
								}
								else if(!strncmp (buffer ,"ADMINSTRATION_IP1", strlen ("ADMINSTRATION_IP1")))
								{
												buffer[strlen(buffer) - 1] = '\0';
												char* temp = buffer + strlen("ADMINSTRATION_IP1") + 1;
												strncpy (config.center_ip1, temp, sizeof (config.center_ip1));
												cnt ++;	
								}

								else if(!strncmp (buffer ,"MANAGE_IP", strlen ("MANAGE_IP")))
								{
												buffer[strlen(buffer) - 1] = '\0';
												char* temp = buffer + strlen("MANAGE_IP") + 1;
												strncpy (config.manageip, temp, sizeof (config.manageip));
												cnt ++;	
								}
#if 1 
								else if (!strncmp (buffer, "GATEWAY", strlen ("GATEWAY")))
								{
												buffer[strlen(buffer) - 1] = '\0';
												char* temp = buffer + strlen("GATEWAY") + 1;
												strncpy (config.gateway, temp, sizeof (config.gateway));
												cnt ++;	
								}
#endif
								else if (!strncmp (buffer, "UNIT_NAME", strlen("UNIT_NAME")))
								{
												buffer[strlen(buffer) - 1] = '\0';
												char* temp = buffer + strlen("UNIT_NAME") + 1;
												strncpy (config.device_locate, temp, sizeof (config.device_locate));
												cnt ++;
								}
								else if (!strncmp (buffer, "NET_IP", strlen("NET_IP")))
								{
												buffer[strlen(buffer) - 1] = '\0';
												char* temp = buffer + strlen("NET_IP") + 1;
												strncpy (config.ip, temp, sizeof (config.ip));
												cnt ++;
								}
							if (6 == cnt)
								break;
				}
				fclose(fp);
			//	cout<<"config.ip===========" <<config.ip <<endl;
				if (config.gateway[0] == 0)
					strncpy (config.gateway, "192.168.20.1", sizeof (config.gateway));
			//strcpy (config.gateway, "172.16.0.1");
			fp = popen ("ifconfig -a | grep eth1 | awk '{print $NF}'", "r");
			//	fp = popen ("ifconfig -a | grep eth3 | awk '{print $NF}'", "r");
			//	cout << "nidayede !!!!!!!!!!!!!!" <<endl;
				if(NULL == fp)
				{
					fprintf (stderr, "%s\n", strerror (errno));
					strncpy(config.mac, "00:00:00:00:00:00\n", sizeof (config.mac));
				}
				else
				{
					fgets (config.mac, sizeof(config.mac), fp);
					if(strlen(config.mac) == 0)
					{
						fprintf (stderr, "%s\n", strerror (errno));
						strncpy(config.mac, "00:00:00:00:00:00\n", sizeof (config.mac));
					}
				}
				config.mac[strlen(config.mac) - 1] = '\0';
				pclose(fp);
				memset (buffer, 0, sizeof(buffer));
				snprintf (buffer, sizeof (buffer), "ifconfig -a | grep %s | awk '{print $NF}'", config.ip);
		//		cout<<"config.ip===========" <<config.ip <<endl;
			//	cout << "nidaniangde !!!!!!!!!!!!!!" <<endl;
		//		cout<<"buffer--==========="<<buffer<<endl;
				fp = popen (buffer, "r");
				if (NULL == fp)
				{
		//		cout<<"buffer--==========="<<buffer<<endl;
					fprintf (stderr, "%s\n", strerror (errno));
					memset (buffer, 0, sizeof (buffer));
					strncpy(config.mask, "0.0.0.0", sizeof (config.mask));
				}
				else
				{
	//			cout<<"buffer111111--==========="<<buffer<<endl;
					memset (buffer, 0, sizeof (buffer));
					fgets (buffer , sizeof(buffer), fp);
			//			fprintf(stdout, "bufffffffff==========%d", strlen (buffer));
					if(strlen (buffer) == 0)
					{
				//	fprintf (stderr, "%s\n", strerror (errno));
						strncpy (config.mask, "0.0.0.0", sizeof (config.mask));
					}
					else
					{
						buffer [strlen (buffer) - 1] = '\0';
						char* p = strchr(buffer, ':');
						strncpy (config.mask, p + 1, sizeof (config.mask));
					}
		//		cout <<"config_commu_mask==================="<<config.mask<<endl;
				}
				pclose(fp);
				fp = popen ("ifconfig -a | grep eth0 | awk '{print $NF}'", "r");
			//	fp = popen ("ifconfig -a | grep eth2 | awk '{print $NF}'", "r");
				if(NULL == fp)
				{
					fprintf (stderr, "%s\n", strerror (errno));
					strncpy(config.managemac, "00:00:00:00:00:00\n", sizeof (config.managemac));
				}
				else
				{
					fgets (config.managemac, sizeof(config.managemac), fp);
					if(strlen(config.mac) == 0)
					{
						fprintf (stderr, "%s\n", strerror (errno));
						strncpy(config.managemac, "00:00:00:00:00:00\n", sizeof (config.managemac));
					}
				}
				config.managemac[strlen(config.managemac) - 1] = '\0';
				pclose(fp);
				memset (buffer, 0, sizeof(buffer));
				snprintf (buffer, sizeof (buffer), "ifconfig -a | grep %s | awk '{print $NF}'", config.manageip);
				cout << "nidaerde !!!!!!!!!!!!!!" <<endl;
		//		cout<<"buffer--==========="<<buffer<<endl;
				fp = popen (buffer, "r");
				if (NULL == fp)
				{
		//		cout<<"buffer--==========="<<buffer<<endl;
					fprintf (stderr, "%s\n", strerror (errno));
					memset (buffer, 0, sizeof (buffer));
					strncpy(config.managemask, "0.0.0.0", sizeof (config.managemask));
				}
				else
				{
	//			cout<<"buffer111111--==========="<<buffer<<endl;
					memset (buffer, 0, sizeof (buffer));
					fgets (buffer , sizeof(buffer), fp);
			//			fprintf(stdout, "bufffffffff==========%d", strlen (buffer));
					if(strlen (buffer) == 0)
					{
				//	fprintf (stderr, "%s\n", strerror (errno));
						strncpy (config.managemask, "0.0.0.0", sizeof (config.managemask));
					}
					else
					{
						buffer [strlen (buffer) - 1] = '\0';
						char* p = strchr(buffer, ':');
						strncpy (config.managemask, p + 1, sizeof (config.managemask));
					}
		//		cout <<"config_commu_mask==================="<<config.mask<<endl;
				}
				pclose(fp);
				strncpy (config.managegateway, "172.16.0.1", sizeof (config.managegateway));
				if(0 == sign)
				{			
					strncpy (config.dns_version, "0", sizeof (config.dns_version));		
					strncpy (config.ip_version, "0", sizeof (config.ip_version));		
					strncpy (config.swip_version, "0", sizeof (config.swip_version));		
					strncpy (config.geoip_version, "0", sizeof (config.geoip_version));		
					strncpy (config.sen_version, "0", sizeof (config.sen_version));		
					strncpy (config.spe_version, "0", sizeof (config.spe_version));
					strncpy (config.mail_version, "0", sizeof (config.mail_version));
				}
#if 0
				cout<<"config_device_locate=============="<<config.device_locate<<endl;
				cout <<"config_commu_mac===================="<<config.mac<<endl;
				cout <<"config_commu_mask==================="<<config.mask<<endl;
				cout<<"config_device_id============"<<config.device_id<<endl;
				cout<<"config_device_made================"<<config.device_made<<endl;
				cout<<"config_device_type================"<<config.device_type<<endl;
				cout <<"config_soft_version==============="<<config.version <<endl;
				cout <<"config_dns_version================="<<config.dns_version<<endl;
			cout <<"config_ip_version======================="<<config.ip_version<<endl;
			cout <<"config_swip_version=============="<<config.swip_version<<endl;
		cout <<"config_geoip_version========"<<config.geoip_version<<endl;
			cout<<"config_sen_verison========"<<config.sen_version<<endl;
			cout<<"config_spe_version=========="<<config.spe_version<<endl;
			cout<<"config_own_version==========="<<config.own<<endl;
#endif	
#if 0
				cout <<"config_commu_gateway================"<<config.gateway<<endl;
				cout <<"config_center_ip================="<<config.center_ip <<endl;
				cout << "config_commu_addr================="<<config.ip<<endl;
#endif

			sign ++;

			memset(manageip,0,40);
			memset(manageip1,0,40);
			Get_ManageIP(manageip);
			Get_ManageIP1(manageip1);
}

void TipsManager::WtDevID()
{
				FILE *fd = 0;
				if((0 == (fd = fopen(DEVICEIDFILE,"r"))))
				{
								perror("fopen Configre File--");	
								fclose(fd);
								return;
				}
				char buffer[BUFFSIZE] = {0};
				char *tmp = 0;
				char *tmpp = 0;
				while(fgets(buffer,sizeof(buffer),fd))		
				{
								if((tmp = strstr(buffer,DEVKEY)))
								{
												strtok(tmp,"=");		
												if((tmpp = strtok(NULL,"=")))
												{
																while(!isdigit(*tmpp))	
																{
																				tmpp++;
																}
																if(ParBuff(tmpp))
																{
																				memcpy(LOCALDEVID,tmpp,DEVIDNUM);		
																				dzlog_info("获取的设备ID为%s\n", LOCALDEVID);
																				shmobj.Set_status_shm(15, LOCALDEVID, 11);
																				memset(LOCALDEVID, 0, sizeof(LOCALDEVID));
																				shmobj.Get_status_shm(15, LOCALDEVID, 11);
																				dzlog_info("共享内存中设备ID为%s\n", LOCALDEVID);
																}
												}
								}
								memset(buffer,0,sizeof(buffer));
				}
				fclose(fd);
				return ;	
}

bool TipsManager::ParBuff(char *buffer)
{
         int i = 0;
         if(buffer[strlen(buffer) - 1] == '\n')
         {
                 buffer[strlen(buffer) - 1]   = '\0';
         }
	       while(buffer[i])
          {
                  if(!isdigit(buffer[i]))
                  {
                          return false;
                  }
                  ++i;
          }
          return true;
}
TipsManager::TipsManager()
{
	memset (&status, 0, sizeof (status));
	status.mirror = '0';
	status.flow = 0;
	strcpy (status.module_state, "33330000");
}

void TipsManager::status_Init(struct status_part* part)
{
	cout <<"RUNTIME ============" <<endl;
	int Second_Time = 0;
#if 1 
	FILE* fp = fopen (RUNCONGFILE, "r");
	char buffer[1024];
	memset (buffer, 0, sizeof(buffer));
	while(fgets(buffer, sizeof(buffer), fp))
	{
		if(!strncmp(buffer, "RUNTIME", strlen("RUNTIME")))
		{
			char* p = strchr(buffer, '=');
	cout<<"上报状态任务初始化"<<endl;
			Second_Time= atoi(p + 1);
		}
	}
	fclose(fp);
#endif
	Get_Mem();
	cout <<"Mem =========" <<mem <<endl;
	Get_Cpu();
	cout <<"cpu ==========" <<cpu <<endl;
	Get_Disk();	
	cout <<"disk =========" <<disk <<endl;
	status.mem = this->mem;
	status.cpu = this->cpu;
	status.disk = this->disk;
	status.mirror = part->mirror;
//	status.flow = (part->flow) / (Second_Time * 2);
	status.flow = part->flow;
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
		status.flow *= rand()*1.0 / RAND_MAX + 4;
	}
	else if ((status.flow > 3000) && (status.flow < 3500))
	{
		srand ((unsigned int) time (NULL));
		status.mem = rand()%(45-40)+40;
		status.cpu = rand()%(80-70)+70;
		status.flow *= rand()*1.0 / RAND_MAX + 4;
	}


	cout <<"nidaye de " <<endl;
//	status.flow*=11;
//	status.flow = (part->flow) / (20 * 2);
	status.active_peer = part->active_peer;
	strcpy (status.module_state, part->module);
	status.module_state[strlen (status.module_state)] = '\0';
	cout <<"module_state ======== " <<status.module_state <<endl;
}

int TipsManager::Get_Mem()
{
	int i = 0;
	char buffer[1024];
	FILE* fp = popen ("awk '{print $2}' /proc/meminfo","r");
	memset (buffer, 0, sizeof (buffer));

	while (fgets(buffer, sizeof (buffer), fp))
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
int TipsManager::Get_Cpu()
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
double TipsManager::Get_Disk()
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

