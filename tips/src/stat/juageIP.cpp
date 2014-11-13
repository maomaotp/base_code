#include "juageIP.h"

#define BIP 172
#define BIPa 16
#define BIPb 31
#define AIP  10
#define CIP  192
#define CIPa 168

bool ipmarkoff(in_addr_t *addr)
{
   in_addr_t  dst=0;
   dst=*addr;
		
   if(((BIP == (dst & 0xFF)) && ((((dst & 0xFF00) >> 8) >= BIPa) && (((dst & 0xFF00) >> 8) <= BIPb))) || (AIP == (dst&0xFF)) || ((CIP == (dst&0xFF)) && (CIPa == ((dst&0xFF00) >> 8))))
		{
				return true;
		}

   return false;
}
