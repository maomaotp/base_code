#include "funclist.h"
#include "hashdata.h"
#include "UnTrojan.h"
#include "init.h"

struct func *head=NULL;

void addfunc()
{
   head=(struct func*)calloc(1,sizeof(struct func));
   if(NULL == head)
   {
       dzlog_info("head calloc failed!!!");
       return;
   }
   head->process=enterprocess;
   
   head->next=(struct func*)calloc(1,sizeof(struct func));
   if(NULL == head->next)
   {
       dzlog_info("head->next calloc failed!!!");
       free(head);
       return;
   }
   head->next->process=enterunkprocess;

   return;
}

