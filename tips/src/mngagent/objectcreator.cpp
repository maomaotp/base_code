#include "datatask.h"
#include "statustask.h"
#include "syntask.h"
#include "reporttask.h"
#include "objectcreator.h"
#include "audittask.h"
#include "backuptask.h"

	void ObjectCreator::CreatTask(int* classid,ITimerTask** timetask)
	{
		for(int i=0;i<CLASSCOUNT;++i)
		{
			if(classid[i]==0)break;
			switch(classid[i])
			{
				case 1:
				{
					timetask[i] = new DataTask;
					break;
				}
				case 2:
				{
					timetask[i] = new ReportTask;
					break;
				}
				case 3:
				{
					timetask[i] = new SynTask;
					break;
				}
				case 4:
				{
					timetask[i] = new StatusTask;
					break;
				}
				case 5:
				{
					timetask[i] = new AuditTask;
					break;
				}
				case 6:
				{
					timetask[i] = new BackupTask;	
				}
				default:
				{
					break;
			//		cout<<"没有当前id类"<<endl;
				}
			}
		}
	}
