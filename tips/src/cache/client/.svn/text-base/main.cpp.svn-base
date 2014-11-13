#include "client.h"
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
using namespace std;
int main()
{
	CCacheclient cacheclient;
	if (cacheclient.Init_Client("172.16.0.107") <= 0)
		cout<<"初始化客户端失败!"<<endl;
	else 
		cout<<"初始化客户端成功!"<<endl;
	char buff[1024];
	memset(buff, 0, sizeof (buff));
	sleep(2);
	for (int i = 0; i < 10250000; ++ i)
	{
		usleep(10);
		sprintf(buff, "4 this is test data %d.\n", i + 1);
		cout << "发送字节数:"<< cacheclient.Send_Msg(buff, 1024) << i << endl;
	}
	cout << "发送完成!" << endl;
	cacheclient.Exit_Client();
}
