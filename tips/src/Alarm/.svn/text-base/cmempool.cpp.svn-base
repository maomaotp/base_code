#include "cmempool.h"
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "data.h"

using namespace std;
bool CMempool::Mempool_Init(int unit_size,int unit_count,int type)
{
    pthread_mutex_init(&mutex,0);
    pthread_cond_init(&cond,0);
    int block_size=(unit_size+sizeof(struct Unit))*unit_count+sizeof(struct Head)+20;
    writeflag=true;
    head=(struct Head*)malloc(block_size);
    if(!head) {
		dzlog_info("malloc failed!!!");
		return false;
	}
    unit_free=unit_size;
    memset(head,0,block_size);
    head->block_size=block_size;
    head->already=0;
    head->unit_size=unit_size+sizeof(struct Unit);
    head->unit_count=unit_count;
    head->first_unit=(struct Unit*)(head+1);
    head->type=type;
    head->last_unit=(struct Unit*)((char*)head+sizeof(Head)+head->unit_size*(unit_count-1));
    PRead=PWrite=(struct Unit*)(head+1);
    PBRead=PBWrite=head;
    struct Unit* unit_temp;
    for(int i=0;i<head->unit_count;++i) {
        unit_temp=(struct Unit*)((char*)PWrite+(i*(head->unit_size)));
        unit_temp->free_size=unit_size;
    }
    return true;
}//初始化函数已没问题


void CMempool::Mempool_Destroy()
{
    struct Head* p=head->next_block;
    while(head) {
        free(head);
        head = p;
        if(head)p = head->next_block;
        else break;
    }
}//注销函数已经没问题

struct Head* CMempool::Expand_Mempool()
{
    struct Head* temp=(struct Head*)malloc(head->block_size);
    if(!temp) {
		dzlog_info("malloc failed!!!");
		return NULL;
	}
    memset(temp,0,head->block_size);
    temp->block_size=head->block_size;
    temp->already=0;
    temp->type=head->type;
    temp->unit_size=head->unit_size;
    temp->unit_count=head->unit_count;
    temp->last_unit=(struct Unit*)((char*)temp+sizeof(struct Head)+(head->unit_size)*(head->unit_count-1));
    temp->first_unit=(struct Unit*)(temp+1);
    struct Unit* unit_temp;
    for(int i=0;i<temp->unit_count;++i) {
        unit_temp=(struct Unit*)((char*)temp+sizeof(struct Head)+(temp->unit_size)*i);
        unit_temp->free_size=temp->unit_size-sizeof(struct Unit);
    }
    struct Head* p=head;
    while(p->next_block)
    {
        p=p->next_block;
    }
    p->next_block=temp;
    return temp;
}//扩容函数已经没问题

bool CMempool::Deal_PWrite()
{
    if(head->type){}
    else
    {
        struct Head* temp=head;
        while(temp)
        {
            if(temp->already<temp->unit_count)
            {
                PWrite=temp->first_unit;
                while(PWrite!=temp->last_unit)
                {
                    if(!PWrite->flag)
                    {
                        PBWrite=temp;
                        return true;
                    }
                    PWrite=(struct Unit*)((char*)PWrite+head->unit_size);
                }
            }
            temp=temp->next_block;
        }
        temp=Expand_Mempool();
        if(!temp)return false;
        PWrite=temp->first_unit;
        PBWrite=temp;
        return true;
    }
    return false;
}
int CMempool::Mem_Write(void* buff, unsigned int len)
{
    if(head->type==UNIT_DYNAMIC){}
    else if(head->type==UNIT_STATIC)
    {
        pthread_mutex_lock(&mutex); 
        if(!Deal_PWrite()) {
            dzlog_info("error!!!");
			pthread_mutex_unlock(&mutex);
            return -1;
        }
        if(len>PWrite->free_size) {
            pthread_mutex_unlock(&mutex);
            return 0;
        }
        char* p=(char*)PWrite+sizeof(struct Unit);
        memcpy(p,buff,len);
        PWrite->employ_size=len;
        PWrite->flag=1;
        PBWrite->already++; 
        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&cond);
        return 1;
    }
    return -1;
}
int CMempool::Mem_Write2(char* name,void* buff,int len)
{
    if(head->type==UNIT_DYNAMIC){}
    else if(head->type==UNIT_STATIC) {
        pthread_mutex_lock(&mutex); 
        if(!Deal_PWrite()) {
            dzlog_info("error!!!");
			pthread_mutex_unlock(&mutex);
            return -1;
        }
        if((len+strlen(name)) > PWrite->free_size)
        {
            pthread_mutex_unlock(&mutex);
            return 0;
        }
        char* p=(char*)PWrite+sizeof(struct Unit);
        memcpy(p,name,strlen(name));
        memcpy(p+256,buff,len);
        PWrite->employ_size=len+256;
        PWrite->flag=1;
        PBWrite->already++; 
        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&cond);
        return 1;
    }
    return -1;
}
void CMempool::Destroy_Mempool()
{
    struct Head* temp=head,*temp2=head;
    //找到最后一个
    while(temp->next_block)
    {
        temp2=temp;
        temp=temp->next_block;
        if(temp2==head)continue;
        free(temp2);
    }
    free(temp);
    head->next_block=NULL;
}
bool CMempool::Deal_PRead()
{
    if(head->type){}
    else {
        struct Head* temp=head;
        while(temp) {
            if(temp->already>0) {
                PRead=temp->first_unit;
                while(PRead!=temp->last_unit) {
                    if(PRead->flag) {
                        PBRead=temp;
                        return true;
                    }
                    PRead=(struct Unit*)((char*)PRead+head->unit_size);
                }
            }
            temp=temp->next_block;
        }   

        //Destroy_Mempool();
        return false;
    }
    return false;
}
void* CMempool::Mem_Read()
{
    if(head->type==UNIT_DYNAMIC){}
    else if(head->type==UNIT_STATIC)
    {
        pthread_mutex_lock(&mutex);
        if(Deal_PRead())
        {
            pthread_mutex_unlock(&mutex);
            return PRead;
        }
        else 
        {   
            pthread_cond_wait(&cond,&mutex);
            pthread_mutex_unlock(&mutex);
            return NULL;
        }
    }
    return NULL;
}
void CMempool::Readed_Set()
{
    PRead->flag=0;
    PBRead->already--;
    PRead->employ_size=0;
    PRead->free_size=PBRead->unit_size-sizeof(struct Unit);
}
