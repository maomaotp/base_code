#include "GetConfigInfo.h"
#include "ReadConfig.h"
#include "GetNicInfo.h"
#define MicroMiniBufferSize 1024
#define NICCONF (char *)"/usr/local/tipschecker/etc/tipscheck/TipsNIC.cfg"
char cmd[MicroMiniBufferSize] = {0};
union ConfigInfo testStu;

bool GetNicInfo(struct DefaultNICList *DefaultNicStu)
{
	if(GetTipsConfigInfo(NICCONF,(char *)"e_monitor",(char *)"=",&testStu))
		memcpy(DefaultNicStu->E_monitor,testStu.autodyne,strlen(testStu.autodyne)+1);
	else {printf("Err:%s\n",testStu.err);return false;}
	ReleaseInfoSpace(&testStu);

	memset(&testStu,0,sizeof(union ConfigInfo));
	if(GetTipsConfigInfo(NICCONF,(char *)"e_commun",(char *)"=",&testStu))memcpy(DefaultNicStu->E_communicate,testStu.autodyne,strlen(testStu.autodyne)+1);
	else {printf("Err:%s\n",testStu.err);return false;}
	ReleaseInfoSpace(&testStu);

	memset(&testStu,0,sizeof(union ConfigInfo));
	if(GetTipsConfigInfo(NICCONF,(char *)"e_image1",(char *)"=",&testStu))memcpy(DefaultNicStu->E_image_1,testStu.autodyne,strlen(testStu.autodyne)+1);
	else {printf("Err:%s\n",testStu.err);return false;}
	ReleaseInfoSpace(&testStu);

	memset(&testStu,0,sizeof(union ConfigInfo));
	if(GetTipsConfigInfo(NICCONF,(char *)"e_image2",(char *)"=",&testStu))memcpy(DefaultNicStu->E_image_2,testStu.autodyne,strlen(testStu.autodyne)+1);
	else {printf("Err:%s\n",testStu.err);return false;}
	ReleaseInfoSpace(&testStu);

	memset(&testStu,0,sizeof(union ConfigInfo));
	if(GetTipsConfigInfo(NICCONF,(char *)"o_monitor",(char *)"=",&testStu))memcpy(DefaultNicStu->O_monitor,testStu.autodyne,strlen(testStu.autodyne)+1);
	else {printf("Err:%s\n",testStu.err);return false;}
	ReleaseInfoSpace(&testStu);

	memset(&testStu,0,sizeof(union ConfigInfo));
	if(GetTipsConfigInfo(NICCONF,(char *)"o_commun",(char *)"=",&testStu))memcpy(DefaultNicStu->O_communicate,testStu.autodyne,strlen(testStu.autodyne)+1);
	else {printf("Err:%s\n",testStu.err);return false;}
	ReleaseInfoSpace(&testStu);

	memset(&testStu,0,sizeof(union ConfigInfo));
	if(GetTipsConfigInfo(NICCONF,(char *)"o_image1",(char *)"=",&testStu))memcpy(DefaultNicStu->O_image_1,testStu.autodyne,strlen(testStu.autodyne)+1);
	else {printf("Err:%s\n",testStu.err);return false;}
	ReleaseInfoSpace(&testStu);

	memset(&testStu,0,sizeof(union ConfigInfo));
	if(GetTipsConfigInfo(NICCONF,(char *)"o_image2",(char *)"=",&testStu))memcpy(DefaultNicStu->O_image_2,testStu.autodyne,strlen(testStu.autodyne)+1);
	else {printf("Err:%s\n",testStu.err);return false;}
	ReleaseInfoSpace(&testStu);

	return true;
}

