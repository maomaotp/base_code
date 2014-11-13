#ifndef GETOIP_H
#define GETOIP_H

struct ipaddr
{
    char startip[15];
    char endip[15];
    char region[30];
    char territory[30];
    char flag;
};

void readIPfile();
void getAddr_ergodic(const unsigned int ip, char* geography, char* scc_flag);
void getAddr_dichotomy(const unsigned int ip, char* geography, char* scc_flag);

#endif
