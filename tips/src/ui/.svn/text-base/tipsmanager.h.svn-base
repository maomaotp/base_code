#ifndef TIPSMANAGER_H
#define TIPSMANAGER_H
#include <stdio.h>
#include <sys/vfs.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <iostream>
#include "gsad_base.h"


class TipsManager{
int Get_Mem();
int Get_Cpu();
double Get_Disk();
char* Memtotal;
char* Cached;
char* Memfree;
char* Buffers;
double total, cached, buffers, memfree;
public:
void status_Init(struct status_struct *status);
double disk;
int mem, cpu;
};
#endif
