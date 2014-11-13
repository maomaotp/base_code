#include "reporttask.h"
extern char LOCALDEVID[];
extern int startrun;
struct sql_struct{
void* data;
int count;
};

void My_Strcpy(char* dest, char* src, int len)
{
	if (src == NULL)
		strncpy (dest, "", 1);
	else
		strncpy (dest, src, len - 1);
}
void My_equalc(char* c, char* str)
{
	if(NULL == str)
		*c = '\0';
	else
		*c = str[0];
}
void My_equal(uint32_t *n, char* p)
{
	if(NULL == p)
		*n = 0;
	else
		*n = atoi(p);
}
void ReportTask::Start_Task()
{
//上报木马报警信息


printf("**************************************ReportTask::Start_Task***********************************\n");
fflush(stdout);


//********************





//	void** cache = NULL;
//	char* w = NULL;
//	char* errMsg = NULL;
//	cache  = (void**)&w;
//	int nrow = 0, ncolumn = 0;
//	char** azResult = NULL;
//	tips_sqlite_sync.tips_lock (2);
//	Get_alarm (db, azResult, nrow, ncolumn, cache, errMsg);
	Get_alarm ();
// 	 azResult = NULL;
	Get_alert();
// 	 azResult = NULL;
//	Get_senstive_alert(db, azResult, nrow, ncolumn, cache, errMsg);
//	  azResult = NULL;
	Get_block();
//	  azResult = NULL;
//	Get_opt(db, azResult, nrow, ncolumn, errMsg);
	Get_opt();
//	  azResult = NULL;
//	tips_sqlite_sync.tips_unlock (2);
	Scan_Dir (REPORTFILE);
	
}
void ReportTask::Scan_Dir(const char* path)
{
	if(path==NULL)
	{
		printf("no path\n");
		return;
	}
	char* filename = NULL;
	struct dirent *dp;
	DIR* dfp;
	if (NULL == (dfp = opendir(path)))
	{
		fprintf (stderr, "%s not find path %s\n", strerror (errno), path);
		return ;
	//	exit(0);
	}
	while (NULL != (dp = readdir (dfp)))
	{
		if ((!strcmp (dp->d_name, ".")) || (!strcmp (dp->d_name, "..")) ||(!strcmp (dp->d_name, "Content")) || (!strcmp (dp->d_name, "pid")))
			continue;
		else
		{
			if (DT_REG == dp->d_type)
			{
				filename = new char[strlen(path) + strlen (dp->d_name) + 2];			
				strcpy (filename, path);
				strcat (filename, "/");
				strcat (filename, dp->d_name);
  	   			fprintf (stdout, "filename ================= %s\n", filename);
				int rc = UploadData(filename,0);
				if(-1 == rc)
				{	
					struct cache_opt_t opt_stut;
					INIT_OPT_HEAD(&opt_stut);
                    strcpy(opt_stut.username,"system");
                    time_t t;
                    time(&t);
                    funhandler->Turntime(t,opt_stut.opttime,24);
                    sprintf(opt_stut.describe,"%s:%s","上报文件失败",filename);
                    int send_err=cacheclient->Send_Msg((char*)&opt_stut,sizeof(opt_stut));
					send_err = send_err;		//hyj 未使用的变量
				}
				rc = UploadData(filename,1);
	//			cout << "rcfile =============" <<rc <<endl;
				char tp[1024];
				memset (tp, 0, sizeof(tp));
	//			unlink (filename);
				if(-1 == rc)
				{	
					struct cache_opt_t opt_stut;
					INIT_OPT_HEAD(&opt_stut);
                    strcpy(opt_stut.username,"system");
                    time_t t;
                    time(&t);
                    funhandler->Turntime(t,opt_stut.opttime,24);
                    sprintf(opt_stut.describe,"%s:%s","上报文件失败",filename);
                    int send_err=cacheclient->Send_Msg((char*)&opt_stut,sizeof(opt_stut));
					send_err = send_err;		//hyj 未使用的变量
	//				delete filename;
					closedir (dfp);
					return;
				}
			//	cout<<"22222222222222222222222222222222"<<endl;
			}
			else if(DT_DIR == dp->d_type)
			{
				filename = new char[strlen(path) + strlen (dp->d_name) + 2];			
				strcpy (filename, path);
				strcat (filename, "/");
				strcat (filename, dp->d_name);
				Scan_Dir (filename);
			}
		}
	}
//	delete filename;
	closedir (dfp);
}
/*
void ReportTask::Get_table_id(sqlite3* db, char** azResult, int nrow, int ncolumn, char* errMsg)
{
//	cout<<"wuxiangfeng1111111111111111111111111111"<<endl;
	char sql_cmd[40];
	memset (sql_cmd, 0, sizeof (sql_cmd));
	sprintf (sql_cmd, "select * from tips_Updata where flag=0");
	sql_cmd[strlen (sql_cmd)] = '\0';
	pthread_mutex_lock (&sqlite_sync_mutex);
	int ret = sqlite3_get_table (db, sql_cmd, &azResult, &nrow, &ncolumn, &errMsg);
	if (SQLITE_OK != ret)
	{
		cout<<errMsg<<endl;
	//	sqlite3_close(db);
	}
  if (azResult != NULL)
 {
 		for(int i = 0; i < nrow + 1; i++)
		{
#if 0
		if (0 == i)
			fprintf (stdout, "azResult[%d]========%s\n",i ,azResult[i * ncolumn]);
		else
			fprintf (stdout, "azResult[%d]========%s\n",i ,azResult[i * ncolumn + j]);
#endif
			alarm_ID = atoi (azResult[i * ncolumn + 1]) + 1;
			alert_ID = atoi (azResult[i * ncolumn + 2]) + 1;
			event_ID = atoi (azResult[i * ncolumn + 3]) + 1;
			block_ID = atoi (azResult[i * ncolumn + 4]) + 1;
		}
 }
	sqlite3_free_table(azResult);
	cout <<"obtain ================alarm_ID============" << alarm_ID <<endl;
	pthread_mutex_unlock (&sqlite_sync_mutex);
}

void ReportTask::Updata_table_id(sqlite3* db, char* errMsg)
{
	char sql_cmd[128];
	memset (sql_cmd, 0, sizeof (sql_cmd));
	sprintf (sql_cmd, "Update tips_Updata set Alarm_id=%d,Alert_id=%d,Event_id=%d,Block_id=%d", alarm_ID, alert_ID, event_ID, block_ID);
	sql_cmd[strlen (sql_cmd)] = '\0';
	pthread_mutex_lock (&sqlite_sync_mutex);
	int ret = sqlite3_exec (db, sql_cmd, NULL, NULL, &errMsg);
	if (SQLITE_OK != ret)
	{
		cout<<errMsg<<endl;
	//	sqlite3_close(db);
	}
	pthread_mutex_unlock (&sqlite_sync_mutex);
}
*/
void report_alarm(char** result,int column,void* param)
{
	struct sql_struct* temp=(struct sql_struct*)param;
	struct alarm al;
	memset (&al, 0, sizeof (struct alarm)); 
	My_Strcpy(al.alarm_id, result[0], sizeof(al.alarm_id));
	cout <<endl<<"al.alarm_id============"<<al.alarm_id<<endl;
	My_Strcpy(al.trojan_id, result[1], sizeof(al.trojan_id));
	cout<<"al.trojan_id============"<<al.trojan_id<<endl;
	My_Strcpy(al.sip, result[2], sizeof(al.sip));
	cout <<"al.sip==================="<<al.sip<<endl;
	My_Strcpy(al.dip, result[3], sizeof(al.dip));
	cout <<"al.dip====================="<<al.dip<<endl;
	My_Strcpy(al.dmac, result[4], sizeof(al.dmac));
	al.dmac[17] = '\0';
	cout <<"al.dmac====================="<<al.dmac<<endl;
	My_Strcpy(al.alarm_time, result[5], sizeof(al.alarm_time));
	cout <<"al.alarm_time===================="<<al.alarm_time <<endl;
	My_Strcpy(al.type, result[6], sizeof(al.type));
	cout <<"al.type======================"<<al.type<<endl;
	My_equalc(&(al.risk), result[7]);
	cout <<"al.risk======================="<<al.risk<<endl;
	My_equalc(&(al.scc_flag), result[8]);
	cout <<"al.scc_flag================="<<al.scc_flag<<endl;
	My_Strcpy(al.scc, result[9], sizeof(al.scc));
	cout <<"al.scc====================="<<al.scc<<endl;
	My_Strcpy(al.descry, result[10], sizeof(al.descry));
	al.descry[strlen(al.descry)] = '\0';
	cout <<"al.descry===================="<<al.descry<<endl;
	My_Strcpy (al.device_id, LOCALDEVID, sizeof(al.device_id));
	cout <<"al.device_id=================="<<al.device_id <<endl;
	memcpy ((char*)temp->data + ((temp->count)*sizeof(struct alarm)), &al, sizeof (struct alarm));
	temp->count++;

	column = column;		//hyj 保留参数
}
void select_count(char** result,int column,void* param)
{
	int* count=(int*)param;
	*count=atoll(result[0]);

	column = column;		//hyj 保留参数
}
void ReportTask::Get_alarm()
{
//得到当前数据库条数,并申请空间.
	printf("report alarm start\n");
	int count;
	char sql_cmd[1024];
	memset (sql_cmd, 0, sizeof (sql_cmd));
	snprintf(sql_cmd,sizeof(sql_cmd),"select count(*) from tips_Alarm where flag=0;");
	sql_cmd[strlen (sql_cmd)] = '\0';
	sqlhandler->Select_Sql_Exec((char*)sql_cmd,select_count,&count);
	if(count<=0)
	{
		cout<<"no alarm info report!"<<endl;
		return;
	}
	else if(count > 500)
	{
		count = 500;
	}
	struct alarm* cache=(struct alarm*)malloc(sizeof(struct alarm)*count);
	if(!cache)
	{
		cout<<"malloc_report_alarm_cache failed!"<<endl;
		return;
	}
	
//*************************************************
//读取数据组装结构
	snprintf (sql_cmd, sizeof (sql_cmd), "select * from tips_Alarm where flag=0 order by Alarm_judgetime limit %d;",count);
	sql_cmd[strlen (sql_cmd)] = '\0';
	struct sql_struct temp;
	temp.data=cache;
	temp.count=0;
	sqlhandler->Select_Sql_Exec(sql_cmd,report_alarm,&temp);
//*************************************************
//发送数据
	int rc = ointerface.Send_Data((void**)&cache, count, 0);
	if (-1 == rc)
	{

		struct cache_opt_t opt_stut;
		INIT_OPT_HEAD(&opt_stut);
                strcpy(opt_stut.username,"system");
                time_t t;
                time(&t);
                funhandler->Turntime(t,opt_stut.opttime,24);
                strcpy(opt_stut.describe,"上传攻击窃密信息失败");
                int send_err=cacheclient->Send_Msg((char*)&opt_stut,sizeof(opt_stut));
				send_err = send_err;		//hyj 未使用的变量
	}
	else
	{
		snprintf (sql_cmd, sizeof (sql_cmd), "update tips_Alarm  set flag=1 where flag=0 order by Alarm_judgetime limit %d;",count);
		sql_cmd[strlen (sql_cmd)] = '\0';
		int update_err=sqlhandler->Insert_Sql_Exec(sql_cmd);
		cout<<"update tips_alarm flag ::"<<update_err<<endl;
	}
	if (cache != NULL)free ((char*)cache);	
}

void report_alert(char** result,int column,void* param)
{
	struct sql_struct* temp=(struct sql_struct*)param;
	struct alert alt;
	memset(&alt, 0, sizeof(struct alert));
//		My_Strcpy (alt.device_id, azResult[i * ncolumn + 17], sizeof(alt.device_id));
		My_Strcpy (alt.device_id, LOCALDEVID, sizeof(alt.device_id));
		//cout<<"alt.device_id============"<<alt.device_id <<endl;
		My_Strcpy(alt.alert_id, result[0], sizeof(alt.alert_id));
		//cout <<"alt.alert_id==========="<<alt.alert_id <<endl;
		printf("alt.alert_id===========%s\n", alt.alert_id);
		My_equal(&(alt.plugin_id), result[1]);
		//cout <<"alt.plugin_id============"<<alt.plugin_id <<endl;
		My_Strcpy(alt.dip, result[2], sizeof(alt.dip));
		//cout <<"alt.dip================="<<alt.dip<<endl;
		printf("alt.dip=================%s\n",alt.dip);
		My_Strcpy(alt.dmac, result[3], sizeof(alt.dmac));
		alt.dmac[17] = '\0';
		//cout <<"alt.dmac==================="<<alt.dmac<<endl;
		My_Strcpy(alt.sip, result[4], sizeof(alt.sip));
		//cout <<"alt.sip================="<<alt.sip<<endl;
		My_Strcpy(alt.alarm_time, result[5], sizeof(alt.alarm_time));
		//cout <<"alt.alarm_time================="<<alt.alarm_time<<endl;
		My_equalc(&(alt.risk), result[6]);
		//cout <<"alt.risk======================"<<alt.risk<<endl;
		My_equalc(&(alt.scc_flag), result[7]);
		//cout<<"alt.scc_flag================="<<alt.scc_flag<<endl;
		My_Strcpy(alt.trojan_id, result[8], sizeof(alt.trojan_id));
		//cout <<"alt.trojan_id======================"<<alt.trojan_id <<endl;
		My_equal(&(alt.sport), result[9]);
		//cout <<"al.sport=========================="<<alt.sport <<endl;
		My_equal(&(alt.protocol), result[10]);
		//cout <<"alt.protocol========================"<<alt.protocol<<endl;
		My_Strcpy(alt.descry, result[11], sizeof (alt.descry));
		alt.descry[strlen (alt.descry) - 1] = '\0';
		//cout <<"alt.descry=========================="<<alt.descry<<endl;
		My_Strcpy(alt.dnshash, result[12], sizeof (alt.dnshash));
		//cout <<"alt.dnshash=========================="<<alt.dnshash<<endl;
		My_Strcpy(alt.scc, result[13], sizeof(alt.scc));
		//cout <<"alt.scc======================================="<<alt.scc<<endl;
		My_Strcpy(alt.extra_text, result[14], sizeof(alt.extra_text));
		//cout <<"alt.extra_text========================"<<alt.extra_text<<endl;
		My_equal(&(alt.dport), result[15]);
		//cout <<"alt.dport=========================="<<alt.dport<<endl;
	//	memcpy ((char*)*cache + ((i - 1)*sizeof(struct alert)), &alt, sizeof (struct alert));

		memcpy ((char*)temp->data + ((temp->count)*sizeof(struct alert)), &alt, sizeof (struct alert));
		temp->count++;
#if 0
	My_Strcpy(al.alarm_id, result[0], sizeof(al.alarm_id));
	cout <<endl<<"al.alarm_id============"<<al.alarm_id<<endl;
	My_Strcpy(al.trojan_id, result[1], sizeof(al.trojan_id));
	cout<<"al.trojan_id============"<<al.trojan_id<<endl;
	My_Strcpy(al.sip, result[2], sizeof(al.sip));
	cout <<"al.sip==================="<<al.sip<<endl;
	My_Strcpy(al.dip, result[3], sizeof(al.dip));
	cout <<"al.dip====================="<<al.dip<<endl;
	My_Strcpy(al.dmac, result[4], sizeof(al.dmac));
	al.dmac[17] = '\0';
	cout <<"al.dmac====================="<<al.dmac<<endl;
	My_Strcpy(al.alarm_time, result[5], sizeof(al.alarm_time));
	cout <<"al.alarm_time===================="<<al.alarm_time <<endl;
	My_Strcpy(al.type, result[6], sizeof(al.type));
	cout <<"al.type======================"<<al.type<<endl;
	My_equalc(&(al.risk), result[7]);
	cout <<"al.risk======================="<<al.risk<<endl;
	My_equalc(&(al.scc_flag), result[8]);
	cout <<"al.scc_flag================="<<al.scc_flag<<endl;
	My_Strcpy(al.scc, result[9], sizeof(al.scc));
	cout <<"al.scc====================="<<al.scc<<endl;
	My_Strcpy(al.descry, result[10], sizeof(al.descry));
	al.descry[strlen(al.descry)] = '\0';
	cout <<"al.descry===================="<<al.descry<<endl;
	My_Strcpy (al.device_id, LOCALDEVID, sizeof(al.device_id));
	cout <<"al.device_id=================="<<al.device_id <<endl;
	memcpy ((char*)temp->data + ((temp->count)*sizeof(struct alarm)), &al, sizeof (struct alarm));
	temp->count++;
#endif
	column = column;		//hyj 保留参数
}

void ReportTask::Get_alert()
{
//得到当前数据库条数,并申请空间.
	printf("report alert start\n");
	int count;
	char sql_cmd[1024];
	memset (sql_cmd, 0, sizeof (sql_cmd));
	snprintf(sql_cmd,sizeof(sql_cmd),"select count(*) from tips_Alert where flag=0;");
	sql_cmd[strlen (sql_cmd)] = '\0';
	sqlhandler->Select_Sql_Exec((char*)sql_cmd,select_count,&count);
	if(count<=0)
	{
		cout<<"no alert info report!"<<endl;
		return;
	}
	else if(count > 500)
	{
		count = 500;
	}
	struct alert * cache=(struct alert*)malloc(sizeof(struct alert)*count);
	if(!cache)
	{
		cout<<"malloc_report_alert_cache failed!"<<endl;
		return;
	}
	
//*************************************************
//读取数据组装结构
	snprintf (sql_cmd, sizeof (sql_cmd), "select * from tips_Alert where flag=0 order by Alarm_time limit %d;",count);
	sql_cmd[strlen (sql_cmd)] = '\0';
	struct sql_struct temp;
	temp.data=cache;
	temp.count=0;
	sqlhandler->Select_Sql_Exec(sql_cmd,report_alert,&temp);
//*************************************************
//发送数据
	int rc = ointerface.Send_Data((void**)&cache, count, 1);
	if (-1 == rc)
	{

		struct cache_opt_t opt_stut;
		INIT_OPT_HEAD(&opt_stut);
    strcpy(opt_stut.username,"system");
    time_t t;
    time(&t);
    funhandler->Turntime(t,opt_stut.opttime,24);
    strcpy(opt_stut.describe,"上传可疑行为信息失败");
    printf("%s\n",opt_stut.describe);
    int send_err=cacheclient->Send_Msg((char*)&opt_stut,sizeof(opt_stut));
	send_err = send_err;		//hyj 未使用的变量
	}
	else
	{
		snprintf (sql_cmd, sizeof (sql_cmd), "update tips_Alert  set flag=1 where flag=0 order by Alarm_time limit %d;",count);
		sql_cmd[strlen (sql_cmd)] = '\0';
		int update_err=sqlhandler->Insert_Sql_Exec(sql_cmd);
		cout<<"update tips_Alert return ::"<<update_err<<endl;
		printf("report alert successful\n");
	}
	if (cache != NULL)free ((char*)cache);	
}

#if 0
void ReportTask::Get_alert(sqlite3* db, char** azResult, int nrow, int ncolumn, void** cache, char* errMsg)
{
//	cout <<"alerttttttttttttttttttttttttttttt="<<endl;

	struct alert alt;
	char sql_cmd[1024];
	memset (&alt, 0, sizeof (struct alert));
	memset (sql_cmd, 0, sizeof (sql_cmd));
	snprintf (sql_cmd, sizeof (sql_cmd), "select * from tips_Alert where flag=0");
	sql_cmd[strlen (sql_cmd)] = '\0';
	//cout <<"alerttttttttttttttttttttttttttttt=11111111111111111"<<endl;
	pthread_mutex_lock (&sqlite_sync_mutex);
	//	cout <<"alertttttttttttttttttttttttttttttttttttt"<<endl;
		int ret =sqlite3_get_table (db, sql_cmd, &azResult, &nrow, &ncolumn, &errMsg);
		if(ret != SQLITE_OK)
		{
			cout<<errMsg<<endl;
		//	cout<<"nidaye======111111111"<<errMsg<<endl;
		}
	if(nrow > 0)
	{
		*cache = (struct alert*)malloc((nrow)* sizeof (struct alert));
//	cout <<"nrow==============="<<nrow<<endl;
	//	cout <<"ncloumn==============="<<ncolumn<<endl;
				for (int i = 1; i < nrow + 1; i++)
		//for (int i = 1; i < nrow + 1; i++)
		{
		//	My_Strcpy (alt.device_id, azResult[i * ncolumn + 17], sizeof(alt.device_id));
			My_Strcpy (alt.device_id, LOCALDEVID, sizeof(alt.device_id));
		//	cout<<"alt.device_id============"<<alt.device_id <<endl;
			My_Strcpy(alt.alert_id, azResult[i * ncolumn + 16], sizeof(alt.alert_id));
		//	cout <<"alt.alert_id==========="<<alt.alert_id <<endl;
			My_equal(&(alt.plugin_id), azResult[i * ncolumn + 1]);
	//	cout <<"alt.plugin_id============"<<alt.plugin_id <<endl;
			My_Strcpy(alt.dip, azResult[i * ncolumn + 2], sizeof(alt.dip));
		//	cout <<"alt.dip================="<<alt.dip<<endl;
			My_Strcpy(alt.dmac, azResult[i * ncolumn + 3], sizeof(alt.dmac));
			alt.dmac[17] = '\0';
		//	cout <<"alt.dmac==================="<<alt.dmac<<endl;
			My_Strcpy(alt.sip, azResult[i * ncolumn + 4], sizeof(alt.sip));
//		cout <<"alt.sip================="<<alt.sip<<endl;
			My_Strcpy(alt.alarm_time, azResult[i * ncolumn + 5], sizeof(alt.alarm_time));
	//		cout <<"alt.alarm_time================="<<alt.alarm_time<<endl;
			My_equalc(&(alt.risk), azResult[i * ncolumn + 6]);
		//	cout <<"alt.risk======================"<<alt.risk<<endl;
			My_equalc(&(alt.scc_flag), azResult[i * ncolumn + 7]);
		//	cout<<"alt.scc_flag================="<<alt.scc_flag<<endl;
			My_Strcpy(alt.trojan_id, azResult[i * ncolumn + 8], sizeof(alt.trojan_id));
		//	cout <<"alt.trojan_id======================"<<alt.trojan_id <<endl;
			My_equal(&(alt.sport), azResult[i * ncolumn + 9]);
		//	cout <<"al.sport=========================="<<alt.sport <<endl;
			My_equal(&(alt.protocol), azResult[i * ncolumn + 10]);
		//	cout <<"alt.protocol========================"<<alt.protocol<<endl;
			My_Strcpy(alt.descry, azResult[i * ncolumn + 11], sizeof (alt.descry));
			alt.descry[strlen (alt.descry) - 1] = '\0';
		//	cout <<"alt.descry=========================="<<alt.descry<<endl;
			My_Strcpy(alt.dnshash, azResult[i * ncolumn + 12], sizeof (alt.dnshash));
		//	cout <<"alt.dnshash=========================="<<alt.dnshash<<endl;
			My_Strcpy(alt.scc, azResult[i * ncolumn + 13], sizeof(alt.scc));
		//	cout <<"alt.scc======================================="<<alt.scc<<endl;
			My_Strcpy(alt.extra_text, azResult[i * ncolumn + 14], sizeof(alt.extra_text));
		//	cout <<"alt.extra_text========================"<<alt.extra_text<<endl;
			My_equal(&(alt.dport), azResult[i * ncolumn + 15]);
		//	cout <<"alt.dport=========================="<<alt.dport<<endl;
			memcpy ((char*)*cache + ((i - 1)*sizeof(struct alert)), &alt, sizeof (struct alert));
		}
	//	My_Strcpy(alert_time, azResult[nrow * ncolumn + 5], sizeof(alert_time));
	sqlite3_free_table(azResult);
//		cout <<"alerttttttttttttttttttttttttttttt=11111111111111111"<<endl;
		OInterface ointerface;
	//	cout<<"ointerface====================11111111111"<<endl;
		int rc = 	ointerface.Send_Data(cache, nrow, 1);
		if (-1 == rc)
			Update_flag (rc, (char*)"上传数据失败");
		else
			Update_flag (rc, (char*)"update tips_Alert set flag=1");
		cout <<"rc =================" <<rc <<endl;
	if (*cache != NULL)
		free ((char*)*cache);
	
	//	cout<<"ointerface===================="<<endl;
	}
	pthread_mutex_unlock (&sqlite_sync_mutex);

}
#endif

#if 0
void ReportTask::Get_senstive_alert(sqlite3* db, char** azResult, int nrow, int ncolumn, void** cache, char* errMsg)
{
/*
	struct senstive_alert alert;
	char sql_cmd[1024];
	memset (sql_cmd, 0, sizeof (sql_cmd));
	memset (&alert, 0, sizeof (struct senstive_alert));snprintf (sql_cmd, sizeof (sql_cmd), "select * from tips_Event where flag='0' group by Event_class,Event_type,Sip,Sport,Dip,Dport,Dmac,Sender,Receiver,Title,File_name,Descr,Sen_msg,Device_id,Event_judgetime");
	sql_cmd[strlen (sql_cmd)] = '\0';
	pthread_mutex_lock (&sqlite_sync_mutex);
	int ret =sqlite3_get_table (db, sql_cmd, &azResult, &nrow, &ncolumn, &errMsg);
	if(ret != SQLITE_OK)
	{
			cout<<errMsg<<endl;
		//cout<<"nidaye======222222222222"<<errMsg<<endl;
	}
	if(nrow > 0)
	{
	//	cout<<"sentive_alert========================"<<endl;
		*cache = (struct senstive_alert*)malloc((nrow) * sizeof (struct senstive_alert));
	//	cout <<"nrow==============="<<nrow<<endl;
	//	cout <<"ncloumn==============="<<ncolumn<<endl;
				for (int i = 1; i < nrow + 1; i++)
		//for (int i = 1; i < nrow + 1; i++)
		{
		//	My_Strcpy (alert.device_id, azResult[i * ncolumn + 15], sizeof(alert.device_id));
			//My_Strcpy (alert.device_id, (char*)"1303030001", sizeof(alert.device_id));
			My_Strcpy (alert.device_id, LOCALDEVID, sizeof(alert.device_id));
		//	cout <<"alert.device_id===================="<<alert.device_id<<endl;
			My_Strcpy(alert.event_id, azResult[i * ncolumn + 14], sizeof(alert.event_id));
		//	cout <<"alert.event_id===================="<<alert.event_id<<endl;
			My_equal(&(alert.event_class), azResult[i * ncolumn + 1]);
		//	cout <<"alert.event_class==============="<<alert.event_class<<endl;
			My_Strcpy(alert.event_type, azResult[i * ncolumn + 2], sizeof(alert.event_type));
		//	cout <<"alert.event_type=================="<<alert.event_type<<endl;
			My_Strcpy(alert.event_time, azResult[i * ncolumn + 3], sizeof(alert.event_time));
		//	cout <<"alert.event_time===================="<<alert.event_time<<endl;
			My_Strcpy(alert.sip, azResult[i * ncolumn + 4], sizeof(alert.sip));
		//	cout <<"alert.sip======================="<<alert.sip<<endl;
			My_equal(&(alert.sport), azResult[i * ncolumn + 5]);
		//	cout <<"alert.sport======================="<<alert.sport<<endl;
			My_Strcpy(alert.dip, azResult[i * ncolumn + 6], sizeof(alert.dip));
			My_equal(&(alert.dport), azResult[i * ncolumn + 7]);
			My_Strcpy(alert.dmac, azResult[i * ncolumn + 8], sizeof(alert.dmac));
			alert.dmac[17] = '\0';
			My_Strcpy(alert.sender, azResult[i * ncolumn + 9], sizeof(alert.sender));
			My_Strcpy(alert.receiver, azResult[i * ncolumn + 10], sizeof(alert.receiver));
			My_Strcpy(alert.title, azResult[i * ncolumn + 11], sizeof(alert.title));
			My_Strcpy(alert.descr, azResult[i * ncolumn + 12], sizeof(alert.descr));
			alert.descr[strlen (alert.descr) - 1] = '\0';
			My_Strcpy(alert.sen_msg, azResult[i * ncolumn + 13], sizeof(alert.sen_msg));
			memcpy ((char*)*cache + ((i - 1)*sizeof(struct senstive_alert)), &alert, sizeof (struct senstive_alert));
		}
	//	My_Strcpy(event_time, azResult[nrow * ncolumn + 3], sizeof(event_time));
	sqlite3_free_table(azResult);
		OInterface ointerface;
	//	cout<<"ointerface====================11111111111"<<endl;
		int rc = ointerface.Send_Data(cache, nrow, 2);
		if (-1 == rc)
			Update_flag (rc, (char*)"上传数据失败");
		else
			Update_flag (rc, (char*)"update tips_Event set flag=1");
	if (*cache != NULL)
		free ((char*)*cache);	
	//	cout<<"ointerface===================="<<endl;
	}
	pthread_mutex_unlock (&sqlite_sync_mutex);
*/
}
#endif 
void report_block(char** result,int column,void* param)
{
	struct sql_struct* temp=(struct sql_struct*)param;
	struct session_blocked bl;
	memset (&bl, 0, sizeof (struct session_blocked)); 
	My_Strcpy(bl.dip, result[1], sizeof(bl.dip));
	cout<<"bl.dip============"<<bl.dip<<endl;
	My_Strcpy(bl.sip, result[2], sizeof(bl.sip));
	cout <<"bl.sip==================="<<bl.sip<<endl;
	My_equal(&(bl.sport), result[3]);
	cout <<"bl.sport====================="<<bl.sport<<endl;
	My_Strcpy(bl.start_time, result[4], sizeof(bl.start_time));
	cout <<"bl.start_time====================="<<bl.start_time<<endl;
	My_Strcpy(bl.expired_time, result[5], sizeof(bl.expired_time));
	cout <<"bl.start_time===================="<<bl.expired_time <<endl;
	My_Strcpy(bl.block_desc, result[6], sizeof(bl.block_desc));
	bl.block_desc[strlen(bl.block_desc)] = '\0';
	cout <<"bl.block_desc======================"<<bl.block_desc<<endl;
	My_Strcpy(bl.device_id, result[7], sizeof(bl.device_id));
	My_Strcpy (bl.device_id, LOCALDEVID, sizeof(bl.device_id));
	cout <<"bl.device_id=================="<<bl.device_id <<endl;
	memcpy ((char*)temp->data + ((temp->count)*sizeof(struct session_blocked)), &bl, sizeof (struct session_blocked));
	temp->count++;

	column = column;		//hyj 保留参数
}


void ReportTask::Get_block()
{
//得到当前数据库条数,并申请空间.
	printf("report block start\n");
	int count;
	char sql_cmd[1024];
	memset (sql_cmd, 0, sizeof (sql_cmd));
	snprintf(sql_cmd,sizeof(sql_cmd),"select count(*) from tips_block where flag=0;");
	sql_cmd[strlen (sql_cmd)] = '\0';
	sqlhandler->Select_Sql_Exec((char*)sql_cmd,select_count,&count);
	if(count<=0)
	{
		cout<<"no block info report!"<<endl;
		return;
	}
	else if(count > 500)
	{
		count = 500;
	}
	struct session_blocked* cache=(struct session_blocked*)malloc(sizeof(struct session_blocked)*count);
	if(!cache)
	{
		cout<<"malloc_report_block_cache failed!"<<endl;
		return;
	}
	
//*************************************************
//读取数据组装结构
	snprintf (sql_cmd, sizeof (sql_cmd), "select * from tips_block where flag=0 order by Expired_time limit %d;",count);
	sql_cmd[strlen (sql_cmd)] = '\0';
	struct sql_struct temp;
	temp.data=cache;
	temp.count=0;
	sqlhandler->Select_Sql_Exec(sql_cmd,report_block,&temp);
//*************************************************
//发送数据
	int rc = ointerface.Send_Data((void**)&cache, count, 3);
	if (-1 == rc)
	{

		struct cache_opt_t opt_stut;
		INIT_OPT_HEAD(&opt_stut);
    strcpy(opt_stut.username,"system");
    time_t t;
    time(&t);
    funhandler->Turntime(t,opt_stut.opttime,24);
    strcpy(opt_stut.describe,"上传窃密阻断信息失败");
    int send_err=cacheclient->Send_Msg((char*)&opt_stut,sizeof(opt_stut));
	send_err = send_err; 		//hyj 未使用的变量
	}
	else
	{
		snprintf (sql_cmd, sizeof (sql_cmd), "update tips_block  set flag=1 where flag=0 order by Expired_time limit %d;",count);
		sql_cmd[strlen (sql_cmd)] = '\0';
		int update_err=sqlhandler->Insert_Sql_Exec(sql_cmd);
		cout<<"update tips_block return ::"<<update_err<<endl;
	}
	if (cache != NULL)free ((char*)cache);	
}


#if 0
void ReportTask::Get_block(sqlite3* db, char** azResult, int nrow, int ncolumn, void** cache, char* errMsg)
{
	struct session_blocked block;
	char sql_cmd[1024];
	memset (sql_cmd, 0, sizeof (sql_cmd));
	snprintf (sql_cmd, sizeof (sql_cmd), "select * from tips_block where flag=0");
	sql_cmd[strlen (sql_cmd)] = '\0';
	pthread_mutex_lock (&sqlite_sync_mutex);
	int ret = sqlite3_get_table (db, sql_cmd, &azResult, &nrow, &ncolumn, &errMsg);
	if(ret != SQLITE_OK)
	{
		cout<<errMsg<<endl;
	}
	if(nrow > 0)
	{
	//	cout<<"senssion=============================="<<endl;
		*cache = (struct session_blocked*)malloc((nrow) * sizeof (struct session_blocked));
	//	cout <<"nrow==============="<<nrow<<endl;
	//	cout <<"ncloumn==============="<<ncolumn<<endl;
				for (int i = 1; i < nrow + 1; i++)
		//for (int i = 1; i < nrow + 1; i++)
		{
			memset (&block, 0, sizeof (struct session_blocked));
		//	My_Strcpy (block.device_id, azResult[i * ncolumn + 7], sizeof(block.device_id));
			My_Strcpy (block.device_id, LOCALDEVID, sizeof(block.device_id));
			My_Strcpy(block.dip, azResult[i * ncolumn + 1], sizeof(block.dip));
			My_Strcpy(block.sip, azResult[i * ncolumn + 2], sizeof(block.sip));
			My_equal(&(block.sport), azResult[i * ncolumn + 3]);
			My_Strcpy(block.start_time, azResult[i * ncolumn + 4], sizeof(block.start_time));
			My_Strcpy(block.expired_time, azResult[i * ncolumn + 5], sizeof(block.expired_time));
			My_Strcpy(block.block_desc, azResult[i * ncolumn + 6], sizeof(block.block_desc));
			block.block_desc[strlen (block.block_desc) - 1] = '\0';
#if 0
		cout <<"device_id=========="<<block.device_id <<endl;
		cout <<"dip==============="<<block.dip <<endl;
		cout <<"sip=============="<<block.sip <<endl;
		cout <<"sport===================="<<block.sport <<endl;
		cout <<"start_time================"<<block.start_time <<endl;
		cout <<"expired_time==================="<<block.expired_time <<endl;
		cout <<"block_desc================="<<block.block_desc <<endl;
#endif
			memcpy ((char*)*cache+((i - 1)*sizeof(struct session_blocked)), &block, sizeof (struct session_blocked));
		}
		//My_Strcpy(block_time, azResult[nrow * ncolumn + 5], sizeof(block_time));
	sqlite3_free_table(azResult);
		OInterface ointerface;
//		cout<<"ointerface====================11111111111"<<endl;
		int rc = ointerface.Send_Data(cache, nrow, 3);
		if (-1 == rc)
			Update_flag (rc, (char*)"上传数据失败");
		else
			Update_flag (rc, (char*)"update tips_block set flag=1");
	if (*cache != NULL)
		free ((char*)*cache);	
	}
	//	cout<<"ointerface===================="<<endl;
	pthread_mutex_unlock (&sqlite_sync_mutex);
}
#endif
/*
void ReportTask::Get_netlog(sqlite3* db, char** azResult, int nrow, int ncolumn, char* errMsg)
{
	char sql_cmd[40];
	memset (sql_cmd, 0, sizeof (sql_cmd));
	sprintf (sql_cmd, "select * from tips_netlog");
	sql_cmd[strlen (sql_cmd)] = '\0';
	pthread_mutex_lock (&sqlite_sync_mutex);
	int ret = sqlite3_get_table (db, sql_cmd, &azResult, &nrow, &ncolumn, &errMsg);
	if(ret != SQLITE_OK)
	{
		cout<<errMsg<<endl;
	}
  cout <<"netlog start ~~~~~~~~~~~~~" <<endl;
  if (azResult == NULL)
 {
	My_Strcpy (netlog_time, azResult[nrow * ncolumn + 7], sizeof (netlog_time));
//	cout <<"netlog time ===========" << azResult[nrow * ncolumn + 6] <<endl;
	//cout <<"netlog time ===========" << azResult[nrow * ncolumn + 7] <<endl;
//	cout <<"netlog time ===========" << azResult[nrow * ncolumn + 8] <<endl;
 }
	sqlite3_free_table(azResult);
	pthread_mutex_unlock (&sqlite_sync_mutex);
}
*/


void ReportTask::Init_Task()
{
	FILE* fp = fopen (RUNCONGFILE, "r");
	char buffer[1024];
	memset (buffer, 0, sizeof(buffer));
	while(fgets(buffer, sizeof(buffer), fp))
	{
		if(!strncmp(buffer, "ALARMTIME", strlen("ALARMTIME")))
		{
			char* p = strchr(buffer, '=');
	//cout<<"上报状态任务初始化"<<endl;
			Second_Time= atoi(p + 1);
			cout<<"second_time========"<<Second_Time<<endl;
			break;
		}
	}
	fclose(fp);
	//cout<<"上报报警信息初始化"<<endl;
	//Second_Time=60*60*24*7;
}


void report_opt(char** result,int column,void* param)
{
	struct sql_struct * temp = (struct sql_struct*)param;
	cache_opt_t opt;
	memset(&opt, 0, sizeof(opt));
//	My_Strcpy (al.device_id, LOCALDEVID, sizeof(al.device_id));
//	cout <<"al.device_id=================="<<al.device_id <<endl;
	My_Strcpy(opt.username, result[1], sizeof(opt.username));
//	cout<<"opt.username============"<<opt.username<<endl;
	My_Strcpy(opt.opttime, result[2], sizeof(opt.opttime));
//	cout <<"opt.opttime==================="<<opt.opttime<<endl;
	My_Strcpy(opt.describe, result[3], sizeof(opt.describe));
//	cout <<"opt.describe====================="<<opt.describe<<endl;
	memcpy ((char*)(temp->data) + ((temp->count)*sizeof(cache_opt_t)), &opt, sizeof (cache_opt_t));
	temp->count++;
	
	column = column;		//hyj 保留参数
}

void ReportTask::Get_opt()
{
	printf("report opt start\n");
	char buffer[1024];
	char temp[100] = {0};
	int count;
	char sql_cmd[1024];
	memset (sql_cmd, 0, sizeof (sql_cmd));
	snprintf(sql_cmd,sizeof(sql_cmd),"select count(*) from tips_opt where flag=0;");
	sql_cmd[strlen (sql_cmd)] = '\0';
	sqlhandler->Select_Sql_Exec((char*)sql_cmd,select_count,&count);
	if(count<=0)
	{
		cout<<"no opt info report!"<<endl;
		return;
	}
	else if(count>500)
	{
		count = 500;
	}
	snprintf (temp, sizeof (temp), "1002_%ld_%s.log", time (NULL), LOCALDEVID);
	FILE* fp = fopen(temp, "w+");
	if (NULL == fp)
		fprintf (stderr, "Error : %s\n", strerror (errno));
	cache_opt_t * cache=(cache_opt_t*)malloc(sizeof(cache_opt_t)*count);
	if(!cache)
	{
		cout<<"malloc_report_opt_cache failed!"<<endl;
		return;
	}
	
	snprintf (sql_cmd, sizeof (sql_cmd), "select * from tips_opt where flag=0 order by Otime limit %d;",count);
	sql_cmd[strlen (sql_cmd)] = '\0';
	struct sql_struct tem;
	tem.data=cache;
	tem.count=0;
	sqlhandler->Select_Sql_Exec(sql_cmd,report_opt,&tem);
#if 1
	if(count > 0)
	{
		for(int i = 0; i < count; i++)
		{
			memset (buffer, 0, sizeof(buffer));
			snprintf (buffer, sizeof (buffer), "%s#%s#%s#%s\n", cache[i].username, LOCALDEVID, cache[i].opttime, cache[i].describe);
			fwrite (buffer, strlen(buffer), 1, fp);
	//		cout <<"buffer ==========" <<buffer <<endl;
		}	
 	}
#endif
	fclose(fp);
//	unlink (buffer);
	memset (buffer, 0, sizeof(buffer));
	char* directory = get_current_dir_name ();
	strcpy (buffer, directory);
	strcat (buffer, "/");
	strcat (buffer, temp);
//	printf("temp=%s\n",temp);
//	strcat (buffer, "1002_1385797196_1403039999.log");
	buffer[strlen(buffer)] = '\0';
	printf("path==================%s\n",buffer);
	int rc = UploadData(buffer,0);
	int rc1 = UploadData(buffer,1);
	if (-1 == rc1 && rc == -1)
		Update_flag (rc,(char*)"上传文件失败");
	else
	{
		snprintf (sql_cmd, sizeof (sql_cmd), "update tips_opt set flag=1 where flag = 0 order by Otime limit %d;",count);
		sql_cmd[strlen (sql_cmd)] = '\0';
		Update_flag (rc,sql_cmd);
	}
	unlink (buffer);
 	char rm[1024];
	memset (rm, 0, sizeof(rm));
	sprintf(rm, "rm -f %s", buffer);
	system(rm);
	printf("Get_opt end\n");
	fflush(stdout);
//	return;
}
#if 1 
void ReportTask::TurnTime(const time_t clock, char *tips_clock, int clocksize)
{
	struct tm *tm;
	char *format=(char *)"%Y-%m-%d %H:%M:%S";
	tm = localtime(&clock);
	strftime(tips_clock,  clocksize, format,tm);
}
void ReportTask::Send_Err(char* str)
{
/*	char *errMsg = NULL;
	char sql_cmd[1024];
	memset (sql_cmd, 0, sizeof (sql_cmd));
	char mytime[20] = {0};
	TurnTime(time(NULL), mytime, 20);
	snprintf (sql_cmd, sizeof (sql_cmd), "insert into tips_opt (username, Otime, Optdsc, flag) values ('system', '%s', '%s', 0)", mytime, str);
	sql_cmd[strlen (sql_cmd)] = '\0';
  cout <<"sql_cmd ========" <<sql_cmd <<endl;
	//	cout <<"2222222222222222222222222222222222222222mutex============" <<endl;
	pthread_mutex_lock (&sqlite_sync_mutex);
	int ret = sqlite3_exec (db, sql_cmd, NULL, NULL, &errMsg);
	if (SQLITE_OK != ret)
	{
		cout<<errMsg<<endl;
	//	sqlite3_close(db);
	}
	pthread_mutex_unlock (&sqlite_sync_mutex);
  cout <<"sql_cmd11111111 ========" <<sql_cmd <<endl;
*/
	str = str;		//hyj 保留参数
}	
#if 0
void ReportTask::Update_flag(char* str)
{	
	char* errMsg = NULL;		
	pthread_mutex_lock (&sqlite_sync_mutex);
	int ret = sqlite3_exec (db, str, NULL, NULL, &errMsg);
	if(ret != SQLITE_OK)
	{
		cout<<errMsg<<endl;
	}
	pthread_mutex_unlock (&sqlite_sync_mutex);
}
#endif
void ReportTask::Update_flag (int n, char* str)
{

	if (str == NULL)
			return;
	char *errMsg = NULL;
	errMsg = errMsg; 		//hyj 未使用的变量
	int ret = 0;
	if (-1 == n)
	{
		char sql_cmd[1024];
		memset (sql_cmd, 0, sizeof (sql_cmd));
		char mytime[20] = {0};
		TurnTime(time(NULL), mytime, 20);
		snprintf (sql_cmd, sizeof (sql_cmd), "insert into tips_opt (username, Otime, Optdsc, flag) values ('system', '%s', '%s', 0)", mytime, str);
		sql_cmd[strlen (sql_cmd)] = '\0';
 	 cout <<"sql_cmd ========" <<sql_cmd <<endl;
//	cout <<"2222222222222222222222222222222222222222mutex============" <<endl;
//		ret = sqlite3_exec (db, sql_cmd, NULL, NULL, &errMsg);
		ret = sqlhandler->Insert_Sql_Exec(sql_cmd);
		if ( -1 == ret)
		{
			cout<<"failed:"<<sql_cmd<<endl;
		}
	}
	else
	{
		printf("line=%d\n", __LINE__);
		ret = sqlhandler->Insert_Sql_Exec(str);
		if(-1 == ret)
		{
			cout<<"faied:"<<str<<endl;
		}
		printf("line=%d\n", __LINE__);
	}

}
#endif
