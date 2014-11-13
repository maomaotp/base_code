#include  "policy.h"
#include  <string.h>
#include  <iostream>
using namespace std;

void mycallback(u_char *user,const struct pcap_pkthdr *hdr,const u_char *bytes)
{
   if(juage_policy(bytes))
   {
 //  printf("ok\n");
   }
}

int main()
{
  pcap_t *handle=NULL;

  init_policy();
  handle=pcap_open_live("eth0",1600,1,0,NULL);
  pcap_loop(handle,-1,mycallback,NULL);
  return 0;
}
