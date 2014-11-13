#include "netlog.h"

extern int startrun;
extern char LOCALDEVID[];

struct audit
{
char dip[19];
char dport[6];
char sip[19];
char sport[6];
char protocol[16];
char start_time[25];
char end_time[25];
char up_KB[32];
char down_KB[32];
};

struct sql_struct{
void* data;
int count;
};

int CNetlog::my_pow(int num, int n)
{
	int i;
	int sum = 1;
	for (i = 1; i <= n; i++)
		sum *= num;
	return sum;
}

#if 1
void CNetlog::getlargeIP(char* source, uint32_t* startIP, uint32_t* largeIP, char** startip, char** overip)
{
	char mytmp[1024];
	int amount = 0;
	int endip = 0;
	int yanma = 0;
	char *tmp = 0;
	char *tmp2 = 0;
	memset(mytmp, 0, sizeof (mytmp));
	strcpy(mytmp, source);
	mytmp[strlen (mytmp)] = '\0';
//	cout << " cao ni da ye" <<endl;
	char *fl = strchr(mytmp, '/');
	if ((tmp = strtok(mytmp, "/")))
	{
		if (inet_pton(AF_INET, tmp, startIP))
		{
			char temp[1024];
			memset(temp, 0, sizeof (temp));
			inet_ntop(AF_INET, startIP, temp, sizeof (temp));
			*startip = new char[strlen(temp) + 1];
			strcpy (*startip, temp);
			*startIP = ntohl(*startIP);
			if (fl && (tmp2 = strtok(NULL, "/")))
			{
				amount = atoi(tmp2);
				endip = (my_pow(2, (32 - amount)) -1 );
				yanma = ~endip;
				*largeIP = ((*startIP) & ((yanma))) | (endip);
			//	*largeIP = (ntohl(*startIP) & ((yanma))) | (endip);

				*largeIP = htonl(*largeIP);
				memset(temp, 0, sizeof (temp));
				inet_ntop(AF_INET, largeIP, temp, sizeof (temp));
				*overip = new char[strlen(temp) + 1];
				strcpy(*overip, temp);
				cout << "start ip === "<< startip << endl;
				cout << "overip === " << overip <<endl;
				return;
			}
			else
			{
				*largeIP = *startIP;
				*largeIP = htonl(*largeIP);
				inet_ntop(AF_INET, largeIP, temp, sizeof (temp));
		//		*startip = new char[strlen(temp) + 1];
		//		strcpy (*startip, temp);
				*overip = new char[strlen(temp) + 1];
				strcpy(*overip, temp);
				cout << "start ip === "<< *startip << endl;
				cout << "overip === " << *overip <<endl;

				return;
			}
		}
	}
}
#endif
#if 0 
void CNetlog::getlargeIP(char *source,uint32_t *startIP,uint32_t *largeIP)
{
		int amount = 0;
		int endip = 0;
		int yanma = 0;
		char *tmp = 0;
		char *tmp2 = 0;
		char *fl = strchr(source,'/');
		if((tmp = strtok(source,"/")))
		{
				if(inet_pton(AF_INET,tmp,startIP))
				{
						*startIP = ntohl(*startIP);
						if(fl && (tmp2 = strtok(NULL,"/")))
						{
								amount = atoi(tmp2);	
								endip = (my_pow(2,(32 - amount)) - 1);
								yanma = ~endip;
								*largeIP = ((*startIP) & ((yanma))) | (endip);
								return;
						}
						else 
								*largeIP = *startIP;
				}
		}
		return;
}
#endif
void CNetlog::Set_Sqlhandler(Sqlhandler * sql)
{
	sqlhandler = sql;
}
void CNetlog::deal()
{

#if 0
	int n = 0;
	FILE* fp = fopen("/usr/local/tipschecker/etc/tipscheck/ui_pid", "r");
	char buffer[1024];
	memset(buffer, 0, sizeof (buffer));
	while (fgets(buffer, sizeof (buffer), fp))
	{
		if (strstr(buffer, "UI_PID:"))
		{
			buffer[strlen(buffer) - 1] = '\0';
			n = atoi(strchr(buffer, ':') + 1);
			break;
		}
	}
	fclose(fp);
#endif
//	kill(n, SIGUSR2);
	Audit();
#if 0
	struct dirent* dp;
	DIR* dfp;
	if (NULL == (dfp = opendir("/root/lirongxun/manage_20130324")))
	{
		fprintf(stderr, "%s\n", strerror(errno));
		return;
	}
	while (NULL != (dp = readdir(dfp)))
	{
		if ((!strcmp(dp->d_name, ".")) && (!strcmp(dp->d_name, "..")) && (!strcmp(dp->d_name, "tips.db")) && (!strcmp(dp->d_name, "tips_bak.db")) && (!strstr(dp->d_name, ".db")))
			continue;
		char temp[1024];
		memset(temp, 0, sizeof (temp));
		char* p = strstr(dp->d_name, ".db");
		strncpy(temp, dp->d_name, strlen(dp->d_name) - strlen(p));
		long long int n = atoll(temp);
		long long int stime = atoll(param[5]);
		long long int etime = atoll(param[6]);
		if ((n >= stime) && (n <= etime))
		{
			char path[1024];
			memset(path, 0, sizeof (path));
			strncpy(path, "/root/lirongxun/manage_20130324", sizeof (path));
			strcat(path, dp->d_name);
			int rc = sqlite3_open(path, &netlog_db);
			if (rc)
				fprintf(stderr, "%s\n", sqlite3_errmsg(netlog_db));

			rc = sqlite3_key(netlog_db, "admin", 5);
			if (rc)

				fprintf(stderr, "%s\n", sqlite3_errmsg(netlog_db));
#endif				
/*
	cout << "buffer =============" <<buffer <<endl;
	memset(buffer, 0, sizeof (buffer));
	char* directory = get_current_dir_name();
	strcpy(buffer, directory);
	strcat(buffer, "/");
	strcat(buffer, buf);
	buffer[strlen (buffer)] = '\0';
	printf("path=====================%s\n", buffer);
	int rc = UploadData(buffer,0);
	if (-1 == rc)
		reprort.Update_flag(rc, (char*)"上传文件失败");
	else
	{	memset(sql_cmd, 0, sizeof(sql_cmd));
		snprintf(sql_cmd, sizeof(sql_cmd),"update tips_netlog set flag='1' order by End_time limit %d", count);
		reprort.Update_flag(rc, sql_cmd);
	}
*/
/*
	memset(buffer, 0, sizeof (buffer));
	char* directory = get_current_dir_name();
	strcpy(buffer, directory);
	strcat(buffer, "/");
	strcat(buffer, buf);
	unlink(buffer);
	sql_cmd[strlen(sql_cmd)] = '\0';
	cout <<"sql_cmd ============ " << sql_cmd <<endl;	
	pthread_mutex_lock (&sqlite_sync_mutex);
	sqlite3_get_table (db, sql_cmd, &azResult, &nrow, &ncolumn, &errMsg);
	time_t etime = atoi(param[6]);
	time_t stime = atoi(param[5]);
	memset (param[6], 0, sizeof(param[6]));
	memset (param[5], 0, sizeof(param[5]));
	reprort.TurnTime(stime, param[5], 1024);
	reprort.TurnTime(etime, param[6], 1024);
	for(int j = 1; j < nrow + 1; j++)
	{
		memset (buffer, 0, sizeof(buffer));
	//	snprintf (buffer, sizeof (buffer), "%s#%s#%s#%s#%s#%s#%s#%s#%s#%s#%s\n", LOCALDEVID, param[7], azResult[j * ncolumn + 1], azResult[j * ncolumn + 2], azResult[j * ncolumn + 3], azResult[j * ncolumn + 4], azResult[j * ncolumn + 5], azResult[j * ncolumn + 6], azResult[j * ncolumn +7], azResult[j * ncolumn + 8], azResult[j * ncolumn + 10]);
		snprintf (buffer, sizeof (buffer), "%s#%s#%s#%s#%s#%s#%s#%s#%s#%s#%s\n", LOCALDEVID, param[7], azResult[j * ncolumn + 1], azResult[j * ncolumn + 2], azResult[j * ncolumn + 3], azResult[j * ncolumn + 4], 5, azResult[j * ncolumn + 6], azResult[j * ncolumn + 7], azResult[j * ncolumn + 8], azResult[j * ncolumn + 10]);
		buffer[strlen (buffer)] = '\0';
		fwrite (buffer, strlen(buffer), 1, fp);
	}
	sqlite3_free_table(azResult);
	fclose(fp);
	cout << "buffer =============" <<buffer <<endl;
	memset (buffer, 0, sizeof(buffer));
	char* directory = get_current_dir_name();
	strcpy (buffer, directory);
	strcat (buffer, "/");
	strcat (buffer, buf);
	buffer[strlen (buffer)] = '\0';

	int rc = UploadData(buffer,0);
	if (-1 == rc)
		reprort.Update_flag(rc, (char*)"上传文件失败");
	else
	{
		reprort.Update_flag(rc, (char*)"update tips_netlog set flag='1'");
	}
	pthread_mutex_unlock(&sqlite_sync_mutex);
	char rm[1024];
	memset (rm, 0, sizeof(rm));
	unlink (buffer);
//	sprintf (rm, "rm -f %s", buffer);
//	system(rm);
	tips_sqlite_sync.tips_unlock (2);
*/
//	kill(n, SIGUSR2);
}

static void My_Strcpy(char* dest, char* src, int len)
{
	if (src == NULL)
		strncpy (dest, "", 1);
	else
		strncpy (dest, src, len - 1);
}

void report_netlog(char** result,int column,void* param)
{
	struct sql_struct* temp=(struct sql_struct*)param;
	struct audit netlog;
	memset (&netlog, 0, sizeof (struct audit)); 
	My_Strcpy(netlog.dip, result[1], sizeof(netlog.dip));
	//cout<<"netlog.Dip============"<<netlog.dip<<endl;
	My_Strcpy(netlog.dport, result[2], sizeof(netlog.dport));
	//cout <<"netlog.Dport==================="<<netlog.dport<<endl;
	My_Strcpy(netlog.sip, result[3], sizeof(netlog.sip));
	//cout <<"netlog.sip====================="<<netlog.sip<<endl;
	My_Strcpy(netlog.sport, result[4], sizeof(netlog.sport));
	My_Strcpy(netlog.protocol, result[11], sizeof(netlog.protocol));
	My_Strcpy(netlog.start_time, result[6], sizeof(netlog.start_time));
	My_Strcpy(netlog.end_time, result[7], sizeof(netlog.end_time));
	My_Strcpy(netlog.down_KB, result[8], sizeof(netlog.down_KB));
	My_Strcpy(netlog.up_KB, result[9], sizeof(netlog.up_KB));
	memcpy ((char*)temp->data + ((temp->count)*sizeof(struct audit)), &netlog, sizeof (struct audit));
	//printf("temp->data=%s\n",(struct audit*) ((char*)(temp->data) + ((temp->count)*sizeof(struct audit)))[count]-> );
	temp->count++;
	column = column;		//hyj 保留参数
}

static void select_count(char** result,int column,void* param)
{
	int* count=(int*)param;
	*count=atoll(result[0]);
	
	column = column;		//hyj 保留参数
}
void CNetlog::Audit()
{	
	char buff[512];
	char buffer[512];
	FILE *fp = NULL;
	int count;
	char sql_cmd[512];
	char sql_cmd2[512];
	char sql_count[512];
	char dstartip[64];
	char dendip[64];
	char sstartip[64];
	char sendip[64];
	memset(dstartip, 0, sizeof (dstartip));
	memset(dendip, 0, sizeof(dendip));
	memset(sstartip, 0, sizeof(sstartip));
	memset(sendip, 0, sizeof(sendip));
	
	ReportTask reprort;
	char starttime[20] = {0};
	char endtime[20] = {0};
	char update_sqlcmd[512];
	char update_sqlcmd2[512];
	memset(update_sqlcmd, 0, sizeof(update_sqlcmd));
	memset(update_sqlcmd2, 0, sizeof(update_sqlcmd2));
	reprort.TurnTime((time_t)atoi(param[5]), starttime, sizeof (starttime));
	reprort.TurnTime((time_t)atoi(param[6]) + 3600, endtime, sizeof (endtime));

	for (int i = 0; i < 2; i ++)
	{
		if ((!strcmp(param[0], "0")) || (!strcmp(param[1], "0")))
			continue;
		char t[1024];
		memset(t, 0, sizeof (t));
		strcpy(t, param[i]);
		cout << "t ========="<< t <<endl;
		uint32_t a, b;
		char *buffer = NULL;
		char *buf = NULL;
		if ((0 == i) || (1 == i))
			getlargeIP(t, &a, &b, &buffer, &buf);
		if (0 == i)
		{
			strcpy(dstartip, buffer);
			strcpy(dendip, buf);
		}
		else if (1 == i)
		{
			strcpy(sstartip, buffer);
			strcpy(sendip, buf);
		}
		if (buffer)
			delete buffer;
		if (buf)
			delete buf;
	}
	
	snprintf(sql_count, sizeof (sql_count), "select count(*) from tips_netlog where Start_time>='%s' and End_time<='%s' and flag='0' ", starttime, endtime);
	snprintf(sql_cmd, sizeof (sql_cmd), "select * from tips_netlog where Start_time>='%s' and End_time<='%s' and flag='0' ", starttime, endtime);
	snprintf(update_sqlcmd, sizeof (update_sqlcmd), "update tips_netlog set flag='1' where Start_time>='%s' and End_time<='%s' and flag='0' ", starttime, endtime);
	if (strcmp(param[0], "0"))
	{
		char temp[100] = {0};
		snprintf(temp, sizeof (temp), " and Sip>='%s' and Sip<='%s' ", sstartip, sendip);
		strcat(sql_cmd, temp);
		strcat(update_sqlcmd, temp);
		strcat(sql_count, temp);
	}
	if (strcmp(param[1], "0"))
	{
		char temp[100] = {0};
		snprintf(temp, sizeof (temp), " and Dip>='%s' and Dip<='%s' ", dstartip, dendip);
		strcat(sql_cmd, temp);
		strcat(update_sqlcmd, temp);
		strcat(sql_count, temp);
	}
	if (strcmp(param[2], "0"))
	{
		char temp[100] = {0};
		snprintf(temp, sizeof (temp), " and Sport='%s' ", param[2]);
		strcat(sql_cmd, temp);
		strcat(update_sqlcmd, temp);
		strcat(sql_count, temp);
	}
	if (strcmp(param[3], "0"))
	{
		char temp[100] = {0};
		snprintf(temp, sizeof (temp), " and Dport='%s' ", param[3]);
		strcat(sql_cmd, temp);
		strcat(update_sqlcmd, temp);
		strcat(sql_count, temp);
	}
	if (strcmp(param[4], "0"))
	{
		char temp[100] = {0};
		snprintf(temp, sizeof (temp), " and Below_protocol='%s' ", param[4]);
		strcat(sql_cmd, temp);
		strcat(update_sqlcmd, temp);
		strcat(sql_count, temp);
	}

	sqlhandler->Select_Sql_Exec(sql_count,select_count,&count);
    int j=0, remain = count;
    printf("select count == %d\n", count);
	if(count<=0)
    {
		cout<<"no audit info report!"<<endl;
		return;
	}


    memset(buff, 0, sizeof (buff));
    snprintf(buff, sizeof (buff), "1014_%ld_%s_%s.log", time (NULL), LOCALDEVID, param[7]);
    fp = fopen(buff, "w+");
    if (NULL == fp) {
    	fprintf (stderr, "Error : %s\n", strerror (errno));
    }

    for(j=0; j<=count/500;j++)
    {
        int num = 0;
	    num = remain>=500 ? 500: remain;
    	printf("j=%d, remain=%d\n", j, remain);

    	snprintf(sql_cmd2, sizeof(sql_cmd2), "%s limit %d;",sql_cmd, num);
    	sql_cmd2[strlen(sql_cmd2)] = '\0';
    	
    	snprintf(update_sqlcmd2, sizeof(update_sqlcmd2), "%s limit %d;",update_sqlcmd, num);
    	update_sqlcmd2[strlen(update_sqlcmd2)] = '\0';

    	struct audit * cache=(struct audit*)malloc(sizeof(struct audit)*num);
    	if(cache == NULL) {
    	    printf("cache malloc error\n");
    	    return;
        }
    
		struct sql_struct tem;
		tem.data=cache;
		tem.count=0;
		int ret = sqlhandler->Select_Sql_Exec(sql_cmd2,report_netlog,&tem);
		if(ret != 1)
        {
            printf("数据库查询失败\n");
            return ;
        }
		
		for(int i = 0; i < num; i++)
		{
			memset(buffer, 0, sizeof (buffer));
			snprintf(buffer, sizeof (buffer), "%s#%s#%s#%s#%s#%s#%s#%s#%s#%s#%s\n", LOCALDEVID, param[7], cache[i].dip, cache[i].dport, cache[i].sip, cache[i].sport, cache[i].protocol, cache[i].start_time, cache[i].end_time, cache[i].down_KB, cache[i].up_KB);
			fwrite(buffer, strlen(buffer), 1, fp);
		}
		free(cache);
		
		ret = sqlhandler->Insert_Sql_Exec(update_sqlcmd2);
		if(-1 == ret)
		{
			cout<<"faied:"<<update_sqlcmd2<<endl;
		}
        else 
        {
		    remain = remain - num;
        }
    }
	fclose(fp);

	memset(buffer, 0, sizeof (buffer));
	char* directory = get_current_dir_name();
	strcpy(buffer, directory);
	strcat(buffer, "/");
	strcat(buffer, buff);
	buffer[strlen (buffer)] = '\0';

	int rc=0;
	if( -1 == (rc = UploadData(buffer,0)) ) {
		reprort.Update_flag(rc, (char*)"上传文件失败");
    }
	
    char rm[1024];
    memset (rm, 0, sizeof(rm));
    unlink (buffer);
    sprintf (rm, "rm -f %s", buffer);
    system(rm);
}

CNetlog::CNetlog(char* p)
{
	char temp[1024];
	memset(temp, 0, sizeof (temp));
	strcpy(temp, p);
	char* token = NULL;
	token = strtok(temp, " ");
	int cnt = 1;
	while ((bool)(token = strtok(NULL, " ")))
	{
		cnt ++;
		if((cnt % 2) == 0)
		{
			memset(param[cnt / 2 - 1], 0, sizeof (param[cnt/2 - 1]));
			strcpy(param[cnt / 2 - 1], token);
		}
		if(cnt == 16)
			break;
	}
//	for (int i = 0; i < 8; i++)
	//	cout <<"param[" << i << "]=========" << param[i] <<endl; 
}

/*
void CNetlog::setDB(sqlite3* dbhandler)
{
	db = dbhandler;
}
*/
