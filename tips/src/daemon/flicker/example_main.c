
/*FIXME The use gives examples
 *
 *1 Circulating flicker or Gleam forever
 *2 a bright a put out
 *3 For ever ON
 *4 For ever OFF
 *
 *by yong 2013.10.14
 * 
 * */

#include "n6601.h"
int main(int argc,char *argv[])
{

	UINT8 gpio_num = sizeof(gPort) / sizeof(gPort[0]);
	if(-1 == InitializeGPIO(&gpio_num))
	{
		printf("Init_GPIO is err~!\n");
		return -1;
	}
	int onspeed = OnFrequence;/*default 50000*/

	/*FIXME "Circulating flicker or Gleam forever"*/
#if 0
	Glimpse(&gpio_num,10,5);/*10*/
	sleep(1);
#endif
	//Glimpse(&gpio_num,-1,5*onspeed / 1000);/*for ever*/
	Glimpse(&gpio_num,-1,1);/*for ever*/

	/*FIXME "a bright a put out"*/
	//CtlLight(&gpio_num,1); /*ON*/
#if 0
	sleep(1);
	CtlLight(&gpio_num,0); /*OFF*/
#endif

#if 0
	/*FIXME "For ever ON"*/
	CtlLight(&gpio_num,1); 

	/*FIXME "For ever OFF"*/
	CtlLight(&gpio_num,0);
#endif

	return 0;
}
