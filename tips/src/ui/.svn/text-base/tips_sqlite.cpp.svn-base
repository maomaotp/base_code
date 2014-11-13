#include "tips_sqlite.h"

GMutex *user_mutex = NULL;
//GMutex *user_bak_mutex = NULL;
////**********************************extern pthread_mutex_t sqlite_sync_mutex;
////**********************************extern int startrun;
////**********************************extern CTips_sqlite_sync tips_sqlite_sync;

int tipscallback_foradmit_time(void* source_xml, int argc, char **argv, char **argvv)
{
	argc=argc;
	argvv=argvv;
	if((argv[0] == NULL) || (strlen(argv[0]) == 0))
		strcpy((char *)source_xml,"");
	else
		strcpy((char *)source_xml,argv[0]);
	return 0;
}

void call_back(char** result,int column,void* param)
{
	char *xmltemp;
	char *xmlparam=strdup("");
	xml_stut_t *xml_opt_temp = (xml_stut_t *)param;
	char **xml = &(xml_opt_temp->xml);
	xml_part* (*xmlfun)[1] = (xml_fun_opt)xml_opt_temp->fun;
	xmltemp = *xml;
	printf("argc=%d\n",column);
	for(int i=0; i < column; i++){
		(*((*xmlfun)[i]))(result[i], &xmlparam);
		//printf("hello tomorrow=====5=5=5=5==5=5==5=5\n");
		//xml_part_opt[i](argv[i], &xmlparam);
		//printf("hello tomorrow=====5=5=5=5==5=5==5=5\n");
	}
	//char *xmlparam=strdup("");
	*xml = g_strdup_printf("%s<list>%s</list>",xmltemp,xmlparam);
	g_free(xmltemp);
	g_free(xmlparam);
	//return ;
}
/*打开数据库，并初始化互斥量*/
CUser_sqlite_handle::CUser_sqlite_handle()
{
	bool err=sqlhandler.Init_Sql((char *)"127.0.0.1",(char *)"root",(char *)"123456",(char *)"tips",0,NULL,0);
	if(!err)
		printf("初始化sql错误!");
	//char* query=(char*)"select * from tttips_netlog;";
	//int ret_sql=sqlhandler.Select_Sql_Exec(query,call_back,&count);
	//printf("ret==========%d\n",count);
	//sqlhandler.Exit_Sql();
//***************************	int ret = sqlite3_open("/data/tipsdb/tips.db", &db);
//***************************	if(ret != 0){
//***************************		printf("user.db miss\n");
//***************************		exit(0);
//***************************	}
//***************************#if 1
//***************************	ret = sqlite3_key(db, "admin", 5);
//***************************	if(ret != 0){
//***************************		printf("Key error\n");
//***************************		exit(0);
//***************************	}
//***************************#endif
//***************************	ret = sqlite3_open("/data/tipsbakdb/user/tips_bak.db", &bdb);
	int ret = sqlite3_open("/data/tipsbakdb/user/tips_bak.db", &bdb);
	if(ret != 0){
		printf("user.db miss\n");
		exit(0);
	}
#if 1
	ret = sqlite3_key(bdb, "admin", 5);
	if(ret != 0){
		printf("Key error\n");
		exit(0);
	}
//	ret =   sqlite3_rekey( bdb, NULL, 0 );
//	if(ret != 0){
//		printf("ReKey error\n");
//		exit(0);
//	}
//		printf("ReKey1111 error\n");
//		sqlite3_close(bdb);
//		exit(0);
#endif
	if(NULL == user_mutex)
		user_mutex = g_mutex_new();
////************************	if(NULL == user_bak_mutex)
////************************		user_bak_mutex = g_mutex_new();
////************************	tips_sqlite_sync.init_sync_mutex();
////************************	tips_sqlite_sync.sqlite_sync_cli_init((char *)TIPS_DIR);
}

CUser_sqlite_handle::~CUser_sqlite_handle()
{
//	sqlite3_close(db);
	sqlite3_close(bdb);
	sqlhandler.Exit_Sql();
	g_mutex_free(user_mutex);
	user_mutex = NULL;
////***********************	g_mutex_free(user_bak_mutex);
////***********************	user_bak_mutex = NULL;
}

int judge_account_exist(void* source_user, int argc, char **argv, char **argvv)
{
/*保留参数？*/argc = argc; argv = argv; argvv = argvv;
	sqlite_user_t *user = (sqlite_user_t *)source_user;
	user->login_flag = 1;
	return 0;
}


int myObtainTime(const char *start_time,const char *end_time)//划分时间段
{
	int start_year=0,start_month = 0,start_day = 0; 
        int end_year=0,end_month = 0,end_day = 0;     
        char start_dalte_buf[10];
        char end_dalte_buf[10];
        memset(start_dalte_buf,0,sizeof(start_dalte_buf));
        memset(end_dalte_buf,0,sizeof(end_dalte_buf));
        sscanf(start_time,"%d-%d-%d%s",&start_year,&start_month,&start_day,start_dalte_buf);
        sscanf(end_time,"%d-%d-%d%s",&end_year,&end_month,&end_day,end_dalte_buf);
        printf("start_year=%d,start_month=%d,start_day=%d\n",start_year,start_month,start_day);
        printf("end_year=%d,end_month=%d,end_day=%d\n",end_year,end_month,end_day);
#if 0
        if(end_year > start_year)
                return 1;
        else 
        {    
                if(end_month > start_month)
                        return 1;
                else 
                {    
                        if(end_day > start_day)
                                return 1;
                }    

        }    
#endif
       if(start_year > end_year)
               return 0;
       else if(start_year < end_year)//1代表用户时间过期，0代表未过期
               return 1;
       else if(start_year == end_year)
       {
               if(start_month > end_month)
                       return 0;
               else if(start_month < end_month)
                       return 1;
               else if(start_month == end_month)
               {

                       if(start_day > end_day)
                               return 0;
                       else if(start_day == end_day)
                               return 0;
		       else
				return 1;
               }

       }
        return 0;
}

int judge_admit_time(char *client_time)//1代表用户时间过期，0代表未过期
{
		if((NULL == client_time) || (!strlen(client_time)))
		{
			return 2;
		}
        char timebuf[100];
        memset(timebuf,0,100);
        struct tm *tm;
        time_t clock;
        clock = time((time_t *)NULL);
        char *format=(char *)"%Y-%m-%d";
        tm = localtime(&clock);
        strftime(timebuf,sizeof(timebuf),format,tm);
	printf("timebuf is is %s\n",timebuf);
	return myObtainTime(client_time,timebuf);
}
/*用来核对帐号和密码*/
int judge_user(void* source_user, int argc, char **argv, char **argvv)
{
/*保留参数？*/argc = argc; argvv = argvv;
	sqlite_user_t *user = (sqlite_user_t *)source_user;
	if((!strcmp(user->password, argv[0]))){
		user->role = g_strdup_printf("%s", argv[1]);
		if(!strcmp(argv[1],"1"))
		{
			printf("超级用户里边\n");
			user->login_flag = 1;
		}
		else
		{
			printf("in 普通用户和管理员中\n");
			user->admit_time = g_strdup_printf("%s",argv[2]);
			user->state = g_strdup_printf("%s",argv[3]);
			printf("user->admit_time=====%s",user->admit_time);
			printf("user->state=====%s",user->state);
			int judge_login_time = judge_admit_time(user->admit_time);
			printf("judge_login_time is %d\n",judge_login_time);
			if(!judge_login_time)	
			{
				if(!strcmp((char *)"启用",user->state))
					user->login_flag = 1;//可以登陆
				else
					user->login_flag = 2;//用户停用但没有过期
			}
			else
				user->login_flag = 3;//用户使用时间到期
			
		}
	}
	return 0;
}


int judge_admit_user(void* source_user, int argc, char **argv, char **argvv)
{
/*保留参数？*/argc = argc; argvv = argvv;
	user_t *user = (user_t *)source_user;
	if(!strcmp(argv[0],"1"))
	{
		printf("超级用户里边\n");
		user->login_flag = 1;
	}
	else
	{
		user->admit_time = g_strdup_printf("%s",argv[1]);
		user->state = g_strdup_printf("%s",argv[2]);
		int judge_login_time = judge_admit_time(user->admit_time);
		if(!judge_login_time)	
		{
			if(!strcmp((char *)"启用",user->state))
				user->login_flag = 1;//可以继续使用
			else
				user->login_flag = 2;//用户停用但没有过期(不可以继续使用)
		}
		else
			user->login_flag = 3;//用户使用时间到期
		
	}
	return 0;
}

/*查找用户接口，判断用户是否已经存在，或者验证密码*/
int CUser_sqlite_handle::User_sqlite_interface(sqlite_user_t *user,
					       tips_callback tipscallback)
{
	char *username = NULL;
	init_sql_num((char *)(user->username), &username);
	char *sql =g_strdup_printf("select password,role,admit_time,state from tips_user where userName='%s'",username);
	free(username);
	char *errmsg;
	g_mutex_lock(user_mutex);
	user->login_flag = 0;
	int ret = sqlite3_exec(bdb, sql, tipscallback, user, &errmsg);
	if(ret != 0){
		printf("user.db1:%s\n",errmsg);
		g_mutex_unlock(user_mutex);
		g_free(sql);
		return 0;
	}
	printf("user->login_flag ==== %d\n",user->login_flag);
	if(user->login_flag == 1){
		g_mutex_unlock(user_mutex);
		g_free(sql);
		return 1;			//验证信息正确
	}
	else if(user->login_flag == 2){
		g_mutex_unlock(user_mutex);
		g_free(sql);
		return 2;			//用户停用但没有过期

	}
	else if(user->login_flag == 3){
		g_mutex_unlock(user_mutex);
		g_free(sql);
		return 3;			///用户使用时间到期
	}
	else
	{
		g_mutex_unlock(user_mutex);
		g_free(sql);
		return 0;				//密码错误或账户不存在
	}
}



int CUser_sqlite_handle::User_admit_sqlite_interface(user_t *user,
					       tips_callback tipscallback)
{
	char *username = NULL;
	init_sql_num((char *)(user->username), &username);
	char *sql =g_strdup_printf("select role,admit_time,state from tips_user where userName='%s'",username);
	free(username);
	char *errmsg;
	g_mutex_lock(user_mutex);
	user->login_flag = 0;
	int ret = sqlite3_exec(bdb, sql, tipscallback, user, &errmsg);
	if(ret != 0){
		printf("user.db1:%s\n",errmsg);
		g_mutex_unlock(user_mutex);
		g_free(sql);
		return 4;//查找数据库失败
	}
	printf("user->login_flag ==== %d\n",user->login_flag);
	if(user->login_flag == 1){
		g_mutex_unlock(user_mutex);
		g_free(sql);
		return 1;			//验证信息正确
	}
	else if(user->login_flag == 2){
		g_mutex_unlock(user_mutex);
		g_free(sql);
		return 2;			//用户停用但没有过期

	}
	else if(user->login_flag == 3){
		g_mutex_unlock(user_mutex);
		g_free(sql);
		return 3;			///用户使用时间到期
	}
	else
	{
		g_mutex_unlock(user_mutex);
		g_free(sql);
		return 0;				//密码错误或账户不存在
	}
}

/*判断密码是否正确，正确返回1，错误返回0*/
int CUser_sqlite_handle::User_login_judge(sqlite_user_t *user)
{
	printf("in user_login_judge\n");
	return User_sqlite_interface(user, judge_user);
}
//在登录过程中判断是不是被超级用户或者是管理员修改了使用日期(admit_time)和权限（state）
int CUser_sqlite_handle::User_admit_login_judge(user_t *user)
{
	printf("in user_login_judge\n");
	return User_admit_sqlite_interface(user, judge_admit_user);
}

/*插入用户，失败返回0成功返回1*/
int CUser_sqlite_handle::User_insert(sqlite_user_t* user)
{
	char *errmsg;
	if(User_sqlite_interface(user, judge_account_exist))
		return 0; 				//帐号存在返回0
	g_mutex_lock(user_mutex);
	char *username = NULL;
	char *password = NULL;
	init_sql_num((char *)(user->username), &username);
	init_sql_num((char *)(user->password), &password);
	gchar *sql = g_strdup_printf("insert into tips_user(userName, password,\
				createTime, role,admit_time,client_name,unit_name,telephone,state,last_time)\
					 values('%s','%s','%s','%s','%s','%s','%s','%s','%s','%s')", 
					username,
					password,
					user->createtime,
					user->role,
					user->admit_time,
					user->client_name,
					user->unit_name,
					user->telephone,
					user->state,"----");
	free(username);
	free(password);
	printf("sql===%s\n",sql);
	int ret = sqlite3_exec(bdb, sql, NULL, NULL, &errmsg);
	if(ret != 0){
		printf("user.db2:%s\n",errmsg);
		g_free(sql);
		g_mutex_unlock(user_mutex);
		return 0;
	}
	g_free(sql);
	g_mutex_unlock(user_mutex);
	return 1;					//插入成功返回1
}




int tips_catchxml(void* source_xml, int argc, char **argv, char **argvv)
{
/*保留参数？*/argc = argc; argvv = argvv;
	gchar *temp;
	char *(turnrole[3]) = {(char *)"普通用户",(char *)"超级管理员",(char *)"管理员"};
	printf("role==************************%d\n",atoi(argv[3]));
	printf("role==*******%s\n",argv[3]);
	if(1 != atoi(argv[3])){
		if(*((char **)source_xml) != NULL){
			int judge_admit = 	judge_admit_time(argv[4]);
			if(1 == judge_admit)
			{
				temp = g_strdup_printf("%s<tips_user><username><![CDATA[%s]]></username><passwd>******</passwd><createtime>%s</createtime><userrole>%s</userrole><admit_time>%s</admit_time><client_name>%s</client_name><unit_name>%s</unit_name><telephone>%s</telephone><last_time>%s</last_time><state>%s</state></tips_user>",*((char **)source_xml), argv[0],argv[2],turnrole[atoi(argv[3])],argv[4],argv[5],argv[6],argv[7],argv[8],(char *)"停用");
			}
			else if( 2 == judge_admit)
			{
				//temp = g_strdup_printf("%s<tips_user><username><![CDATA[%s]]></username><passwd>******</passwd><createtime>%s</createtime><userrole>%s</userrole><admit_time>%s</admit_time><client_name>%s</client_name><unit_name>%s</unit_name><telephone>%s</telephone><last_time>%s</last_time><state>%s</state></tips_user>",*((char **)source_xml), argv[0],argv[2],turnrole[atoi(argv[3])],(char *)"永久有效",argv[5],argv[6],argv[7],argv[8],(char *)"启用");
				temp = g_strdup_printf("%s<tips_user><username><![CDATA[%s]]></username><passwd>******</passwd><createtime>%s</createtime><userrole>%s</userrole><admit_time>%s</admit_time><client_name>%s</client_name><unit_name>%s</unit_name><telephone>%s</telephone><last_time>%s</last_time><state>%s</state></tips_user>",*((char **)source_xml), argv[0],argv[2],turnrole[atoi(argv[3])],(char *)"永久有效",argv[5],argv[6],argv[7],argv[8],argv[9]);//modify lipeiting 2014-05-07
			}
			else
			{
				temp = g_strdup_printf("%s<tips_user><username><![CDATA[%s]]></username><passwd>******</passwd><createtime>%s</createtime><userrole>%s</userrole><admit_time>%s</admit_time><client_name>%s</client_name><unit_name>%s</unit_name><telephone>%s</telephone><last_time>%s</last_time><state>%s</state></tips_user>",*((char **)source_xml), argv[0],argv[2],turnrole[atoi(argv[3])],argv[4],argv[5],argv[6],argv[7],argv[8],argv[9]);
			}
			g_free(*((char **)source_xml));
		}
		else
		{
			int judge_admit = 	judge_admit_time(argv[4]);
			if(1 == judge_admit)
			{
				temp = g_strdup_printf("<tips_user><username><![CDATA[%s]]></username><passwd>******</passwd><createtime>%s</createtime><userrole>%s</userrole><admit_time>%s</admit_time><client_name>%s</client_name><unit_name>%s</unit_name><telephone>%s</telephone><last_time>%s</last_time><state>%s</state></tips_user>",  argv[0], argv[2], turnrole[atoi(argv[3])],argv[4],argv[5],argv[6],argv[7],argv[8],(char *)"停用");
			}
			else if( 2== judge_admit)
			{
				temp = g_strdup_printf("<tips_user><username><![CDATA[%s]]></username><passwd>******</passwd><createtime>%s</createtime><userrole>%s</userrole><admit_time>%s</admit_time><client_name>%s</client_name><unit_name>%s</unit_name><telephone>%s</telephone><last_time>%s</last_time><state>%s</state></tips_user>",  argv[0], argv[2], turnrole[atoi(argv[3])],(char *)"永久有效",argv[5],argv[6],argv[7],argv[8],(char *)"启用");
			}
			else
			{
				temp = g_strdup_printf("<tips_user><username><![CDATA[%s]]></username><passwd>******</passwd><createtime>%s</createtime><userrole>%s</userrole><admit_time>%s</admit_time><client_name>%s</client_name><unit_name>%s</unit_name><telephone>%s</telephone><last_time>%s</last_time><state>%s</state></tips_user>",  argv[0], argv[2], turnrole[atoi(argv[3])],argv[4],argv[5],argv[6],argv[7],argv[8],argv[9]);
			}
		}
		*((char **)source_xml) = temp;
		printf("callback:xml==%s\n",*((char **)source_xml));
	}
	return 0;
}
char *CUser_sqlite_handle::User_catch(char *role)
{
	char sql[1024];
	memset(sql, 0, 1024);
	printf("role in user_catch------------------------------------%s\n",role);
	if(NULL != role){
		snprintf(sql, 1024, "select userName,password,createTime,role,admit_time,client_name,unit_name,telephone,last_time,state from tips_user where role!='%s'", role);
	}
	else{
		strcpy(sql, "select userName,password,createTime,role,admit_time,client_name,unit_name,telephone,last_time,state from tips_user");
	}
	char *errmsg;
	char *xml=NULL;
	g_mutex_lock(user_mutex);
	int ret = sqlite3_exec(bdb, sql, tips_catchxml, &xml, &errmsg);
	if(ret != 0){
		printf("user.db3:%s\n",errmsg);
		g_mutex_unlock(user_mutex);
		return xml;
	}
	g_mutex_unlock(user_mutex);
	return xml;

}

char *CUser_sqlite_handle::User_search(const char *username,const char *role)
{
         /*1代表超级用户管理员，0代表普通用户管理员*/
	printf("user role is %s\n",role);
         char sql[1024]; 
         memset(sql, 0, 1024); 
         printf("username------------------------------------%s\n",username);
         if(!strcmp(username,"")){
         printf("usrename is NULLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL");
	   sprintf(sql,"select userName,password,createTime,role,admit_time,client_name,unit_name,telephone,last_time,state from tips_user where role='%s'",role);
//           strcpy(sql, "select userName,password,createTime,role from tips_user");                        
         }
         else{                                                                                            
         printf("usrename is NontttttttttttttttttttttttttttttttttttttttttttLLLLLLLLLLLLLLLLLLLLLLLLLLLL");
           snprintf(sql, 1024, "select userName,password,createTime,role,admit_time,client_name,unit_name,telephone,last_time,state from tips_user where  userName like'%s%%' and role='%s'", username,role);
         }                  
         printf("sql************====%s\n",sql);
         char *errmsg;      
         char *xml=NULL;    
         g_mutex_lock(user_mutex);
         int ret = sqlite3_exec(bdb, sql, tips_catchxml, &xml, &errmsg);
         if(ret != 0){
           g_mutex_unlock(user_mutex);
           printf("user.db3:%s\n",errmsg);
	   return xml;
         }
         g_mutex_unlock(user_mutex);
         return xml;

}

int CUser_sqlite_handle::User_delete(const char *username)
{
	char *tempuser = NULL;
	init_sql_num((char *)username, &tempuser);
	char *sql = g_strdup_printf("delete from tips_user where userName='%s';"
					,tempuser);
	printf("sql===%s\n",sql);
	free(tempuser);
	g_mutex_lock(user_mutex);
	int ret = sqlite3_exec(bdb, sql, NULL, NULL, NULL);
	if(ret != 0){
		printf("delete fail\n");
		g_mutex_unlock(user_mutex);
		free(sql);
		return 0;
	}
	g_mutex_unlock(user_mutex);
	free(sql);
	return 0;
}

int CUser_sqlite_handle::User_stop(const char *username)
{
	char *tempuser = NULL;
	init_sql_num((char *)username, &tempuser);
	char *sql = g_strdup_printf("update tips_user set state='停用' where userName='%s';"
					,tempuser);
	printf("sql===%s\n",sql);
	free(tempuser);
	g_mutex_lock(user_mutex);
	int ret = sqlite3_exec(bdb, sql, NULL, NULL, NULL);
	if(ret != 0){
		printf("update fail\n");
		g_mutex_unlock(user_mutex);
		free(sql);
		return 0;
	}
	g_mutex_unlock(user_mutex);
	free(sql);
	return 0;
}
int CUser_sqlite_handle::get_admit_time(const char *username)
{
	char temp[30];
	char *tempuser = NULL;
	memset(temp,0,30);
	init_sql_num((char *)username, &tempuser);
	char *sql = g_strdup_printf("select admit_time from tips_user where userName='%s';"
					,tempuser);
	printf("sql===%s\n",sql);
	free(tempuser);
	g_mutex_lock(user_mutex);
	//int ret = sqlite3_exec(bdb, sql, NULL, NULL, NULL);
	int ret = sqlite3_exec(bdb, sql, tipscallback_foradmit_time, temp, NULL);
	if(ret != 0){
		printf("update fail\n");
		g_mutex_unlock(user_mutex);
		free(sql);
		return 0;
	}
	g_mutex_unlock(user_mutex);
	free(sql);
	if(!strlen(temp))
		return 0;
	int judge_limit = judge_admit_time(temp);
	return judge_limit;
	
}
int CUser_sqlite_handle::User_start(const char *username)
{
	char *tempuser = NULL;
	init_sql_num((char *)username, &tempuser);
	char *sql = g_strdup_printf("update tips_user set state='启用' where userName='%s';"
					,tempuser);
	printf("sql===%s\n",sql);
	free(tempuser);
	g_mutex_lock(user_mutex);
	int ret = sqlite3_exec(bdb, sql, NULL, NULL, NULL);
	if(ret != 0){
		printf("update fail\n");
		g_mutex_unlock(user_mutex);
		free(sql);
		return 0;
	}
	g_mutex_unlock(user_mutex);
	free(sql);
	return 0;
}
int role_judge(void *role, int argc, char **argv, char **argvv)
{
/*保留参数?*/argc = argc; argvv = argvv;
	*((gchar **)role) = g_strdup_printf("%s",argv[0]);
	return 0;
}

char* CUser_sqlite_handle::User_Role_Judge(const char *username)
{
	char *role;
	char *errmsg;
	char *tempuser = NULL;
	init_sql_num((char *)username, &tempuser);
	char *sql = g_strdup_printf("select role from tips_user where userName='%s';", tempuser);
	free(tempuser);
	g_mutex_lock(user_mutex);
	int ret = sqlite3_exec(bdb, sql, role_judge, &role, &errmsg);
	if(ret != 0){
		printf("judge error:%s",errmsg);
	}
	free(sql);
	g_mutex_unlock(user_mutex);
	return role;
}
int CUser_sqlite_handle::User_Update_myself(sqlite_user_t *user)
{
	char *errmsg;
	int ret;
	char *username = NULL;
	char *password = NULL;
	char *sql = NULL;
	init_sql_num((char *)(user->username), &username);
	init_sql_num((char *)(user->password), &password);
	sql = g_strdup_printf("update tips_user set password='%s' where userName='%s';", password,username);
	free(username);
	free(password);
	g_mutex_lock(user_mutex);
	ret = sqlite3_exec(bdb, sql, NULL, NULL, &errmsg);
	if(ret != 0){
		printf("update error:%s",errmsg);
	}
	free(sql);
	g_mutex_unlock(user_mutex);
	return 0;
}

int CUser_sqlite_handle::User_Update(sqlite_user_t *user)
{
	char *errmsg;
	int ret;
	char *username = NULL;
	char *password = NULL;
	char *admit_time = NULL;
	char *client_name = NULL;
	char *unit_name = NULL;
	char *telephone = NULL;
	//char *state = NULL;
	char *sql = NULL;
	init_sql_num((char *)(user->username), &username);
	init_sql_num((char *)(user->password), &password);
	init_sql_num((char *)(user->admit_time), &admit_time);
	init_sql_num((char *)(user->client_name), &client_name);
	init_sql_num((char *)(user->unit_name), &unit_name);
	init_sql_num((char *)(user->telephone), &telephone);
	//init_sql_num((char *)(user->state), &state);
	printf("username==%s,,,password==%s,,,admit_time==%s,,,client_name==%s,,,unit_name==%s,,telephone==%s\n",username,password,admit_time,client_name,unit_name,telephone);
	if(strlen(user->password) != 0)
		sql = g_strdup_printf("update tips_user set password='%s', admit_time='%s', client_name='%s', unit_name='%s', telephone='%s' where userName='%s';", password, admit_time,client_name,unit_name,telephone,username);
	else
		sql = g_strdup_printf("update tips_user set admit_time='%s', client_name='%s', unit_name='%s', telephone='%s' where userName='%s';",admit_time,client_name,unit_name,telephone,username);
//sql===update tips_user set state='启用' where userName='lipeiting';
	//char *sql = g_strdup_printf("update tips_user set last_time='%s' where userName='%s';",clock,tempuser);
	printf("zhao zi li===%s\n",sql);
	printf("zhao zi li===%s\n",sql);
	printf("zhao zi li===%s\n",sql);
	free(username);
	free(password);
	free(admit_time);
	free(client_name);
	free(unit_name);
	free(telephone);
	//free(state);
	g_mutex_lock(user_mutex);
	ret = sqlite3_exec(bdb, sql, NULL, NULL, &errmsg);
	if(ret != 0){
		printf("update error:%s",errmsg);
	}
	free(sql);
	g_mutex_unlock(user_mutex);
	return 0;
}

//extern xml_part* xml_part_opt[5];
int xml_callback(void *xml_opt, int argc, char **argv, char **argvv)
{
/*保留参数？*/argvv = argvv;
	char *xmltemp;
	char *xmlparam=strdup("");
	xml_stut_t *xml_opt_temp = (xml_stut_t *)xml_opt;
	char **xml = &(xml_opt_temp->xml);
	xml_part* (*xmlfun)[1] = (xml_fun_opt)xml_opt_temp->fun;
	xmltemp = *xml;
	printf("argc=%d\n",argc);
	for(int i=0; i < argc; i++){
		(*((*xmlfun)[i]))(argv[i], &xmlparam);
		//printf("hello tomorrow=====5=5=5=5==5=5==5=5\n");
		//xml_part_opt[i](argv[i], &xmlparam);
		//printf("hello tomorrow=====5=5=5=5==5=5==5=5\n");
	}
	*xml = g_strdup_printf("%s<list>%s</list>", xmltemp
							    , xmlparam);
	g_free(xmltemp);
	g_free(xmlparam);
	return 0;
}

char* CUser_sqlite_handle::catch_xml_interface(char *sql, void *xmlpartfun)
{
	//char* query=(char*)"select * from tttips_netlog;";
	//int ret_sql=sqlhandler.Select_Sql_Exec(sql,call_back,&xml_opt);
	//printf("ret==========%d\n",count);
	//sqlhandler.Exit_Sql();
	printf("sql========%s\n",sql);
//hyj	char *errmsg;
	xml_stut_t xml_opt;
	xml_opt.xml = g_strdup("");
	xml_opt.fun = xmlpartfun;
	//tips_sqlite_sync.tips_lock(1);
	//*******************************g_mutex_lock(user_mutex);
	/*hyj返回值没有被使用int ret=*/sqlhandler.Select_Sql_Exec(sql,call_back,&xml_opt);
	//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&if(ret != 0){
	//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&	printf("sqlite interface error:%s\n",errmsg);
	//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&	////***********************tips_sqlite_sync.tips_unlock(1);
	//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&	return NULL;
	//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&	////*********************** exit(0);
	//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&}
	//*******************************g_mutex_unlock(user_mutex);
	//*******************************tips_sqlite_sync.tips_unlock(1);
	printf("xml_opt.xml = %s\n",xml_opt.xml);
	return xml_opt.xml;
}


char* CUser_sqlite_handle::catch_xml_data(char *sql, void *xmlpartfun)
{
	printf("sql========%s\n",sql);
	char *errmsg;
	xml_stut_t xml_opt;
	xml_opt.xml = g_strdup("");
	xml_opt.fun = xmlpartfun;
	g_mutex_lock(user_mutex);
	int ret = sqlite3_exec(bdb, sql, xml_callback, &xml_opt, &errmsg);
	if(ret != 0){
		printf("sqlite interface error:%s\n",errmsg);
	}
	g_mutex_unlock(user_mutex);
	printf("xml_opt.xml = %s\n",xml_opt.xml);
	return xml_opt.xml;
}

int CUser_sqlite_handle::insert_login_time(const char *user)//插入上次登陆时间
{
	char *tempuser = NULL;
	time_t nowtemp;
	nowtemp = time(NULL);
	char clock[20];
	memset(clock, 0, 20);
	TurnTime(nowtemp, clock, 20);
	init_sql_num((char *)user, &tempuser);
	char *sql = g_strdup_printf("update tips_user set last_time='%s' where userName='%s';",clock,tempuser);
	printf("sql===%s\n",sql);
	free(tempuser);
	g_mutex_lock(user_mutex);
	int ret = sqlite3_exec(bdb, sql, NULL, NULL, NULL);
	if(ret != 0){
		printf("update fail\n");
		g_mutex_unlock(user_mutex);
		free(sql);
		return 0;
	}
	g_mutex_unlock(user_mutex);
	free(sql);
	return 0;
	
}
int CUser_sqlite_handle::catch_opt(char *user, char *des)
{
	int ret;
//hyj	char *errmsg;
	time_t nowtemp;
	gchar *sql;
	char *username = NULL;
	init_sql_num(user, &username);
	nowtemp = time(NULL);
	char clock[20];
	memset(clock, 0, 20);
	TurnTime(nowtemp, clock, 20);
	sql = g_strdup_printf("insert into tips_opt(username,Otime,Optdsc,flag) values('%s','%s','%s','0')",username, clock, des);
	free(username);
	/////***************tips_sqlite_sync.tips_lock(1);
	/////***************g_mutex_lock(user_mutex);
	ret = sqlhandler.Insert_Sql_Exec(sql);
	//ret = sqlite3_exec(db, sql, NULL, NULL, &errmsg);
	if(ret == -1 ){
		printf("insert error\n");
		/////***************tips_sqlite_sync.tips_unlock(1);
		/////***************exit(0);
		return 1;
	}
	/////***************g_mutex_unlock(user_mutex);
	/////***************tips_sqlite_sync.tips_unlock(1);
	g_free(sql);
	return 0;
}

void CUser_sqlite_handle::TurnTime(const time_t clock, char *tips_clock, int clocksize)
{
   struct tm *tm;
   char *format=(char *)"%Y-%m-%d %H:%M:%S";
   tm = localtime(&clock);
   strftime(tips_clock,  clocksize, format,tm);
}

int CUser_sqlite_handle::init_sql_num(char* src, char** dest)
{
	int destlen;
	char *temp=NULL;
	if(src != NULL){
		if(*dest == NULL){
			destlen = 0;
		}
		else{
			destlen = strlen(*dest);
		}
		if(NULL != (temp = strchr(src, '\''))){
			*dest = (char *)realloc(*dest, strlen(src)-strlen(temp)+1+destlen+2);
			memset(*dest+destlen,0,strlen(src)-strlen(temp)+1+2);
			if(destlen != 0)
				strncat(*dest,src,strlen(src)-strlen(temp));
			else{
				memcpy(*dest, src, strlen(src)-strlen(temp));
			}
			strcat(*dest,"\'\'");
			init_sql_num(temp+1,dest);
		}
		else{
			*dest=(char *)realloc(*dest,destlen+strlen(src)+1);
			memset(*dest+destlen,0,strlen(src)+1);
			if(destlen != 0){
				strcat(*dest,src);
			}
			else{
				strcpy(*dest, src);
			}
		}
	}
	return 0;
}

void callback_alarm(char **argv,int argc,void *dbnow)
{
	db_now_t* dbnowtemp = (db_now_t *)dbnow;
	char sql[2048];
	int ret = 0;
	char *err = NULL;
	memset(sql, 0, 2048);
	if(argc != 14){
		printf("alarm lenth wrong\n");
		return ;
	}
	ret = snprintf(sql, 2047, "insert into tips_Alarm(Trojan_id,Sip,Dip,Dmac,Alarm_time,Type,Risk,Scc_flag,Scc,Descry,Alarm_iid,Device_id,BackID,Alarm_judgetime,flag) values('%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%lld','%s','%s')",argv[0], argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], argv[7], argv[8], argv[9], argv[10], argv[11],dbnowtemp->nowspace,argv[12], argv[13]);
	if((ret < 2047) && (ret > 0)){
		//g_mutex_lock(user_mutex);
		ret = sqlite3_exec(dbnowtemp->db, sql, NULL, NULL, &err);
		if(ret != 0){
			printf("copy db error:%s",err);
			//g_mutex_unlock(user_mutex);
			return ;
		}
		//g_mutex_unlock(user_mutex);
	}
	else{
		printf("alarm format err\n");
		return ;
	}
	return ;
}


void callback_event(char **argv,int argc,void *dbnow)
{
	db_now_t* dbnowtemp = (db_now_t *)dbnow;
	char sql[2048];
	int ret = 0;
	char *err = NULL;
	memset(sql, 0, 2048);
	if(argc != 18){
		printf("event lenth wrong\n");
		return ;
	}
	ret = snprintf(sql, 2047, "insert into tips_Event(Event_class,Event_type,Event_time,Sip,Sport,Dip,Dport,Dmac,Sender,Receiver,Title,File_name,Descr,Sen_msg,Event_iid,Device_id,BackID,Event_judgetime,flag) values('%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%lld','%s','%s')",argv[0], argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], argv[7], argv[8], argv[9], argv[10], argv[11], argv[12], argv[13], argv[14], argv[15], dbnowtemp->nowspace, argv[16], argv[17]);
	if((ret < 2047) && (ret > 0)){
		//g_mutex_lock(user_mutex);
		ret = sqlite3_exec(dbnowtemp->db, sql, NULL, NULL, &err);
		if(ret != 0){
			printf("copy db error:%s",err);
			//g_mutex_unlock(user_mutex);
			return ;
		}
		//g_mutex_unlock(user_mutex);
	}
	else{
		printf("alarm format err\n");
		return ;
	}
	return ;
}

void callback_netlog(char **argv,int argc,void *dbnow)
{
	db_now_t* dbnowtemp = (db_now_t *)dbnow;
	char sql[2048];
	int ret = 0;
	char *err = NULL;
	memset(sql, 0, 2048);
	if(argc != 9){
		printf("netlog lenth wrong\n");
//		exit(0);
		return ;
	}
	ret = snprintf(sql, 2047, "insert into tips_netlog(Dip,Dport,Sip,Sport,Protocol,Start_time,End_time,Load,Upload,BackID) values('%s','%s','%s','%s','%s','%s','%s','%s','%s','%lld')",argv[0], argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], argv[7], argv[8],dbnowtemp->nowspace);
	if((ret < 2047) && (ret > 0)){
		//g_mutex_lock(user_mutex);
		ret = sqlite3_exec(dbnowtemp->db, sql, NULL, NULL, &err);
		if(ret != 0){
			printf("copy db error:%s",err);
			//g_mutex_unlock(user_mutex);
			return ;
		}
		//g_mutex_unlock(user_mutex);
	}
	else{
		printf("netlog format err\n");
		return ;
	}
	return ;
}

void callback_opt(char **argv,int argc,void *dbnow)
{
	db_now_t* dbnowtemp = (db_now_t *)dbnow;
	char sql[2048];
	int ret = 0;
	char *err = NULL;
	memset(sql, 0, 2048);
	if(argc != 4){
		printf("opt lenth wrong\n");
		return ;
	}
	ret = snprintf(sql, 2047, "insert into tips_opt(username,Otime,Optdsc,BackID,flag) values('%s','%s','%s','%lld','%s')",argv[0], argv[1], argv[2], dbnowtemp->nowspace, argv[3]);
	if((ret < 2047) && (ret > 0)){
		//g_mutex_lock(user_mutex);
		ret = sqlite3_exec(dbnowtemp->db, sql, NULL, NULL, &err);
		if(ret != 0){
			printf("copy db error:%s",err);
			//g_mutex_unlock(user_mutex);
			return ;
		}
		//g_mutex_unlock(user_mutex);
	}
	else{
		printf("alarm format err\n");
		return ;
	}
	return ;
}



int CUser_sqlite_handle::tips_Copy(char *findsql, tips_callback_mysql tipscopy, char *name,int oid)
{
	int ret = 0;
	char *err = NULL;
	char bdbsql[1024];
	time_t clock = time(NULL);
	char tips_clock[20];
	memset(tips_clock, 0, 20);
	memset(bdbsql, 0, 1024);
	TurnTime(clock, tips_clock, 20);
	snprintf(bdbsql, 1023,"insert into tips_back(BackName,BackTime,Oid) values('%s-%ld','%s','%d')", name, clock, tips_clock, oid);
	////****************************g_mutex_lock(user_bak_mutex);
	g_mutex_lock(user_mutex);
	ret = sqlite3_exec(bdb, bdbsql, NULL, NULL, &err);
	if(ret != 0){
		printf("err in tips_back.db==%s\n",err);
		g_mutex_unlock(user_mutex);
		return 0;
	} sqlite3_exec(bdb, "begin;", 0, 0, 0);
	unsigned long long int nowspace = sqlite3_last_insert_rowid(bdb);
	db_now_t dbnow = {nowspace, bdb};
	printf("findsql===%s\n",findsql);
	////*********************************tips_sqlite_sync.tips_lock(1);
	ret = sqlhandler.Select_Sql_Exec(findsql,tipscopy,&dbnow);
	////*********************************ret = sqlite3_exec(db, findsql, tipscopy, &dbnow, &err);
////********************************	if(ret != 0){
////********************************		printf("err in sqlite==%s\n",err);
////********************************		////*********************tips_sqlite_sync.tips_unlock(1);
////********************************		exit(0);
////********************************	}
	////********************************tips_sqlite_sync.tips_unlock(1);
	sqlite3_exec(bdb, "commit;", 0, 0, 0);
	g_mutex_unlock(user_mutex);
	////*********************************g_mutex_unlock(user_bak_mutex);
	return 0;
}

int CUser_sqlite_handle::Data_delete(unsigned long long int oid, char *tips_table)
{
	int ret = 0;
	char *err = NULL;
	char *(value[4]) = {(char *)"tips_Alarm",(char *)"tips_Event",(char *)"tips_netlog", (char *)"tips_opt"};
	char *sql = g_strdup_printf("delete from %s where BackID='%lld'", value[atoi(tips_table)], oid);
	printf("sql;;;%s\n",sql);
	g_mutex_lock(user_mutex);
	ret = sqlite3_exec(bdb, sql, NULL, NULL, &err);
	if(ret != 0){
		printf("data_delete err:%s\n",err);
		g_free(sql);
		g_mutex_unlock(user_mutex);
		return 0;
	}
	g_free(sql);
	sql = NULL;
	sql = g_strdup_printf("delete from tips_back where BackID='%lld'",oid);
	printf("sql====%s\n",sql);
	ret = sqlite3_exec(bdb, sql, NULL, NULL, &err);
	if(ret != 0){
		printf("data_delete err:%s\n",err);
		g_free(sql);
		g_mutex_unlock(user_mutex);
		return 0;
	}
	g_mutex_unlock(user_mutex);
	g_free(sql);
	return 0;
}

int callback_rebuild(void *sqlhandler, int argc, char **argv, char **argvv)
{
/*保留参数?*/argvv = argvv;
	char sql[2048];
	int ret = 0;
//hyj	char *err = NULL;
	memset(sql, 0, 2048);
	if(argc != 13){
		printf("alarm lenth wrong\n");
		return 0;
	}
	
	ret = snprintf(sql, 2047, "insert into tips_Alarm(Trojan_id,Sip,Dip,Dmac,Alarm_time,Type,Risk,Scc_flag,Scc,Descry,Alarm_iid,Device_id,Alarm_judgetime) values('%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s')",argv[0], argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], argv[7], argv[8], argv[9], argv[10], argv[11],argv[12]);
	
	if((ret < 2047) && (ret > 0)){
		ret = ((Sqlhandler *)sqlhandler)->Insert_Sql_Exec(sql);
		//ret = ((Sqlhandler *)sqlhandler).Insert_Sql_Exec(sql);
		////**********************ret = sqlite3_exec((sqlite3 *)db, sql, NULL, NULL, &err);
		///***********************if(ret != 0){
		///***********************	printf("err in sqlite==%s\n",err);
		///***********************	return 0;
		///***********************}
		return 0;
	}
	return 0;
}

int CUser_sqlite_handle::Data_rebuild(unsigned long long int oid, char *tips_table)
{
/*保留参数?*/tips_table = tips_table;
	int ret = 0;
	char *err = NULL;
	g_mutex_lock(user_mutex);
	////***************************sqlite3_exec(db, "begin;", 0, 0, 0);
	sqlhandler.Select_Sql_Exec((char *)"begin;",0,0);
	char *sql = g_strdup_printf("select tips_Alarm.Trojan_id,tips_Alarm.Sip,tips_Alarm.Dip,tips_Alarm.Dmac,tips_Alarm.Alarm_time,tips_Alarm.Type,tips_Alarm.Risk,tips_Alarm.Scc_flag,tips_Alarm.Scc,tips_Alarm.Descry,tips_Alarm.Alarm_iid,tips_Alarm.Device_id,tips_Alarm.Alarm_judgetime from tips_back,tips_Alarm where tips_back.BackID=tips_Alarm.BackID and tips_back.BackID='%lld'",oid);
	ret = sqlite3_exec(bdb, sql, callback_rebuild, &sqlhandler, &err);
	if(ret != 0){
		printf("data_rebuild_alarm err2:%s\n",err);
		g_mutex_unlock(user_mutex);
		return 0;
	}
	sqlhandler.Select_Sql_Exec((char *)"commit;",0,0);
	////*******************************sqlite3_exec(db, "commit;", 0, 0, 0);
	g_mutex_unlock(user_mutex);
/*返回值？*/return 1;
}

int callback_rebuild_opt(void *sqlhandler, int argc, char **argv, char **argvv)
{
/*保留参数?*/argvv = argvv;
	char sql[2048];
	int ret = 0;
//hyj	char *err = NULL;
	memset(sql, 0, 2048);
	if(argc != 4){
		printf("opt lenth wrong\n");
		return 0;
	}
	
	ret = snprintf(sql, 2047, "insert into tips_opt(username,Otime,flag,Optdsc) values('%s','%s','%s','%s')",argv[0], argv[1], argv[2], argv[3]);
	printf("###sql==%s\n",sql);
	if((ret < 2047) && (ret > 0)){
		ret = ((Sqlhandler *)sqlhandler)->Insert_Sql_Exec(sql);
		//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&ret = ((Sqlhandler *)sqlhandler).Insert_Sql_Exec(sql);
		////************************ret = sqlite3_exec((sqlite3 *)db, sql, NULL, NULL, &err);
		////************************if(ret != 0){
		////************************	printf("err in sqlite==%s\n",err);
		////************************	return 0;
		////************************}
		return 0;
	}
	return 0;
}



int CUser_sqlite_handle::Data_rebuild_opt(unsigned long long int oid, char *tips_table)
{
/*保留参数?*/tips_table = tips_table;
	int ret = 0;
	char *err = NULL;
	g_mutex_lock(user_mutex);
	sqlhandler.Select_Sql_Exec((char *)"begin;",0,0);
	/////////sqlite3_exec(db, "begin;", 0, 0, 0);
	char *sql = g_strdup_printf("select  tips_opt.username,tips_opt.Otime,tips_opt.flag,tips_opt.Optdsc from tips_back,tips_opt where tips_back.BackID=tips_opt.BackID and tips_back.BackID='%lld'",oid);
	ret = sqlite3_exec(bdb, sql, callback_rebuild_opt, &sqlhandler, &err);
	if(ret != 0){
		printf("data_rebuild_opt err2:%s\n",err);
		g_mutex_unlock(user_mutex);
		return 0;
	}
	sqlhandler.Select_Sql_Exec((char *)"commit;",0,0);
	/////***********************sqlite3_exec(db, "commit;", 0, 0, 0);
	g_mutex_unlock(user_mutex);
/*返回值？*/return 1;
}

int callback_rebuild_netlog(void *sqlhandler, int argc, char **argv, char **argvv)
{
/*保留参数?*/argvv = argvv;
	char sql[2048];
	int ret = 0;
//hyj	char *err = NULL;
	memset(sql, 0, 2048);
	if(argc != 9){
		printf("alarm lenth wrong\n");
		return 0;
	}
	
	ret = snprintf(sql, 2047, "insert into tips_netlog(Dip,Dport,Sip,Sport,Protocol,Start_time,End_time,Load,Upload) values('%s','%s','%s','%s','%s','%s','%s','%s','%s')",argv[0], argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], argv[7], argv[8]);
	printf("###sql==%s\n",sql);
	if((ret < 2047) && (ret > 0)){
		ret = ((Sqlhandler *)sqlhandler)->Insert_Sql_Exec(sql);
		////*****************ret = sqlite3_exec((sqlite3 *)db, sql, NULL, NULL, &err);
		////*****************if(ret != 0){
		////*****************	printf("err in sqlite==%s\n",err);
		////*****************	return 0;
		////*****************}
		return 0;
	}
	return 0;
}

int CUser_sqlite_handle::Data_rebuild_netlog(unsigned long long int oid, char *tips_table)
{
/*保留参数?*/tips_table = tips_table;
	int ret = 0;
	char *err = NULL;
	char *sql = g_strdup_printf("select tips_netlog.Dip,tips_netlog.Dport,tips_netlog.Sip,tips_netlog.Sport,tips_netlog.Protocol,tips_netlog.Start_time,tips_netlog.End_time,tips_netlog.Load,tips_netlog.Upload from tips_netlog,tips_back where tips_back.BackID=tips_netlog.BackID and tips_back.BackID='%lld'",oid);
	printf("sql in rebuild on netlog :%s\n",sql);
	g_mutex_lock(user_mutex);
	sqlhandler.Select_Sql_Exec((char *)"begin;",0,0);
	///*******************sqlite3_exec(db, "begin;", 0, 0, 0);
	ret = sqlite3_exec(bdb, sql, callback_rebuild_netlog, &sqlhandler, &err);
	if(ret != 0){
		printf("data_rebuild_alarm err3:%s\n",err);
		g_mutex_unlock(user_mutex);
		return 0;
	}
	sqlhandler.Select_Sql_Exec((char *)"commit;",0,0);
	////********************sqlite3_exec(db, "commit;", 0, 0, 0);
	g_mutex_unlock(user_mutex);
/*返回值？*/return 1;
}

int callback_rebuild_event(void *sqlhandler, int argc, char **argv, char **argvv)
{
/*保留参数?*/argvv = argvv;
	char sql[2048];
	int ret = 0;
//hyj	char *err = NULL;
	memset(sql, 0, 2048);
	if(argc != 16){
		printf("alarm lenth wrong\n");
		return 0;
	}
	
	ret = snprintf(sql, 2047, "insert into tips_Event(Event_class,Event_type,Event_time,Sip,Sport,Dip,Dport,Dmac,Sender,Receiver,Title,File_name,Descr,Sen_msg,Event_iid,Device_id) values('%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s')",argv[0], argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], argv[7], argv[8], argv[9], argv[10], argv[11],argv[12],argv[13],argv[14],argv[15]);
	
	if((ret < 2047) && (ret > 0)){
		ret = ((Sqlhandler *)sqlhandler)->Insert_Sql_Exec(sql);
	//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&	ret = ((Sqlhandler *)sqlhandler).Insert_Sql_Exec(sql);
		////*****************ret = sqlite3_exec((sqlite3 *)db, sql, NULL, NULL, &err);
		///******************if(ret != 0){
		///******************	printf("err in sqlite==%s\n",err);
		///******************	exit(0);
		///******************}
		return 0;
	}
	return 0;
}

int CUser_sqlite_handle::Data_rebuild_event(unsigned long long int oid, char *tips_table)
{
/*保留参数?*/tips_table = tips_table;
	int ret = 0;
	char *err = NULL;
	char *sql = g_strdup_printf("select tips_Event.Event_class,tips_Event.Event_type,tips_Event.Event_time,tips_Event.Sip,tips_Event.Sport,tips_Event.Dip,tips_Event.Dport,tips_Event.Dmac,tips_Event.Sender,tips_Event.Receiver,tips_Event.Title,tips_Event.File_name,tips_Event.Descr,tips_Event.Sen_msg,tips_Event.Event_iid,tips_Event.Device_id from tips_Event,tips_back where tips_back.BackID=tips_Event.BackID and tips_back.BackID='%lld'",oid);
	g_mutex_lock(user_mutex);
	sqlhandler.Select_Sql_Exec((char *)"begin;",0,0);
	///****************sqlite3_exec(db, "begin;", 0, 0, 0);
	ret = sqlite3_exec(bdb, sql, callback_rebuild_event, &sqlhandler, &err);
	if(ret != 0){
		printf("data_rebuild_alarm err1:%s\n",err);
		g_mutex_unlock(user_mutex);
		return 0;
	}
	sqlhandler.Select_Sql_Exec((char *)"commit;",0,0);
	////********************sqlite3_exec(db, "commit;", 0, 0, 0);
	g_mutex_unlock(user_mutex);
	return 0;
}

int CUser_sqlite_handle::Data_Delete_obj(char *sql)
{
	int ret = 0;
//hyj	char *err = NULL;
	printf("sql=====%s\n",sql);
	ret = sqlhandler.Select_Sql_Exec(sql,NULL,NULL);
	////****************ret = sqlite3_exec(db, sql, NULL, NULL, &err);
	///*****************if(ret != 0){
	///*****************	printf("data_delete_obj err****:%s\n",err);
	///*****************	return 0;
	///*****************}
	return 0;
}

int onedata_callback(void *backname, int argc, char **argv, char **argvv)
{
/*保留参数?*/argc = argc; argvv = argvv;
	*((char **)backname) = g_strdup(argv[0]);
	return 0;
}

char * CUser_sqlite_handle::One_Data_catch(unsigned long long int oid)
{

  char *backname = NULL;
	char *err = NULL;
	char* sql = g_strdup_printf("select BackName from tips_back where BackID='%lld'",oid);
	g_mutex_lock(user_mutex);
	int ret = sqlite3_exec(bdb, sql, onedata_callback, &backname, &err);
	if(ret != 0){
		printf("data_delete_obj err###:%s\n",err);
		g_mutex_unlock(user_mutex);
		g_free(sql);
		return backname;
	}
	g_mutex_unlock(user_mutex);
	g_free(sql);
	return backname;
}

int page_get_user(void *last_page,int argc,char **argv,char **argvv)
{
/*保留参数?*/argc = argc; argvv =argvv;
	*((int *)last_page) = atoll(argv[0]);
	return 0;
}

void page_get_picture(char** result,int column,void* param)
{
/*保留参数？*/column = column;
	if(atoll(result[0]) != 0)
		(*((int *)param))++;
	//*((int *)param) = atoll(result[0]);
	return ;
}

void page_get(char** result,int column,void* param)
{
/*保留参数？*/column = column;
	//if(atoll(result[0]) != 0)
	//	(*((int *)param))++;
	*((int *)param) = atoll(result[0]);
	return ;
}

int CUser_sqlite_handle::last_page(char *sql, int judge,uint32_t *total_number)//数据管理里边judge为1，则在tips_user.db里边查找备份数据
{
	
	//sqlite3 *nowdb=db;
	
	sqlite3 *nowdb=NULL;
	int ret = 0;
	char *err = NULL;
	int lastpage = 0;
	int every_pagenum =0;
	FILE *fp;
	char id_buf[10];
	memset(id_buf,0,sizeof(id_buf));
	fp = fopen((char *)"/usr/local/tipschecker/etc/tipscheck/every_pagenum","r+");
	fread(id_buf,sizeof(id_buf),1,fp);
	if(strlen(id_buf)==0)
		/*hyj int*/ every_pagenum = 50;
	else 
		every_pagenum = atoi(id_buf);
	fclose(fp);
	if(every_pagenum==0)
		every_pagenum = 50;
	printf("every_pagenum==%d\n",every_pagenum);
	if(judge)
	{
		nowdb = bdb;
		g_mutex_lock(user_mutex);
		ret = sqlite3_exec(nowdb, sql, page_get_user, &lastpage, &err);
		if(ret != 0)
		{
			printf("count err:%s\n",err);
			g_mutex_unlock(user_mutex);
			return 0;
			////*******************************exit(0);
		}
		g_mutex_unlock(user_mutex);
		//return (lastpage-1)/ONCE_MAXDATA+1;
		return (lastpage-1)/every_pagenum+1;
	}
	else
	{
		ret=sqlhandler.Select_Sql_Exec(sql,page_get,&lastpage);
		if(total_number != NULL)
			*total_number = lastpage;
		printf(":::jfiejfiejfeizhaozhanfeng%d\n",lastpage);
		printf("wocaowocaowocaowcoawowowowow\n");
		//return (lastpage-1)/ONCE_MAXDATA+1;
		return (lastpage-1)/every_pagenum+1;
	}

	////******************************if(!judge){
	////******************************	tips_sqlite_sync.tips_lock(1);
	////******************************	g_mutex_lock(user_mutex);
	////******************************}
	////******************************else{
	////******************************	g_mutex_lock(user_bak_mutex);
	////******************************}
	////******************************printf("sql == %s\n",sql);
	////******************************ret = sqlite3_exec(nowdb, sql, page_get, &lastpage, &err);
	////******************************if(ret != 0){
	////******************************	if(!judge){
	////******************************		tips_sqlite_sync.tips_unlock(1);
	////******************************	}
	////******************************	printf("count err:%s\n",err);
	////******************************	exit(0);
	////******************************}
	////******************************if(!judge){
	////******************************	tips_sqlite_sync.tips_unlock(1);
	////******************************	g_mutex_unlock(user_mutex);
	////******************************}
	////******************************else{
	////******************************	g_mutex_unlock(user_bak_mutex);
	////******************************}
	////******************************return (lastpage-1)/ONCE_MAXDATA+1;
}

int CUser_sqlite_handle::picture_count(char *sql, int judge)
{
/*保留参数？*/judge = judge;

        //sqlite3 *nowdb=db;
        int ret = 0;
//hyj        char *err = NULL;
        int lastpage = 0;
        ///*******************if(judge){
        ///*******************        nowdb = bdb;
        ///*******************}
        ////************if(!judge){
        ////************        tips_sqlite_sync.tips_lock(1);
        ////************        g_mutex_lock(user_mutex);
        ////************}
        ////*********************else{
        ////*********************        g_mutex_lock(user_bak_mutex);
        ////*********************}
        printf("sql == %s\n",sql);
	ret=sqlhandler.Select_Sql_Exec(sql,page_get_picture,&lastpage);
        ////*********************ret = sqlite3_exec(nowdb, sql, page_get, &lastpage, &err);
        ////********************if(ret != 0){
        ////********************        if(!judge){
        ////********************                tips_sqlite_sync.tips_unlock(1);
        ////********************        }
        ////********************        printf("count err:%s\n",err);
        ////********************        exit(0);
        ////********************}
        ////*******************if(!judge){
        ////*******************        tips_sqlite_sync.tips_unlock(1);
        ////*******************        g_mutex_unlock(user_mutex);
        ////*******************}
        ////*******************else{
        ////*******************        g_mutex_unlock(user_bak_mutex);
        ////*******************}
        return lastpage;
}

