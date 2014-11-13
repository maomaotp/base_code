/*
 *2013.11.02 by yong
 * */

#include "JudgePartition.h"
void CallBack(char **result,int column,void *param)
{
		/*
		 * FIXME test
		 int i = 0;
		 while(i < column)
		 printf("result = %s\n",result[i++]);
		 */
		*(int *)param = atoi(result[0]);

		column = column;		//hyj 保留参数		

		return;
}
bool JudgePartition::JudgeADD(Sqlhandler *mysql)
{
		/*Get Curr date*/
		char currdate[1024] = {0};
		char _currdate[1024] = {0};
		char *format = (char *)"\%Y\%m\%d";
		char *_format = (char *)"\%Y-\%m-\%d";
		time_t clock = 0;
		struct tm *tm = 0;

		time(&clock);
#if 0
		tm = gmtime(&clock);
#endif
		tm = localtime(&clock);
		strftime(currdate,sizeof(currdate),format,tm);
		strftime(_currdate,sizeof(_currdate),_format,tm);
		printf("currdate:%s\n",currdate);
		sleep(4);
		/*Judge*/
		char sql[1024] = {0};
		snprintf(sql,sizeof(sql),"select count(*) from INFORMATION_SCHEMA.PARTITIONS where TABLE_NAME = '%s' and PARTITION_NAME = 'p%s';",TIPS_NETLOG,currdate);
		int ccount = 0;	
		if(mysql->Select_Sql_Exec(sql,CallBack,(void *)(&ccount)) < 0)
		{
				printf("(1)Judge partition exec-select-sql is err!\n");	
				return false;
		}
		/*ADD*/
		if(0==ccount)
		{
				snprintf(sql,sizeof(sql),"ALTER TABLE %s ADD PARTITION (PARTITION p%s VALUES LESS THAN ('%s 23:59:59'));",TIPS_NETLOG,currdate,_currdate);	
				if(mysql->Insert_Sql_Exec(sql) < 0)
				{
						printf("Judge partition exec-alter-sql is err!\n");	
						/*比自己大的有没有*/
						snprintf(sql,sizeof(sql),"select count(*) from INFORMATION_SCHEMA.PARTITIONS where TABLE_NAME = '%s' and PARTITION_NAME > 'p%s';",TIPS_NETLOG,currdate);
						if(mysql->Select_Sql_Exec(sql,CallBack,(void *)(&ccount)) < 0)
						{
								printf("(2)Judge partition exec-select-sql is err!\n");	
								return false;
						}
						/*没有但也创建不了分区*/
						if(ccount < 0)
								return false;
				}
		}
		return true;
}

