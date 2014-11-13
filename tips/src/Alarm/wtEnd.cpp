//by --- yong 2013.04.03PM13:30

#include "wtEnd.h"

u_int32_t  ID_NUM;
//char ID_NUM[12];
char LOCALDEVID[11];
pthread_mutex_t id_mutex;

bool ParBuff(char *buffer)
{
        int i = 0;
        if(buffer[strlen(buffer) - 1] == '\n')
        {
                buffer[strlen(buffer) - 1]   = '\0';
        }
        while(buffer[i])
        {
                if(!isdigit(buffer[i]))     
                {
                        return false;   
                }
                ++i;    
        }
        return true;
}
bool ParID(int fd,u_int32_t *id)//读
{
    char buffer[BUFFSIZE] = {0};
        int n = 0;
    if(-1 == (n = read(fd,buffer,sizeof(buffer))))      
    {
        perror("(WtEnd)read--");
        return false;
    }
    if(ParBuff(buffer))
    {
        *id = atoi(buffer);                     
        if((*id) > TopLimit)
        {
            *id = 1;
        }
        return true;
    }
    return false;
}

void OpeID(bool flag)
{
    pthread_mutex_lock(&id_mutex);
    int fd = 0;
    if(-1 == (fd = open(NUM_FILE,O_RDWR)))
    {
        perror("Open--");   
        pthread_mutex_unlock(&id_mutex);
        return ;    
    }
    if(flag) //回写
    {
#if 0
        char buffer[BUFFSIZE] = {0};
        sprintf(buffer,"%d\0",ID_NUM);
        cout<<"++++++++++++++++++++++++++++++++++++++++++++++++++++"<<endl;
        lseek(fd,0,SEEK_SET);
        write(fd,buffer,sizeof(u_int32_t));
#endif
        char zzf_buff[12];
        memset(zzf_buff,0,12);
        u_int zzf_temp=1000000000;
        int zzf_j=0;
        for(int i=0;i<10;)
        {
            zzf_j=ID_NUM/zzf_temp;
            if(zzf_j)
            {
                    zzf_buff[i]=zzf_j+48;   
                    zzf_j=0;
                    i++;
            }
            ID_NUM%=zzf_temp;
            if(!ID_NUM)break;
            zzf_temp/=10;
        }
    write(fd,zzf_buff,strlen(zzf_buff));
    }
    else //读 更新ID
    {
        if(!ParID(fd,&ID_NUM))
        {
            cout<<"更新ID出错~!~!"<<endl;   
        }
    }
    close(fd);
    pthread_mutex_unlock(&id_mutex);
    return ;
}
void WtEnd(int)//关机前的操作
{
        //(1) 将ID（IPBlack和DNS模块） ID 号回写到ID.txt
        OpeID(true);
        cout<<"9999999999999999999999999999999999999999999999"<<endl;

#if 0
        sleep(1);
        kill(getpid(),SIGKILL);
#endif

        //N:................
        return ;
}
void WtDevID()
{
    FILE *fd = 0;
    if((0 == (fd = fopen(CONFIGFILE,"r"))))
    {
        perror("fopen Configre File--");    
        fclose(fd);
        return;
    }
    char buffer[BUFFSIZE] = {0};
    char *tmp = 0;
    char *tmpp = 0;
    while(fgets(buffer,sizeof(buffer),fd))      
    {
        if((tmp = strstr(buffer,DEVKEY)))
        {
            strtok(tmp,"=");        
            if((tmpp = strtok(NULL,"=")))
            {
                while(!isdigit(*tmpp))  
                {
                    tmpp++;
                }
                if(ParBuff(tmpp))
                {
                    memcpy(LOCALDEVID,tmpp,DEVIDNUM);       
                }
            }
        }
        memset(buffer,0,sizeof(buffer));
    }
    fclose(fd);
    return ;    
}
void Outinit()//程序启动时初始化
{
    //WtPid(); //保存PID    
    //  cout<<"11111111111111"<<endl;
    WtDevID(); //读取LOCALDEVID
    //      cout<<"11111111111111"<<endl;
    OpeID(false);//读取ID 给后续的IP blaok和DNS使用
    //      cout<<"11111111111111"<<endl;
    return ;
}
