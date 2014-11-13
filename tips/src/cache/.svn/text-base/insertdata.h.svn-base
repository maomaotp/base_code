#ifndef _INSERTDATA_H_
#define _INSERTDATA_H_

#include "JudgePartition.h"
#include <iostream>
#include "sqlhandler.h"

using namespace std;

class Insertdb{
	public:
		void Set_DB();
		~Insertdb();
		void Insertdata(char *sql);
	private:
		Sqlhandler * sqlhandler;
		JudgePartition partition;
		long num;//插入几个数
		int is_succed;//判断插入是否成功
};

#endif
