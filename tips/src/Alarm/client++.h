#ifndef _CLIENT_H____
#define _CLIENT_H____
#define SERVERPORT 8224
class CCacheclient{
	private:
		int serverfd;
	public:
		//int Init_Client(char* serverip);//成功返回serverfd；失败返回小于0;
		bool Init_Client(char* serverip);//成功返回serverfd；失败返回小于0;
		void Exit_Client();
		int Send_Msg(char* buff,int len);
};
#endif

