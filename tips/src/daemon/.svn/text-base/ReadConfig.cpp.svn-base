#include "AdaptiveData.h"
#include "GetConfigInfo.h"
#include "GetNicInfo.h"
#include "ReadConfig.h"
#include "zlog.h"

char cmd[MicroMiniBufferSize] = {0};
union ConfigInfo testStu;

bool GetNicInfo(struct DefaultNICList *DefaultNicStu)
{
	if( GetTipsConfigInfo ( NICCONF, (char *)"e_monitor", (char *)"=", &testStu ) )
		memcpy( DefaultNicStu->E_monitor, testStu.autodyne, strlen( testStu.autodyne ) + 1 );
	else {
		dzlog_info("getNiceINfo Err:%s\n", testStu.err);
		return false;
	}
	
	ReleaseInfoSpace( &testStu );
	dzlog_info("DefaultNicStu->E_monitor:%s\n", DefaultNicStu->E_monitor);

	memset(&testStu, 0, sizeof(union ConfigInfo));
	if( GetTipsConfigInfo ( NICCONF, (char *)"e_commun", (char *)"=", &testStu ))
		memcpy( DefaultNicStu->E_communicate, testStu.autodyne, strlen ( testStu.autodyne ) + 1 );
	
	else {
		dzlog_info("error:%s", testStu.err);
		return false;
	}
	
	ReleaseInfoSpace( &testStu );
	dzlog_info("DefaultNicStu->E_communicate:%s", DefaultNicStu->E_communicate);

	memset(&testStu,0, sizeof( union ConfigInfo ));
	if( GetTipsConfigInfo( NICCONF, (char *)"e_image1", (char *)"=", &testStu ))
		memcpy(DefaultNicStu->E_image_1, testStu.autodyne, strlen(testStu.autodyne) + 1);
	else {
		dzlog_info("Err:%s", testStu.err);
		return false;
	}
	
	ReleaseInfoSpace( &testStu );
	dzlog_info("DefaultNicStu->E_image_1:%s\n", DefaultNicStu->E_image_1);

	memset(&testStu, 0, sizeof(union ConfigInfo));
	
	if( GetTipsConfigInfo( NICCONF, (char *)"e_image2", (char *)"=", &testStu ))
		memcpy(DefaultNicStu->E_image_2, testStu.autodyne, strlen(testStu.autodyne) + 1);
	else
	{
		dzlog_info("Err:%s",testStu.err);
		return false;
	}
	
	ReleaseInfoSpace( &testStu );
	dzlog_info("DefaultNicStu->E_image_2:%s", DefaultNicStu->E_image_2);

	memset(&testStu, 0, sizeof(union ConfigInfo));
	
	if( GetTipsConfigInfo( NICCONF, (char *)"o_monitor", (char *)"=", &testStu ))
		memcpy(DefaultNicStu->O_monitor, testStu.autodyne, strlen(testStu.autodyne) + 1 );
	else {
		dzlog_info("Err:%s\n",testStu.err);
		return false;
	}
	
	ReleaseInfoSpace( &testStu );
	dzlog_info("DefaultNicStu->O_monitor:%s", DefaultNicStu->O_monitor);

	memset(&testStu, 0, sizeof(union ConfigInfo));
	if( GetTipsConfigInfo( NICCONF, (char *)"o_commun", (char *)"=", &testStu ))
		memcpy(DefaultNicStu->O_communicate,testStu.autodyne,strlen(testStu.autodyne)+1);
	else 
	{
		dzlog_info("Err:%s",testStu.err);
		return false;
	}
	
	ReleaseInfoSpace( &testStu );
	dzlog_info("DefaultNicStu->O_communicate:%s", DefaultNicStu->O_communicate );

	memset(&testStu, 0, sizeof(union ConfigInfo));
	if( GetTipsConfigInfo( NICCONF, (char *)"o_image1", (char *)"=", &testStu))
		memcpy(DefaultNicStu->O_image_1, testStu.autodyne, strlen(testStu.autodyne) + 1 );
	else 
	{
		dzlog_info("Err:%s",testStu.err);
		return false;
	}
	
	ReleaseInfoSpace( &testStu );
	dzlog_info("DefaultNicStu->O_image_1:%s\n", DefaultNicStu->O_image_1);

	memset(&testStu, 0, sizeof(union ConfigInfo));
	if( GetTipsConfigInfo( NICCONF, (char *)"o_image2", (char *)"=", &testStu ))
		memcpy(DefaultNicStu->O_image_2, testStu.autodyne, strlen(testStu.autodyne) + 1 );
	else 
	{
		dzlog_info("Err:%s\n",testStu.err);
		return false;
	}
	
	ReleaseInfoSpace( &testStu );
	printf("DefaultNicStu->O_image_2:%s\n", DefaultNicStu->O_image_2);

	return true;
}


bool GetInfoByConfig(char *KEY,char *separtor,char *dstbuf,int len)
{
	memset(dstbuf,0,len);
	union ConfigInfo testStu;
	memset(&testStu,0,sizeof(union ConfigInfo));

	if(false == GetTipsConfigInfo(CONFIG_FILE,KEY,separtor,&testStu))
	{
		dzlog_info("Err:%s",testStu.err);
		ReleaseInfoSpace(&testStu);/*FIXME 必须手动释放*/
		return false;
	}
	//printf("Value:<%s>\n",testStu.autodyne);	

	memcpy(dstbuf,testStu.autodyne,strlen(testStu.autodyne));
	ReleaseInfoSpace(&testStu);/*FIXME 必须手动释放*/
	return true;
}
bool ReadConfigFunc(int* LoopTimes)
{
	/*Times*/
	char tmpbuff[128] = {0};
	int i = 0;
	bool flag = GetInfoByConfig((char *)"LoopFrequence",(char *)"=",tmpbuff,sizeof(tmpbuff));
	while(tmpbuff[i])
	{
		if(!isdigit(tmpbuff[i++]))
			return false;
	}
	*LoopTimes = atoi(tmpbuff);
	return flag;
}
