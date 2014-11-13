#ifndef HASH_H
#define HASH_H
#include <sys/types.h>
#include <netinet/in.h>

#define JHASH_GOLDEN_RATIO 0x9E3779B9

 
#define  __jhash_mix(srcip,destip,port)\
{\
  srcip -= destip;     srcip -= port;      srcip ^= (port>>13);\
  destip -= port;      destip -= srcip;    destip ^= (srcip<<8);\
  port -= srcip;       port -= destip;     port ^= (destip>>13);\
  srcip -= destip;     srcip -= port;      srcip ^= (port>>12);\
  destip -= port;      destip -= srcip;    destip ^= (srcip<<16);\
  port -= srcip;       port -= destip;     port ^= (destip>>5);\
  srcip -= destip;     srcip -= port;      srcip ^= (port>>3);\
  destip -= port;      destip -= srcip;    destip ^= (srcip<<10);\
  port -= srcip;       port -= destip;     port ^= (destip>>15);\
}

inline uint32_t jhash_3words(uint32_t srcip,uint32_t destip,uint32_t port,uint32_t initval)
{
   srcip += JHASH_GOLDEN_RATIO;
   destip += JHASH_GOLDEN_RATIO;
   port += initval;
   
   __jhash_mix(srcip,destip,port);

   return port;
}

inline size_t BKDRHash(const char *str)
{
  register size_t hashvalue=0;
  size_t ch=0;
  while((ch = (size_t)*str++))
  {
     hashvalue=hashvalue*131 + ch;
  }
  return hashvalue;
}
#endif
