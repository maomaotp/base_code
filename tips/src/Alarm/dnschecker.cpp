/*
 * (1)在原有代码的基础上未做大的改动,增加过滤机制减少断错误
 * (2)修改报警的数据结构
 * (3)将报警方式由原来的插内存池改为发送给cache
 * 2014.01.08
 */




#include "dnschecker.h"
#include <netinet/in.h>
#include <arpa/inet.h>

extern pthread_mutex_t id_mutex;

//FIXME by -- yong 20130710
pthread_mutex_t DnsAlert_mutex;
//DnsAlertMap
map<u_int32_t,cache_alert_t> DnsAlertMap;

u_int32_t DNS_NUM = 0;
void DNSChecker::GreateHash(const char* str,u_int32_t* ip,u_int32_t* hash)
{
    u_int32_t seed = 131; 
    if(NULL == str)
    {
        while(*str)
        {
            *hash = (*hash)*seed + (*str++);
        }
    }
    else if(ip)
    {
        *hash = (*ip)+seed;
    }
    *hash &= 0x7FFFFFFF;
    return ;
}
void DNSChecker::Clear()
{
    return;
}
#if 0
void DNSChecker::Clear()
{
    //      DnameRigour.clear();    
    //      last.clear();
    //FIXME 判断报警表中每条结构体中报警时间戳 距离现在是否大于 AlertTimeOut  如果是就报警 并删除此条记录

    pthread_mutex_lock(&DnsAlert_mutex);
    map<u_int32_t ,cache_alert_t>::iterator l_it;; 
    for ( l_it = DnsAlertMap.begin(); l_it != DnsAlertMap.end(); l_it++ )
    {
        if(((time(NULL)) - (l_it->second.alert_time)) >= AlertTimeOut)
        {
            cacheclient.Send_Msg((char *)&(l_it->second),sizeof(cache_alert_t));
            //WtMemPool(&(l_it->second),sizeof(dnsblack_info)); //报警
            DnsAlertMap.erase(l_it); //删除此报警记录
        }
    }
    pthread_mutex_unlock(&DnsAlert_mutex);

    return ;
}
#endif
void DNSChecker::DgrousLevel(u_int8_t *srclevel,char *newlevel)
{
    switch(*srclevel)
    {
        case '2':   
            *newlevel = '2';
            break;
        case '5':   
            *newlevel = '3';
            break;
        case '8':   
            *newlevel = '4';
            break;
    }
    return;
}
int DNSChecker::Load_Rules(char* rule,int len)
{
    cout<<"DNS checkLoadrouleSSSSSS~!~!"<<endl;
    keymatch.MatchClear();
    DnameRigour.clear();
    Dgrous.clear();
    for(int i = 0;i < len;i++){
        //黑域名
        if('1' != ((struct dns_rules *)rule)[i].type){
            cout<<"load_rules======="<<((struct dns_rules*)rule)[i].dns_name<<endl;
            u_int32_t hash = 0; 
            char level;
            GreateHash(((struct dns_rules *)rule)[i].dns_name,NULL,&hash);  
            //cout<<"hash==========="<<hash<<endl;
            DgrousLevel(&(((struct dns_rules *)rule)[i].dangerous),&level);
            Dgrous.insert(pair<u_int32_t,char>(hash,level));
            if('1' == (((struct dns_rules *)rule)[i].match_mode)) //精确匹配域名
            {
                u_int32_t hash = 0;
                GreateHash(((struct dns_rules *)rule)[i].dns_name,NULL,&hash);
                DnameRigour.insert(pair<u_int32_t,string>(hash,((struct dns_rules *)rule)[i].dns_name));                    
                //cout<<"111111111111111(1)-->dns_name):"<<((struct dns_rules *)rule)[i].dns_name<<endl;
            }
            else if('2' == ((struct dns_rules *)rule)[i].match_mode) //模糊匹配域名
            {
                //keymatch.AddKey(((struct dns_rules *)rule)[i].dns_name,((struct dns_rules *)rule)[i].dns_name);       
                keymatch.AddKey(((struct dns_rules *)rule)[i].dns_name);        
                //cout<<"222222222222222222222(2)-->dns_name):"<<((struct dns_rules *)rule)[i].dns_name<<endl;
                keymatch.AddKey(((struct dns_rules *)rule)[i].dns_name);        
            }
        }   
    }
    keymatch.MatchInit();
    return 0;
}

int DNSChecker::ReLoad_Rules(char* rule,int len)
{
    if(NULL == rule || 0 == len) return 0;
    return 0;
}

int GetDomainName(struct dnshdr *dns_addr,char *tmp,int *mis,int *len,char *DomainName)
{
    char DnLen;
    //cout<<"((*tmp) >> 7):"<<((*tmp) >> 7)<<"|((*tmp & 64):"<<(*tmp & 64)<<endl;
    //  printf("*tmp:<%s>,mis<%d>,len<%d>,DomainName<%s>\n",tmp,*mis,*len,DomainName);
    if(0xC0 == ((*tmp) & (0xC0))) //压缩
    {
        //cout<<"我压缩"<<endl;
        short * tmpp = NULL;
        tmpp = (short *)tmp; //拿到16位指针
        if(*len)     //全域名压缩
        {
            strcpy(DomainName,(char *)dns_addr + (*tmpp & 0x3FFF)); //通过剩余14位表示的偏移量  拿到域名
        }
        else  //尾部域名压缩
        {
            strcat(DomainName,".");
            strncat(DomainName,tmp + 1,sizeof(tmp+1));      
        }
    }
    else if(*tmp)//非压缩
    {
        //cout<<"非压缩"<<endl;
        DnLen = *tmp;  
        //printf("DnLen::::%d\n",DnLen);
        if(0 == *len)   
        {
            //cout<<"XXX(dd)"<<endl;
            strncpy(DomainName,tmp + 1,DnLen);      
            *len = 1;
        }
        else
        {
            strcat(DomainName,".");
            strncat(DomainName,tmp + 1,DnLen);      
            //cout<<"XXX(ff):"<<DomainName<<endl;
        }
        *mis = DnLen + 1;
        //printf("Mis-->:%d\n",*mis);
        strcat(DomainName,"\0");
        //cout<<"XXX(4)"<<endl;
        //cout<<"(int)*(tmp + DnLen + 1):"<<(int)*(tmp + DnLen + 1)<<endl;
        //printf("*(tmp + DnLen + 1):%c\n",*(tmp + DnLen + 1));
        return (int)*(tmp + DnLen + 1);
    }
    return 0;
}
#if 0
void DNSChecker::WtMemPool(cache_alert_t * dns_alert,int len)
{
    printf("================================================================dns_alert->dip:%s\n",dns_alert->dip);
    //cmempool->Mem_Write(dns_alert,len);           

    return ;
}
#endif

void DNSChecker::WtFileMem(char *Name,char *PacketContent,int ContentLen)
{
    if(NULL == Name || NULL == PacketContent || 0 == ContentLen) return;
    //filemem->Mem_Write2(Name,PacketContent,ContentLen);   
    return ;
}

void DNSChecker::JudgeDNS(char *DomainName,struct ip* addr,int len,struct net_element *e)
{
    if(NULL == addr)
    {
        addr = NULL;
    }

    u_int32_t hash = 0;
    GreateHash(DomainName,NULL,&hash);
    cache_alert_t dns_alert;
    memset(&dns_alert,0,sizeof(dns_alert));
    INIT_ALERT_HEAD(&dns_alert);/*init alert_stu*/

    map<u_int32_t ,string >::iterator l_it;; 
    l_it=DnameRigour.find(hash);
    if(l_it != DnameRigour.end()) //精确匹配
    {
        cout<<"DomainNamedns========"<<DomainName<<":::"<<strlen(DomainName)<<endl;
        cout<<"hashdns==========="<<hash<<endl;
        if(Fill_alert(DomainName,len,&dns_alert,e)) //组报警结构体
        {
            cacheclient.Send_Msg((char *)&(dns_alert),sizeof(cache_alert_t));
            //FIXME 插报警链表
            pthread_mutex_lock(&DnsAlert_mutex);
            DnsAlertMap.insert(pair<u_int32_t,cache_alert_t>(hash,dns_alert));
            pthread_mutex_unlock(&DnsAlert_mutex);

            //FIXME 写内存池 但不在此地 (在遍历报警表并匹配上时和clear时间到了时报警)
            //WtMemPool(&dns_alert,sizeof(dnsblack_info));
        }
        else 
        {
            return ;
        }
    }
    else //模糊匹配
    {
        //      cout<<"hash================end"<<endl;
        //cout<<"daskf----------------??????DomainName:::::"<<DomainName<<endl;
        last.clear();
        keymatch.MatchMain(DomainName,&last,strlen(DomainName));                    
        if(!last.empty())
        {
            //cout<<"(1)++++++++Domain():"<<DomainName<<endl;
            if(Fill_alert(DomainName,len,&dns_alert,e)) //组报警结构体
            {
                //FIXME 插报警链表      
                pthread_mutex_lock(&DnsAlert_mutex);
                DnsAlertMap.insert(pair<u_int32_t,cache_alert_t>(hash,dns_alert));
                pthread_mutex_unlock(&DnsAlert_mutex);

                //cout<<"KJKJKJKJKJKJKJKJ"<<endl;
                //cout<<"(2)模糊+++++++++Domain():"<<DomainName<<endl;
                //FIXME 写内存池 但不在此地 (在遍历报警表并匹配上时和clear时间到了时报警)
                //WtMemPool(&dns_alert,sizeof(dnsblack_info));
                last.clear();
            }
            else
            {
                last.clear();
                return ;    
            }
        }
        else
        {
            return ;
        }
    }
    //WtMem2(addr);
    return ;
}

void DNSChecker::WtMem2(struct ip *addr)
{
    //--Write filemem--
    int Currentime = time((time_t *)NULL);
    char Name[FileNameSize] = {0};
    sprintf(Name,"%s/%d_%d_%s_%u_%d_%d.pcap",DNSFileDIR,DataPacketType,Currentime,LOCALDEVID,DNS_NUM,TorjanID,AlertType);   
    int iplen = (int)(ntohs(addr->ip_len));
    int ContentLen = (PcapPacketHeadSize + MacFrameLen + iplen);
    char *PcapMemBuff = (char *)malloc(ContentLen);
    wtpcap->WtPcapDataH(PcapMemBuff,iplen + MacFrameLen,iplen + MacFrameLen);//Write pcap-data-header

    struct macframes MF;
    memset(&MF,0,sizeof(MF));
    MF.type = MacFrameProtocolType;
    wtpcap->WtPcapMacFrame(PcapMemBuff,&MF);

    wtpcap->WtPcapData(PcapMemBuff,addr,iplen); 

    WtFileMem(Name,PcapMemBuff,ContentLen); 

    free(PcapMemBuff);
    PcapMemBuff = 0;

    return ;
}

void DNSChecker::Print_Error(int fd,char *Api)
{
    if(Api)
    {
        perror("Api--");    
    }
    close(fd);
    return ;
}

//bool DNSChecker::DNS_ID(char *id)
bool DNSChecker::DNS_ID(u_int32_t *id)
{
    pthread_mutex_lock(&id_mutex);
    if(ID_NUM >= TopLimit)
    {
        ID_NUM = 1;
    }
    else
    {
        ++ID_NUM;
    }
    *id = ID_NUM;
    DNS_NUM = ID_NUM;
    pthread_mutex_unlock(&id_mutex);
    return true;
}

bool CreateMD5(char *srcbuff,char *dstbuff)
{
    MD5_CTX ctx;
    unsigned char md[16];
    char tmp[3] = {'\0'};
    int i = 0;
    if(MD5_Init(&ctx) ==0 )/*init MD5_CTX*/
    {
        printf("Error!MD5_Init() Error!\n");
        return false;
    }
    //int MD5_Update(MD5_CTX *c, const void *data, unsigned long len);
    if(MD5_Update(&ctx, (unsigned char*)srcbuff, strlen(srcbuff)) == 0 )
    {
        printf("Error! MD5_Update() Error!\n");
        return false;
    }
    if(MD5_Final(md, &ctx) == 0) //最后把ctx中的数据按照MD5算法生成16位的MD5码，存放到buff中
    {
        printf("Error! MD5_Final() Error!\n");
        return false;
    }
    for(i=0; i< 16; i++)
    {
        sprintf(tmp, "%02X", md[i]);
        strcat(dstbuff, tmp);
    }
    //printf("The String is [ %s ]\nThe String MD5 is [ %s ]\n",srcbuff,dstbuff);
    return true;
}

bool DNSChecker::Fill_alert(char *DomainName,int len,cache_alert_t *dns_alert,struct net_element *e)
{
    /*alert_id*/
    u_int32_t tmpint = 0;
    if(!DNS_ID(&tmpint))
    {
        return false;   
    }
    itoa(tmpint,dns_alert->alert_id);
    
    /*plugin_id*/
    dns_alert->plugin_id = DnsDealPlugin_id;
    
    /*risk*/
    memcpy(dns_alert->risk,"12",3);
    
    /*trojan_id,非木马类告警可填写0*/           
    memcpy(dns_alert->trojan_id,"11111",6);
    
    /*alarm_time*/
    time_t mytime = 0;
    time(&mytime);
    struct tm *tm_p = localtime(&mytime);
    sprintf(dns_alert->alarm_time,"%d-%d-%d %d:%d:%d",(1900+tm_p->tm_year),(1+tm_p->tm_mon),(tm_p->tm_mday),(tm_p->tm_hour),(tm_p->tm_min),(tm_p->tm_sec));
    
    /*sip*/
    inet_ntop(AF_INET,&(e->srcip),(dns_alert->sip),sizeof(dns_alert->sip));

    /*dip 初始值 为0 如果在Clear之前抓到Dns服务器返回的应答包 则此初始值作废*/
    memcpy(dns_alert->dip,(char *)"0.0.0.0",strlen((char *)"0.0.0.0")+1);
    /*sport dport*/
    dns_alert->sport = ntohs(e->srcp);
    dns_alert->dport = ntohs(e->dstp);

    /*dmac 内部主机mac*/
    snprintf(dns_alert->dmac, sizeof(dns_alert->dmac), "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",e->srcmac[0], e->srcmac[1],e->srcmac[2], e->srcmac[3], e->srcmac[4], e->srcmac[5]);          
    
    /*protocol*/
    dns_alert->protocol = e->protocol;
    
    /*descry*/
    snprintf(dns_alert->descry,sizeof(dns_alert->descry),"%s",DomainName);

    /*md5*/
    if(!CreateMD5(DomainName,dns_alert->dnshash))
    {
        return false;
    }

    /*scc scc_flag由manager负责*/
    memcpy(dns_alert->scc,"0",1);   
    dns_alert->scc_flag='1';
    
    /*extra_text*/
    strcpy(dns_alert->extra_text,DomainName);   

    unsigned char *md5string = MD5((const unsigned char*)DomainName,len,NULL);

    char *md5hash = (char *)malloc(33);
    if(NULL == md5hash)
    {
        return false;
    }

    snprintf(md5hash, 33, "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x", md5string[0], md5string[1], md5string[2],md5string[3],md5string[4],md5string[5],md5string[6],md5string[7],md5string[8],md5string[9],md5string[10],md5string[11],md5string[12],md5string[13],md5string[14],md5string[15]);

    strncpy(dns_alert->dnshash,md5hash,strlen(md5hash));    
    strcpy(dns_alert->extra_text,DomainName);
    
    char geography[30] = {0};
    char flag;
    getAddr_ergodic(htonl(e->srcip), geography, &flag);
    strcpy(dns_alert->scc, geography);
    dns_alert->scc_flag = flag;

    cout<<"dns_alert->id:"<<dns_alert->alert_id<<endl;
    cout<<"alarm_time:"<<dns_alert->alarm_time<<endl;
    cout<<"dns_alert->sip:"<<dns_alert->sip<<endl;
    cout<<"dns_scc:"<<dns_alert->scc<<endl;
    cout<<"dns_flag:"<<dns_alert->scc_flag<<endl;
    cout<<"mac:"<<dns_alert->dmac<<endl;
    cout<<"7755:"<<dns_alert->descry<<endl;
    cout<<"dns_alert->extra_text:"<<dns_alert->extra_text<<endl;
    cout<<"dns_md5hash:"<<md5hash<<endl;

    return true;
}

void DNSChecker::UpdateAlertList(char *DomainBuf,unsigned int addr)
{
    //printf("DomainBuf::%s\n",DomainBuf);
    pthread_mutex_lock(&DnsAlert_mutex);
    u_int32_t hash = 0;
    GreateHash(DomainBuf,NULL,&hash);
    //cout<<"(2)Hash::"<<hash<<endl;
    map<u_int32_t ,cache_alert_t>::iterator l_it;; 
    l_it=DnsAlertMap.find(hash);
    //cout<<"zhaozhanfeng"<<endl;
    if(l_it != DnsAlertMap.end()) //
    {
        //  cout<<"tipfocus..........."<<endl;
        inet_ntop(AF_INET,&addr,(l_it->second.dip),sizeof(l_it->second.dip));
        //l_it->second.dip = addr;
        cacheclient.Send_Msg((char *)&(l_it->second),sizeof(cache_alert_t));
        //WtMemPool(&(l_it->second),sizeof(dnsblack_info)); //报警
        //  cout<<"tipfocus.........2"<<endl;
        DnsAlertMap.erase(l_it); //删除此报警记录
        //  cout<<"tipfocus.........3"<<endl;
    }
    pthread_mutex_unlock(&DnsAlert_mutex);
    //  cout<<"tipfocus.........4"<<endl;
    return;
}

int DNSChecker::UdpParse(struct ip* addr,struct net_element *e)
{
    struct udphdr *udp_addr = (struct udphdr *)((char*)addr+IP_HL(addr));
    char DomainName[DNLEN] = {0};
    int len = 0;
    int misSum = 0;
    int mis = 0;
    //ntohs()网络字节序转换为主机字节序
    if(ntohs(udp_addr->dest))
    {
        if((DNSPORT == ntohs(udp_addr->dest)) || (DNSPORT == ntohs(udp_addr->source)))//--DNS---    
        {
            struct dnshdr *dns_addr = (struct dnshdr *)((char *)udp_addr + 8);

            /*--------(1) FIXME 查询报文解析 用解析到的域名匹配策略中的黑域名 ---------*/
            if((0 == (((ntohs(dns_addr->mark)) & 0x8000) >> 15)) && (1 == (ntohs(dns_addr->nproblem))) && (ntohs(dns_addr->nresource)) == 0) //--2013.04.04 Night 过滤一条以上的
                /*2014.01.08 通常情况下查询报文的回答记录数为0*/
            {
                while(1)
                {
                    misSum += mis;
                    if(0 == GetDomainName(dns_addr,(char *)((char *)dns_addr + DNSHL + misSum),&mis,&len,DomainName))
                    {
                        break;
                    }
                }
                //cout<<"@@@@DomainName:"<<DomainName<<endl;
                //比对策略中的黑域名
                JudgeDNS(DomainName,addr,strlen(DomainName),e);
            }

            /*-----(2) FIXME 同样是DNS报文 解析响应报文 遍历报警链表中的域名比对 (成功->报警 并删除比对成功的)  否则返回继续 ------*/

            else if((1 == (((ntohs(dns_addr->mark)) & 0x8000) >> 15)) && (1 == (ntohs(dns_addr->nproblem))) && (ntohs(dns_addr->nresource)) <= 4 ) //应答报文
                /*过滤应答中应答资源记录大于4条的包 2014.01.08*/
            {
#if 0
                cout<<"(Response)@@@@"<<endl;
                int val = 0;

                struct dnshead *DnsHead = 0;
                DnsHead = (struct dnshead *)((char *)udp_addr + sizeof(struct udphdr));
#endif
                char DomainBuf[256] = {0};
                int  LenProblem = 0;
                //应答包中的 问题
                //for(val = 0;val < (dns_addr->nproblem);val++)//根据问题数跳过查询问题
                //{ 
                while(1)
                {
                    misSum += mis;
                    if(0 == GetDomainName(dns_addr,(char *)((char *)dns_addr + DNSHL + misSum),&mis,&len,DomainBuf))//遇到‘\0’ 结束 或者 拿到压缩域名后 结束
                    {
                        misSum += mis;
                        break;
                    }
                }
                LenProblem = misSum + 1;     // '\0'
                //cout<<"Response-----DomainBuf:"<<DomainBuf<<endl;

                //}

                //(资源记录 开始)
                //cout<<"LenProblem::::"<<LenProblem<<endl;
                char *BR = (char *)dns_addr + DNSHL + LenProblem + 4; //跳过查询问题的 查询类型 和 查询类 共4字节
                //printf("Init:::BR:%p\n",BR);
                //cout<<"(1)+++++++++++++++++,dns_addr->nresource:"<<ntohs(dns_addr->nresource)<<endl;
                // -- BR 此时 是指向 资源记录的指针 --- 
                int BRmisSum = 0; //记录每条资源记录的长度
                for(int i = 0;i < ntohs(dns_addr->nresource);i++,BR += BRmisSum)//跳过上一条资源记录的字节 开始新的资源记录
                {
                    //printf("XXXXXX:%p\n",BR);
                    //cout<<"BRmisSum:::"<<BRmisSum<<endl;
                    //cout<<"(2)+++++++++++++++++"<<endl;
                    misSum = 0; 
                    mis = 0;
                    len = 0;
                    if(0xC0 == (*BR & 0xC0)) //压缩 
                    {
                        //cout<<"(3)+++++++++++++++++"<<endl;
                        BRmisSum += 2; //记录 2 字节的压缩指针  
                        struct dns_response *Response = (struct dns_response *)(BR + 2); //跳过 2 字节的压缩指针
                        //printf("ntohs(Response->type):%u\n",ntohs(Response->type));
                        //cout<<"length:"<<ntohs(Response->length)<<endl;
                        if(1 == ntohs(Response->type)) //A:IP地址
                        {
                            //cout<<"(4)+++++++++++++++++"<<endl;
                            //struct in_addr addr; //资源数据length字节...
                            char IPBuff[256] = {0};

                            unsigned int *addr = (unsigned int *)((char *)Response + 10); //跳过+ 类型+ 类+ 生存时间+ 资源数据长度标识
                            //printf("XXXXXXXXXXxxx%u\n",*addr);;
                            inet_ntop(AF_INET,addr,IPBuff,sizeof(IPBuff));
                            //cout<<"Have Rar --> IPBuFFFFFFFFFFF:"<<IPBuff<<endl;
                            UpdateAlertList(DomainBuf,*addr);

                            //cout<<"tipfocus.........6"<<endl;
                            break;//跳出 循环找到一个IP地址就够了   
                        }
                        else //其他类型
                        {
                            //cout<<"(5)+++++++++++++++++"<<endl;
                            BRmisSum += (10 + ntohs(Response->length));
                        }
                    }
#if 0
                    else //全域名无压缩 或  有可能是尾域名有压缩
                    {
                        cout<<"(6)+++++++++++++++++"<<endl;
                        char DomainTmpBuf[256] = {0};
                        while(1) //跳过资源记录中的域名
                        {
                            misSum += mis;
                            printf("misSum::%d\n",misSum);
                            printf("BR--->:%p\n",BR);
                            if(0 == GetDomainName(dns_addr,(BR + misSum),&mis,&len,DomainTmpBuf))//遇到‘\0’ 结束 或者 拿到压缩域名后 结束
                            {
                                cout<<"(7)+++++++++++++++++"<<endl;
                                misSum += mis;
                                break;
                            }
                        }
                        BRmisSum += (misSum + 1 + 4); //记录 域名长度 加 ‘\0’ 加 类型 加 类
                        struct dns_response *Response = (struct dns_response *)(BR + misSum + 1 + 4);                                                                                                                                       //跳过 域名长度
                        printf(")ntohs(Response->type):%u\n",ntohs(Response->type));
                        cout<<")length:"<<ntohs(Response->length)<<endl;
                        //if(1 == ntohs(Response->type) || (1 == ntohs(Response->type))) //A:IP地址 AAA:IPV6
                        if(1 == ntohs(Response->type)) //A:IP地址 
                        {
                            char IPBuff[24] = {0};
                            cout<<"(8)+++++++++++++++++"<<endl;

                            //跳过+ 类型+ 类+ 生存时间+ 资源数据长度标识
                            unsigned int *addr = (unsigned int *)(char *)(Response + sizeof(dns_response)); 
                            inet_ntop(AF_INET,addr,IPBuff,sizeof(IPBuff));
                            cout<<"(NOne Rar -->)IPBuFFFFFFFFFFF:"<<IPBuff<<endl;
                            UpdateAlertList(DomainBuf,*addr);
                            break;//跳出 循环找到一个IP地址就够了   
                        }
                        else //其他类型
                        {
                            cout<<"(9)+++++++++++++++++"<<endl;
                            BRmisSum += (sizeof(dns_response) + (Response->length));
                        }
                    }
#endif
                    //cout<<"i:"<<i<<endl;
                }

            }
            //cout<<"tipfocus.........7"<<endl;
            return 0;
        }
    }
    return 1;
}

int DNSChecker::Start_Check(struct ip* addr, bool type, struct net_element *e)
{
    /*liuqiang code resolve warning*/
    if(0 == type)
    {
        type=0;
    }

#if 1
    u_int32_t hash = 0;
    const char* str = "www.google.com";
    GreateHash(str,NULL,&hash);

    DnameRigour.insert(pair<u_int32_t,string>(hash,"www.google.com"));
    cache_alert_t dns_alert; 
    memset(&dns_alert,0,sizeof(cache_alert_t));

    pthread_mutex_lock(&DnsAlert_mutex);
    DnsAlertMap.insert(pair<u_int32_t,cache_alert_t>(hash,dns_alert));
    pthread_mutex_unlock(&DnsAlert_mutex);
#endif

    if(17 == (addr->ip_p))
    {
        if(4 == IP_VR(addr))
        {
            if(UdpParse(addr,e))
            {
                return 1;       
            }
        }
        else if(6 == IP_VR(addr))
        {
        }
    }
    return 0;
}
