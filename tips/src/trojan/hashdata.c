#include "hashdata.h"
#include "hash.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pcap.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <openssl/md5.h>
#include <stdio.h>
#include <arpa/inet.h>
#include "cachestruct.h"
#include "juageIP.h"
#include "init.h"

#define HASHSIZE 420000
#define RULESIZE 800

#define TIMEE 180

struct horserule  **hashtable;
cache_alarm_t trojan;


static struct pcap_file_header filehead;
extern struct filerule *rule[RULESIZE];

void clearhash(int alarmTime)
{
    alarmTime = 180;
    int i=0;
    time_t now=0;
    struct horserule *temp=NULL;
    struct horserule *pos=NULL;
    dzlog_debug("clear hash table");
    for(;i<HASHSIZE;i++){
        temp=hashtable[i];
        time(&now);
        if(temp && stre(now,temp,&pos)){
            if(!pos->prev && pos->next){
                pos->next->prev=NULL;
                hashtable[i]=pos->next;
                temp=pos->next;
            }
            else if(pos->next && pos->prev){
                pos->next->prev=pos->prev;
                pos->prev->next=pos->next;
            }
            else if(!pos->next && pos->prev){
                pos->prev->next=NULL;
            }
            else if(!pos->next && !pos->prev){
                hashtable[i]=NULL;
            }

            clearstream(pos);
            printf("clear\n");
            pos=NULL;
        }
    }

    clearunhash();
    printf("done\n");
    alarm(alarmTime);
    
    return;
}


bool stre(time_t now,struct horserule *begin,struct horserule **pos)
{
   struct horserule *temp=NULL;
   temp=begin;
   
   while(temp)
   {
      if(now - temp->last >= TIMEE)
      {
         *pos=temp;
         return true;
      }

      temp=temp->next;
   }

   return false;
}


void createHashTable()
{
  void *temp=calloc(HASHSIZE,sizeof(struct horserule));
  if(!temp)
  {
    dzlog_info("calloc memory");
    exit(-1);
  }
  hashtable=(struct horserule **)temp;
}



void initpacphead()
{
    filehead.magic=0xa1b2c3d4;
    filehead.version_major=0x0002;
    filehead.version_minor=0x0004;
    filehead.thiszone=0;
    filehead.sigfigs=0;
    filehead.snaplen=0x0000ffff;
    filehead.linktype=0x00000001;
}

void savestream(struct horserule *save,const struct pcap_pkthdr *head,const u_char *packet)
{
    write(save->flow->filefd,head,sizeof(struct pcap_pkthdr));
    write(save->flow->filefd,packet,head->caplen);
    save->last=time(NULL);
}

void enterprocess(const struct pcap_pkthdr *head,const u_char *packet)
{
    uint32_t position=0;
    uint32_t location=0;
    uint32_t hashtemp=0;
    uint32_t temp=0;
    struct iphdr *ip=NULL;
    struct tcphdr *tcp=NULL;

    ip=(struct iphdr *)(packet+14);
    tcp=(struct tcphdr *)(packet+14+ip->ihl*4);
   
    temp=(tcp->source)<<16|(tcp->dest);
    hashtemp=temp;
    position=jhash_3words(ip->saddr,ip->daddr,temp,0)%HASHSIZE;

    memset((void*)&temp,0,sizeof(temp));
    temp=(tcp->dest)<<16|(tcp->source);
    location=jhash_3words(ip->daddr,ip->saddr,temp,0)%HASHSIZE;
     
    int result=0;
    struct horserule *pos=NULL;
    uint32_t index=0;
    //  检测数据包的TCP流是否在已保存的木马hash表中，当存在时保存数据流中所有数据包，不存在时会分析是否符合木马规则中的任意一个，如何符合就把数据包增加到数据流中  position/location :key  hashtemp:源端口和目的端口组成的key result:是否存在stream中 pos：木马结构体指针  index：hash表下标 
    if(searchstream(position,location,ip,hashtemp,&result,&pos,&index))
    {
        if(NULL != pos && pos->flow->isTrojanHorse)
        {
            if(tcp->fin || tcp->rst)
            {
                close(pos->flow->filefd);
                pos->flow->filefd=0;
                freethisstream(pos,index);
                pos=NULL;
                return;
            }
            else
            {
                savestream(pos,head,packet);
            }
        }
        else
        {
            processstream(pos,ip,packet);
        }
    }
    else{
        if(result == 1){
            createhashvalue(position,ip,packet);  
        }
        else{
            pos=hashtable[position];
            while(NULL != pos && NULL != pos->next){
                pos=pos->next;
            }
            if(NULL == pos){
                addrulehorse(hashtable[position],ip,tcp,packet);
            }
            else{
                addrulehorse(pos,ip,tcp,packet);
            }
        }
    }
}

//检测消息包长度和内容，判断是否符合木马特征
void processstream(struct horserule *cur,struct iphdr *ip,const u_char *packet)
{
    uint32_t datalength=0;
    int tempcurrent=0;
    uint32_t min=0;
    uint32_t max=0;
    u_char *data=NULL;

    tempcurrent=cur->regulation->current;
    min=cur->regulation->vect[tempcurrent*3]-cur->regulation->vect[tempcurrent*3+1];
    max=cur->regulation->vect[tempcurrent*3]+cur->regulation->vect[tempcurrent*3+1];
    //获取消息中的数据长度  
    datalength=ntohs(ip->tot_len)-ip->ihl*4-20;

    if(!(datalength >= min && datalength <= max)){
        return;
    }

    if(cur->regulation->vect[tempcurrent*3+2]){
        data=(u_char *)(void *)(packet+14+ip->ihl*4+20+cur->regulation->begin); 
        
        if(0 == memcmp(data,cur->regulation->content,cur->regulation->length/2)){
            cur->flow->content=true;
            designtre(cur,packet);
        }
        else{
            return;
        }
    }
    else{
        designtre(cur,packet);
    }

    return;
}

void designtre(struct horserule *tr,const u_char *packet)
{
    tr->regulation->current += 1;
    tr->last=time(NULL);

    if(tr->regulation->current == tr->regulation->totallength){
        alar(tr,packet);
        tr->flow->isTrojanHorse=true;
    }

    return;
}

void alar(struct horserule *save,const u_char *packet)
{

    static uint32_t counter=1;
    if(0 == counter)
    {
        counter=1;
    }

    if(NULL == save)
    {
        return;
    }

    struct iphdr *ipo=NULL;
    struct tcphdr *tcp=NULL;

    ipo=(struct iphdr *)(packet+14);
    tcp=(struct tcphdr *)(packet+14+ipo->ihl*4);

    char srcip[128]={0};
    char destip[128]={0};
    if(NULL == inet_ntop(AF_INET,&(ipo->saddr), srcip, 128))
    {
        dzlog_error("inet_ntop error");
        return;
    }
    if(NULL == inet_ntop(AF_INET,&(ipo->daddr), destip, 128))
    {
        dzlog_info("inet_ntop error");
        return;
    }
    strncpy(trojan.trojan_id, save->regulation->identifier, 5);
    strcpy(trojan.sip, srcip);
    strcpy(trojan.dip, destip);

    time(&(trojan.alarm_time));
    strcpy(trojan.type,save->regulation->type);
    strncpy(trojan.dmac,save->dmac,18);
    trojan.risk='2';
    trojan.head.type=ALARM;
    sprintf(trojan.alarm_id,"%u",counter);
    strcpy(trojan.descry,save->regulation->name);

    memset(srcip,0,128);
    memset(destip,0,128);
    strcpy(srcip,"cat /usr/local/tipschecker/etc/deviceid.conf | cut -d'=' -f2");
    FILE *file=popen(srcip,"r");
    fread(destip,1,128,file);
    pclose(file);
    file=NULL;
    destip[strlen(destip)-1]='\0';
    strcpy(trojan.device_id,destip);

    /*liuq code IP_gogrophy*/
    char geography[30] = {0};
    char flag;
    getAddr_ergodic(htonl(ipo->saddr), geography, &flag);

    strcpy(trojan.scc, geography);
    trojan.scc_flag = flag;

    Send_Msg((char* )&trojan, sizeof(trojan));
    writefile(save);

    dzlog_debug("/*********trojan*************/\ntrojan_descry:%s\nstrojan_id:%s\n trojan_name:%s\n trojan_time:%ld\n trojan_type:%s\nsrcip:%s destip:%s\n mac:%s...geography:%s\nsend to cache success\n/*********end********\n", trojan.descry, trojan.trojan_id, save->regulation->name, trojan.alarm_time, trojan.type, trojan.sip, trojan.dip, trojan.dmac, geography);

    memset((void *)&trojan,0,sizeof(cache_alarm_t));
    counter++;
}

void writefile(struct horserule *save)
{
    time_t filename=0;
    filename=time(NULL);
   
    char buffer[128]={0};
 
    if(NULL == save)
    {
        return;
    }

    snprintf(buffer,sizeof(buffer), "/data/file/alarm/1000_%ld_%s_%s_%s_%s.pcap",filename,trojan.device_id,trojan.alarm_id,trojan.trojan_id,trojan.type);
 
    save->flow->filefd=creat(buffer, O_CREAT|O_APPEND|O_RDWR);
    write(save->flow->filefd,&filehead, sizeof(struct pcap_file_header));

    return;
}

void freethisstream(struct horserule *freestr,uint32_t index)
{
    struct horserule* temp=NULL;
    if(NULL == freestr){
        return;
    }
    temp=freestr;

    if(NULL == freestr->prev && NULL != freestr->next){
        freestr->next->prev=NULL;
    }
    else if(NULL != freestr->prev && NULL == freestr->next){
        freestr->prev->next=NULL;
    }
    else if(NULL != freestr->prev && NULL != freestr->next){
        freestr->prev->next=freestr->next;
        freestr->next->prev=freestr->prev;
    }
    else{
        hashtable[index]=NULL;
    }
  
    clearstream(temp);
    temp=NULL;
}

void clearstream(struct horserule *freestr)
{
    free(freestr->message);
    freestr->message=NULL;
    free(freestr->flow);
    freestr->flow=NULL;
    free(freestr->regulation->content);
    freestr->regulation->content=NULL;
    free(freestr->regulation);
    freestr->regulation=NULL;
    free(freestr);
}


void addrulehorse(struct horserule *end,struct iphdr *ip,struct tcphdr *tcp,const u_char *packet)
{
   uint32_t datalength=0;
   datalength=ntohs(ip->tot_len)-ip->ihl*4-20;

    if(datalength >= RULESIZE){
        dzlog_info("the ip->tot_len is out of RULESIZE");
        return;
    }

    if(!rule[datalength]){
        dzlog_info("the rule datalength is NULL");
        return;
    }
   
    uint32_t min=0;
    uint32_t max=0;
    struct filerule *temp=rule[datalength];

    while(NULL != temp){
        min=temp->vect[0] - temp->vect[1];
        max=temp->vect[0] + temp->vect[1];

        if(!((datalength >= min) && (datalength <= max))){
            temp=temp->next;
            continue;
        }   
        u_char *data=NULL;
        data=(u_char *)(void *)(packet+14+ip->ihl*4+20+temp->begin);


        if(temp->vect[2]){
            if(0 == memcmp(data,temp->content,temp->length/2)){
                addhorse(end, ip, tcp, temp);
                end->next->flow->content=true;
                break;
            }
            else{
                temp=temp->next;
                continue;
            }
        }
        else{
            addhorse(end, ip, tcp, temp);
            break;
        }

        temp=temp->next;
    }
}


void addhorse(struct horserule *end, struct iphdr *ip, struct tcphdr *tcp, struct filerule *pos)
{
    uint32_t forwardwords=0;
    uint32_t reservewords=0;

    struct horserule *temp=NULL;

    forwardwords=(tcp->source)<<16|(tcp->dest);
    reservewords=(tcp->dest)<<16|(tcp->source);

    unsigned char forwardmessage[16]={0};
    unsigned char reservemessage[16]={0};

    createmd5(ip,forwardwords,forwardmessage,reservewords,reservemessage);
    
    temp=(struct horserule *)calloc(1,sizeof(struct horserule));
    if(NULL == temp){
        dzlog_info("addhorse calloc fail!!!");
        return;
    }
    temp->regulation = (struct filerule *)calloc(1,sizeof(struct filerule));
    if(NULL == temp->regulation){
        dzlog_info("addhorse calloc fail!!!");
        return;
    }
    temp->flow = (struct stream *)calloc(1,sizeof(struct stream));
    if(NULL == temp->flow){
        dzlog_info("addhorse calloc fail!!!");
        return;
    }
    temp->prev = NULL;
    temp->next = NULL;
    temp->last = time(NULL);
    temp->message = (unsigned char *)calloc(1,32);
    if(NULL == temp->message){
        dzlog_info("addhorse calloc fail!!!");
        return;
    }
    memcpy(temp->message,forwardmessage,16);
    memcpy(temp->message+16,reservemessage,16);
 
    strcpy(temp->regulation->identifier,pos->identifier);
    strcpy(temp->regulation->name,pos->name);
    temp->regulation->length=pos->length;
    temp->regulation->content=(char*)calloc(1,128);
    strcpy(temp->regulation->content,pos->content);
    temp->regulation->begin=pos->begin;
    temp->regulation->current=pos->current;
    temp->regulation->totallength=pos->totallength;

    int i;
    for(i=0;i<9;i++){
        temp->regulation->vect[i]=pos->vect[i];
    }

    end->next=temp;
    temp->prev=end;
}

void createhashvalue(uint32_t position,struct iphdr* ip,const u_char* packet)
{
    uint32_t datalength=0;
    datalength=ntohs(ip->tot_len)-ip->ihl*4-20;

    if(datalength>=RULESIZE || !(rule[datalength])){
        return;
    }

    uint32_t min=0;
    uint32_t max=0;
    struct filerule* temp=NULL;
   
    temp=rule[datalength];

    while(temp)
    {
        min=temp->vect[0] - temp->vect[1];
        max=temp->vect[0] + temp->vect[1];

        if(!((datalength >= min) && (datalength <= max)))
        {
            temp=temp->next;
            continue;
        }

        u_char *data=NULL;
        data=(u_char *)(void *)(packet+14+ip->ihl*4+20+temp->begin);

        if(temp->vect[2])
        {
            if(0 == memcmp(data,temp->content,temp->length/2))
            {
                create(position,packet,datalength,temp);
                hashtable[position]->flow->content=true;
                if(hashtable[position]->regulation->current == hashtable[position]->regulation->totallength)
                {
                    alar(hashtable[position], packet);     
                    //writefile(hashtable[position]);
                    hashtable[position]->flow->isTrojanHorse=true;
                }
                break;
            }
            else
            {
                temp=temp->next;
                continue ;
            }
        }
        else
        {
            dzlog_debug("create hashtable");
            create(position,packet,datalength,temp);
            break;
        }
        temp=temp->next;
    }
}

void getlocalether(in_addr_t *src,in_addr_t *dest,u_char **temp,struct ether_header *ether)
{
  if(ipmarkoff(src))
  {
     *temp=(ether->ether_shost);
     return ;
  }

  if(ipmarkoff(dest))
  {
    *temp=ether->ether_dhost;
    return ;
  }
     *temp=ether->ether_shost;
}

void create(uint32_t hashpos, const u_char *packet, uint32_t datalength, struct filerule *pos)
{
    struct horserule *temp=NULL;
    struct iphdr *ip=NULL;
    struct tcphdr *tcp=NULL;
    struct ether_header *ether=NULL;
    int n=ETHER_ADDR_LEN;
    u_char *p=NULL;
    

    ip=(struct iphdr *)(packet+14);
    tcp=(struct tcphdr *)(packet+14+ip->ihl*4);
    ether=(struct ether_header*)packet;

    temp = (struct horserule *)calloc(1,sizeof(struct horserule));
    if(NULL == temp){
        dzlog_info("create hash calloc fail%d!!!", datalength);
        return;
    }
    temp->regulation = (struct filerule *)calloc(1,sizeof(struct filerule));
    if(NULL == temp->regulation){
        dzlog_info("create hash calloc fail!!!");
        return;
    }
    temp->flow = (struct stream *)calloc(1,sizeof(struct stream));
    if(NULL == temp->flow){
        dzlog_info("create hash calloc failed!!!");
        return;
    }
    temp->prev = NULL;
    temp->next = NULL;
    temp->last = time(NULL);
    temp->message = (unsigned char *)calloc(1,32);
    if(NULL == temp->message){
        dzlog_info("create hash calloc fail!!!");
        return;
    }
    uint32_t forwardwords=0;
    forwardwords=(tcp->source)<<16|(tcp->dest);

    uint32_t reservewors=0;
    reservewors=(tcp->dest)<<16|(tcp->source);

    unsigned char forwardmessage[16]={0};
    unsigned char reservemessage[16]={0};
    createmd5(ip,forwardwords,forwardmessage,reservewors,reservemessage);
    memcpy(temp->message,forwardmessage,16);
    memcpy(temp->message+16,reservemessage,16);

    strcpy(temp->regulation->identifier,pos->identifier);
    strcpy(temp->regulation->name,pos->name);
    temp->regulation->length=pos->length;
      
    temp->regulation->content=(char*)calloc(1,128);
    if(NULL == temp->regulation->content){
        dzlog_info("create hash calloc fail!!!");
        return;
    }
    memcpy(temp->regulation->content,pos->content,128);

    temp->regulation->begin=pos->begin;
    temp->regulation->current=pos->current;
    temp->regulation->totallength=pos->totallength;
    strcpy(temp->regulation->type,pos->type);

    getlocalether((in_addr_t *)(&(ip->saddr)), (in_addr_t *)(&(ip->daddr)), &p, ether);
    char byt[3];
    do
    {
        byt[0]=convertbyt((char)(*p>>4));
        byt[1]=convertbyt((char)(*p & 0x0F));
        byt[2]='-';
        p++;
        strncat(temp->dmac,byt,3);
    }while(--n>0);

    temp->dmac[17]='\0';
    int i;
    for(i=0;i<9;i++){
       temp->regulation->vect[i]=pos->vect[i];
    }

    hashtable[hashpos]=temp;
    temp->regulation->current+=1;
}   

char convertbyt(char t)
{
   if(t>=0 && t<=9)
   {
     return t+48;
   }
   else
       return t+55;
}

void packmd5(char *data,unsigned char *value)
{
   MD5_CTX ctx;
   MD5_Init(&ctx);
   MD5_Update(&ctx,data,strlen(data));
   MD5_Final(value,&ctx);
}


void createmd5(const struct iphdr *ip,uint32_t hashtemp,unsigned char *digest,uint32_t hashtep,unsigned char *summary)
{
    char str[128]={0};
    snprintf(str,sizeof(str),"%u%u%u",ip->saddr,ip->daddr,hashtemp);
    packmd5(str,digest);

    if(summary){
        memset(str,0,sizeof(str));
        snprintf(str,sizeof(str),"%u%u%u",ip->daddr,ip->saddr,hashtep);
        packmd5(str,summary);
    }
}

bool searchstream(uint32_t position,uint32_t location,struct iphdr *ip,uint32_t hashtemp,int *result,struct horserule **pos,uint32_t *index)
{
    if(NULL == hashtable[position] && NULL == hashtable[location])
    {
        *result=1;
        return false;
    }

    struct horserule *temp=NULL;
    unsigned char mess[16]={0};
    //mess 源目的IP/端口组成的一个key 
    createmd5(ip,hashtemp,mess,0,NULL);

    if(NULL != hashtable[position])
    {
        temp=hashtable[position];
        if(NULL != temp && NULL != temp->message)
        {
            if(memcmp(temp->message,mess,16)==0 || memcmp(temp->message+16,mess,16)==0)
            {
                *pos=temp;
                *index=position;
                return true;
            }
        }
        temp=temp->next;
    }

    if(NULL != hashtable[location])
    {
        temp=hashtable[location];
        if(NULL != temp && NULL != temp->message)
        {
            if(memcmp(temp->message,mess,16)==0 || memcmp(temp->message+16,mess,16)==0)
            {
                *pos=temp;
                *index=location;
                return true;
            }
        }
        temp=temp->next;
    }   
    return false;
}

