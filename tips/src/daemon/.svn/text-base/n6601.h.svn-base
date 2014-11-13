#ifndef _N6601_H_
#define _N6601_H_

#include <stdio.h>
#include <sys/io.h>
#include <unistd.h>
#include <stdlib.h>
#include "ich8.h"
#define OnFrequence 50000

/*by yong 2013.10.14*/

extern UINT8 gPort[100];

/*Initiate GPIO*/
int  InitializeGPIO(UINT8 *gpio_num);

/*FIXME 
 *Func: On light and off light*
 *args: 
 *flag
 *	0: off light
 *	1: on  light
 * */
int CtlLight(UINT8 *gpio_num,int flag);



/*
 *FIXME 
 *Func: Circulating flicker
 *args:
 *cnt
 *      -1     :for ever
 *	cnt > 0:cnt times 
 *opspeed: 
 *	The frequency of flicker,default "OnFrequence"
 * */
int Glimpse(UINT8 *gpio_num,int cnt,int onspeed);

#endif
