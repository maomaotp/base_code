#ifndef PTHREADDATA_H
#define PTHREADDATA_H
#include "itipschecker.h"
#include <list>
#include <pthread.h>
using namespace std;
struct check_struct{
	ITipsChecker* checker;
	bool operator<(check_struct& chk)
	{
		if(checker->Get_Type()<chk.checker->Get_Type())return true;
		else return false;
	}
	bool operator==(check_struct& chk)
	{
		if(checker->Get_Type()==chk.checker->Get_Type())return true;
		else return false;
	}
};
#endif
