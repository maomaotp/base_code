#ifndef MANAGECOMMAND_H
#define MANAGECOMMAND_H
#include <time.h>
#include <list>
#include <netinet/in.h>
#include <sys/types.h>

#include "cachestruct.h"

typedef struct _Block_ADDR
{
    in_addr_t  startIP;
    in_addr_t  endIP;
}BlockAddr;

typedef struct _PORT
{
    std::list<in_port_t>  port;
}PORT;

typedef struct _BLOCKORDER
{
    BlockAddr       inSideIp;
    BlockAddr       outSideIp;
    PORT            insidePort;
    PORT            outsidePort;
    time_t          times;
    time_t          createtime;
}BlockOrder;

#endif
