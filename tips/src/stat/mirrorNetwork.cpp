#include "mirrorNetwork.h"

extern char eth2[];
extern char eth3[];
void getworkstatus(workmode_t &mirror)
{
	if(eth2 != NULL && strlen(eth2))
	{
		mirror.eth0_status=1;
	}
	else
	{
		mirror.eth0_status=2;
	}
	if(eth3 != NULL && strlen(eth3))
	{
		mirror.eth1_status=1;
	}
	else
		mirror.eth1_status=2;
}


