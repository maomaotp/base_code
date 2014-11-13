#ifndef _WTEND_H__
#define _WTEND_H__
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>
#include <math.h>
#include <signal.h>
#include <iostream>
using namespace std;

#define TopLimit 4294967295UL
#define NUM_FILE (char *)"./ID.txt"
//#define PIDFILE (char *)"/usr/local/tipschecker/etc/alarmid"
#define PIDFILE (char *)"/usr/local/tipschecker/etc/tipscheck/alarmid"
#define CONFIGFILE (char *)"/usr/local/tipschecker/etc/tipscheck/device.conf"
#define DEVKEY (char *)"DEVICE_ID"
#define BUFFSIZE 256
#define DEVIDNUM 11
extern u_int32_t ID_NUM;
extern char LOCALDEVID[11];

bool ParID(int fd,u_int32_t *id);//读
void OpeID(bool flag);
void WtEnd(int);
void Outinit();


#endif
