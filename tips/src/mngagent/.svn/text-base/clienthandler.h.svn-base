#ifndef _CLIENTHANDLER_H_
#define _CLIENTHANDLER_H_
#include <pthread.h>
#define SERVERPORT 8224
class CCacheclient{
	private:
	int serverfd;
	pthread_mutex_t mutex;
	public:
	int Init_Client(char* serverip);//成功返回serverfd；失败返回小于0;
	void Exit_Client();
	int Send_Msg(char* buff,int len);
};
#endif

