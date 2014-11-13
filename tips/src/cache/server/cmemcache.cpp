#include "cmemcache.h"
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <sys/time.h>
using namespace std;

int CMemcache::Init_Head(struct Mem_Head* memhead)
{
	memset(memhead,0,size);
	memhead->read_sign = memhead->write_sign; 
	memhead->write_sign = (struct Mem_Unit*)(memhead + 1);
	memhead->write_sign->free_len = memhead->cache_size;
	memhead->cache_size = size - sizeof (struct Mem_Head) - sizeof (struct Mem_Unit) * 2;//*2保证最后一个存在
	memhead->current_count = 0;
}

int CMemcache::Cast_Cond()
{
	return pthread_cond_broadcast(&cond);
}

int CMemcache::Wait_Cond(int sec)
{
	struct timeval now;
	struct timespec tsp;
	gettimeofday(&now, NULL);
	tsp.tv_sec = now.tv_sec;
	tsp.tv_nsec = now.tv_usec*1000;
	tsp.tv_sec += sec;
	pthread_mutex_lock(&cond_mutex);
	int err = pthread_cond_timedwait(&cond, &cond_mutex, &tsp);
	pthread_mutex_unlock(&cond_mutex);
	return err;
}

int CMemcache::Mem_Init(int cache_size, void* param)
{
	read_head = NULL;
	write_head = NULL;
	pthread_mutex_init(&mutex, 0);
	pthread_mutex_init(&cond_mutex, 0);
	pthread_cond_init(&cond, 0);
	change_flag = 0;//是否存在正在写内存池的线程
	write_flag = false;//当前可写
	read_finish = true;//当前读数据内存池已经没有数据可读
	int block_size = sizeof (struct Mem_Unit) * cache_size + sizeof (struct Mem_Head) + cache_size * 1024;
	size = block_size;
	write_head = head_one;
	head_one = (struct Mem_Head*)malloc(block_size);
	if (write_head == NULL)
		return 0;
	read_head = head_two;
	head_two = (struct Mem_Head*)malloc(block_size);
	if (read_head == NULL)
		return 0;
	Init_Head(head_one);
	Init_Head(head_two);
	return 1;
}

int CMemcache::Mem_Exit()
{
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&cond);
	free(head_one);
	free(head_two);
}

int CMemcache::Mem_Read(void* buff, int* len)
{
	if (read_head->current_count == 0)
	{
		if (read_finish == false)
			read_finish = true;
		return 0;
	}
	if (read_finish)
		return 0;//当前读取已经完成
	if (buff == NULL)
		return -1;
	if (read_head->read_sign->data_len > *len)
		return -2;//当前buff长度不够
	memcpy(buff, (read_head->read_sign + 1), read_head->read_sign->data_len);//拷贝数据
	*len = read_head->read_sign->data_len;
	read_head->current_count --;
	if (read_head->current_count == 0)
	{
		read_finish = true;
		cout<<"数据读取完成,重新设置缓冲区!"<<endl;
		Init_Head(read_head);//重新初始化读取内存池
	}
	else
	{
		read_head->read_sign = read_head->read_sign->next;
	}
	return 1;
	//读取完成
}

int CMemcache::Mem_Write(void* buff, int* len)
{
	//判断当前剩余空间是否够写
	if (write_flag)
	{
		cout<<"缓冲区不能写入!"<<endl;
		return 0;//当前不能写入
	}
	if (buff == NULL)
		return -1;//buff为空
	pthread_mutex_lock(&mutex);
	if ((write_head->write_sign->free_len) <= *len)
	{
		pthread_mutex_unlock(&mutex);
		Cast_Cond();//给读取线程发送写满信号
		return -2;//当前空间不够写
	}
	change_flag --;//设置当前在写标志
	void* write_temp = (void*)(write_head->write_sign + 1);
	write_head->write_sign->next = (struct Mem_Unit*)(((u_int8_t*)write_temp) + *len);//找到下一个单元起始地址
	write_head->write_sign->next->free_len = write_head->write_sign->free_len - sizeof (struct Mem_Unit) - *len;
	write_head->write_sign->data_len = *len;
	write_head->write_sign = write_head->write_sign->next;
	write_head->current_count ++;
	pthread_mutex_unlock(&mutex);
	memcpy(write_temp, buff, *len);
	change_flag ++;
	return 1;
}

int CMemcache::Mem_Change(int sec)
{
	if (sec != 0)
	{
		int err = Wait_Cond(sec);
		if (err == 0)
			cout<<"等到交换条件!"<<endl;
		else 
			cout<<"时间超时返回!"<<endl;
	}
	if (!read_finish)
		eturn -1;//当前读取没有完成
	write_flag = true;
	while(1)
	{
		if (change_flag)//当前有线程往内存池写数据
		{
			cout<<"内存池正在写入，不能交换!"<<endl;
			usleep(1000);
			continue;
		}
		if (write_head->current_count <= 0)
		{
			write_flag = false;
			return 0;
		}
		if(read_head == head_one)
		{
			read_head = head_two;
			write_head = head_one;
			write_flag = false;//设置可写标志
			read_finish = false;//设置当前读取没有完成
			return 1;
		}
		else if (read_head == head_two)
		{
			read_head = head_one;
			write_head = head_two;
			write_flag = false;//设置可写标志
			read_finish = false;//设置当前读取没有完成
			return 1;
		}
	}
	return 0;
}

int CMemcache::Mem_Read_All(int (*fun)(void*, void*), void* param)
{
	cout<<"1234567897"<<endl;
}
