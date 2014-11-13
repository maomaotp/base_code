#ifndef OINTERFACE_H
#define OINTERFACE_H
#include "data.h"
class OInterface{
	public:
	int Send_Data (void** alt, int len, int type);
	int Send_Config(struct config_struct* status,char* cmdbuff);
	int Send_Status(struct status_struct* status,char** cmdbuff);
	//int Send_Status(struct status_struct* status,char* cmdbuff);
	int Load_Rules(char* filename,void** rule_obj,int type);
	int Update_Sys(char* filename);
};
#endif
