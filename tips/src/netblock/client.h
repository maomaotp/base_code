#ifndef _CLIENT_H_
#define _CLIENT_H_
#define SERVERPORT 8224
	int Init_Client();//成功返回serverfd；失败返回小于0;
	void Exit_Client();
	int Send_Msg(char* buff, int len);
#endif

