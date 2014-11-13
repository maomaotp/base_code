#include "GetConfigInfo.h"
#include "data.h"
FILE *fp = 0;

void AllocSpace(char **point,char* info)
{
    *point = (char *)calloc(strlen(info) + 1,sizeof(char));   
    snprintf(*point,strlen(info) + 1,"%s",info);
    return;
}


bool JudgeKeySepartor(char *CmdBuff,char *ConfigPath,char *dst,union ConfigInfo *BackInfo,int flag)
{
    char ErrInfoBuff[ErrSize] = {0};
    if(!(fp = popen(CmdBuff,"r"))) /*打开失败*/
    {
		dzlog_info("GetConfigInfo->Initcheck->Popen failed!!!");
        snprintf(ErrInfoBuff,ErrSize,"The \"%s\" is Open failed!",ConfigPath);
        AllocSpace(&(BackInfo->err),ErrInfoBuff);
        pclose(fp);
        return false;       
    }

    char TmpBuff[LocalBuffSize] = {0};
  
    if(1 == flag) /*查找信息*/
    {
        if(!fgets(TmpBuff,LocalBuffSize,fp) || strlen(TmpBuff) <= 1){
            snprintf(ErrInfoBuff,ErrSize,"The \"%s\"is not include   \"%s\"   or the out-value is not found!",ConfigPath,dst);
            AllocSpace(&(BackInfo->err),ErrInfoBuff);
            pclose(fp);
            return false;
		}
   
        char *save=NULL;
        char *res=NULL;
        res=strtok_r(TmpBuff," =",&save); 
        res=strtok_r(NULL," =",&save);
        if(res[strlen(res) - 1] == '\n')        
        {
          res[strlen(res) - 1] = '\0';
        }
        if(res){
    		strcpy(TmpBuff,res);
    	}
        else{
    		strcpy(TmpBuff,"NULL");
			AllocSpace(&(BackInfo->autodyne),TmpBuff);
		}
    }
    pclose(fp);
    fp = 0;
    return true;
}
bool InitCheck(char *ConfigPath,char *key,char *separtor,union ConfigInfo *BackInfo)
{
    char ErrInfoBuff[ErrSize] = {0};
    memset(BackInfo,0,sizeof(union ConfigInfo));
    //determine a file exists
	if(-1 == access(ConfigPath,F_OK)) {
        snprintf(ErrInfoBuff,ErrSize,"The  \"%s\"  is not found!",ConfigPath);
        AllocSpace(&(BackInfo->err),ErrInfoBuff);
        return false;
    }
    char CmdBuff[LocalBuffSize] = {0};
    snprintf(CmdBuff,LocalBuffSize,"grep -i \"%s\" %s",separtor,ConfigPath);
	//the separator
    if(false == JudgeKeySepartor(CmdBuff,ConfigPath,separtor,BackInfo,0)) {
		dzlog_info("the separator is not");
        return false;   
    }
	// the key world
    if(false == JudgeKeySepartor(CmdBuff,ConfigPath,key,BackInfo,0)) {
		dzlog_info("the key world is not");
        return false;   
    }
    return true;
}

bool GetInfo(char *ConfigPath,char *key,char *separtor,union ConfigInfo *BackInfo)
{
    char CmdBuff[LocalBuffSize] = {0};

    snprintf(CmdBuff,LocalBuffSize,"egrep -v '^$|^#' %s | grep '%s' | grep -v '^$' | grep '%s'",ConfigPath,key,separtor);

    if(false == JudgeKeySepartor(CmdBuff,ConfigPath,key,BackInfo,1)) {
		dzlog_info("the separator is not");
        return false;   
    }
    return true;
}

bool GetTipsConfigInfo(char *ConfigPath,char *key,char *separtor,union ConfigInfo *BackInfo)
{
    /*初始化检查*/
    if(false == InitCheck(ConfigPath,key,separtor,BackInfo)) {
		dzlog_info("initcheck failed!!!");
        return false;
    }
    /*开始查找*/
    if(false == GetInfo(ConfigPath,key,separtor,BackInfo)) {
		dzlog_info("getInfo failed!!!");
        return false;   
    }
    return true;
}

bool ReleaseInfoSpace(union ConfigInfo *BackInfo)
{
    if(NULL != BackInfo->err){
		dzlog_info("backInfo->err is NULL!!!");
        free(BackInfo->err);    
    }
    else if(NULL != BackInfo->autodyne) {
		dzlog_info("backInfo->autodyne is NULL!!!");	
        free(BackInfo->autodyne);
    }
    else {
        dzlog_info("backInfo both is NULL!!!");
		return false;
    }
    memset(BackInfo,0,sizeof(union ConfigInfo));
    return true;
}
