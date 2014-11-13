#ifndef FILEPTHREAD_H
#define FILEPTHREAD_H
#include "cmempool.h"
#include "wtpcap.h"
#include <iostream>
using namespace std;
#define BUFFSIZE 256
#define FileNameSize 256
#define IPSize 16
//#define FileDIR ((char *)"/tipschecker/")
#define FileDIR ((char *)"")
#define PcapFlag ((char *)".pcap")

class StartFileSave{
		CMempool* mempool;
		Pcap * wtpcap;
		public:
		void Run_File();
		void Set_Mempool(CMempool* mem);
		void Start(pthread_t* pid);
		bool JudgeType(char *Name,char *sub);
		void WtFile(char *temp,char *pathbuff,int used_size,bool type);
		void WtContent(FILE *fp,char *temp,int used_size);
		void WtPFH(FILE *fp);
};
#endif
