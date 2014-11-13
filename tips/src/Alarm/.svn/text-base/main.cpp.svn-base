#include "data.h"

extern "C"
{
#include "init.h"
#include "client.h"
}

char ETH_ALARM[10] = {0};
CCacheclient cacheclient;
UnixDomain unixdomain;
CMempool filemem;
CheckObj check;
StartCapture startpacket;
StartFileSave startfile;

void WtPid()
{
	FILE *fp=NULL;
	char id_buf[10];
	pid_t pid;

	memset(id_buf,0,10);
	pid = getpid();
	dzlog_info("alarm start now, pid==%d", pid);
	sprintf(id_buf,"%d",pid);
	if(NULL == (fp = fopen(PIDFILE,"w+"))){
		dzlog_info("open PIDFILE failed!!!");	
		return;
	}
	fwrite(id_buf,strlen(id_buf),1,fp);
	fclose(fp);
	return;
}

void handle_func(int sig)
{
	dzlog_info("tipsALARM: i received the signal and i will sleep 30 ");
    sleep(30);
    dzlog_info("i wake up begin start work");
	if(sig==SIGSTOP) return;

	return;
}

bool InitWorker()
{
	/*
	 * 初始化cache client,用于将报警信息传给cache
	 * 初始化本进程UnixDomain,用于接收manager的管控命令和策略文件
	 * 初始化上传文件内存池,用于保存pcap文件
	 * 初始化检测对象
	 * */
	Outinit();
#if 0
	redis_init("127.0.0.1", 6379);
    char command[30] = "lpush send_Msg ";
    char* send_buff = "lq112132";
    strcat(command, send_buff);
    redis_command(command);
#endif
	if(cacheclient.Init_Client((char *)"127.0.0.1") && unixdomain.Init_UnixDomain() && filemem.Mempool_Init(65535,5000,UNIT_STATIC) && check.Init_Check()){
		return true;
	}
	else{
		dzlog_info("init_client fail!!!");
		return false;
	}
}

void StartWorker()
{
	pthread_t file_id = 0,domain_id = 0;
	
	/*为file线程传递filemem类,保证使用的是同一个mem*/
	startfile.Set_Mempool(&filemem);	
	/*FIXME 线程1 开启文件保存线程*/
	startfile.Start(&file_id);	

	/*(必须先注册所有的检查项再开启unixdomain的aceept,保证各子模块能接到策略)将所需子模块的Start函数注册进tcp\ip\udp链表*/
	check.RegisterAll();	

	/*FIXME 初始化状态机*/
	unixdomain.InitFsm();

	/*FIXME 线程2 开启domain监听线程*/
	unixdomain.Start(&domain_id);

	/*FIXME 线程3(主线程) 主线程负责抓包,永不返回*/
	startpacket.Start();
	return ;
}

int main(int argc, char *argv[])
{
    if(argc <= 1) {
        printf("please take interface parameters, such as:\n'./tipsAlarm eth2'\n");
        return -1;
    }
    
    //zlog init
    if(dzlog_init("/usr/local/tipschecker/etc/log.conf", "alarm")){
        printf("alarm dzlog init failed\n");
    }
	WtPid(); //保存PID	
	
	init();
	strcpy(ETH_ALARM,argv[1]);

    signal(SIGUSR1, handle_func);
    /*初始化相关*/
	if(!InitWorker())
	{
		dzlog_info("init worker failed!!!");
		return -1;
	}

    /*开启各子模块,开始检查*/
	StartWorker();
	return 0;
}
