#include "checkobj.h"
#include "ipblackchecker.h"
#include "dnschecker.h"
#include "init.h"

list<struct check_struct> iplist;
list<struct check_struct> tcplist;
list<struct check_struct> udplist;
pthread_mutex_t ipmutex,tcpmutex,udpmutex;
ITipsChecker* checker[CLASSCOUNT];

/* 由于是长期部署于网络环境中，所以此处new的对象没有对应的delete操作
 * case : 1-9:IP    10-19:TCP   20-29:UDP  
 */
bool CheckObj::CreatChecker(int* classid,ITipsChecker** checker)
{
	for(int i=0;i<CLASSCOUNT;++i)
	{
		switch(classid[i])
		{
#if 1
			case 2:/*ip黑名单*/
			{
				checker[i]=new IPBlackChecker;
				checker[i]->Set_Type(classid[i]);
				break;
			}
#endif
			case 4:/*DNS*/
			{
					checker[i]=new DNSChecker;
					checker[i]->Set_Type(classid[i]);
					break;
			}
/*暂时用不到的检查项位置*/
		    default:
			{
			    break;
			}

	    }
	}
	printf("create checker-object is ok!\n");	
	return true;
}

/*初始化线程锁*/
void init_pthread_mutex()
{
	pthread_mutex_init(&ipmutex,NULL);
	pthread_mutex_init(&tcpmutex,NULL);
	pthread_mutex_init(&udpmutex,NULL);
	return ;
}


/*FIXME
 * 由于是长期部署于网络环境中，所以此处的线程锁销毁函数和checker
 * 及ip\tcp\udp链表销毁函数没有调用...
 */

/*销毁线程锁*/
void destroy_pthread_mutex()
{
	pthread_mutex_destroy(&ipmutex);
	pthread_mutex_destroy(&tcpmutex);
	pthread_mutex_destroy(&udpmutex);
	return ;
}

/*checker和ip\tcp\udp销毁函数*/
void DestroyCheckerList()
{
	for(int i=0;i<CLASSCOUNT;++i)
	{
		if(checker[i])
		{
		    delete checker[i];
        }
	}
	iplist.clear();
	tcplist.clear();
	udplist.clear();
	return ;
}

/*初始化各检查项，调用CreatChecker将checker赋值*/
bool CheckObj::Init_Check()
{
	/*初始化ip\tcp\dup 链表的线程锁*/
	init_pthread_mutex();

	for(int i=0;i<CLASSCOUNT;++i)classid[i]=i+1;/*初始化classid*/
	return CreatChecker(classid,checker);/*初始化checker*/
}


/*通过checker各项将对应的检查项注册到ip\tcp\udp链表中*/
void CheckObj::RegisterAll()
{
	for(int i=0;i<CLASSCOUNT;++i)
	{
		if(checker[i])
		{
			checker[i]->Set_FileMem(&filemem);
			Register(checker[i]);
			cout<<"register obj is "<<i<<endl;
		}
	}
	dzlog_info("register check-object is ok");
	return ;
}

/*根据classid(每个itipschecker类的标号)除以10得到0\1\2从而注册到对应的ip\tcp\udp链表*/
void CheckObj::Register(ITipsChecker* check)
{
	struct check_struct check_data;
	check_data.checker=check;
	check_data.checker->Set_Exist(true);
	switch(check->Get_Type()/10)
	{
		case 0:/*IP链表*/
		{
			pthread_mutex_lock(&ipmutex);
			printf("register ip list \n");
			iplist.push_back(check_data);
			iplist.sort();
			pthread_mutex_unlock(&ipmutex);
			break;
		}
		case 1:/*tcp链表*/
		{
			pthread_mutex_lock(&tcpmutex);
			tcplist.push_back(check_data);
			tcplist.sort();
			pthread_mutex_unlock(&tcpmutex);
			break;
		}
		case 2:/*udp链表*/
		{
			pthread_mutex_lock(&udpmutex);
			udplist.push_back(check_data);
			udplist.sort();
			pthread_mutex_unlock(&udpmutex);
			break;
		}
		default:
		{
			printf("the register-obj is unknown!\n");
			break;
		}
	}
	return;
}

/*FIXME 
 * 每次只能解除tcp\ip\udp链表中的所有子模块
 */

void CheckObj::UnRegister(int type)
{
	list<struct check_struct>::iterator beg;
	switch(type/10)
	{
		case 0:
		{
			pthread_mutex_lock(&ipmutex);
			beg=iplist.begin();
			while(beg!=iplist.end())
			{
					if(beg->checker->Get_Type()==type)break;
					beg++;
			}
			if(beg!=iplist.end())
			{
					beg->checker->Set_Exist(false);
					iplist.erase(beg);
			}
			pthread_mutex_unlock(&ipmutex);
			break;
		}
		case 1:
		{
			pthread_mutex_lock(&tcpmutex);
			beg=tcplist.begin();
			while(beg!=tcplist.end())
			{
					if(beg->checker->Get_Type()==type)break;
					beg++;
			}
			if(beg!=tcplist.end())
			{
					beg->checker->Set_Exist(false);
					tcplist.erase(beg);
			}
			pthread_mutex_unlock(&tcpmutex);
			break;
		}
		case 2:
		{
			pthread_mutex_lock(&udpmutex);
			beg=udplist.begin();
			while(beg!=udplist.end())
			{
				if(beg->checker->Get_Type()==type)break;
				beg++;
			}
			if(beg!=udplist.end())
			{
				beg->checker->Set_Exist(false);
				udplist.erase(beg);
			}
			pthread_mutex_unlock(&udpmutex);
			break;
		}
		default:
		{
			if(checker[type-1]->Get_Exist())
			{
				checker[type-1]->Start_Check(NULL,false,NULL);
				checker[type-1]->Set_Exist(false);
			}
			break;
		}

	}
	return ;
}
