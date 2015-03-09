
/*	进度记录：
 *	(1)完善tipsstat.sh 进行重启此进程
 *	(2)自测、联测、交给测试
 *	2014.03.18
 *	by yong
 * */



/*	进度记录：
 *	175:void 	ResetAction(char *sh_name,int CacheFlag)
 *	2014.03.10
 *	by yong
 * */



/* 进度记录：
 * 222 void 	GetCurrImage()
 * 2014.03.11
 */


/* 进度记录：
 * (1)光电自适应模块缺读内存池，获取镜像模式(与李培庭够同改为读配置文件)
 * (2)daemon模块缺将当前使用的网口信息写入共享内存(按照冯晓亮提供的共享内存字段写入)
 * (3)各执行脚本整理、优化
 * (3)自测、联测、提交测试
 * 2014.03.14
 */


/* 进度记录：
 * (1)各执行脚本整理、优化
 * (2)流程梳理
 * (3)自测、联测、提交测试
 * 2014.03.14 21:00
 */






/*	FIXME 持续优化
 * (1)尝试在子进程轮询pipe时改用epoll
 * 精确判断想用pid是否存在proc中
 * (2)尝试在双镜像口和光电自适应模块中使用
 * inotify机制监控各网卡文件的变化
 *	2014.03.07 	
 *	by yong
 */


/* update
 * Reservation originally all things,add LightCtl
 * -Program's Light-
 * hardware flatform
 * CPU:Intel(R) Atom(TM) CPU D525   @ 1.80GHz
 * Base Board:Acrosser AR-B6006 Ver1.1
 * date:2013.11.02
 * by yong 
 * */


#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<fcntl.h>
#include<string.h>
#include<dirent.h>
#include <pwd.h>
#include <ctype.h>
#include <pthread.h>
#include <signal.h>

#include "ReadConfig.h"
#include "pubdata.h"
#include "Sigsuspend.h"
#include "AdaptiveData.h"
#include "sharemem.h"
#include "zlog.h"

//int LoopTimes;
#ifdef __cplusplus
extern "C"{
#endif

#include "n6601.h"

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C"{
#endif
extern UINT8 gPort[100];
int  InitializeGPIO(UINT8 *gpio_num);
int  CtlLight(UINT8 *gpio_num,int flag);
int  Glimpse(UINT8 *gpio_num,int cnt,int onspeed);
#ifdef __cplusplus
}
#endif

/*liuq*****************/

typedef struct PACKED_M
{
    char name[20];
    unsigned long total;
    char name2[20];
    unsigned long free;
}MEM_OCCUPY;


typedef struct PACKED_C
{
    char name[20];
    unsigned int user;
    unsigned int nice;
    unsigned int system;
    unsigned int idle;
}CPU_OCCUPY;

int get_memoccupy(MEM_OCCUPY* m)
{
    FILE* fd;
    int n;
    char buff[256];

    fd = fopen("/proc/meminfo", "r");
    fgets(buff, sizeof(buff), fd);
    sscanf(buff, "%s %u", m->name, &m->total);

    fgets(buff, sizeof(buff), fd);
    sscanf(buff, "%s %u", m->name2, &m->free);

    fclose(fd);
    
    //printf("get_memoccupy>>>>>>>>>>\n%s:%u %s:%u\n******************\n", m->name, m->total, m->name2, m->free);
    
    return (m->total - m->free)*100/m->total; 
}

int cal_cpuoccupy(CPU_OCCUPY* o, CPU_OCCUPY* n)
{
    unsigned long od, nd;
    unsigned long id, sd;
    int cpu_use=0;

    od = (unsigned long)(o->user + o->nice + o->system + o->idle);

    nd = (unsigned long)(n->user + n->nice + n->system + n->idle);

    id = (unsigned long)(n->user - o->user);

    sd = (unsigned long)(n->system - o->system);

    if((nd-od) != 0){
        cpu_use = (int)(sd+id)*10000/(nd-od);
    }
    else cpu_use = 0;

    return cpu_use;
}

int get_cpuoccupy(CPU_OCCUPY* cpust)
{
    FILE* fd;
    int n;
    char buff[256];
    CPU_OCCUPY* cpu_occupy;
    cpu_occupy=cpust;
    
    fd = fopen("/proc/stat", "r");
    fgets(buff, sizeof(buff), fd);

    sscanf(buff, "%s %u %u %u %u", cpu_occupy->name, &cpu_occupy->user, &cpu_occupy->nice, &cpu_occupy->system, &cpu_occupy->idle);
    //printf("cpu>>>>>>>>>>>>>>>>>\ncpu->name==%s, cpu->user==%u, cpu->nice==%u, cpu->system==%u, cpu->idle==%u\n***************\n", cpu_occupy->name, cpu_occupy->user, cpu_occupy->nice, cpu_occupy->system, cpu_occupy->idle);

    fclose(fd);
}
/*end*******************/

/* 用于将当前工作的网口信息写入共享内存 */
ShareMem obj;		

/* FIXME 在AdaptiveData.cpp中定义 */
extern pthread_mutex_t 		curr_mutex;	 			/*"(检测进程)保活线程"用*/
extern struct DefaultNICList 	DefaultNicStu;				/*用于存放设备默认网口信息*/
extern bool 			ReadNicInfo();
extern void 			init_pthread_mutex();
extern struct UseCard 		LastUse;		 		/*用于存放上次使用的网口信息*/
extern bool			GetCurrImage(struct UseCard *curr);
extern void 			AdaptiveImageCtl();


/* fd[0]读端 fd[1]写端 */
int fd[2];							
/* 存放需要实时监测的进程PID的数组 */
int PID_ARR[ProCnt];		
/* 存放各进程pid的数组 */
char *PATH_ARR[ProCnt] = {CAPID,MNPID,UserPID,ARPID,NBPID,AUPID,STPID};

struct ProActionStu ProAction[ProCnt] = 
{
	/* KILL,	重启，		点灯 	,	 进程名称*/
	{	0,		ResetCA , 	CAAbend ,	TipsCache	},/* cache */
	{	-1,		ResetMN ,	MNAbend	,	TipsManager	},/* manager */
	{	-1,		ResetUI ,	UIAbend	,	TipsUser	},/* UI */
	{	-1,		ResetAR ,	ARAbend	,	TipsAlarm	},/* Alarm */
	{	-1,		ResetNB ,	NBAbend	,	TipsNetblock	},/* NetBlock */
	{	-1,		ResetAU ,	AUAbend	,	TipsAudit	},/* Audit */
	{	-1,		ResetST ,	STAbend	,	TipsStat	},/* Stat */
};

/* 存储系统当前最新的镜像口状态 */
struct UseCard	 AdaptiveCurr;

/* 临时变量 */
char exe_cmd[MicroMiniBufferSize];
char tmp_buff[MicroMiniBufferSize];


/* 小板gpio */
UINT8 gpio_num = sizeof(gPort) / sizeof(gPort[0]);

void *QuickGlimpse(void *arg)/*quick for ever*/
{
	Glimpse(&gpio_num,-1,15);
}
void *NormalGlimpse(void *arg)/* normally for ever*/
{
	Glimpse(&gpio_num,-1,15*OnFrequence);
}

void sendKill()
{
   //alarm signal
   kill(PID_ARR[3], SIGUSR1);
   //netblock signal
   kill(PID_ARR[4], SIGUSR1);
   //audit signal
   kill(PID_ARR[5], SIGUSR1);
}

void* cal_CpuAndMem(void*)
{
    CPU_OCCUPY cpu_stat1;
    CPU_OCCUPY cpu_stat2;
    MEM_OCCUPY mem_stat;
    int mem_use=0;
    int cpu_use=0;
    int i=0,j=0;

    sleep(10);
    while(1){
        get_cpuoccupy(&cpu_stat1);
        sleep(10);
        get_cpuoccupy(&cpu_stat2);

        mem_use = get_memoccupy(&mem_stat);
        
        cpu_use = cal_cpuoccupy(&cpu_stat1, &cpu_stat2);


        if(mem_use>=80){
            i+=1;
            dzlog_info("mem_use==%d,the num==%d", mem_use, i);
            if(i >= 10) {
			    dzlog_info("the mem used >=80% more than 10 times\n >>>now send signal to alarm pid:%d, netblock pid:%d, audit pid:%d", PID_ARR[3], PID_ARR[4], PID_ARR[5]);
                sendKill();
				i=0;
            }
        }
        else i=0;

        if(cpu_use >= 8000) {
            j += 1;
            dzlog_info("cpu_use==%d, the num=%d\n", cpu_use, j);
            
            if(j >= 10) {
			    dzlog_info("the cpu used >=80% more than 10 times!!!\n >>>now send signal to alarm pid:%d, netblock pid:%d, audit pid:%d", PID_ARR[3], PID_ARR[4], PID_ARR[5]);
                sendKill();
				j=0;
            }
        }
        else j=0;
    }
}

void ChildWork()
{
#if 1
	printf("I'm ChildWork...\n");
	char buffer[1024] = {0};
#endif

#if 0
	pthread_t id[2] = {0};
	sigset_t n1mask;	
	sigemptyset(&n1mask);	
	sigaddset(&n1mask,SIGUSR1);
	sigset_t n2mask;	
	sigemptyset(&n2mask);	
	sigaddset(&n2mask,SIGUSR2);


	int errnum = 0;
	void *ret = 0;

	/*Initialize Sigsuspend*/
	InitSigsuspend();

	/*Open the thread in advance*/
	pthread_create(&id[0],0,QuickGlimpse,0);
	pthread_create(&id[1],0,NormalGlimpse,0);


	/*FIXME default:suspend checker && Normal thread*/
	suspend(id[0]);		
	suspend(id[1]);		


	//resume(id[1]);
#endif

	while(read(fd[0],buffer,sizeof(buffer)) > 0)/*FIXME For ever Block_Wait*/
	{
		//printf("buffer:%s\n",buffer);
#if 0
		if(strstr(buffer,"CheckerAbend"))
		{
			//printf("+++++++++++++++++++++++++++++\n");
			/*resume the checker's thread*/
			//sleep(1);
			suspend(id[1]);/*suspend Normal*/		
			resume(id[0]);
		}
		if(strstr(buffer,"ManagerAbend"))
		{
			//printf("----------------------------\n");
			suspend(id[0]);		
			suspend(id[1]);		
			CtlLight(&gpio_num,1); /*ON*/
		}

		if(strstr(buffer,"UIAbend"))
		{
			//printf("@@@@@@@@@@@\n");
			suspend(id[0]);		
			suspend(id[1]);		
			CtlLight(&gpio_num,0); /*OFF*/
		}

		if(strstr(buffer,"Normal"))/*Normal*/
		{
			/*resume the Normal's thread*/
			//suspend(id[0]);/*suspend checker*/	
			resume(id[1]);
		}
#endif
    }

	return;
}

void	CatStrEXE(char *sub1,char *sub2)
{
	if(!sub1 || !sub2)	return ;
	char cmd[MicroMiniBufferSize] = {0};
	snprintf(cmd,sizeof(cmd),"%s %s",sub1,sub2);
//	sleep(2);
	system(cmd);
	/*
	if(!strcmp(sub1, "TipsAlarm")) {
	    obj.Set_status_shm(1, "1", sizeof(char));
    }
	if(!strcmp(sub1, "TipsAudit")) {
	    obj.Set_status_shm(2, "1", sizeof(char));
    }
	if(!strcmp(sub1, "TipsNetblock")) {
	    obj.Set_status_shm(4, "1", sizeof(char));
    }
    */

	return ;
}

void 	CheckerUI(char *sh_name)
{
	CatStrEXE(sh_name,AdaptiveCurr.AdaptiveStu.monitor);
	return ;
}

void 	CheckerImage(char *sh_name)
{
	char sh_cmd[MicroMiniBufferSize] = {0};
	switch(AdaptiveCurr.ImageStu.image_mode)
	{
		case SingleImageMode: /*单镜像*/
			CatStrEXE(sh_name,AdaptiveCurr.ImageStu.image_card_one);
			break;
		case AloneImageMode:	/*独立模式*/
			CatStrEXE(sh_name,AdaptiveCurr.ImageStu.image_card_one);
			CatStrEXE(sh_name,AdaptiveCurr.ImageStu.image_card_two);
			break;
		case ProtectImageMode:/*保护模式*/
			CatStrEXE(sh_name,AdaptiveCurr.ImageStu.image_card_one);
			break;
		default:
			//dzlog_info("the imageMode is not single/alone/protect,use the default value:'eth2'");
			CatStrEXE(sh_name,"eth2");
			break;
	}
	return ;
}

/* 启动各检测进程 */
void 	CacheImage()
{
	CheckerImage(ResetAU);
	CheckerImage(ResetAR);
	CheckerImage(ResetNB);
	return ;
}

void WriteShareMem(struct UseCard *use)
{
//	obj.Set_status_shm(Minor_MEM,use->AdaptiveStu.monitor,sizeof(use->AdaptiveStu.monitor));
//	obj.Set_status_shm(Commu_MEM,use->AdaptiveStu.communicate,sizeof(use->AdaptiveStu.communicate));
//	obj.Set_status_shm(Image_Mode_MEM,&(use->ImageStu.image_mode),sizeof(use->ImageStu.image_mode));
//	obj.Set_status_shm(Image_1_MEM,use->ImageStu.image_card_one,sizeof(use->ImageStu.image_card_one));
//	obj.Set_status_shm(Image_2_MEM,use->ImageStu.image_card_two,sizeof(use->ImageStu.image_card_two));

//	dzlog_info("in daemon++++++++++++++use->AdaptiveStu.monitor:%s",use->AdaptiveStu.monitor);
//	dzlog_info("in daemon++++++++++++++use->AdaptiveStu.communicate:%s",use->AdaptiveStu.communicate);
//	dzlog_info("in daemon++++++++++++++use->ImageStu.image_mode:%d", (use->ImageStu.image_mode));
//	dzlog_info("in daemon++++++++++++++use->ImageStu.image_card_one:%s", use->ImageStu.image_card_one);
//	dzlog_info("in daemon++++++++++++++use->ImageStu.image_card_two:%s", use->ImageStu.image_card_two);

	char buf[4][32] = {0};
	memcpy(buf[0], use->AdaptiveStu.monitor, sizeof(buf[0]));
	memcpy(buf[1], use->AdaptiveStu.communicate, sizeof(buf[0]));
	memcpy(buf[2], use->ImageStu.image_card_one, sizeof(buf[0]));
	memcpy(buf[3], use->ImageStu.image_card_two, sizeof(buf[0]));
	for(int i=0;i<4;i++)
	{
	//	printf("in daemon++++++++++++++buf[%d]:%s\n",i, buf[i]);
	}
	obj.Set_status_shm(Minor_MEM,buf[0],sizeof(buf[0]));
	obj.Set_status_shm(Commu_MEM,buf[1],sizeof(buf[1]));
	obj.Set_status_shm(Image_Mode_MEM,&(use->ImageStu.image_mode),sizeof(use->ImageStu.image_mode));
	obj.Set_status_shm(Image_1_MEM,buf[2],sizeof(buf[2]));
	obj.Set_status_shm(Image_2_MEM,buf[3],sizeof(buf[3]));

	return ;
}

/*
 *	判断将要重启的进程
 *	是checker进程	(使用最新的AdaptiveCurr.Image.像口)
 *	还是监控和通信进程
 *	还是cache进程	(使用最新的AdaptiveCurr.Image.像口)
 */
void 	ResetAction(char *sh_name,char *pro_name)
{

	/*获取当前镜像口，填充AdaptiveCurr*/
	if(GetCurrImage(&AdaptiveCurr))
		return ;/*当前可使用的镜像口还没有准备好*/
	WriteShareMem(&AdaptiveCurr);
	/* cache */
	if(JudgeStr(TipsCache,pro_name)) {
		/* 按相应规则依次启动其他进程 */
		system(ResetCA);
		dzlog_info("call cache.sh restart all process");
		
		system(ResetST);
		dzlog_info("call tipsstat.sh restart stat process");
		
		system(ResetMN);
		dzlog_info("call manager.sh restart manager process");
	//	system(ResetUI);/*FIXME user 模块也需要接收ethX*/
		CheckerUI(ResetUI);
		dzlog_info("call user.sh restart UI process");
		CacheImage();
	}
	/* (user)监控口 和 (manager)通信口 */
	else if(JudgeStr(TipsManager,pro_name) || JudgeStr(TipsStat , pro_name)) {
		system(sh_name);
	}
	else if(JudgeStr(TipsUser,pro_name)) {
		CheckerUI(sh_name);
	}
	else {
		//dzlog_info("Checker sh_name:%s\n",sh_name);
		/*按相应规则执行 sh_name*/	
		CheckerImage(sh_name);
	}
	return ;
}

/*	pid:	将要接收SIG信号的进程
 *	SIG:	将要给pid发送的信号
 */
int KillProByPID(pid_t pid,	int SIG)
{
	return kill(pid,SIG);
}
void KillProByName(char *ProName)
{
	if(!ProName) return ;
	if(JudgeStr(TipsCache,ProName)) {
		dzlog_info("kill all the process!!!");
		system(KillALL);
	}
	else {
		char sh_cmd[MicroMiniBufferSize] = {0};
	    snprintf(sh_cmd,sizeof(sh_cmd),"%s %s",KillProName,ProName);
		system(sh_cmd);
		return ;
	}
}

void KillAction(int pid, char* proname)
{
	if(pid==0) return ;
    else if(pid > 0) {
	    KillProByPID(pid,SIGKILL);
	    dzlog_info("process %s pid is %d killed", proname, pid);
    }
    else {
        system(KillALL);
        dzlog_info("kill all process");
    }
}
void 	LEDAction(char *proAbend)
{
	if(proAbend)	write(fd[1],proAbend,strlen(proAbend));
	return ;
}


/* FIXME
 * 建议将各进程pid写入共享内存
 * 轮询式open\read\close
 * 极其浪费资源
 * 已与冯晓亮沟通改善
 * 牵扯到其他模块操作pid
 * 的部分
 * 2014.03.11
 */
int GetPidByPath(char *path)
{
//	printf("path:%s\n",path);
	int fd;
	if(-1 == (fd = open(path,O_RDONLY)))
	{
		dzlog_info("GetPidByPath->Open");
		close(fd);	
		return -1;
	}
	char pid_buff[LargeBuffSize] = {0};
	int pid = 0;
	read(fd,pid_buff,sizeof(pid_buff));
	pid = atoi(pid_buff);
	close(fd);
	return pid;
}

/*填充PID_ARR*/
int	Fill_PID_ARR()
{
	int pid = 0;
	for(int i = 0 ;(i < ProCnt) &&  PATH_ARR[i] ; i++) {
		((pid = GetPidByPath(PATH_ARR[i]))>0) ? PID_ARR[i]	=  pid : PID_ARR[i] = 0;
	}
	return 0;
}

int JudgeEXE(char *proc_pid,char *pro_name)
{
	memset(exe_cmd,0,sizeof(exe_cmd));
	snprintf(exe_cmd,sizeof(exe_cmd),"%s/%s",proc_pid,"cmdline");
	int fd ;
	if(-1 == (fd = open(exe_cmd,O_RDONLY))) {
		dzlog_info("open file failed!!!");
		close(fd);
		return -1;
	}
	memset(tmp_buff,0,sizeof(tmp_buff));
	read(fd,tmp_buff,sizeof(tmp_buff));	
	close(fd);

	if(strstr(tmp_buff,pro_name))
		return 0;
	else return -1;
}
int	JudgePath(char *path)
{
	return access(path,F_OK);
}

void GeneralOpera(pid_t pid , char *ResetName,char* ProName,char *led_abend)
{
	KillAction(pid, ProName);
	KillProByName(ProName);
	ResetAction(ResetName,ProName); 
	LEDAction(led_abend);	
	return ;
}

void MalFunc(int node)
{
	/*kill*/
	KillProByName(ProAction[node].ProName);
	/*reset*/
	ResetAction(ProAction[node].ResetName,ProAction[node].ProName);
	return ;
}

void 	JudgeAlive()
{
	char 	ProcPID[MicroMiniBufferSize] = {0};
	int		ProFlag = 0; 		/*标识是否是cache*/
    char    alarm_status;
    char    netblock_status;
    char    audit_status;

    //obj.Set_status_shm(2, "1", sizeof(char));

    obj.Get_status_shm(1, &alarm_status, sizeof(char));
    obj.Get_status_shm(2, &audit_status, sizeof(char));
    obj.Get_status_shm(4, &netblock_status, sizeof(char));
    
    //dzlog_info(">>>alarm_status==%c, audit_status==%c, netblockstatus==%c", alarm_status, audit_status, netblock_status);

	for(int i = 0;i < ProCnt;i++) {	
		if(PID_ARR[i]>0)
			snprintf(ProcPID,sizeof(ProcPID),"%s%d",ProcPath,PID_ARR[i]) ;
		else {
			MalFunc(i);	
			continue;
		}

		/* FIXME
		 * 当一个进程挂掉，其所有数据将被从内存中清除
		 * 其PID号也将被置为"可分配给新进程"
		 * 某一个检测器进程已经挂掉时，
		 * 但其pid文件记录的pid却被内核分配给了
		 * 其他新进程。所以应该判断其"cmd"是否
		 * 是我们要判断的进程名称,以此增加确定性
		 * (虽然这种可能性暂未遇到 2014.03.11)
		 */
		//判断进程还活着
		if((0 == JudgePath(ProcPID)) && (0 == JudgeEXE(ProcPID,ProAction[i].ProName)))
		{
			continue;	
		}
        else if(i == 3 && alarm_status == '3')
        {
            dzlog_info("the alarm share memory status is 2 and i am not do anyting");
            continue;
        }
        else if(i == 4 && netblock_status == '3')
        {
            dzlog_info("the netblock share memory status is 2 and i am not do anyting");
            continue;
        }
        else if(i == 5 && audit_status == '3')
        {
            dzlog_info("the audit share memory status is 2 and i am not do anyting.");
            continue;
        }
		else 	/*此进程已挂掉*/
		{

			/* FIXME
			 * 区分是否是cache，如果是应该:
			 * 统一重启相关进程后
			 * 立即返回到ParentWork的while中
			 */

			if(JudgeStr(TipsCache,ProAction[i].ProName))
			{
				/*设置模块停掉标识*/
				obj.Set_status_shm(1, "0", sizeof(char));
				obj.Set_status_shm(2, "0", sizeof(char));
				obj.Set_status_shm(3, "0", sizeof(char));
				obj.Set_status_shm(4, "0", sizeof(char));
						
				GeneralOpera(-1,NULL,ProAction[i].ProName,ProAction[i].proAbend);
				/*设置模块启用标识*/
				obj.Set_status_shm(1, "1", sizeof(char));
				obj.Set_status_shm(2, "1", sizeof(char));
				obj.Set_status_shm(3, "1", sizeof(char));
				obj.Set_status_shm(4, "0", sizeof(char));
				break;	
			}
			else /*写入到共享内存，某进程已经失效，并再次重启，再次写入共享内存该进程的状态*/
			{
				if( i == 3 && alarm_status == '2')								/*Alarm*/
					obj.Set_status_shm(1, "0", sizeof(char));
				else if ( i == 4 && netblock_status == '2')							/*Netblock*/
					obj.Set_status_shm(4, "0", sizeof(char));
				else if ( i == 5 && audit_status == '2')							/*Audit*/
					obj.Set_status_shm(2, "0", sizeof(char));
				
				obj.Set_status_shm(3, "0", sizeof(char));		/*涉密标识not implement*/

				GeneralOpera(PID_ARR[i],ProAction[i].ResetName,ProAction[i].ProName,ProAction[i].proAbend);

				if( i == 3 )								/*Alarm*/
					obj.Set_status_shm(1, "1", sizeof(char));
				else if ( i == 4 )							/*Netblock*/
					obj.Set_status_shm(4, "1", sizeof(char));
				else if ( i == 5 )							/*Audit*/
					obj.Set_status_shm(2, "1", sizeof(char));
			}
		}

	}
	return ;
}


void ParentWork()
{
#if 1
	//printf("I'm ParentWork....\n");
	while(1)
	{
		/*填充PID_ARR*/
		if(Fill_PID_ARR()) continue;/*填充失败*/
		/*判断在内存中是否有此进程运行*/
		JudgeAlive();
		usleep(500000);
	}
#endif
	return ;
}


void *DaemonWork(void *arg)
{
#if 1
	/*initial gpio*/
	if(-1 == InitializeGPIO(&gpio_num)) {
		dzlog_info("init gpio is err~!\n");
		return NULL;
	}

	CtlLight(&gpio_num,0); /*OFF*/
#endif

	pid_t pid;	
	pipe(fd);
	if((pid = fork()) < 0) {
		dzlog_info("Fork: error");
		close(fd[0]);	
		close(fd[1]);
		exit(-1);
	}
	else if(0 == pid) {
		close(fd[1]); 
		ChildWork();
	}
	else if(pid > 0) {
		/* parent process close read */
		close(fd[0]);
		ParentWork();
	}
}

void *AdaptiveImageWork(void *arg)
{
	AdaptiveImageCtl();
}


void	InitWork()
{
    char status = '1';
    //zlog init
    if(dzlog_init("/usr/local/tipschecker/etc/log.conf", "daemon")) {
        printf("dzlog init failed\n");
    }
	
	/*初始化共享内存*/
	obj.Init_shm();
    obj.Set_status_shm(1, &status, sizeof(char));
    obj.Set_status_shm(2, &status, sizeof(char));
    obj.Set_status_shm(3, &status, sizeof(char));
    obj.Set_status_shm(4, &status, sizeof(char));


	/*初始化daemon和AdaptiveData之间获取Curr的锁*/
	init_pthread_mutex();

	/*按出厂要求*/
	return ;
}

int main(int argc,char *argv[])
{

	/*读取设备中网卡名称、功能分配对应关系*/
	if(!ReadNicInfo()) {
		dzlog_info("read niceinfo failed!!!");
		return -1;
	}

	InitWork();

    dzlog_info("daemon:start now");

	pthread_t id[3];
	
	if( pthread_create(&id[0], 0, DaemonWork,0) ) {
	    dzlog_info("daemonwork pthread failed!!!");
	    exit(1);
    }

	if( pthread_create(&id[1], 0, AdaptiveImageWork,0) ) {
	    dzlog_info("cal_CPU_MEM pthread failed!!!");
	    exit(1);
    }
	
	if( pthread_create(&id[2], NULL, cal_CpuAndMem, 0) ) {
	    dzlog_info("cpu load test pthread failed!!!");
	    exit(1);
    }

	pthread_join(id[0], NULL);
	pthread_join(id[1], NULL);
	pthread_join(id[2], NULL);
	return 0;
}
