#include "GetConfigInfo.h"
#include "capturepacket.h"
extern char ETH_ALARM[10];

bool GetEth(char *eth)
{
	union ConfigInfo testStu;
	memset(&testStu,0,sizeof(union ConfigInfo));

	/*返回false说明获取失败，错误原因详见err*/
	if(false == GetTipsConfigInfo(AlarmConfig,EthTag,(char *)"=",&testStu))
	{
		printf("Err:%s\n",testStu.err);
		//return false;/*add by lipeiting 2014-03-19*/
	}
#if 1
	else  /*返回ture说明值获取成功*/
	{
		printf("get-info is <%s>\n",testStu.autodyne);	
	}
#endif
//	memcpy(eth,testStu.autodyne,strlen(testStu.autodyne) + 1);
	strcpy(eth,ETH_ALARM);/*add by lipeiting 2014-03-19*/
	ReleaseInfoSpace(&testStu);/*FIXME 必须手动释放*/
	return true;
}
