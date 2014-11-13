#ifndef AUDITTASK_H
#define AUDITTASK_H
#include "itimertask.h"
#include "statustask.h"
#include <iostream>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>

class AuditTask:public ITimerTask{
	void Deal_Task(char*,char* str);
	void Deal_CPU();
	void Deal_DISK();
	void Deal_MEM();
	ReportTask report;
	public:
	//	AuditTask();
		virtual void Start_Task();
		virtual void Init_Task();
};
#endif
