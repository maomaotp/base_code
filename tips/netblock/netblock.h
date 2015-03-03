#ifndef _HEAD_NETBLOCK
#define _HEAD_NETBLOCK

#define STOPBLOCK    "stopnetblock"
#define STARTBLOCK        "netblock"

#define UNIX_DOMAIN "/tmp/domain.block"

#define ETHERNET_TYPE_IP    0x0800

typedef struct _EtherHdr
{
    u_int8_t ether_dst[6];
    u_int8_t ether_src[6];
    u_int16_t ether_type;
}EtherHdr;

typedef struct _blockInfo
{
    in_addr_t srcip;
    in_addr_t dstip;
}blockinfo;

#endif

