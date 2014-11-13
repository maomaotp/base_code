#include "cachehandle.h"
#include <sys/socket.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
using namespace std;

bool CCachehandler::Set_Non_Block(int sockfd)
{
	int opts = fcntl(sockfd, F_GETFL);
   if(-1 == opts)
   {
      printf("%d->fcntl F_GETFL is faild!\n",getpid());
      return false;
   }
   opts = opts | O_NONBLOCK;
   if (fcntl(sockfd, F_SETFL, opts) < 0)
   {
      printf("%d->fcntl F_SETFL is faild!\n",getpid());
      return false;
   }
   return true;
}

int CCachehandler::Read_Data(int fd, void* buff, int len)
{
	int read_len = 0, temp = 0;
	while (len > read_len)
	{
		temp = read(fd, (u_int8_t*)buff + read_len, len - read_len);
		if (temp <= 0)
			return -1;
		read_len += temp;
	}
	return read_len;
}

int CCachehandler::Read_Data_Len(int fd)
{
	int len;
	int* temp = &len;
	int err = Read_Data(fd, temp, sizeof (len));
	if (err <= 0)
		return err;
	return len;
}

int CCachehandler::Deal_Data(void* data, int len)
{
	int data_len = len;
	int err = memcache.Mem_Write(data, &data_len);
	if (err == -1)
	{
		cout<<"给的buff为空!"<<endl;
		return -1;
	}
	else if (err <= 0)
	{
		struct Data_Cache cache_data;
		cache_data.data = malloc(len);
		cache_data.len = len;
		if (cache_data.data)
			eturn -2;
		memcpy(cache_data.data, data, len);
		pthread_mutex_lock(&list_mutex);
		cache_list.push_back(cache_data);
		pthread_mutex_unlock(&list_mutex);
	}
	else 
	{
		if (cache_list.size())
		{
			pthread_mutex_lock(&list_mutex);
			list<Data_Cache>::iterator beg = cache_list.begin();
			int err = memcache.Mem_Write(beg->data, &(beg->len));
			if (err != -1)
			{
				free(beg->data);
				cache_list.erase(beg);	
			}
			pthread_mutex_unlock(&list_mutex);
		}
		return 1;
	}
}

int CCachehandler::Deal_Client_Data(int sockfd)
{
	int len = Read_Data_Len(sockfd);	
	if (len <= 0)
		return -1;
	u_int8_t data[len];
	int err = Read_Data(sockfd, data, len);
	if (err <= 0)
		return -1;
	//处理数据
	Deal_Data(data, len);
	return err;
}

int CCachehandler::Init_Socket(int listenquerenum)
{
	listenfd = socket(PF_INET, SOCK_STREAM, 0);
	if (listenfd <= 0)
	{
		cout<<"创建服务器套接字失败!"<<endl;
		return -1;
	}	
	//允许地址重用
	int reuse = 1;
	setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof (reuse));
	bool err = Set_Non_Block(listenfd);
	if (err == false)
	{
		cout<<"设置非阻塞失败!"<<endl;
		return -2;
	}
	struct sockaddr_in serveraddr;
	bzero(&serveraddr, sizeof (serveraddr));
	serveraddr.sin_family = PF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
//	inet_aton("172.16.0.107",&(serveraddr.sin_addr));
	serveraddr.sin_port = htons(SERVERPORT);
	int berr = bind(listenfd, (struct sockaddr*)&serveraddr, sizeof (serveraddr));
	if (berr < 0)
	{
		cout<<"绑定主机IP失败!"<<endl;
		return -3;
	}
	berr = listen(listenfd, listenquerenum);
	if (berr < 0)
	{
		cout<<"设置监听队列失败!"<<endl;
		return -4;
	}
	return listenfd;
}

int CCachehandler::Epoll_Connect()
{
	cout<<"Epoll_Connect............"<<listenfd<<endl;
	struct sockaddr_in clientaddr;
	socklen_t client = sizeof (clientaddr);
	int client_connect = 0;	
	client_connect = accept(listenfd, (struct sockaddr*)&clientaddr, &client);
	if (client_connect < 0)
	{
		cout<<"accept faild!"<<client_connect<<endl;
		return 0;
	}
	Set_Non_Block(client_connect);
	struct epoll_event ev;
	ev.data.fd = client_connect;
	ev.events = EPOLLIN | EPOLLET;
	epoll_ctl(epfd, EPOLL_CTL_ADD, client_connect, &ev);
	return 1;
}

int CCachehandler::Epoll_In(struct epoll_event* ev)
{
	int tempfd = 0;
	if ((tempfd = ev->data.fd) < 0)
	{
		cout<<"当前可读链接失效!"<<endl;
		return -1;
	}
	//调用函数处理数据
	int readerr = Deal_Client_Data(tempfd);
	if (readerr == -1)
	{
		cout<<"对方关闭链接!"<<endl;
		close(tempfd);
		ev->data.fd=-1;
	}
	else
	{
		memset(ev, 0, sizeof (ev));
		ev->data.fd = tempfd;
		ev->events = EPOLLIN | EPOLLET;
	}
	epoll_ctl(epfd, EPOLL_CTL_MOD, tempfd, ev);
	return 1;
}

int CCachehandler::Epoll_Out(struct epoll_event* ev)
{
	int tempfd = 0;
	tempfd = ev->data.fd;
	memset(ev, 0, sizeof (ev));
	ev->data.fd = tempfd;
	ev->events = EPOLLIN | EPOLLET;
	epoll_ctl(epfd, EPOLL_CTL_MOD, tempfd, ev);
	return 1;
}

void CCachehandler::Productor_Do()
{
	struct epoll_event events[CLIENTNUM] = {0};
	int nfds = 0;
	while(1)
	{
		nfds = epoll_wait(epfd, events, CLIENTNUM - 1,-1);
		for (int i = 0; i < nfds; ++ i)
		{
			if (events[i].data.fd == listenfd)
			{
				cout<<"监听fd接受到链接请求!"<<endl;
				Epoll_Connect();
			}	
			else if (events[i].events & EPOLLIN)
			{//可读返回
				cout<<"客户端发来数据!"<<endl;
				Epoll_In(&(events[i]));
			}
			else if (events[i].events & EPOLLOUT)
			{
				cout<<"链接可读!"<<endl;
				Epoll_Out(&(events[i]));
			}
		}
	}
}

void* pro_fun(void* pobj)
{
	CCachehandler* cache = (CCachehandler*)pobj;
	cache->Productor_Do();
}

int CCachehandler::Start_Productor()
{
	if (!pthread_create(&proid, NULL, pro_fun, this))
		return 1;
	else 
		return 0;
}

int CCachehandler::Pop_Data()
{
	char buff[DATAUNITLEN];
	int len = DATAUNITLEN;
	int read_cong = 1;
	while (1)
	{
		len = DATAUNITLEN;
		int err = memcache.Mem_Read(buff, &len);
		if (err == 0)
		{
			cout<<"当前内存池没有数据!"<<endl;
			return 1;
		}
		else if (err == -1)
		{
			cout<<"buff=======NULL!"<<endl;
			break;
		}
		else if (err == -2)
		{
			cout<<"读取buff长度不够!"<<endl;
			if (read_cong > 50)
				read_cong = 2;
			read_cong += 1;
			len = len * read_cong;
			char temp[read_cong * DATAUNITLEN];
			int temperr = memcache.Mem_Read(temp, &len);
			if (temperr > 0)
			{
				cout<<"处理数据......."<<temp<<endl;
				//处理数据
			}
		}
		else
		{
			cout<<"处理数据2......."<<buff<<endl;
				//处理数据
		}
	}
}

void CCachehandler::Consumer_Do()
{
	while (1)
	{
		int err = memcache.Mem_Change(second);
		cout<<"交换返回"<<err<<endl;
		if (err == -1)
		{
			cout<<"当前读取没有完成!"<<endl;
			//继续读取
			Pop_Data();
			continue;
		}
		else if (err == 0)
		{
			cout<<"当前没有数据可以读取!"<<endl;
			continue;
		}
		cout<<"交换成功开始读取"<<endl;
		Pop_Data();
	}

}

void* csm_fun(void* pobj)
{
	CCachehandler* cache = (CCachehandler*)pobj;
	cache->Consumer_Do();
}

int CCachehandler::Start_Consumer(int sec)
{
	second = sec;
	if (!pthread_create(&csmid, NULL, csm_fun, this))
		return 1;
	else 
		return 0;
}

int CCachehandler::Init_Server(int listenqnum, int cache_size)
{
	//初始化私有变量
	pthread_mutex_init(&list_mutex, NULL);
	cache_flag = false;
	//初始化socket服务器
	int init_err = Init_Socket(listenqnum);
	if (init_err < 0)
	{
		cout<<"init socket err!"<<init_err<<endl;
		return -1;
	}
	//初始化缓存池
	if (!memcache.Mem_Init(cache_size, NULL))
	{
		cout<<"init memcache filed!"<<endl;
		return -2;
	}
	//初始化epoll
	epfd = epoll_create(EPOLLMAXFD);
	struct epoll_event ev;
	bzero(&ev, sizeof(ev));
	ev.data.fd = listenfd;
	ev.events = EPOLLIN | EPOLLET;
	int perr = epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev);
	if (-1 == perr)
	{
		cerr << strerror(errno) <<endl;
		return -3;
	}
	return 1;
}

int CCachehandler::Start_Server(int sec)
{
	int err = Start_Productor();
	if (err <= 0)
	{
		cout<<"start productor failed!"<<err<<endl;
		return -1;
	}
	err = Start_Consumer(sec);
	if (err <= 0)
	{
		cout<<"start consumer failed!"<<err<<endl;
		return -2;
	}
	return 1;
}

int CCachehandler::Exit_Server()
{
	close(listenfd);
	close(epfd);
	pthread_mutex_destroy(&list_mutex);
	memcache.Mem_Exit();
	return 1;
}

void CCachehandler::Wait_Exit()
{
	pthread_join(proid,NULL);
	pthread_join(csmid,NULL);
}
