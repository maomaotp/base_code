#include "cachehandle.h"
#include <sys/socket.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "zlog.h"
using namespace std;

///extern sqlite3* db;
#define SQLMAX 2048
//extern pthread_mutex_t sqlite_sync_mutex;
//extern int startrun;
//extern CTips_sqlite_sync tips_sqlite_sync;
uint32_t alarmstartip = 0;
uint32_t alarmendip = 0;
uint32_t alarmstartip1 = 0;
uint32_t alarmendip1 = 0;
int cache_stop_flag = 0;
map<struct ipflag,struct ipscc>* ipmap;


void stop_cache(int param)
{
	printf("cache prepare to stop\n");
	cache_stop_flag = 1;
	param = param;			//hyj 保留参数
}

bool CCachehandler::Set_Non_Block(int sockfd)
{
	int opts = fcntl(sockfd, F_GETFL);
	if(-1 == opts)
	{
		printf("%d->fcntl F_GETFL is faild!\n",getpid());
		return false;
	}
	opts = opts | O_NONBLOCK;
	if (fcntl(sockfd, F_SETFL, opts) < 0)
	{
		printf("%d->fcntl F_SETFL is faild!\n",getpid());
		return false;
	}
	return true;
}

int CCachehandler::Read_Data(int fd, void* buff, int len)
{
	int read_len = 0, temp = 0;
	while (len > read_len)
	{
		temp = read(fd, (u_int8_t*)buff + read_len, len - read_len);
		if (temp <= 0)
			return -1;
		read_len += temp;
	}
#if 0 
	audit_info_t* audit_info = (audit_info_t*)buff;
	char buffer[32] = {0};
	u_int sip=ntohl(audit_info->sip);
	u_int dip=ntohl(audit_info->dip);
	printf("sip======%u,dip===========%u\n",sip,dip);
	printf("\nsip=%s ", inet_ntop(AF_INET, (void*)&sip, buffer, 32));
	printf("sipstr=============%s\n",buffer);
	printf("\ndip=%s ", inet_ntop(AF_INET, (void*)&dip, buffer, 32));
	printf("dipstr=============%s\n",buffer);
	printf("sport=%u  dport=%u\n", audit_info->sport, audit_info->dport);

#endif
	return read_len;
}

int CCachehandler::Read_Data_Len(int fd)
{
	int len;
	int* temp = &len;
	int err = Read_Data(fd, temp, sizeof (len));
	if (err <= 0)
		return err;
	return len;
}

int CCachehandler::Deal_Data(void* data, int len)
{
	int data_len = len;
	int err = memcache.Mem_Write(data, &data_len);
	if (err == -1)
	{
//		cout<<"给的buff为空!"<<endl;
		return -1;
	}
	else if (err <= 0)
	{
		struct Data_Cache cache_data;
		cache_data.data = malloc(len);
		cache_data.len = len;
		if (cache_data.data)
			return -2;
		memcpy(cache_data.data, data, len);
		pthread_mutex_lock(&list_mutex);
		cache_list.push_back(cache_data);
		pthread_mutex_unlock(&list_mutex);
	}
	else 
	{
		if (cache_list.size())
		{
			pthread_mutex_lock(&list_mutex);
			list<Data_Cache>::iterator beg = cache_list.begin();
			int err = memcache.Mem_Write(beg->data, &(beg->len));
			if (err != -1)
			{
				free(beg->data);
				cache_list.erase(beg);	
			}
			pthread_mutex_unlock(&list_mutex);
		}
		return 1;
	}
	return 0;		//hyj 返回值?
}

int CCachehandler::Deal_Client_Data(int sockfd)
{
	int len = Read_Data_Len(sockfd);
	//	fprintf(stdout, "length ===== %d\n", len);	
	if (len <= 0)
		return -1;
	u_int8_t data[len];
	int err = Read_Data(sockfd, data, len);
	//	cout<<"读取socket数据:"<<err<<endl;
	if (err <= 0)
		return -1;
	//处理数据
	Deal_Data(data, len);
	return err;
}

int CCachehandler::Init_Socket(int listenquerenum)
{
	listenfd = socket(PF_INET, SOCK_STREAM, 0);
	if (listenfd <= 0)
	{
		dzlog_info("创建服务器套接字失败!");
		return -1;
	}	
	//允许地址重用
	int reuse = 1;
	setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof (reuse));
	bool err = Set_Non_Block(listenfd);
	if (err == false)
	{
		dzlog_info("设置非阻塞失败!");
		return -2;
	}
	struct sockaddr_in serveraddr;
	bzero(&serveraddr, sizeof (serveraddr));
	serveraddr.sin_family = PF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	//	inet_aton("172.16.0.107",&(serveraddr.sin_addr));
	serveraddr.sin_port = htons(SERVERPORT);
	int berr = bind(listenfd, (struct sockaddr*)&serveraddr, sizeof (serveraddr));
	if (berr < 0)
	{
		dzlog_info("bind server IP failed");
		return -3;
	}
	berr = listen(listenfd, listenquerenum);
	if (berr < 0)
	{
//		cout<<"设置监听队列失败!"<<endl;
		return -4;
	}
	return listenfd;
}

int CCachehandler::Epoll_Connect()
{
//	cout<<"Epoll_Connect............"<<listenfd<<endl;
	struct sockaddr_in clientaddr;
	socklen_t client = sizeof (clientaddr);
	int client_connect = 0;	
	client_connect = accept(listenfd, (struct sockaddr*)&clientaddr, &client);
	if (client_connect < 0)
	{
	//	cout<<"accept faild!"<<client_connect<<endl;
		return 0;
	}
	Set_Non_Block(client_connect);
	struct epoll_event ev;
	ev.data.fd = client_connect;
	ev.events = EPOLLIN | EPOLLET;
	epoll_ctl(epfd, EPOLL_CTL_ADD, client_connect, &ev);
	return 1;
}

int CCachehandler::Epoll_In(struct epoll_event* ev)
{
	int tempfd = 0;
	if ((tempfd = ev->data.fd) < 0)
	{
//		cout<<"当前可读链接失效!"<<endl;
		return -1;
	}
	//调用函数处理数据
	int readerr = Deal_Client_Data(tempfd);
	//	fprintf(stdout, "readerr ===== %d\n", readerr);
	if (readerr == -1)
	{
//		cout<<"对方关闭链接!"<<endl;
		close(tempfd);
		ev->data.fd=-1;
	}
	else
	{
		memset(ev, 0, sizeof (ev));
		ev->data.fd = tempfd;
		ev->events = EPOLLIN | EPOLLET;
	}
	epoll_ctl(epfd, EPOLL_CTL_MOD, tempfd, ev);
	return 1;
}

int CCachehandler::Epoll_Out(struct epoll_event* ev)
{
	int tempfd = 0;
	tempfd = ev->data.fd;
	memset(ev, 0, sizeof (ev));
	ev->data.fd = tempfd;
	ev->events = EPOLLIN | EPOLLET;
	epoll_ctl(epfd, EPOLL_CTL_MOD, tempfd, ev);
	return 1;
}

int parse_cmd(char* cmdbuff, int len)
{
	printf("cmdbuff===%s  len=%d\n", cmdbuff, len);
	int num = 0;
 	int i = 0;
 	printf("%s\n", cmdbuff);
 	if(!strncmp(cmdbuff,"L+6",3))
 	{
 		char num_str[10] = {0};
 		char* p = strstr(cmdbuff,"{");
 		char* q = strstr(cmdbuff,"}");
 		memcpy(num_str,p+1,q-p-1);
 		num_str[strlen(num_str)] = '\0';
 		num = atoi(num_str);
 		printf("num=%d\n", num);
 		int offset = 6+strlen(num_str);
 		printf("offset=%d\n",offset);
 		printf("%s\n", cmdbuff+offset);
		struct ippos_rules* rules = (struct ippos_rules*) (cmdbuff+offset);

	//	ipmap = new map<struct ipflag,struct ipscc>;	
    struct ipscc ips;
    struct ipflag ipf;
    for (int i = 0; i < num; i++)
    {
				memset (&ips, 0, sizeof (ips));
				memset (&ipf, 0, sizeof (ipf));
        ips.startip = ((struct ippos_rules*)rules)[i].startip;
        ipf.startip = ((struct ippos_rules*)rules)[i].startip;
        ips.endip = ((struct ippos_rules*)rules)[i].endip;
        ipf.endip = ((struct ippos_rules*)rules)[i].endip;
				ips.flag = ((struct ippos_rules*)rules)[i].flag;
        // cout <<"ips.flag ============= "<<ips.flag <<endl;
				strcpy(ips.region, ((struct ippos_rules*)rules)[i].region);
				strcpy(ips.territory, ((struct ippos_rules*)rules)[i].tirr);
				//		cout <<"wuxiangfeng-====region===="<<ips.region <<endl;
				ipmap->insert (map<struct ipflag, struct ipscc>::value_type(ipf, ips));
		}
  	map<ipflag, ipscc>::iterator mit;
    mit = ipmap->begin();
    while(mit != ipmap->end()){
    //	cout << "wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww" << mit->second.territory << endl;
			mit++;
		}
	}
	
	i = i;		//hyj 未使用的变量
	return 0;	//返回值?
} 		
  		
void CCachehandler::Rules_Do()
{ 		
	    
			int server_sock = Init_accept_server();
			if(server_sock == -1)
			{
				printf("Init accept_Rules server failed\n");
				return;
	}
//	printf("line=%d\n", __LINE__);
//	char * cmdbuff =(char *) calloc(0, 1024*1024*10);
	char  cmdbuff[1024*1024*7] = {0};
//	printf("line=%d\n", __LINE__);
	int len = 0;
	while(1)
	{
	//	printf("line=%d\n", __LINE__);
		accept_policy(server_sock, cmdbuff, &len);
//		printf("line=%d\n", __LINE__);
		parse_cmd(cmdbuff, len);
//		printf("line=%d\n", __LINE__);
	}
//	free(cmdbuff);
}

void* rules_fun(void* pobj)
{
	CCachehandler* cache = (CCachehandler*)pobj;
	cache->Rules_Do();
	
	return NULL;	//返回值？
}


int CCachehandler::Start_Rules()
{
	if (!pthread_create(&rulesid, NULL, rules_fun, this))
		return 1;
	else 
		return 0;
}

void CCachehandler::Productor_Do()
{
	struct epoll_event events[CLIENTNUM] = {{0, {0}}};	//update by hyj
	int nfds = 0;
	while(1)
	{
//		printf("cache_stop_flag = %d\n",cache_stop_flag);
		if(1 == cache_stop_flag)
				return;
		nfds = epoll_wait(epfd, events, CLIENTNUM - 1,-1);
		for (int i = 0; i < nfds; ++ i)
		{
			if (events[i].data.fd == listenfd)
			{
		//		cout<<"listenfd:"<<listenfd<<endl;
		//		cout<<"监听fd接受到链接请求!:"<<events[i].data.fd<<endl;
				Epoll_Connect();
			}	
			else if (events[i].events & EPOLLIN)
			{//可读返回
				//	cout<<"客户端发来数据!"<<endl;
				Epoll_In(&(events[i]));
			}
			else if (events[i].events & EPOLLOUT)
			{
		//		cout<<"链接可读!"<<endl;
				Epoll_Out(&(events[i]));
			}
		}
	}
}

void* pro_fun(void* pobj)
{
	CCachehandler* cache = (CCachehandler*)pobj;
	cache->Productor_Do();
	
	return NULL;		//hyj 返回值?
}

int CCachehandler::Start_Productor()
{
	if (!pthread_create(&proid, NULL, pro_fun, this))
		return 1;
	else 
		return 0;
}

int CCachehandler::Pop_Data()
{
	char buff[DATAUNITLEN];
	int len = DATAUNITLEN;
	int read_cong = 1;
	while (1)
	{
//		cout<<"111111111111"<<endl;
		len = DATAUNITLEN;
		int err = memcache.Mem_Read(buff, &len);
//		cout<<"111111111111"<<endl;
#if 0 
		audit_info_t* audit_info = (audit_info_t*)buff;
		char buffer[32] = {0};
		u_int sip=ntohl(audit_info->sip);
		u_int dip=ntohl(audit_info->dip);
		printf("sip======%u,dip===========%u\n",sip,dip);
		printf("\nsip=%s ", inet_ntop(AF_INET, (void*)&sip, buffer, 32));
		printf("sipstr=============%s\n",buffer);
		printf("\ndip=%s ", inet_ntop(AF_INET, (void*)&dip, buffer, 32));
		printf("dipstr=============%s\n",buffer);
		printf("sport=%u  dport=%u\n", audit_info->sport, audit_info->dport);

#endif

		//		audit_info_t* adt = (audit_info_t*)buff;
		/*  fprintf(stdout, "sip ====== %u\n", adt->sip);
		    fprintf(stdout, "dip ====== %u\n", adt->dip);
		    fprintf(stdout, "sport ==== %d\n", adt->sport);
		    fprintf(stdout, "dport ==== %d\n", adt->dport);
		    fprintf(stdout, "protocol =============== %u\n", adt->protocol);
		    fprintf(stdout, "up_bytes =============== %llu\n", adt->up_bytes);
		    fprintf(stdout, "down_bytes ============= %llu\n", adt->down_bytes);
		    fprintf(stdout, "start_time ============= %u\n", adt->start_time);
		    fprintf(stdout, "end_time =============== %u\n", adt->end_time);
		    */	if (err == 0)
		{
			//cout<<"当前内存池没有数据!"<<endl;
			return 1;
		}
		else if (err == -1)
		{
			//cout<<"buff=======NULL!"<<endl;
			break;
		}
		else if (err == -2)
		{
			//		cout<<"读取buff长度不够!"<<endl;
			if (read_cong > 50)
				read_cong = 2;
			read_cong += 1;
			len = len * read_cong;
			char temp[read_cong * DATAUNITLEN];
			int temperr = memcache.Mem_Read(temp, &len);

			if (temperr > 0)
			{
				fprintf(stdout, "insert database start!\n");
				Insert_DB(temp);				
#if 0
				cout<<"处理数据......."<<temp<<endl;
				//处理数据
#endif
			}
		}
		else
		{
			fprintf(stdout, "insert database start!\n");
			Insert_DB(buff);				
#if 0
			cout<<"处理数据......."<<temp<<endl;
			cout<<"处理数据2......."<<buff<<endl;
			//处理数据
#endif
		}
	}
	return 0;	//hyj 返回值?
}
//}
CCachehandler::CCachehandler()
{

}

int CCachehandler::Insert_DB(void* data)
{
	char sip[32];
	char dip[32];
	memset(sip, 0, sizeof (sip));
	memset(dip, 0, sizeof (dip));
	char sql[4096];
	CInit_sql init;
	//	fprintf(stdout, "获取flag标志\n");
	cache_head_t * head = (cache_head_t*)data;
	fprintf(stdout, "type ============ %d\n", head->type);
	switch (head->type)
	{
		case AUDIT:
			{
#if 0
				char starttime[21];
				char endtime[21];
				memset(starttime, 0, sizeof (starttime));
				memset(endtime, 0, sizeof (endtime));
				//			fprintf(stdout, "netlog start!\n");
				//	fprintf(stdout, "netlog obtain successful!\n");
				cache_audit_t * audit = (cache_audit_t *)data;
				init.TurnTime(audit->start_time, starttime, 21);
				init.TurnTime(audit->end_time, endtime, 21);

				u_int sp = htonl(audit->sip);
				u_int dp = htonl(audit->dip);
				inet_ntop(AF_INET, (void*)&sp, sip, 32); 
				inet_ntop(AF_INET, (void*)&dp, dip, 32); 
				memset(sql, 0, sizeof (sql));
				//	fprintf(stdout, "111111111111111111111111111111111111111111\n");
				sprintf(sql, "insert into tips_netlog(Dip, Dport, Sip, Sport,Protocol, Start_time, End_time,`Load`, `Upload`,flag) values('%s','%u','%s','%u','%u','%s','%s','%.2lf','%.2lf','0');",
						dip,
						audit->dport,
						sip,
						audit->sport,
						audit->protocol,
						starttime,
						endtime,
						audit->up_KB,
						audit->down_KB);
				fprintf(stdout, "2222222222222222222222222222222222222222\n");
#endif
#if 1
				fprintf(stdout, "audit start!\n");
				cache_audit_t * audit = (cache_audit_t *)data;
				int ret = init.sql_netopt_info(audit,sql);
				ret = ret;		//hyj 变量未被使用？
				fprintf(stdout, "audit end\n");
#endif
				break;
			}

		case ALARM:
			{
				fprintf(stdout, "alarm start!\n");
				cache_alarm_t * alm = (cache_alarm_t *)data;
				int ret = init.sql_alarm_info(alm, sql);
				ret = ret;		//hyj 变量未被使用？
				fprintf(stdout, "alarm end\n");
			}
			break;
		case OPT:
			{
				fprintf(stdout, "opt start!\n");
				cache_opt_t * opt = (cache_opt_t *)data;
				int ret = init.sql_opt_info(opt, sql);
				ret = ret;		//hyj 变量未被使用？
				fprintf(stdout, "opt end\n");
			}
			break;
		case SECRET:
			break;
		case BLOCK:
			{
				fprintf(stdout, "block start!\n");
				cache_block_t * block = (cache_block_t *)data;
				int ret = init.sql_block_info(block, sql);
				ret = ret;		//hyj 变量未被使用？
				fprintf(stdout, "block end\n");
			}
			break;
		case ALERT:
			{
				fprintf(stdout, "alert start!\n");
				cache_alert_t * alert = (cache_alert_t *)data;
				int ret = init.sql_alert_info(alert, sql);
				ret = ret;		//hyj 变量未被使用？
				fprintf(stdout, "alert end\n");
			}
			break;
		default:
			break;
	}
	//fprintf(stdout, "zzf............database inserts start\n");
	insertdb.Insertdata(sql);
	//fprintf(stdout, "zzf.........database inserts successful!\n");

	return 0;		//hyj 返回值?
}

void CCachehandler::Consumer_Do()
{
	while (1)
	{
//		printf("cache_stop_flag = %d\n",cache_stop_flag);
		if(1 == cache_stop_flag)
		{
				return;
//			second = 1;
		}
		int err = memcache.Mem_Change(second);
	//	cout<<"交换返回"<<err<<endl;
		if (err == -1)
		{
	//		cout<<"当前读取没有完成!"<<endl;
			//继续读取
			Pop_Data();
			continue;
		}
		else if (err == 0)
		{
	//		cout<<"当前没有数据可以读取!"<<endl;
			if(1 == cache_stop_flag)
			{
				return;
			}
			continue;
		}
	//	cout<<"交换成功开始读取"<<endl;
		Pop_Data();
	}
}

void* csm_fun(void* pobj)
{
	CCachehandler* cache = (CCachehandler*)pobj;
	cache->Consumer_Do();

	return NULL;		//返回值?
}

int CCachehandler::Start_Consumer(int sec)
{
	second = sec;
	if (!pthread_create(&csmid, NULL, csm_fun, this))
		return 1;
	else 
		return 0;
}

int CCachehandler::Init_Server(int listenqnum, int cache_size)
{
	/*	if(db==NULL)
		{
		printf("db========NULL\n");
		}
		else
		{
		printf("db!========NULL\n");
		}
		*/	insertdb.Set_DB();
	ipmap = new map<struct ipflag,struct ipscc>;	
	//初始化私有变量
	pthread_mutex_init(&list_mutex, NULL);
	cache_flag = false;
	//初始化socket服务器
	int init_err = Init_Socket(listenqnum);
	if (init_err < 0)
	{
	//	cout<<"init socket err!"<<init_err<<endl;
		return -1;
	}
	//初始化缓存池
	if (!memcache.Mem_Init(cache_size, NULL))
	{
//		cout<<"init memcache filed!"<<endl;
		return -2;
	}
	//初始化epoll
	epfd = epoll_create(EPOLLMAXFD);
	struct epoll_event ev;
	bzero(&ev, sizeof(ev));
	ev.data.fd = listenfd;
	ev.events = EPOLLIN | EPOLLET;
	int perr = epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev);
	if (-1 == perr)
	{
	//	cerr << strerror(errno) <<endl;
		return -3;
	}
	return 1;
}

int CCachehandler::Start_Server(int sec)
{
	signal(SIGUSR1, stop_cache);
	int err = Start_Rules();
	if (err <= 0)
	{
//		cout<<"start rules failed!"<<err<<endl;
		return -1;
	}
	err = Start_Productor();
	if (err <= 0)
	{
//		cout<<"start productor failed!"<<err<<endl;
		return -1;
	}
	err = Start_Consumer(sec);
	if (err <= 0)
	{
//		cout<<"start consumer failed!"<<err<<endl;
		return -2;
	}
	return 1;
}

int CCachehandler::Exit_Server()
{
	close(listenfd);
	close(epfd);
	pthread_mutex_destroy(&list_mutex);
	memcache.Mem_Exit();
	return 1;
}

void CCachehandler::Wait_Exit()
{
	pthread_join(rulesid,NULL);
	pthread_join(proid,NULL);
	pthread_join(csmid,NULL);
}
