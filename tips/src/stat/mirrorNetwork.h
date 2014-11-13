#ifndef MIRRORNETWORK_H
#define MIRRORNETWORK_H

#define NORMAL 1
#define NOTALL 2

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef  struct  _workmode
{
   int mode;
   int eth0_status;
   int eth1_status;
   double eth0_flow;
   double eth1_flow;
   double max_flow;
   int eth0_user_counter;
   int eth1_user_counter;
   int max_user_counter;
   int eth0_link;
   int eth1_link;
   int max_link;
}workmode_t;

void getworkstatus(workmode_t &mirror);
#endif
