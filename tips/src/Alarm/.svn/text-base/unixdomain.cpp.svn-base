#include "unixdomain.h"
#include "zlog.h"

/*此结构用于自动转移实现传递策略*/
struct rules_fsm loadtab[LoadTabCnt] = {
				/*  pro	   module_no	 checker	  rules_action 		opt_dsc */
				{ MPro,		DNS_NO,		  checker[3],		0,		(char *)"DNS策略下发成功"},				/*DNS*/
				{ MPro, 	IPBlack_NO,	  checker[1],		0,		(char *)"IP策略加载成功"},				/*IP黑名单*/
				{ MPro,		Trojan_NO,		0,				0,		(char *)"木马策略加载成功"},			/*FIXME c 木马策略*/

				/*根据各个进程实际子模块定义结构体*/
#if 0 
				{ MPro,		Comptro_NO,		0,				0,		(char *)"审计IP白名单策略加载成功"},	/*FIXME c 审计白名单*/
				{ MPro,		Arcanum_NO,		0,				0,		(char *)"senlist涉密标志文件加载成功"}, /*FIXME c 涉密标识*/
				{ MPro,		Location_NO,	0,				0,		(char *)"IP地理信息加载成功"},			/*FIXME c 地理信息*/
				{ MPro,		Mail_NO,		0,				0,		(char *)"maillist策略加载成功"},		/*FIXME c 邮件策略*/

				{ MCmd,		CmdParCom_NO,	0,				0,		(char *)"netshark命令下发成功"},		/*FIXME c netshark*/
				{ MCmd,		CmdNoCom_NO,	0,				0,		(char *)"stopnetshark命令下发成功"},	/*FIXME c stopnetshark*/
				{ MCmd,		CmdParBlock_NO,	0,				0,		(char *)"netblock命令下发成功"},		/*FIXME c netblock*/
				{ MCmd,		CmdNoBlock_NO,	0,				0,		(char *)"stropnetblock命令下发成功"},	/*FIXME c stopnetblock*/
#endif
};

void GetMsgBySys(char *user,cache_opt_t *OptStu,char *dsc)
{
	time_t mytime = 0;
	time(&mytime);
	struct tm *tm_p = localtime(&mytime);
	sprintf(OptStu->opttime,"%d-%d-%d %d:%d:%d",(1900+tm_p->tm_year),(1+tm_p->tm_mon),(tm_p->tm_mday),(tm_p->tm_hour),(tm_p->tm_min),(tm_p->tm_sec));
	memcpy(OptStu->username,user,strlen(user) + 1);
	memcpy(OptStu->describe,dsc,strlen(dsc)+1);
	return ;
}


/*FIXME 以下函数用于处理接收到的管控命令、策略和转发给相应子摸块*/
void Parse_Command(char *cmdbuff,int cmdlen)
{
	int i = 0;
	char *p = 0, *q = 0;
	char tmp[20] = {0};

	if(((MPro != cmdbuff[0]) && (MCmd != cmdbuff[0])) && !('+' == cmdbuff[1]))
	{
		printf("cmdbuffer:%s is unknown! the cmdlen==%d\n", cmdbuff, cmdlen);
		return;
	}

	cache_opt_t OptStu;/*用于将操作记录发送给cache写入数据库*/

	for(i = 0;i < TrueLoadTabCnt;i++ )
	{
		memset(tmp,0,sizeof(tmp));
		if((p = strchr(cmdbuff,'{')))
		{
			memcpy(tmp,&cmdbuff[2],p - &cmdbuff[2]);
		}
		if((loadtab[i].module_no == atoi(tmp)))
		{
			printf("loadtab[%d].module_no:%d\n",i,atoi(tmp));
			memset(&OptStu,0,sizeof(OptStu));
			INIT_OPT_HEAD(&OptStu);/*init opt_stu*/
			//if(!(loadtab[i].obj))
			//printf("i'm girl!\n");
			if((q = strchr(p,'}')) && (loadtab[i].obj))/* FIXME 加载策略 source"L+4{6} **"*/
			{
				memcpy(tmp,p+1,q - p + 1);
				(loadtab[i].obj)->Load_Rules(q+2,atoi(tmp));/*q+2:跳过空格*/
			}
			//FIXME 加载命令 source"C+11{ netshark"
			else if((q = strchr(p,'>')) && (loadtab[i].rules_action))
			{
				//printf("C->loadtab[%d].module_no:%s - 参数:%s-|*(q+2):|-|总字节数%d|\n", loadtab[i].module_no, q+2, *(q+2), strlen(q+2));	
				(loadtab[i].rules_action)(q+2,strlen(q+2));/*q+2:跳过空格*/
			}
			else{
				printf("cmdbuffffffffffffff:<%s>\n",cmdbuff);continue;
			}
			/*发送操作记录到cache*/
			GetMsgBySys((char *)"system",&OptStu,loadtab[i].opt_dsc);
			cacheclient.Send_Msg((char *)&OptStu,sizeof(OptStu));
		}
	}
	return ;
}



/*FIXME 以下函数用于建立unixdomain和接收管控命令、策略
 *Create unixdomain - listen
 */
bool UnixDomain::Init_UnixDomain()
{
	unlink(UNIX_DOMAIN);
	manager_domain=socket(PF_UNIX,SOCK_STREAM,0);
	if(manager_domain<0)
	{	
		cout<<"domain.alarm create err!"<<endl;
		close(manager_domain);
		return false;
	}
	struct sockaddr_un saddr;
	saddr.sun_family=AF_UNIX;
	//strncpy(saddr.sun_path,UNIX_DOMAIN,sizeof(saddr.sun_path)-1);
	strncpy(saddr.sun_path,UNIX_DOMAIN,strlen(UNIX_DOMAIN)+1);
	if(bind(manager_domain,(struct sockaddr*)&saddr,sizeof(saddr))<0)
	{
		cout<<"domain.alarm bind err!"<<endl;
		close(manager_domain);
		unlink(UNIX_DOMAIN);
		return false;
	}
	if(listen(manager_domain,1)<0)
	{
		cout<<"domain.alarm listen err!"<<endl;
		close(manager_domain);
		unlink(UNIX_DOMAIN);
		return false;
	}
	printf("unixdomain listen ok!\n");
	return true;
}


void  UnixDomain::InitFsm()
{
	loadtab[0].obj = checker[3];/*register dns checker*/
	loadtab[1].obj = checker[1];/*register ipblack checker*/
	return;
}



void* start_accept(void* argc)
{
	UnixDomain *thisclass = (UnixDomain *)argc;

	char cmdbuff[1024*1024*8] = {0};
	char buff[1024*1024*1] = {0};
	char *p = NULL;
	unsigned int cmdlen = 0,getlen = 0,cnt = 0;

	struct sockaddr_un daddr;
	memset(&daddr,0,sizeof(daddr));
	socklen_t socklen=sizeof(daddr);
	while(1){
		if((thisclass->data_domain=accept(thisclass->manager_domain,(struct sockaddr*)&daddr,&socklen)) <= 0)
		{
			printf("unixdomain accept is err!\n");
			close(thisclass->data_domain);
			thisclass->data_domain = 0;
			continue;
		}
		//先读出将要接收的管控命令或策略的长度
		read(thisclass->data_domain, (int*)&getlen, sizeof(getlen));

		/*拼接管控命令或策略*/
		cmdlen = 0;
		cnt = 0;
		while(cmdlen < getlen)
		{
			if(getlen < sizeof(buff))
			{
				cnt = read(thisclass->data_domain, buff, getlen - cmdlen);
            }
			else
            {
			    cnt = read(thisclass->data_domain, buff, sizeof (buff));
			}
			memcpy (cmdbuff+cmdlen, buff, cnt);
			cmdlen += cnt;
			memset(buff,0,sizeof(buff));
		}
		/*将管控命令或策略传给parse_cmd*/
		p=cmdbuff;
		Parse_Command(p,cmdlen);
		memset(cmdbuff, 0, sizeof (cmdbuff));
		p = 0;
		getlen = 0;
    }
	return NULL;
}

bool  UnixDomain::Start(pthread_t *id)
{
	printf("I'm Start accept!\n");
	if (pthread_create (id, NULL, start_accept, this) != 0)
	{
		dzlog_info ("create start unixdomain'pthread  is error!\n");
		return false;
	}
	return true;
}
