#include "insertdata.h"
#include <stdio.h>
//extern pthread_mutex_t sqlite_sync_mutex;
//extern int startrun;
//extern CTips_sqlite_sync tip_sqlte_sync;
void Insertdb::Set_DB()
{
	sqlhandler = Sqlhandler::getInstance();
	bool rc = sqlhandler->Init_Sql((char *)"127.0.0.1", (char *)"root", (char *)"123456", (char *)"tips",0,NULL,0);
  	if(!rc)
  	{
   	   cout<<"sqlhandler Init_Sql failed\n"<<endl;
  	}
	if(partition.JudgeADD(sqlhandler))
	{
		printf("create mysql part success\n");
	}
	num = 0;
	is_succed = 1;
//	pthread_mutex_lock (&sqlite_sync_mutex);
		sqlhandler->Begin();
//		startrun = 1;
//	pthread_mutex_unlock (&sqlite_sync_mutex);
//	}
}

Insertdb::~Insertdb()
{
//	pthread_mutex_lock (&sqlite_sync_mutex);
	if(is_succed){
		sqlhandler->Commit();
	}
	else{
		sqlhandler->Rollback();
	}
//	startrun = 0;
//	pthread_mutex_unlock (&sqlite_sync_mutex);
	sqlhandler->Exit_Sql();
}

void Insertdb::Insertdata(char *sql)
{
//	printf("sql====%s\n",sql);
	 //	pthread_mutex_lock (&sqlite_sync_mutex);
	 	int ret = sqlhandler->Insert_Sql_Exec(sql);
	 	if(-1 == ret){
	 		is_succed = 0;
	 		printf("数据库插入失败!\n");
	 	}
	 	else
	 	{
//	 		printf("数据库插入成功！\n");
	 	}
	 //	pthread_mutex_unlock (&sqlite_sync_mutex);
	 	if(0 == num%10){
	 //	pthread_mutex_lock (&sqlite_sync_mutex);
	 		if(is_succed){
	 			printf("开始提交!!\n");
	 			bool res = sqlhandler->Commit();
	 			printf("提交完成!");
	 			if ( false == res)
	 			{
	 				cout <<"database error"<< endl;
	 			}
				num = 0;
	 		}
	 		else{
	 			printf("回滚！");
	 			sqlhandler->Rollback();
	 			is_succed = 1;
	 			printf("回滚结束!");
	 		}
	 //	startrun = 0;
	 //	pthread_mutex_unlock (&sqlite_sync_mutex);
	 //	pthread_mutex_lock (&sqlite_sync_mutex);
	 	sqlhandler->Begin();
	 //	startrun = 1;
	 //	pthread_mutex_unlock (&sqlite_sync_mutex);
	 	}
	 	num++;
	 	//cout << "nbbbbb=== "<< num << endl;
	 	//
}
