

/*FIXME 进度记录： 2014.01.24 317行接收到双镜像模式但只有一个网口满足镜像要求，则启动单镜像!*/
/*FIXME 进度记录： 2014.01.26 292行接收到保护模式　判断是否有包!*/
/*FIXME 进度记录： 2014.02.18 392行判断是否有包!*/
/*FIXME 进度记录： 2014.02.19 416行FindDualImage() */

/*FIXME 进度记录： 
 * 2014.02.20 16:00 
 * (1)进程kill的shell和func未写
 * (2)读共享内存 ReadShareMem(&ModeByUI)) 未交互，未写
 * (3)Deamon模块交互部分未写
 * (4)未联调编译
 * */

/*FIXME 进度记录： 
 * 2014.02.20 19:00 
 * (1)读共享内存 ReadShareMem(&ModeByUI)) 未交互，未写
 * (2)Deamon模块交互部分未写
 * (3)未联调编译
 * */

/*FIXME 进度记录:
 * 2014.02.21	18:00
 * (1)读共享内存 ReadShareMem(&ModeByUI)) 未交互，未写
 * (2)Deamon模块交互部分未写
 * (3)未联调编译
 * (4)光电自适应和双镜像口业务逻辑代码调试进行到683行的
 * communicate口插拔测试
 * */

/*FIXME 进度记录:
 * 2014.02.25	19:40
 * (1)读共享内存 ReadShareMem(&ModeByUI)) 未交互，未写
 * (2)Deamon模块交互部分未写
 * (3)模块自测,轮询protect 时，业务逻辑需要重新设计
 * */

/*FIXME 进度记录:
 * 2014.03.03	10:30
 * (1)读共享内存 ReadShareMem(&ModeByUI)) 未交互，未写
 * (2)Deamon模块交互部分未写
 * */



/* 2014.01.18
 * by yong 
 */

/* FIXME 
 * ****************************************************
 * 一、光电自适应
 * 网口布局
 * 监控1电口 监控2光口 (监控组)
 * 通信1电口 通信2光口 (通信组)
 * 1.判定规则 (从各种情况中选取 1个 网口):
 * [1]两组网口，一组中只会按照光口优先的规则选取一个网口
 * 2.每组中的两个网口都插线 并有数据传输
 * 监控1电口，监控2光口,通信1电口\...(每组两个网口或其中一个组两个口)将会选取监控2光口，通信1电口[+]
 * 3.每组中只有一个网口插线 并有数据传输
 * 监控电口1，通信光口2\...		(每组一个口)		将会直接选取			[@]
 * Waring:2/3中出现的'+'/'@'
 * '@'代表如果按照判定规则进行选取，选取的网口将会与实际使用的网口一致.
 * '+'代表如果按照判定规则进行选取，但实际情况是'+'中的描述将会出现选取网口与用户实际使用的网口不一致!
 *
 * ****************************************************
 * 网口布局:
 * 镜像1电口 镜像1光口 (镜像1组)
 * 镜像2电口 镜像2光口 (镜像2组)
 * 一、单镜像口(当未接收到界面传来的双镜像模式 且 在轮询网口之前也未使用双镜像模式时)
 * 与光电自适应情况一致
 * 二、双镜像口
 * (一)独立模式(当接收到界面由用户选择的双镜像模式为独立模式):
 * 1.判定规则 (从各种情况中选取 2个 网口):
 * [1]两组网口，一组中只会按照光口优先的规则选取一个网口
 * 2.两组镜像口中出现 [3个及以上] 状态为UP 并有数据传输 
 * [1]电口1，电口2，光口1		(两电口一光口) 		将会选取光口1，电口2 		[+]
 * [2]电口1，光口1，光口2		(两光口一电口) 		将会选取光口1，光口2 		[+]
 * [3]电口1，电口2，光口1，光口2	(两电口两光口) 		将会选取光口1，光口2 		[+]
 * 3.两组镜像口中出现 [1个或2个] 状态为UP 并有数据传输
 * [1]电口1 / 电口2 ／ 光口1 ／ 光口2	(一个镜像口)   		将会选取其中一个转为单镜像
 * [2]电口1，光口1 / 电口2,光口2  	(同组合内的一电口一光口)将会只选取组合内的一个，即光口 	[+]
 * [3]电口1，电口2 ／ 光口1，光口2 / 交叉... （两电口 或 两光口 或 交叉使用）将直接选取两个口 	[@]
 * Waring:2/3中出现的'+'/'@'
 * '@'代表如果按照判定规则进行选取，选取的网口将会与实际使用的网口一致.
 * '+'代表如果按照判定规则进行选取，但实际情况出现'+'中的描述将会出现选取网口与用户实际使用的网口不一致!
 * (二)保护模式(当接收到界面由用户选择的双镜像模式为保护模式)：
 * 1.判定规则:
 * 在独立模式的判定规则基础上增加流量验证，只有选取的网口在一定时间范围内流量达到一定阈值才能最终被使用；
 * ******************************************************
 */


/******************************************************************
 * FIXME /sys/class/net/ethX/carrier
 * (1)系统识别了的网卡，都会在/sys/class/net有其对应的参数文件
 * (2)当ethX状态为UP时(ifconfig ethX up),carrier可读,此时为1或0
 * 其中:
 * 1:暂且称为"网卡握手成功"(有数据包或有连接)
 * 0:暂且称为"网卡握手失败"(无数据包或无连接)
 * (3)当ethX状态为down时(ifconfig ethX down),carrier不可读但可以打开
 * ****************************************************************/


#include "AdaptiveData.h"
#include "GetNicInfo.h"
#include "zlog.h"

struct G_Struct E_MonitorStu;
struct G_Struct E_CommunicateStu;
struct G_Struct E_Image_1Stu;
struct G_Struct E_Image_2Stu;

struct G_Struct O_MonitorStu;
struct G_Struct	O_CommunicateStu;
struct G_Struct O_Image_1Stu;
struct G_Struct O_Image_2Stu;


/* 轮询保护模式时，使用其中的type，确定镜像组和类型 */
struct G_Change G_Stu_Array[] = {{&E_MonitorStu},{&E_CommunicateStu},{&E_Image_1Stu},{&E_Image_2Stu},
	{&O_MonitorStu},{&O_CommunicateStu},{&O_Image_1Stu},{&O_Image_2Stu},
	{NULL}
};

/* 返回同组的另一个 G_Struct */
struct G_Other	G_Oth_Array[] = {{E_Image_1,&O_Image_2Stu},{O_Image_1,&E_Image_1Stu},
	{E_Image_2,&O_Image_2Stu},{O_Image_2,&E_Image_2Stu},
	{-1,NULL}
};


struct G_Struct Image_one_stu; 		/*镜像处理用*/
struct G_Struct Image_two_stu;

struct UseCard 	CurrUse;		/*用于存放当前使用的网口信息*/
struct UseCard 	LastUse;		/*用于存放上次使用的网口信息*/
pthread_mutex_t curr_mutex;		/*"(检测进程)保活线程"用*/

struct DefaultNICList DefaultNicStu; 	/*用于存放设备默认网口信息*/

/* 写文件的话可用 1代表独立模式 2代表保护模式 */
/* FIXME 低1位代表独立模式 低2位代表保护模式 可扩展 */ 
u_int8_t ModeByUI;			/*用于存放ui放到内存池的镜像模式*/ 


FILE * RX_FP; 				/* 用于存放打开网卡接收到数据包个数的文件指针 */




/* 用于拷贝和清零CurrUse或其中的某一成员
 * 两种操作的简单封装
 * flag为1 代表memcpy
 * flag为0 代表memset
 */
void CpySetCurrUse(void *src_Curr,void *dst_unknow,int dst_len,int flag)
{
	pthread_mutex_lock(&curr_mutex);
	flag ? memcpy(src_Curr,dst_unknow,dst_len) : memset(src_Curr, 0 ,dst_len);
	pthread_mutex_unlock(&curr_mutex);
	return ;
}

void UpdateCurrUse(struct UseCard *CurrUse, char *Grup_one, char *Grup_two, int  ModeFlag, int ImageFlag)
{
	pthread_mutex_lock(&curr_mutex);
	UpdateStu(CurrUse, Grup_one, Grup_two,  ModeFlag, ImageFlag);	
	pthread_mutex_unlock(&curr_mutex);
	return ;
}

/* 用于Daemon进程每次检测各checker使用的
 * 当前网口与最新的网口(CurrUse)
 * 进行对比
 * result 记录比对结果
 */
void CmpUseByDaemon(char *Grup_one, char *Grup_two, int ImageFlag, int *result,int ModeFlag)
{
	pthread_mutex_lock(&curr_mutex);
	CmpCurrupLastuse(Grup_one, Grup_two, &CurrUse, ImageFlag, result, 1);
	pthread_mutex_unlock(&curr_mutex);
	return ;
}

/* 获取当前系统的镜像口情况
 * 将结果放入curr
 */
bool 	GetCurrImage(struct UseCard *curr)
{
	CpySetCurrUse(curr,&CurrUse,sizeof(struct UseCard),1);

//	printf("\n\n\n\n++++++++++++++use->AdaptiveStu.monitor:%s\n",curr->AdaptiveStu.monitor);
//	printf("++++++++++++++use->AdaptiveStu.communicate:%s\n",curr->AdaptiveStu.communicate);
//	printf("++++++++++++++use->ImageStu.image_mode:%d\n", (curr->ImageStu.image_mode));
//	printf("++++++++++++++use->ImageStu.image_card_one:%s\n", curr->ImageStu.image_card_one);
//	printf("++++++++++++++use->ImageStu.image_card_two:%s\n", curr->ImageStu.image_card_two);


	return JudgeLastEmpty(curr);
}


void DealCR(char *buffer)
{
	if('\n' == buffer[strlen(buffer) - 1])
		buffer[strlen(buffer) - 1] = '\0';
	return ;
}


/*********************************************
 * 判断"上次使用网口"是否为空
 * false:不为空
 * true :空
 * 为保证是绝对的首次上电，除判断monitor 和 
 * communicate外，还判断镜像模式和镜像1、镜像2
 *********************************************/
bool JudgeLastEmpty(struct UseCard *use)
{
	if((strlen(use->AdaptiveStu.monitor) <=0) && (strlen(use->AdaptiveStu.communicate) <= 0)
			&& (use->ImageStu.image_mode <= 0) && (strlen(use->ImageStu.image_card_one)<=0) 
			&& (strlen(use->ImageStu.image_card_two) <= 0))
		return true;
	return false;
}


/*********************************************
 * FIXME 按接收的关键字kill系统中的进程
 * 会造成进程及其数据被终止，慎用！
 ********************************************/
#include <sys/wait.h>
extern int KillProByPID(pid_t pid,	int SIG);


void ProScavenger(char *ProName)
{
//	printf("I'm ProScavenger....\n");
	char shellcmd[LargeBuffSize] = {0};

/* FIXME 
 * 给每一个相关进程发送SIGKILL
 * */
#if 0
	snprintf(shellcmd,sizeof(shellcmd),"ps -ef | grep -v 'mysql' | grep -v 'grep' | grep -v 'daemon' | grep '%s' | awk  '{print $2}'",ProName);
	printf("shellcmd:%s\n",shellcmd);
	
	FILE *fp = popen(shellcmd,"r");
	if(!fp)
	{
		pclose(fp);
		fp = 0;
		return ;
	}
	char buffer[LargeBuffSize] = {0};
	int tmp_pid = 0;
	while(fgets(buffer,sizeof(buffer),fp))
	{
		DealCR(buffer);
		tmp_pid = atoi(buffer);
		KillProByPID(tmp_pid,SIGKILL);
	}
	pclose(fp);
	fp = 0;
	return ;
#endif

//	printf("ProName:%s\n",ProName);
	/* 调用Scavenger脚本完成 进程清理 */
	ProName?snprintf(shellcmd,sizeof(shellcmd),"%s %s",ScavengerPath,ProName),system(shellcmd):printf("The Proname is null pointer!");
	return ;
}


/*********************************************
 * 更新CurrUse\LastUse
 * 当ModeFlag为AdaptiveUpdate时，Grup_one代表minor,Grup_two代表communicate
 * 当ModeFlag为ImageUpdate时，Grup_one代表Image1,Grup_two代表Image2 
 * ModeFlag: 0:AdaptiveUpdate 1:ImageUpdate
 * ImageFlag:1:单镜像 2:独立模式 3:保护模式 (ModeFlag 为 ImageUpdate时有效)
 * 保护模式下的image_card_one永远为主镜像口，image_card_two永远为从镜像口
 * UseFlag:  0:CurrUse1:LastUse
 ********************************************/
void EasyCopy(void *dst,int dst_len,void *src,int src_len )
{
//	printf("dst:%s,src:%s\n",dst,src);
	if(!dst || !src)
		return;
	memset(dst,0,dst_len);
	memcpy(dst,src,src_len);
	return ;
}
void UpdateStu(struct UseCard *stu, char *Grup_one, char *Grup_two, int ModeFlag, int ImageFlag)
{
	//printf("UpdateStu->grup_one:%s,grup_two:%s\n",Grup_one,Grup_two);
	//printf("ModeFlag:%d\n",ModeFlag);
	if(0 == ModeFlag)						/*AdaptiveUpdate*/	
	{
		if(Grup_one)
			EasyCopy(stu->AdaptiveStu.monitor,MicroMiniBufferSize,Grup_one,strlen(Grup_one) + 1);
	//	printf("stu->AdaptiveStu.monitor:%s\n",stu->AdaptiveStu.monitor);	
		if(Grup_two)
			EasyCopy(stu->AdaptiveStu.communicate,MicroMiniBufferSize,Grup_two,strlen(Grup_two) + 1);
	//	printf("stu->AdaptiveStu.communicate:%s\n",stu->AdaptiveStu.communicate);	
	}
	else if((1 == ModeFlag) && (-1 != ImageFlag))  			/*ImageUpdate*/
	{
		stu->ImageStu.image_mode = ImageFlag;
		//printf("(1)LastUse.image_mode:%d\n",LastUse.ImageStu.image_mode);
		//printf("(1)CurrUse.image_mode:%d\n",CurrUse.ImageStu.image_mode);
		if(Grup_one)
		{
			EasyCopy(stu->ImageStu.image_card_one,MicroMiniBufferSize,Grup_one,strlen(Grup_one) + 1);
			memset(stu->ImageStu.image_card_two,0,MicroMiniBufferSize); /*单镜像下image_two应为空*/
		}
		if(1 != ImageFlag && Grup_two)	/* 不是单镜像 */
			EasyCopy(stu->ImageStu.image_card_two,MicroMiniBufferSize,Grup_two,strlen(Grup_two) + 1);
//		printf("(2)LastUse.image_card_one:%s\n",LastUse.ImageStu.image_card_one);
//		printf("(2)CurrUse.image_card_two:%s\n",CurrUse.ImageStu.image_card_two);
	}
	return ;
}

void UpdateUse(char *Grup_one, char *Grup_two, int ModeFlag, int ImageFlag, int UseFlag)
{
	//printf("UpdateUse->grup_one:%s,grup_two:%s\n",Grup_one,Grup_two);
	if(0 == UseFlag)      /*CurrUse*/	
	{
		/* FIXME 获取CurrUse需先获取锁 */
		UpdateCurrUse(&CurrUse, Grup_one, Grup_two,  ModeFlag, ImageFlag);	
	}
	else if(1 == UseFlag) /*LastUse*/
		UpdateStu(&LastUse, Grup_one, Grup_two,  ModeFlag, ImageFlag);	
	return ;
}


/* ***********************************************
 * 判断当前选取的网口名称是否与"上次使用的网口"一致
 * 当ModeFlag为AdaptiveUpdate时，Grup_one代表minor,Grup_two代表communicate
 * 当ModeFlag为ImageUpdate时，Grup_one代表Image1,Grup_two代表Image2 
 * 一致返回true 不一致返回false
 * ModeFlag: 0:Adaptive 1:Image
 * result: [0]one [1]two [2]image_mode
 * ImageFlag:1:单镜像口 2:独立模式 3:保护模式
 * ***********************************************/

bool CmpCurrupLastuse(char *Grup_one, char *Grup_two, struct UseCard *Use,int ImageFlag, int *result,int ModeFlag)
{
#if 0
	printf("ImageFlag:<%d> -- LastUse.ImageStu.image_mode<%d>\n",ImageFlag,LastUse.ImageStu.image_mode);
	printf("Grup_one:<%s>LastUse.ImageStu.image_card_one:<%s>\n",Grup_one,LastUse.ImageStu.image_card_one);
	printf("Grup_two:<%s>LastUse.ImageStu.image_card_two:<%s>\n",Grup_two,LastUse.ImageStu.image_card_two);	
#endif
	/*result [0]one [1]two [2]image_mode*/
	if(0 == ModeFlag)  	/*Adaptive*/
	{
		if((Grup_one && !strcmp(Grup_one,Use->AdaptiveStu.monitor))
				|| (!Grup_one && strlen(Use->AdaptiveStu.monitor)<=0))		
			result[0] = 1;
		if((Grup_two && !strcmp(Grup_two,Use->AdaptiveStu.communicate))	
				|| (!Grup_two && strlen(Use->AdaptiveStu.communicate)<=0))		
			result[1] = 1;
		//printf("result[0]:%d result[1]:%d\n",result[0],result[1]);
		if(result[0] && result[1]) return true;
		else return false;
	}
	else if(1 == ModeFlag)  /*Image*/
	{
		if(ImageFlag == Use->ImageStu.image_mode) 			result[2] = 1;
		if((Grup_one && !strcmp(Grup_one,Use->ImageStu.image_card_one))	
				|| (!Grup_one && strlen(Use->ImageStu.image_card_one)<=0))
			result[0] = 1;
		if((Grup_two && !strcmp(Grup_two,Use->ImageStu.image_card_two))
				|| (!Grup_two && strlen(Use->ImageStu.image_card_two)<=0))		
			result[1] = 1;
		if(result[0] && result[1] && result[2]) return true;
		else return false;
	}
	return true;
}

/* KILL所有检测进程 */
void KillAllChecker()
{
	ProScavenger(TIPS_alarm);								
	ProScavenger(TIPS_auditor);								
	ProScavenger(TIPS_netblock);								
	//FIXME ProScavenger(TIPS_event);/*截止2014.02.20 无event进程*/					
}



/****************************************
 *  以下 为镜像 相关函数
 *  FIXME 仅判断、操作 镜像口及其相关进程
 * **************************************/
#if 1

void OpenSingleImage(char *eth_name)
{
	printf("OpenSingleImage ...\n");
	UpdateUse(eth_name,NULL,1,SingleImageMode,0);	/*仅更新image_one*/
	UpdateUse(eth_name,NULL,1,SingleImageMode,1);	/*仅更新image_one*/
	KillAllChecker();

	return ;
}


void FirstBoot(void OperSinglefunc(char *eth_name))
{
	//printf("Image_one_stu.Core_data.eth_name:%s\n",Image_one_stu.Core_data.eth_name);
	//printf("Image_two_stu.Core_data.eth_name:%s\n",Image_two_stu.Core_data.eth_name);
	/* 判断Image_one_stu 和 Image_two_stu哪个有包
	 * 如果两个都有包，优先使用Image_one_stu
	 * 开启单镜像模式
	 */
	int packet_ret_1 = 0,packet_ret_2 = 0;

	GetPacketRise(Image_one_stu.Core_data.eth_name,&packet_ret_1);
	GetPacketRise(Image_two_stu.Core_data.eth_name,&packet_ret_2);

//	printf("packet_ret_1:%d packet_ret_2:%d\n",packet_ret_1,packet_ret_2);	


	/* FIXME 
	 * 当选择的两个镜像口都无数据包时
	 * 将当前和上次使用的镜像口表删除,
	 * Kill所有检测进程
	 * 出现这种情况说明镜像口无数据包,
	 * 所以不起任何进程!
	 * */
	if(!packet_ret_1 && !packet_ret_2)
	{

		/*FIXME 仅清理ImageStu,属于更新*/
		/* FIXME 获取CurrUse需先获取锁 */
		CpySetCurrUse(&(CurrUse.ImageStu),0,sizeof(CurrUse.ImageStu),0);
		memset(&(LastUse.ImageStu),0,sizeof(LastUse.ImageStu));
		KillAllChecker();
	}


	(packet_ret_1 >= packet_ret_2) 
		? (packet_ret_1 ? OperSinglefunc(Image_one_stu.Core_data.eth_name) :(void)NULL) 
		: (packet_ret_2 ? OperSinglefunc(Image_two_stu.Core_data.eth_name) :(void)NULL) ;

	return ;
}

void R_SingleImage()
{
	//printf("I'm R_SingleImage...\n");
	/* 判断Image_one_stu 和 Image_two_stu哪个有包
	 * 如果两个都有包，优先使用Image_one_stu
	 * 开启单镜像模式
	 */
	FirstBoot(D_SingleImage);

	return ;
}

/* 仅用于保护模式下的LastUse和CurrUse更新 */
void ProtectUpdateUse(char *M_image,char *S_image)
{
	/* Image,保护模式,CurrUse */
	UpdateUse(M_image,S_image,1,ProtectImageMode,0);	/*仅更新镜像口*/
	/* Image,保护模式,LastUse */
	UpdateUse(M_image,S_image,1,ProtectImageMode,1);	/*仅更新镜像口*/
	KillAllChecker();
	return ;
}


/* 确定Last使用的镜像口是哪一组、是电口还是光口*/
int Confirm_E_O(char *last_eth)
{
	for(int i = 0;G_Stu_Array[i].g_stu;i++)
	{
		if(!strcmp(G_Stu_Array[i].g_stu->Core_data.eth_name,last_eth))  
			return (G_Stu_Array[i].g_stu->Core_data.type); 
		continue;
	}
	return -1;
}

/* 返回同组另一个镜像口 */
struct G_Struct * GetOtherStu(int type)
{
	for(int i = 0;G_Oth_Array[i].type != -1;i++)
	{
		if(type == G_Oth_Array[i].type)
			return G_Oth_Array[i].OthStu;
		continue;
	}
	return NULL;
}


/* 更新从口 */
void Update_S_Image(char *eth)
{
	int eth_len = strlen(eth);
	if(eth_len > 1)
	{
		memcpy(LastUse.ImageStu.image_card_two,eth,eth_len);
		/* FIXME 获取CurrUse需先获取锁 */
		CpySetCurrUse(CurrUse.ImageStu.image_card_two,eth,eth_len,1);
	}
	return ;
}


/* 轮询到保护模式时 
 * (1)尽量不切换镜像口，避免丢包
 * (2)当检测到上次使用主口无数据包时，
 * 优先使用与上次使用主口同组的另一个
 * 镜像口，但当同组的另一个镜像口也无
 * 数据包时，再考虑另一个组的镜像口,
 * 即使用从口；
 * (3)
 *
 * */
void R_Protect()
{
//	printf("I'm R_Protect ....\n");
	struct G_Struct *tmp_stu = 0;

	int image_type = 0;

	char M_image[MicroMiniBufferSize] = {0};	/*主口*/
	char S_image[MicroMiniBufferSize] = {0};	/*从口*/	
	/* 验证两个主从口是否都有包 */
	int result = 0;
	int result_packet[2] = {0}; 			/*存放last使用的镜像口有无数据包的结果*/
	int new_result_packet[2] = {0}; 		/*存放新选取的镜像口有无数据的结果*/

	JudgeM_S_Image(&Image_one_stu,&Image_two_stu,M_image,S_image);
	JudgePacket(M_image,S_image,new_result_packet);

	int M_Flag , S_Flag; 		/* 为1代表不相同，0代表相同 */
	(!strcmp(M_image,LastUse.ImageStu.image_card_one)) ? M_Flag = 0 : M_Flag = 1;
	(!strcmp(S_image,LastUse.ImageStu.image_card_two)) ? S_Flag = 0	: S_Flag = 1;

	if(!M_Flag && !S_Flag) 		/* 新选取的与上次使用的一致 */
		return ;

	if(JudgePacket(LastUse.ImageStu.image_card_one,LastUse.ImageStu.image_card_two,result_packet))	/*都有包*/
	{
		/* 不考虑新选取的镜像口的优先级是否高低，
		 * 只要上次使用的主\从口有包，就延续之前使用
		 * 的主\从口 */	
		return ;	
	}
	else	/* 只有其中一个或两个都无包,此时涉及切换主从镜像口问题(更新主从镜像口) */
	{
		/* 重新判定选取主从口，并判定主从关系 */
		/* 修正主从关系 */


		if(!result_packet[0] && result_packet[1]) /*image_card_one(上次使用的主口)无包，image_card_two(上次使用的从口)有包*/
		{
			/* 判断last镜像口类型,获得同组其他结构体 */
			if(-1 != (image_type = Confirm_E_O(LastUse.ImageStu.image_card_one)))  
				tmp_stu = GetOtherStu(image_type); 

			/*判断选取的网口有没有包*/	
			GetPacketRise(tmp_stu->Core_data.eth_name,&result);
			if(result) 	/*有包，直接将其作为主口*/
			{
				memcpy(M_image,tmp_stu->Core_data.eth_name,strlen(tmp_stu->Core_data.eth_name) + 1);
				memcpy(S_image,LastUse.ImageStu.image_card_two,strlen(LastUse.ImageStu.image_card_two));
			}
			else 		/*没有,使用从口作为主口,查找新的从口*/
			{
				memcpy(M_image,LastUse.ImageStu.image_card_two,strlen(LastUse.ImageStu.image_card_two));
				/* FIXME 
				 * 上次使用的主口无包，与主口同组的镜像口也无包
				 * 此时从口为空
				 * */
				memset(S_image,0,sizeof(S_image));
				memset(LastUse.ImageStu.image_card_two,0,MicroMiniBufferSize);
			}

			ProtectUpdateUse(M_image,S_image);	
		}
		else if(!result_packet[1] && result_packet[0])/*image_card_two(上次使用的从口无包),主口有包*/
		{

			/* 判断last从镜像口类型,获得同组其他结构体 */
			if(-1 != (image_type = Confirm_E_O(LastUse.ImageStu.image_card_two)))  
				tmp_stu = GetOtherStu(image_type); 

			/*验证从口一组的另一个网口是否有包*/
			GetPacketRise(tmp_stu->Core_data.eth_name,&result);
			if(result)	/*有包*/	
			{
				memcpy(S_image,tmp_stu->Core_data.eth_name,strlen(tmp_stu->Core_data.eth_name) + 1);
			}
			else  		/*无包,从口为空*/
			{
				memset(S_image,0,sizeof(S_image));
				memset(LastUse.ImageStu.image_card_two,0,MicroMiniBufferSize);
			}
			/*FIXME 仅更新一下Last和Curr使用的从口,不Kill任何进程*/
			Update_S_Image(S_image);
		}
		else 	/*两者都无包,直接更新last,curr*/
		{
			ProtectUpdateUse(M_image,S_image);
		}
	}
	return ;
}

void R_DoubleImage()
{
	//printf("I'm R_DoubleImage...\n");
	if(AloneImageMode == LastUse.ImageStu.image_mode)		/* 独立镜像 */	
		AloneMode();
	else if(ProtectImageMode == LastUse.ImageStu.image_mode)	/* 保护镜像 */	
		R_Protect();
	return ;
}


void Routines()
{
	//printf("I'm Routines...\n");
	//printf("LastUse.ImageStu.image_mode:%d\n",LastUse.ImageStu.image_mode);	
	if(0 == LastUse.ImageStu.image_mode)		/* 首次上电或上次使用时无镜像口工作 */	
	{
		//printf("I'm FirstBoot...\n");
		FirstBoot(OpenSingleImage);		/* 按照出厂规则，开单镜像模式，防止检测器空闲*/
	}
	else if(SingleImageMode == LastUse.ImageStu.image_mode)	/* 单镜像 */	
		R_SingleImage();
	else R_DoubleImage();				/* 双镜像 */
	return ;
}

void GeneralDeal() /*用户未选择独立模式或保护模式*/
{
	//printf("I'm GeneralDeal...\n");
	Routines();					/* 正常轮询 */
	return ;
}

/***************************************************
 * 独立模式当检测到任意一个镜像口有变化时，将会将所有
 * 检测进程kill掉 ,更新CurrUse\LastUse
 * 与光电自适应单独判断、kill监控口和通信口不一样
 ****************************************************/
void AloneMode()
{
	//printf("I'm Alone...\n");
	/**/	
	int result[3] = {0}; /*[0]one [1]two [2]image_mode*/
	if(CmpCurrupLastuse(Image_one_stu.Core_data.eth_name,Image_two_stu.Core_data.eth_name, &LastUse ,AloneImageMode,result,1))
		return ; /*一致 不做任何操作*/
	else  /*不一致*/
	{
		//printf("不一样...\n");
		/* FIXME one和two只有一个不为NULL时，会出现独立模式的单镜像口现象，
		 * 即处于独立模式但每个检测进程只有一个实例在工作
		 */
		/* Image,独立模式,CurrUse*/
		UpdateUse(Image_one_stu.Core_data.eth_name,Image_two_stu.Core_data.eth_name,1,AloneImageMode,0);			/*仅更新image_one,image_two*/
		/* Image,独立模式,LastUse*/
		UpdateUse(Image_one_stu.Core_data.eth_name,Image_two_stu.Core_data.eth_name,1,AloneImageMode,1);			/*仅更新image_one,image_two*/
		KillAllChecker();
	}
	return ;
}

void GetPacketCnt(char *cmd,char *PacketCntBuff,int BuffSize)
{
	RX_FP ? RX_FP = 0 : NULL;
	(RX_FP  = popen(cmd,"r")) ? fgets(PacketCntBuff,BuffSize,RX_FP),fclose(RX_FP) : NULL;
	DealCR(PacketCntBuff);
	return ;
}

void GetPacketRise(char *eth_X,int *result)
{
	/* 过滤未使用的镜像口 */
	if(strlen(eth_X) < 1)
		return ;

	char PacketCntBuff[1024] = {0};
	char RX[1024] = {0};
	int  FirstCnt = 0;	
	int  SecondCnt = 0;	

	snprintf(RX,sizeof(RX),"cat %s/%s/%s",CarrierPath,eth_X,RX_Packets);
	/* The eth_X first get packet count */
	GetPacketCnt(RX,PacketCntBuff,sizeof(PacketCntBuff));
	FirstCnt = atoi(PacketCntBuff);
	sleep(1);
	/* The eth_X second get packet count */
	GetPacketCnt(RX,PacketCntBuff,sizeof(PacketCntBuff));
	SecondCnt = atoi(PacketCntBuff);

	//printf("FirstCnt:<%d>,SecondCnt:<%d>\n",FirstCnt,SecondCnt);
	(SecondCnt - FirstCnt >= PacketRise) ? *result = 1 /*有包*/ : *result = 0 /* 无包 */ ;
	return ;
}


bool JudgePacket(char *eth_one,char *eth_two,int *result_packet)
{

	GetPacketRise(eth_one,&result_packet[0]);
	GetPacketRise(eth_two,&result_packet[1]);

	if(result_packet[0] && result_packet[1])
		return  true;

	return false;
}


void GetOtherImage(struct G_Struct *Image_Stu_1,struct G_Struct *Image_Stu_2,int *SingleImageFlag,struct G_Struct *TmpStu)
{
	int  ret = 0;
	if(0 == (ret = (JudgeStu(Image_Stu_1,Image_Stu_2))))	/* 两个网卡的carrier都为0,未连线,开启单镜像模式 */
		*SingleImageFlag = 1;
	else switch(ret)
	{
		case 2:
			memcpy(TmpStu,Image_Stu_2,sizeof(struct G_Struct));
			break;
		case 1:
			memcpy(TmpStu,Image_Stu_1,sizeof(struct G_Struct));
			break;
	}
	return ;
}

/* 判定主从口并将判定结果存入M_image,S_image */
void JudgeM_S_Image(struct G_Struct *src_one,struct G_Struct *src_two,char *M_image,char *S_image)
{
	/* 判断one和two是光口还是电口，光口优先使用;如果都是电口或光口，优先使用镜像1组中的镜像口 */

	/* (1)one_stu是光纤口，two_stu是电口 或 两者同为光纤口或电口
	 * 优先选取镜像1组中的口 
	 */
	if(((O_Monitor == src_one->Core_data.type) && (E_Monitor == src_two->Core_data.type))
			|| (((O_Monitor == src_two->Core_data.type) && (O_Monitor == src_one->Core_data.type)) || ((E_Monitor == src_two->Core_data.type) && (E_Monitor == src_one->Core_data.type))))	
	{
		memcpy(M_image,src_one->Core_data.eth_name,strlen(src_one->Core_data.eth_name));
		memcpy(S_image,src_two->Core_data.eth_name,strlen(src_two->Core_data.eth_name));
	}
	/*(2)one_stu是电口，two_stu是光纤口,选取two_stu*/
	else if((O_Monitor == Image_two_stu.Core_data.type) && (E_Monitor == Image_one_stu.Core_data.type))
	{
		memcpy(M_image,src_two->Core_data.eth_name,strlen(src_two->Core_data.eth_name));
		memcpy(S_image,src_one->Core_data.eth_name,strlen(src_one->Core_data.eth_name));
	}
	return ;
}

void FindOtherImage(struct G_Struct *G_stu)
{
	//printf("FindOtherImage->G_stu->Core_data.eth_name:%s\n",G_stu->Core_data.eth_name);
	int  	SingleImageFlag = 0; 
	int	result_packet = 0;
	int 	result	      = 0;
	char 	M_image[1024] = {0};
	char 	S_image[1024] = {0};
	struct  G_Struct TmpStu;

	/* FIXME 判断carrier是否为1,确定另一个镜像口 */
	if((E_Image_1 == G_stu->Core_data.type) || (O_Image_1 == G_stu->Core_data.type)) /*镜像１组中有包，应从镜像２组中查找一个有包的网口*/
	{
		GetOtherImage(&E_Image_2Stu,&O_Image_2Stu,&SingleImageFlag,&TmpStu);
	}
	else if((E_Image_2 == G_stu->Core_data.type) || (O_Image_2 == G_stu->Core_data.type)) /*镜像2组中有包，应从镜像1组中查找一个有包的网口*/
	{
		GetOtherImage(&E_Image_1Stu,&O_Image_2Stu,&SingleImageFlag,&TmpStu);
	}
	//printf("SingleImageFlag:%d\n",SingleImageFlag);
	if((1 == SingleImageFlag)) 	/* 开启单镜像模式 */
	{
		D_SingleImage(G_stu->Core_data.eth_name);
		return ;
	}


	/* 判断是否接收到数据包 */
	GetPacketRise(TmpStu.Core_data.eth_name,&result_packet);

//	printf("TmpStu.Core_data.eth_name:%s\n",TmpStu.Core_data.eth_name);
//	printf("result:%d\n",result);
	if(!result) 
	{
		/* 无包且与上次不一致,开启单镜像模式 */
		/*调用D_SingleImage()*/
		D_SingleImage(G_stu->Core_data.eth_name);
		return ;
	}
	else 	
	{
		/* 有包 */
		/* 确定主从口 */
		JudgeM_S_Image(G_stu,&TmpStu,M_image,S_image);

		if(!CmpCurrupLastuse(M_image,S_image,&LastUse,ProtectImageMode,&result,1))		/* 有包且与上次使用的不一致 */
		{
			/* Image,保护模式,CurrUse */
			UpdateUse(M_image,S_image,1,ProtectImageMode,0);
			/* Image,保护模式,LastUse */
			UpdateUse(M_image,S_image,1,ProtectImageMode,1);
			KillAllChecker();
		}
	}	
	return ;
}

/* FIXME 从两个镜像组中分别找一个carrier为1且有包的网口 
 * 传入的G_stu_1和G_stu_2的carrier不为1或无数据包
 * 2014.02.20
 */
void FindDualImage()
{

	return ;
}

/* 保护模式 */
void ProtectMode()
{
	//printf("I'm Protect...\n");
	int result[3] = {0}; /*[0]one [1]two [2]image_mode(此处没用)*/
	if(CmpCurrupLastuse(Image_one_stu.Core_data.eth_name,Image_two_stu.Core_data.eth_name,&LastUse,ProtectImageMode,result,1))
		return ;	/* 当前插网线的口和上次使用的口一致且镜像模式也一致,不做任何处理 */
	else
	{
	//	printf("ProtectImageMode  不一致...\n");
		/* 接收到UI的镜像模式为保护模式后
		 * 且当前插网线的口与上次使用的不
		 * 一样，判断是否有数据包，carrier为1但
		 * 不一定代表有数据包
		 * 一致或镜像模式不一致时:
		 * (1)从当前插网线的网口中选出“镜像主口”
		 * 并使各抓包进程开始抓包
		 * (2)至于何时判断需要切换"主从镜像口"
		 * 由每次正常轮询(Routines)负责
		 * */
		int result_packet[2] = {0}; /*[0]image_one [1]image_two */
		char M_image[1024]   = {0};
		char S_image[1024]   = {0};
		if(JudgePacket(Image_one_stu.Core_data.eth_name,Image_two_stu.Core_data.eth_name,result_packet))/*都有数据包*/
		{
			/* FIXME 确保在保护模式下更新的image_card_one永远为主镜像口，
			 * image_card_two永远为从镜像口
			 */

			JudgeM_S_Image(&Image_one_stu,&Image_two_stu,M_image,S_image);

			/* Image,保护模式,CurrUse */
			UpdateUse(M_image,S_image,1,ProtectImageMode,0);	/*仅更新镜像口*/
			/* Image,保护模式,LastUse */
			UpdateUse(M_image,S_image,1,ProtectImageMode,1);	/*仅更新镜像口*/
			KillAllChecker();
		}
		else  	/* 两个网卡只有一个有包或两个都没有包 result_packet中被置 1 代表有包 */
		{
			/* FIXME 
			 * (1)有可能one 和 two 虽然优先级较高被选中，但也有可能实际是其他网口作为
			 * 镜像口.所以此时可以重新选取网线连接且有数据包的网口作为镜像口
			 * (2)如果都查找不到合适的网口作为镜像口，则当出现one或two只有一个网口有数据包 
			 *  时开启单镜像模式，当出现one和two都无包时，则不启任何检测进程···
			 */
			if(result_packet[0] && !result_packet[1]) 	  	/*image_one 有包*/
				FindOtherImage(&Image_one_stu);
			else if(result_packet[1] && !result_packet[0]) 		/*image_two 有包*/
				FindOtherImage(&Image_two_stu);

			/* 两个网卡都没有包
			 * 不做任何操作，不考虑
			 * 其他情况：
			 * 如：选择的两个是优先级
			 * 最高的但实际没有数据包
			 * 真正使用的时另外两个网口
			 * */

			else{
				FindDualImage();
			}
		}
	}
	return ;
}

/* 接收到双镜像模式 但实际只有一个网卡UP 则起单镜像口 */
void D_SingleImage(char *eth_name)
{
	int result[3] = {0}; /*[0]one [1]two [2]image_mode*/
	if(CmpCurrupLastuse(eth_name,NULL,&LastUse,SingleImageMode,result,1))  /* 一致 */
		return ;
	else  						/* 不一致 */
	{
		dzlog_info("the image has change");
		OpenSingleImage(eth_name);
	}
	return ;
}

void  DoubleImage() /* 双镜像 */
{
	//printf("I'm DoubleImage...\n");
	if((ModeByUI&0x1) )					/*第一位 独立模式*/
		AloneMode();
	else if(ModeByUI>>1&0x1)  	/*第二位 保护模式 */
		ProtectMode();
	return ;
}


int GetModeByUI(u_int8_t *mode)
{
	int fd = 0;
	if((fd = open(ModePath,O_RDONLY)) < 0)
	{
		perror("Open ModePath");		
		return -1;
	}
	char buffer[MiniBufferSize] = {0};
	read(fd,buffer,sizeof(buffer));
	DealCR(buffer);
	*mode = atoi(buffer);
	close(fd);
	fd = 0;
	return 0;
}



bool GetImageModeByUI()
{
#if 1
	/* 读共享内存 Do Something ... */
	//if(!ReadShareMem(&ModeByUI))
	/* 读配置文件 Do Something ... */
	if(-1 == GetModeByUI(&ModeByUI))
	{
		dzlog_info("ReadShareMem is err! ... ");
		return false;
	}
	/*取完ModeByUI 清空配置文件*/
#endif
	/* 开始判断 */
	switch(ModeByUI)
	{
		case 0: /* 用户 未 选择独立模式或保护模式,则进入轮询模块 */
			GeneralDeal();
			break;
		case 3:
			if(Image_one_stu.Core_data.carrier_value > 0)
				D_SingleImage(Image_one_stu.Core_data.eth_name);
			else if(Image_two_stu.Core_data.carrier_value > 0)
				D_SingleImage(Image_two_stu.Core_data.eth_name);
			break;
		default: 
			/* ***********************************************
			 * 双镜像中的一种 
			 * (1)只有镜像１结构体和镜像２结构体中的carrier_value
			 * 都为１时才调用DoubleImage() !
			 * (2)只有一个为１则启动单镜像模式!(用户感受不到他的
			 * 界面选择和实际网口矛盾的问题!! FIXME)
			 * ************************************************/
			if((Image_one_stu.Core_data.carrier_value > 0) && (Image_two_stu.Core_data.carrier_value > 0))
				DoubleImage();
			else if((Image_one_stu.Core_data.carrier_value > 0) && (Image_two_stu.Core_data.carrier_value <= 0))
				D_SingleImage(Image_one_stu.Core_data.eth_name); /*one卡UP且网卡握手成功,two卡down,起单镜像*/
			else if((Image_two_stu.Core_data.carrier_value > 0) && (Image_one_stu.Core_data.carrier_value <= 0))
				D_SingleImage(Image_two_stu.Core_data.eth_name); /*two卡UP且网卡握手成功,one卡down,起单镜像*/
	}
	return true;
}
#endif

/* 镜像 判断镜像口 */
void Image(struct G_Struct *image_one,struct G_Struct *image_two)
{
	/* FIXME 都为空直接返回，所以在界面中选择镜像模式之前
	 * 就应该先将镜像口切换正确
	 */
	if(!image_one && !image_two)
		return;
	
	

	
	EasyCopy(&Image_one_stu,sizeof(struct G_Struct),image_one,sizeof(struct G_Struct));
	EasyCopy(&Image_two_stu,sizeof(struct G_Struct),image_two,sizeof(struct G_Struct));


//	printf("++++++++++++++use->ImageStu.image_mode:%d\n", (Image_one_stu.Core_data.type));
//	printf("++++++++++++++use->ImageStu.image_card_one:%s\n", Image_one_stu.Core_data.eth_name);
//	sleep(1);
//	printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@(2)");
//	printf("++++++++++++++use->ImageStu.image_mode:%d\n", (Image_two_stu.Core_data.type));
//	printf("++++++++++++++use->ImageStu.image_card_one:%s\n", Image_two_stu.Core_data.eth_name);
//	sleep(1);



//	printf("I'm image............\n");
	/* 从UI指令表中获取指令 */
	GetImageModeByUI();
	return ;
}


/**********************************************
 * 以下 为光电自适应 相关函数
 * FIXME 仅判断、操作 通信口\监控口及其相关进程 
 * 仅判断、更新CurrUse 、LastUse 中的minor communicate 
 * ********************************************/
void Adaptive(struct G_Struct *Grup_minor,struct G_Struct *Grup_communicate)
{
	/* 都为空 */
	if(!Grup_minor && !Grup_communicate)
		return;

	//printf("I'm Adaptive............ \n");

	char Minor_eth[MicroMiniBufferSize] = {0};
	char Comm_eth[MicroMiniBufferSize] = {0};
	char *Minor_eth_p	= Minor_eth;
	char *Comm_eth_p		= Comm_eth;
	Grup_minor ? memcpy(Minor_eth_p,Grup_minor->Core_data.eth_name,strlen(Grup_minor->Core_data.eth_name)+1) : Minor_eth_p = NULL;
	Grup_communicate ? memcpy(Comm_eth_p,Grup_communicate->Core_data.eth_name,strlen(Grup_communicate->Core_data.eth_name)+1) : Comm_eth_p = NULL;

	int result[3] = {0}; /*[0]one [1]two [2]image_mode*/
	if(JudgeLastEmpty(&LastUse)) /* 空 ,首次上电 */
	{
		//printf("为空！\n");
		/* 清空LastUse 、CurrUse,防止残留*/
		memset(&LastUse,0,sizeof(LastUse));
		/* FIXME 获取CurrUse需先获取锁 */
		CpySetCurrUse(&CurrUse,0,sizeof(CurrUse),0);
		//printf("Minor_eth_p:%s -- Comm_eth_p:%s\n",Minor_eth_p,Comm_eth_p);

		UpdateUse(Minor_eth_p,Comm_eth_p,0,-1,0);
		UpdateUse(Minor_eth_p,Comm_eth_p,0,-1,1);
		/* FIXME 清除设备中网监相关进程 ,慎用！*/			
		ProScavenger(TipsProName);
	}
	else 	/* 不为空 正常轮询 */
	{
	//	printf("不 为空！\n");
		/* ModeFlag: 0:Adaptive 1:Image */
		if(CmpCurrupLastuse(Minor_eth_p,Comm_eth_p,&LastUse,-1,result,0))
			return ; /*一致 不做任何操作*/
		else  /*不一致*/
		{
		//	printf("不一致！\n");
			/* result: [0]one [1]two [2]image */
			if(!result[0]) /* minor */
			{
				/* FIXME 获取CurrUse需先获取锁 */
				CpySetCurrUse(CurrUse.AdaptiveStu.monitor,0,MicroMiniBufferSize,0);
				UpdateUse(Minor_eth_p,NULL,0,-1,0);		/*仅更新minor*/
				memset(LastUse.AdaptiveStu.monitor,0,MicroMiniBufferSize);
				UpdateUse(Minor_eth_p,NULL,0,-1,1);		/*仅更新minor*/
				ProScavenger(TIPS_ui);				/*kill UI*/
			}
			if(!result[1]) /* communicate */
			{
				//printf("communicate... \n");
				/* FIXME 获取CurrUse需先获取锁 */

				CpySetCurrUse(CurrUse.AdaptiveStu.communicate,0,MicroMiniBufferSize,0);
				UpdateUse(NULL,Comm_eth_p,0,-1,0);	/*仅更新communicate*/
				memset(LastUse.AdaptiveStu.communicate,0,MicroMiniBufferSize);
				UpdateUse(NULL,Comm_eth_p,0,-1,1);	/*仅更新communicate*/
				ProScavenger(TIPS_manager);		/*kill manager*/
			}
		}
	}
	return ;
}

/* ******************************************************
 * 以下 为规则 控制相关函数
 * 选取每组中一个网口，为镜像和光电自适应进一步筛选做准备
 * *****************************************************/

/* FIXME return -> 1:G_one 2:G_two 0:None !*/
int JudgeStu(struct G_Struct *G_one,struct G_Struct *G_two)
{
	/* carrier_value 是 -1 或　0　的网卡不能作为监控、通信、镜像 */
	if(G_two->Core_data.carrier_value > 0 && (strlen(G_two->Core_data.eth_name) > 0))		
		return 2;
	else if(G_one->Core_data.carrier_value > 0 && (strlen(G_one->Core_data.eth_name) > 0))		
		return 1;
	return 0;
}

void CallFunc(struct G_Struct *p_M,int C_flag,struct G_Struct *Grup_two_1,struct G_Struct *Grup_two_2,void func(struct G_Struct *one,struct G_Struct *two),char *err_2)
{
	switch(C_flag)
	{
		case 1:
			func(p_M,Grup_two_1);
			break;
		case 2:
			func(p_M,Grup_two_2);
			break;
		case 0:
		//	printf(err_2); 			/*通信口或镜像 2 组无连接或无UP 网口*/
			p_M ? func(p_M,NULL) : (void)NULL;
			return;
	}
	return ;
}

/*ModeFlag 0:Adaptive 1:Image*/
void Judge_A_I(struct G_Struct *Grup_one_1,struct G_Struct *Grup_one_2,struct G_Struct *Grup_two_1,struct G_Struct *Grup_two_2,void func(struct G_Struct *argv1,struct G_Struct *arg2),char *err_1,char *err_2,int ModeFlag)
{
	/* FIXME 传入参数顺序固定！
	 * (监控组)E_MonitorStu , O_Monitor, (通信组)E_Communicate, O_Communicate 
	 * (镜像1组)E_Image_1	, O_Image_1, (镜像2组)E_Image_2	  , O_Image_2
	 */
	int M_flag = JudgeStu(Grup_one_1,Grup_one_2); 	/*arg1:电口 arg2:光口*/
	int C_flag = JudgeStu(Grup_two_1,Grup_two_2); 	/*arg1:电口 arg2:光口*/
	switch(M_flag)	/*第一组*/
	{
		case 1:	/*Grup_one_1 carrier_value为1*/
			CallFunc(Grup_one_1,C_flag,Grup_two_1,Grup_two_2,func,err_2);
			break;
		case 2: /*Grup_one_2 carrier_value为1*/
			CallFunc(Grup_one_2,C_flag,Grup_two_1,Grup_two_2,func,err_2);
			break;
		case 0:
			//dzlog_info(err_1); 			/*Adaptive*/
			CallFunc(NULL,C_flag,Grup_two_1,Grup_two_2,func,err_2); /*监控组或镜像１组无UP网卡*/
			return;
	}
	return ;
}


/* FIXME 传入参数顺序固定！
 * &E_MonitorStu,&O_MonitorStu,&E_CommunicateStu,&O_CommunicateStu
 * 如果两个监控口无连接或两个通讯口无连接都不会调用Adaptive!
 */
void JudgeAdaptive()
{
	Judge_A_I(&E_MonitorStu,&O_MonitorStu,&E_CommunicateStu,&O_CommunicateStu,Adaptive,(char *)"两监控口无连接!\n",(char *)"两通信口无连接!\n",0);
	return ;
}

/* FIXME 传入参数顺序固定！
 * &E_Image_1Stu,&O_Image_1Stu,&E_Image_2Stu,&O_Image_2Stu 
 * 如果两组镜像口中任何一组
 */
void JudgeImage()
{
	Judge_A_I(&E_Image_1Stu,&O_Image_1Stu,&E_Image_2Stu,&O_Image_2Stu,Image,(char *)"镜像1组无数据!\n",(char *)"镜像2组无数据!\n",1);
	return ;
}

void RulesCtl()
{
	/*Adaptive Judge*/
	JudgeAdaptive();	

	/*Image Judge*/
	JudgeImage();
	return;
}


/* *******************************************
 *  以下 为轮询 网口状态相关函数
 *  按照系统默认网口列表进行轮询，获取相应状态
 * ******************************************/

/*获取carrier_value*/
bool PrintFlag(struct G_Struct* p_G)
{
	lseek(p_G->fd,0,SEEK_SET);
	char buffer[MiniBufferSize] = {0};
	int cnt = 0;
	if(-1 == (cnt = read(p_G->fd,buffer,sizeof(buffer))))
	{
		/* ******************************
		 * FIXME "Invalid argument"
		 * 当检测到/sys/class/net下有相应
		 * 网卡的信息且有carrier后，但read
		 * carrier时出现"Invalid argument",
		 * 证明此网卡状态现在down  !
		 * 赋值-1表明其状态为down!有可能会
		 * 使用到 !
		 * ********************************/
		p_G->Core_data.carrier_value = -1; 
		return false;
	}
	if(2==strlen(buffer) && isdigit(*buffer)) /*1 or 0 加 \n*/
	{
		DealCR(buffer);
		p_G->Core_data.carrier_value = atoi(buffer);
		//printf("fd:%d-flag:<%d> -cnt:<%d>\n",p_G->fd,p_G->Core_data.carrier_value,cnt);
		return true;
	}
	return false;
}

void TraverGetFlag(struct G_Struct *G_Stu)
{
	if(G_Stu->fd < 0)/*文件描述有问题*/
		return;

	/* 填充当前插网线网口表 */
	PrintFlag(G_Stu);	
	return ;
}


void OpenAliveEthCarrier(struct G_Struct *p_carrier)
{
	int fd = 0;
	(fd = GOpen(p_carrier->carrier_path))>=0 ? (void)(p_carrier->fd = fd) : perror("Open");
	return ;
}

bool JudgeExist(char *pathname)
{
	if(-1 == access(pathname,F_OK))
	{
		//printf("pathname:<%s> is not found!\n",pathname);
		return false;
	}
	return true;
}

void AssembledStr(struct G_Struct *G_Stu,char *src,int type)
{
	G_Stu->Core_data.type = type;	 /*type*/
	snprintf(G_Stu->Core_data.eth_name,MicroMiniBufferSize,"%s",src); /*eth_name*/
	snprintf(G_Stu->carrier_path,MiniBufferSize,"%s/%s/%s",CarrierPath,src,CarrierName); /*carrier_path*/
	if(JudgeExist(G_Stu->carrier_path))
	{
		OpenAliveEthCarrier(G_Stu);	/*得到fd*/
		TraverGetFlag(G_Stu);		/*得到carrier*/
	}
	return ;
}

void ConfirmAliveEth()
{

#if 1	//仅用于打印 方便查看  进行测试
	struct UseCard tmp_CurrUse;
	CpySetCurrUse(&tmp_CurrUse,&CurrUse,sizeof(struct UseCard),1);
	/* FIXME 获取CurrUse需先获取锁 */
//	printf("monitor:    <%s>\n",tmp_CurrUse.AdaptiveStu.monitor);
//	printf("communicate:<%s>\n",tmp_CurrUse.AdaptiveStu.communicate);
//	printf("image_mode: <%d>\n",tmp_CurrUse.ImageStu.image_mode);
//	printf("image_one:  <%s>\n",tmp_CurrUse.ImageStu.image_card_one);
//	printf("image_two:  <%s>\n",tmp_CurrUse.ImageStu.image_card_two);
#endif
	/* 电口 */
	if(strlen(DefaultNicStu.E_monitor) > 0)
		AssembledStr(&E_MonitorStu,DefaultNicStu.E_monitor,E_Monitor);
	if(strlen(DefaultNicStu.E_communicate) > 0)
		AssembledStr(&E_CommunicateStu,DefaultNicStu.E_communicate,E_Communicate);
	if(strlen(DefaultNicStu.E_image_1) > 0)
		AssembledStr(&E_Image_1Stu,DefaultNicStu.E_image_1,E_Image_1);
	if(strlen(DefaultNicStu.E_image_2) > 0)
		AssembledStr(&E_Image_2Stu,DefaultNicStu.E_image_2,E_Image_2);
	/* 光口 */
	if(strlen(DefaultNicStu.O_monitor) > 0)
		AssembledStr(&O_MonitorStu,DefaultNicStu.O_monitor,O_Monitor);
	if(strlen(DefaultNicStu.O_communicate) > 0)
		AssembledStr(&O_CommunicateStu,DefaultNicStu.O_communicate,O_Communicate);
	if(strlen(DefaultNicStu.O_image_1) > 0)
		AssembledStr(&O_Image_1Stu,DefaultNicStu.O_image_1,O_Image_1);
	if(strlen(DefaultNicStu.O_image_2) > 0)
		AssembledStr(&O_Image_2Stu,DefaultNicStu.O_image_2,O_Image_2);
	return ;
}
void DestroyStu(struct G_Struct *p_G)
{
	close(p_G->fd);
	memset(p_G,0,sizeof(struct G_Struct));
	return;
}

/*收尾工作*/
void  AdaptiveEndRelated()
{
	DestroyStu(&E_MonitorStu);
	DestroyStu(&E_CommunicateStu);
	DestroyStu(&E_Image_1Stu);
	DestroyStu(&E_Image_2Stu);

	DestroyStu(&O_MonitorStu);
	DestroyStu(&O_CommunicateStu);
	DestroyStu(&O_Image_1Stu);
	DestroyStu(&O_Image_2Stu);

	return ;
}


/*初始化相关结构等*/
void AdaptiveInitRelated()
{

	return;
}

/* 初始化相关锁 */
void init_pthread_mutex()
{
	pthread_mutex_init(&curr_mutex,NULL);
	return ;
}


void SelectFD()
{
	while(1)
	{
		/*关闭所有描述符*/
		AdaptiveEndRelated();	
		/*填充所有结构体*/
		ConfirmAliveEth();

		memset(&Image_one_stu,0,sizeof(struct G_Struct));
		memset(&Image_two_stu,0,sizeof(struct G_Struct));

		/*规则控制*/
		RulesCtl();

		/*FIXME 此处使用select与轮询意义一样.无论内容有无变化,打开的文件描述符都可读!*/
#if 0
		fd_set rfds;
		FD_ZERO(&rfds);

		list<int>::iterator p_fd;
		for(p_fd = fdOne.begin();p_fd != fdOne.end();p_fd++)
			FD_SET(*p_fd,&rfds);
		int max_fd = *max_element(fdOne.begin(),fdOne.end());
		struct timeval timeout = {0,0};
		switch(select(max_fd+1,&rfds,(fd_set *)0,(fd_set *)0,&timeout))
		{
			case -1:printf("select is err...");break;
			case  0:break;
			default:
				for(p_fd = fdOne.begin();p_fd != fdOne.end();p_fd++)
				{
					if(FD_ISSET(*p_fd,&rfds))
					{
						PrintFlag(*p_fd);
					}
				}
		}
#endif
		sleep(1);
	}
	return;
}

/* 将网卡对应关系存入DefaultNicStu */
bool ReadNicInfo()
{
	return GetNicInfo(&DefaultNicStu);
}

void AdaptiveImageCtl()
{
	/*初始化(分配空间)*/
	AdaptiveInitRelated();

	SelectFD();

	AdaptiveEndRelated();
	return ;
}
