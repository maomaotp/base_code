/*FIXME 
 *GPIO light'API  Capsulation
 * 2013.10.14 
 * by yong 
 * */

#include "n6601.h"
#define OffFrequence 100000

/*define GPIO number*/
UINT8 gPort[100] = {17, 20, 18, 37};

/*Initiate GPIO*/
int  InitializeGPIO(UINT8 *gpio_num)
{

	/* change I/O privilege level */
	if(iopl(3)){
		perror("iopl");
		return -1;
	}

	Ich_GpioInitialization(gPort, *gpio_num);
	return 0;
}

/*On light and off light*/
int CtlLight(UINT8 *gpio_num,int flag)
{

	/*FIXME 
	 * flag:
	 *0: off light
	 *1: on  light
	 * */

	if((1 != flag) && (0 != flag))
	{
		printf("CtlLight's arg is failed!\n");
		return -1;
	}
	UINT8 level = (UINT8)flag;
	int i;
	for(i = 0; i < *gpio_num; i++){
		Ich_SetGpioMode(gPort[i], IO_MODE_OUT);
		Ich_SetGpiolevel(gPort[i], level);
	}
	printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!11\n");
	//sleep(1);
	usleep(500000);
	return 0;
}

int Glimpse(UINT8 *gpio_num,int cnt,int onspeed)
{
	int keep_live = 0;
	if(-1 == cnt)/*for ever*/
		keep_live = 1;	
	int i = cnt;
	while((keep_live) || ((i--) > 0))
	{
		CtlLight(gpio_num,1); /*ON*/
		//usleep(onspeed);
		CtlLight(gpio_num,0); /*OFF*/
		//usleep(OffFrequence);
	}
	return 0;
}

