#include "mngchkpostman.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
using namespace std;
void MngChkPostman::Init_Postman(const char * domain)
{
	saddr.sun_family=AF_UNIX;
	strcpy(saddr.sun_path,domain);
}
int MngChkPostman::Send_Msg(char* buff,int len,char* rbuf,int rlen)
{
//	Init_Postman(domain);
//	cout<<"调用进程见通信"<<endl;
	chksock=socket(PF_UNIX,SOCK_STREAM,0);
	if(chksock<0)
	{
	//	cout<<"创建socket失败!"<<endl;
		return 0;
	}
	int i=0;
	while(connect(chksock,(struct sockaddr*)&saddr,sizeof(saddr))<0)
	{
		if(i++==10)
		{
			close(chksock);
			return 0;
		}
	//	cout<<"链接失败!"<<endl;
	}
	write(chksock,buff,len);
	//cout<<"postman==="<<buff<<endl;
	if(rbuf)
	{
		int rdlen=read(chksock,rbuf,rlen);
		close(chksock);
		return rdlen;
	}
	else 
	{
		close(chksock);
		return 0;
	}
}

int MngChkPostman::Send_Msg2(const char* domain, char* buff,int len,char* rbuf,int rlen, int flag)
{
	Init_Postman(domain);
	cout<<"buffer========="<<buff <<endl;
//	cout <<"len=============================="<<len<<endl;
	chksock=socket(PF_UNIX,SOCK_STREAM,0);
	if(chksock<0)
	{
		cout<<"创建socket失败!"<<endl;
		return 0;
	}
	int i=0;
	while(connect(chksock,(struct sockaddr*)&saddr,sizeof(saddr))<0)
	{
//		cerr <<"connect error:"<<strerror(errno)<<endl;
		if(i++==10)
		{
			close(chksock);
			return 0;
		}
		cout<<"链接失败!"<<endl;
	}
	cout<<"调用进程间通信"<<endl;

//	write(chksock,(void*)&len, sizeof (int));

	/*
	int ret = ioctl (chksock, FIONBIO, 0);
	if (-1 == ret)
		cout<<strerror(errno)<<endl;*/	
	write(chksock,(void*)&len, sizeof (int));
	cout <<"len=============================="<<len<<endl;
	int buflen = 200 * 1024;
	if(buflen > len)
	{
		int c = 0;
		while (c < len)
		{
	   	c = write(chksock, buff, len - c);
	  	len -= c;
	  	buff += c;
		}
	}
	else
	{
		while (len > 0)
		{
			if(len < buflen)
			{
				printf("left        len = %d\n", len);
				int n = write(chksock, buff, len);
				len -=n;
				buff +=n;
				printf("left        len = %d\n", len);
				if(len <= 0)
					break;
				continue;
			}
			cout<<"len======"<<len<<endl;
			cout<<"buff============"<<buff<<endl;
		//	int n = write(chksock, buff, len);
			int n = write(chksock, buff, buflen);
			cout<<"n=========="<<n<<endl;
			len -= n;
			buff += n;
			printf("left len = %d\n", len);
			//usleep (50);
		}
	}
	printf("send end\n");
#if 0
	int rdlen = 0;
	cout <<"len1111111111=============================="<<endl;
	if(1 == flag)
	{
#if 0
		while (rdlen < rlen)
		{
			cout <<"len2222222=============================="<<endl;
			rdlen=read(chksock,rbuf,rlen - rdlen);
			rdlen += rdlen;
			cout <<"len2222222=============================="<<endl;
		}
		//	sleep(10);
#endif
		close(chksock);
	cout <<"len2222222=============================="<<endl;
		return rdlen;
	}
	else 
	{
	//	sleep(10);
		close(chksock);
		return 0;
	}
#endif
		close(chksock);
		cout <<"post end" <<endl;
		
		rbuf = rbuf; rlen = rlen; flag = flag; 		//hyj 保留参数
		
		return 0;
}
