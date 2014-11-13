#ifndef FUNCLIST_H
#define FUNCLIST_H

#include <stdbool.h>
#include <pcap.h>
#include <sys/types.h>
#include <stdlib.h>

typedef void (*Func)(const struct pcap_pkthdr *hdr,const u_char *data);

struct func
{
  Func process;
  struct func *next;
};

void addfunc();
#endif
