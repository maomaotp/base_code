#ifndef _DATA_H__
#define _DATA_H__

#include <pcap.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <sys/un.h>
#include <netinet/if_ether.h>
#include <sys/types.h>
#include <stdlib.h>
#include <signal.h>
/*其他主进程间通信*/
#include "unixdomain.h"
#include "client++.h"
/*本进程内通信*/
#include "cmempool.h"

/*抓包分包模块*/
#include "capturepacket.h"

/*各自摸块注册*/
#include "checkobj.h"

/*从内存池获取数据并保存文件模块*/
#include "savefile.h"
#include "multikeymatch.h"
#include "wtpcap.h"

#include <iostream>
#include <list>
#include <map>


extern "C"
{
#include "init.h"
#include "client.h"
}

using namespace std;

#endif
