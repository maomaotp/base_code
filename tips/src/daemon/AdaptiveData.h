#ifndef Adaptive_Data_H__
#define Adaptive_Data_H__
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>

#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <sys/select.h>
#include <pthread.h>

#include <iostream>
#include <vector>
#include <algorithm>
#include <list>
using namespace std;

#define NICCONF			(char *)"/usr/local/tipschecker/etc/tipscheck/TipsNIC.cfg" 	/* 需要与网监其他模块如UI等协商位置 */

#define CarrierPath 		(char *)"/sys/class/net"	/* 存放网卡相关实时参数的文件目录 */
#define CarrierName 		(char *)"carrier"

#define RX_Packets 		(char *)"statistics/rx_packets"
#define PacketRise 		5 		/* 与第一次获取某网卡接收到的包个数与第二次获取的相减的差相比较,判断是否接收到包 */


#define ScavengerPath		(char *)"./ProScavenger.sh" 	/* 接收进程模糊名称,负责清除的脚本 */
#define TipsProName		(char *)"tips" 			/* 进程名中包含tips的都将被清道夫杀死,应过滤mysql*/

#define ModePath 		(char *)"/usr/local/tipschecker/etc/ModeByUI" /*存放用户选择的镜像模式*/

#define TIPS_ui			(char *)"tipsuser"
#define TIPS_manager		(char *)"tipsmanager"
#define TIPS_cache		(char *)"tipscache"
#define TIPS_netblock		(char *)"tipsnetblock"
#define TIPS_alarm		(char *)"tipsalarm"
#define TIPS_auditor		(char *)"tipsauditor"

#define E_Monitor 		0
#define E_Communicate		1
#define E_Image_1		2
#define E_Image_2		3
#define O_Monitor		4
#define O_Communicate		5
#define O_Image_1		6
#define O_Image_2		7

#define MicroMiniBufferSize 	256		/*256字节*/
#define MiniBufferSize 		512 		/*512字节*/
#define GeneralBuffSize 	1024 		/*1K*/
#define LargeBuffSize 		1024*1024*4 	/*4M*/

#define AliveEthCnt 		12 		/*假定设备中网口不会超过12个*/
#define EthNameSi 		65 		/*假定网口名称65字节*/

#define GOpen(path) 		open(path,O_RDONLY)
#define GClose(fd)  		close(fd)


#define SingleImageMode		1		/*单镜像*/
#define AloneImageMode		2		/*独立模式*/	
#define ProtectImageMode	3		/*保护模式*/	



struct Adaptive 	/* 光电自适应结构体*/
{
	char monitor[MicroMiniBufferSize];
	char communicate[MicroMiniBufferSize];
};

/* FIXME
 * 当image_mode为3(保护模式)时，
 * image_card_one代表主镜像口
 * image_card_two代表从镜像口
 * */
struct Image 		/* 镜像口结构体 */
{
	int  image_mode;/* 1:单镜像口 2:独立模式 3:保护模式 */
	char image_card_one[MicroMiniBufferSize]; 
	char image_card_two[MicroMiniBufferSize]; /* 当image_mode为1或2时有效 */ 
};

struct UseCard 		/* 网口信息 监控口、通信口、镜像模式、镜像口1、镜像口2 */
{
	struct Adaptive AdaptiveStu;
	struct Image 	ImageStu;
};

struct DefaultNICList 	/* 存放设备默认使用的网卡名称及其对应关系 */
{
	char E_monitor[MicroMiniBufferSize];		
	char E_communicate[MicroMiniBufferSize];
	char E_image_1[MicroMiniBufferSize];
	char E_image_2[MicroMiniBufferSize];

	char O_monitor[MicroMiniBufferSize];		
	char O_communicate[MicroMiniBufferSize];
	char O_image_1[MicroMiniBufferSize];
	char O_image_2[MicroMiniBufferSize];
};

struct Core 		/* 核心数据 */
{
	int 	type; 				/*网卡功能分配,如E_Monitor等*/		
	char 	eth_name[MicroMiniBufferSize];	/*网卡名称*/
	char 	carrier_value;			/*carrier 值*/
};

struct G_Struct		/* 通用的临时结构体 */
{
	struct 	Core Core_data;
	int 	fd;	  			/*carrier_path 文件描述符*/
	char 	carrier_path[MiniBufferSize]; 	/*carrier 完整路径*/
};


struct G_Change		/*用于定义转移表,方便轮询到保护模式时，主从口的切换等*/
{
	struct G_Struct *g_stu;
};
struct G_Other		/* 用于返回同组另一个镜像口 */
{
	int 	type;
	struct	G_Struct *OthStu;
};



void AloneMode();
void ProtectMode();
void GetPacketRise(char *eth_X,int *result);
void D_SingleImage(char *eth_name);
int  JudgeStu(struct G_Struct *G_one,struct G_Struct *G_two);
void JudgeM_S_Image(struct G_Struct *src_one,struct G_Struct *src_two,char *M_image,char *S_image);
bool JudgePacket(char *eth_one,char *eth_two,int *result_packet);
void UpdateStu(struct UseCard *stu, char *Grup_one, char *Grup_two, int ModeFlag, int ImageFlag);
bool CmpCurrupLastuse(char *Grup_one, char *Grup_two, struct UseCard *Use,int ImageFlag, int *result,int ModeFlag);
bool JudgeLastEmpty(struct UseCard *use);

#endif
