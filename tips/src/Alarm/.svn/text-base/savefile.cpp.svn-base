#include "savefile.h"
#include <iostream>
using namespace std;
void StartFileSave::Set_Mempool(CMempool* mem)
{
	mempool=mem;
}

void* filefun(void* pthobj)
{
	StartFileSave* filepthread=(StartFileSave*)pthobj;
	filepthread->Run_File();
	return NULL;
}

void StartFileSave::Start(pthread_t* pid)
{
		pthread_create(pid,NULL,filefun,(void*)this);
}

void StartFileSave::Run_File()
{
	printf("save file pthread is run....\n");
#if 1
	while(1){
		//	cout<<"读内存吃 写文件 线程函数执行!"<<endl;
		struct Unit* temp = NULL;
		temp = (struct Unit *)mempool->Mem_Read();
		if(NULL == temp)
		{
			continue;
		}
		char Name[FileNameSize] = {0};
		memcpy(Name,temp+1,FileNameSize);//File Name
		int used_size = temp->employ_size;								
		bool flag = false;
		if(JudgeType(Name,PcapFlag)) //是Pcap文件
		{
			flag = true;
		}
		char *pp = (char *)(temp + 1);
		WtFile(pp,Name,used_size,flag);
		mempool->Readed_Set();
	}
#endif
	return ;
}

bool StartFileSave::JudgeType(char *Name,char *sub)
{
	if(strstr(Name,sub) || strchr(Name,'&'))
	{
		return true;	
	}
	return false;
}
void StartFileSave::WtFile(char *temp,char *name,int used_size,bool type)
{
	int Flag = 0;
	char pathbuff[2 * FileNameSize] = {0};
	sprintf(pathbuff,"%s%s",FileDIR,name);
	FILE *fp = NULL;
	if(type){
		//		cout<<"==================================================pathbuff::"<<pathbuff<<endl;
		if(-1 == access(pathbuff,F_OK)){//不存在此文件
			Flag = 1;
			//cout<<"NOT sex :::"<<pathbuff<<endl;
		}
		if(0 == (fp = fopen(pathbuff,"ab"))){
			perror("(1)fopen a");	
			fclose(fp);
			fp = 0;
			return;
		}
		if(Flag){//不存在才写PCAP头
			WtPFH(fp);
		}
	}
	else{
		if(0 == (fp = fopen(pathbuff,"ab"))){
			perror("(2)fopen a");	
			fclose(fp);
			fp = 0;
			return;
		}
	}
	WtContent(fp,temp,used_size);

	fclose(fp);
	fp = 0;
	return;
}
void StartFileSave::WtContent(FILE *fp,char *temp,int used_size)
{
	fwrite((char *)temp + FileNameSize,used_size - FileNameSize,1,fp);			
	return ;
}
void StartFileSave::WtPFH(FILE *fp)
{
	wtpcap->WtPcapFileH(fp,0);
}
