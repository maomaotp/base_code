#ifndef CheckObj_H_
#define CheckObj_H_
#include "itipschecker.h"
#include "pthreaddata.h"
#include "capturepacket.h"

extern CMempool filemem;

class CheckObj{
		private:
				int classid[CLASSCOUNT];
		public:
				bool CreatChecker(int* classid,ITipsChecker** checker);
				void UnRegister(int type);
				void Register(ITipsChecker* check);
				bool Init_Check();
				void RegisterAll();
};
#endif
