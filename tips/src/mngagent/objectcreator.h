#ifndef OBJECTCREATOR_H
#define OBJECTCREATOR_H
#include "itimertask.h"
#define CLASSCOUNT 6 
class ObjectCreator{
	public:
	void CreatTask(int* classid,ITimerTask** timetask);
};
#endif
