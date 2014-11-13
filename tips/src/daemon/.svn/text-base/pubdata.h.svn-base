/*by yong 2013.10.15*/
#ifndef _PUBDATA_H_
#define _PUBDATA_H_

#define LoopTimes 10 


#define SingleImageMode		1		/*单镜像*/
#define AloneImageMode		2		/*独立模式*/	
#define ProtectImageMode	3		/*保护模式*/	


#define KillALL 			(char *)"./ProAllKill.sh"
#define KillProName 			(char *)"./ProScavenger.sh"
#define ProcPath			(char *)"/proc/"


/* FIXME
 * 随着检测器版本的迭代
 * 各进程的序号和PID_ARR
 * 以及ProAction\PATH_ARR可能都要
 * 改变
 * 但PID_ARR和ProAction以及
 * PATH_ARR中
 * 数据应始终一一对应
 */

#if 0
#define TipsCache			0	
#define TipsManager	  1	
#define TipsUser			2	

#define TipsAlarm			3	
#define TipsNetblock	4	
#define TipsAudit			5	
#endif

/* 各进程名称 */
#define TipsCache			(char *)"tipscache"
#define TipsManager		(char *)"tipsmanager"
#define TipsUser			(char *)"tipsuser"

#define TipsAlarm			(char *)"tipsalarm"
#define TipsNetblock	(char *)"tipsnetblock"
#define TipsAudit			(char *)"tipsauditor"
//event
//状态检测
#define TipsStat			(char *)"tipsstat"

/* 向run灯控制进程传递用 */
#define CAAbend 		(char *)"CAAbend"	/*cache*/
#define MNAbend			(char *)"MNAbend"	/*manager*/
#define UIAbend			(char *)"UIAbend"	/*user*/

#define ARAbend			(char *)"ARAbend"	/*alarm*/
#define NBAbend			(char *)"NBAbend"	/*netblock*/
#define AUAbend			(char *)"AUAbend"	/*audit*/
#define STAbend			(char *)"STAbend"	/*stat*/

#define ResetMN (char *)"source /root/daemon/manager.sh"
//#define ResetCA	(char *)"source /root/daemon/all.sh"	/* cache挂掉时，需要重启其他所有进程 */
#define ResetCA	(char *)"source /root/daemon/cache.sh"	/* cache挂掉时，需要重启其他所有进程 */
#define ResetUI (char *)"source /root/daemon/user.sh"
#define ResetST	(char *)"source /root/daemon/tipsstat.sh"

#define ResetAU (char *)"source /root/daemon/audit.sh"
#define ResetAR (char *)"source /root/daemon/alarm.sh"
#define ResetNB	(char *)"source /root/daemon/netblock.sh"


/* FIXME 关闭相应进程 
 * Kill某一个进程时:
 * 直接传递pid到脚本
 * Kill所有脚本时:
 * 直接调用杀死所有除数据库之外的
 * 有关网监的进程
 */

/* 获取各进程PID适用 */
#define CAPID (char *)"/usr/local/tipschecker/etc/tipscheck/cachepid"
#define MNPID (char *)"/usr/local/tipschecker/etc/tipscheck/mngagentpid"
#define UserPID (char *)"/usr/local/tipschecker/etc/tipscheck/userpid"

#define ARPID (char *)"/usr/local/tipschecker/etc/tipscheck/alarmid"
#define NBPID (char *)"/usr/local/tipschecker/etc/tipscheck/netblockid"
#define AUPID (char *)"/usr/local/tipschecker/etc/tipscheck/auditpid"

#define STPID (char *)"/usr/local/tipschecker/etc/tipscheck/stat_id"


#define ProCnt	7		/* 需要实时监测的进程总数 FIXME 每加一个要检测的进程 此数要加一 */

#define JudgeStr(src,sub)	strstr(src,sub)



#if 0
5#int#1#mirror_mode#镜像口运行模式
48#char#32#monitor_netcard#监控口网卡
49#char#32#connect_netcard#通信口网卡
50#char#32#mirror1_netcard#镜像口1网卡
51#char#32#mirror2_netcard#镜像口2网卡
#endif

#define Minor_MEM				48
#define Commu_MEM 			49
#define Image_Mode_MEM 	5
#define Image_1_MEM 		50
#define Image_2_MEM 		51




/*	FIXME 此规定适用于PID_ARR和ProAction之间的约定
 *	PID_ARR[0] 	Cache		
 *	PID_ARR[1] 	Manager		
 *	PID_ARR[2] 	UI		
 *
 *	PID_ARR[3] 	Alarm
 *	PID_ARR[4] 	NetBlock
 *	PID_ARR[5] 	Audit	
 *	PID_ARR[6]  Stat
 *
 *	PID_AEE[X]	Event 暂无 FIXME X:未知
 */
struct ProActionStu
{
	int 	killflag;
	char *ResetName;
	char *proAbend;
	char *ProName;
};

#endif
