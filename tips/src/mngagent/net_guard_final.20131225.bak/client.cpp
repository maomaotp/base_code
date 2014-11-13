#include "client.h"
#include "../rulesdata.h"
#include "ssl.nsmap"
#include <sys/time.h>

using namespace std;

extern char manageip[];
extern char manageip1[];

char Server[50] = "";      //用来存储服务器的IP地址后面是端口号
char Device_ID[] = "1303030001";
char t_auth_str[] = "2DB84CFC3B1B3907DCB24FB3746064D244A40218625BCC60B908853A140EA89784338A7734FAFD3E";

int rulesnum = 0;//策略结构体个数；
int ruleslen = 0;//缓存策略文件的长度；
char *rulescontent = NULL; //用于缓存策略文件的buff；
	
int GetTime(unsigned long* timebuff,int servertype, time_t *old_time, time_t *new_time)
{
//	return 0;
	unsigned long ret_time;
	int result = 0;
	struct soap soap;
	if(client_init(&soap,0,servertype) == -2)
		return result;

	struct ns__ResultList resultlist;
	struct ns__MsgList state;
	
	resultlist.__size = 0;
 	struct ns__MsgItem a;
    state.__size = 1;
    state.__ptr = (struct ns__MsgItem*)soap_malloc(&soap,(state.__size)*sizeof(a));
    //  memset((state->__ptr),0,sizeof(a));
	 (state.__ptr[0]).type = (char*)"get_time";
     (state.__ptr[0]).msg = (char*)"";
//     printf("=======%s=======\n",(state.__ptr[0]).type);

	if(soap_call_ns__upload_message(&soap,Server, NULL, Device_ID,1,&state,t_auth_str,&resultlist) == SOAP_OK)
	{
		cout<<"send to server OK !(GetTime)"<<endl;
	//	cout<<"resultlist->__size = "<<resultlist.__size<<endl;
	//	cout<<"the geted time is :"<<resultlist.__ptr[0]->TimeStamp<<endl;
cout<<resultlist.__ptr[0]->TimeStamp<<endl;
		ret_time = resultlist.__ptr[0]->TimeStamp;
	}
	else
	{
		cout<<"send to server failed !(GetTime)"<<endl;
		result = -1;
	}

	client_end(&soap);

  struct tm *p;
  char time_buff[30];
  char cmd_buff[100];
  memset(time_buff,0,sizeof(time_buff));
  memset(cmd_buff,0,sizeof(cmd_buff));
  p = localtime((time_t *)&ret_time);
  sprintf(time_buff,"%d-%02d-%02d %02d:%02d:%02d",p->tm_year+1900,p->tm_mon+1,p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);
  cout<<">>>>>>>>>>>>>>>>>>>"<<time_buff<<"<<<<<<<<<<<<<<<<<<"<<endl;
  sprintf(cmd_buff,"date -s \"%s\"",time_buff);
*old_time = time(NULL);
  system(cmd_buff);
*new_time = time(NULL);
	cout<<"time----------->"<<cmd_buff<<endl;
  memset(cmd_buff,0,sizeof(cmd_buff));
  sprintf(cmd_buff,"hwclock -w --directisa");
 	system(cmd_buff);
	cout<<"time2---------->"<<cmd_buff<<endl;
	struct itimerval it;
	it.it_value.tv_sec=0;
	it.it_value.tv_usec=1;
	it.it_interval.tv_sec=0;
	it.it_interval.tv_usec=0;
	setitimer(ITIMER_REAL,&it,NULL);
	return result; 
}

int SendStatus(struct status_struct * status, char*** cmdbuff,int servertype)
{//如果没有管控命令返回0，否则返回命令长度
	int command_len = 0;
	struct soap soap;

	if(client_init(&soap,0,servertype) == -2)
		return 0;

//cout<<"@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@sendstatus"<<endl;
	struct ns__ResultList resultlist;
	struct ns__MsgList state;
	
	resultlist.__size = 0;

	get_device_state(&soap,&state,status);
	if(soap_call_ns__upload_message(&soap,Server, NULL, Device_ID,1,&state,t_auth_str,&resultlist) == SOAP_OK)
	{
	     cout<<"send to server OK !(SendStatus)"<<endl;
	//     cout<<"resultlist->__size = "<<resultlist.__size<<endl;
//	     cout<<resultlist.__ptr[0].TimeStamp<<"  "<<resultlist.__ptr[0].Order<<"  "<<resultlist.__ptr[0].Param<<endl;
	     if(resultlist.__size == 0)
					command_len = 0;
	     else
	     {
					*cmdbuff = (char**)malloc(resultlist.__size*(sizeof(char*)));
					int i = 0;
        	for(i=0;i<resultlist.__size;i++)
         	{
		    		 (*cmdbuff)[i] = (char*)malloc(256);
             sprintf((*cmdbuff)[i],"%s %s",resultlist.__ptr[i]->Order,resultlist.__ptr[i]->Param);
		//							cout<<"@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"<<(*cmdbuff)[i]<<"@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"<<endl;
          }
	     }
	}
	else
	{
		cout<<"send to server failed !(SendStatus)"<<endl;
		client_end(&soap);
		return -1;
	}

	client_end(&soap);
	return resultlist.__size; 
}

int SendData(void **data,int size,int type,int servertype)
{
	int result = 0;
	struct soap soap;
//	client_init(&soap,0,servertype);
	if(client_init(&soap,0,servertype) == -2)
		return 0;

//cout<<"@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@senddata"<<endl;

	struct ns__ResultList resultlist;
	struct ns__MsgList state;

	resultlist.__size = 0;

	get_device_data(&soap,&state,data,size,type);
	if(soap_call_ns__upload_message(&soap,Server,NULL,Device_ID,0,&state,t_auth_str,&resultlist) == SOAP_OK)
	{
		cout<<"send to server OK !(SendData)"<<endl;
	//	cout<<"resultlist->__size = "<<resultlist.__size<<endl;
//		cout<<resultlist.__ptr[0].TimeStamp<<"  "<<resultlist.__ptr[0].Order<<"  "<<resultlist.__ptr[0].Param<<endl;
	}
	else
	{
		cout<<"send to server failed !SendData"<<endl;
		result = -1;
	}

	client_end(&soap);
	return result;
}

int SendData1(struct session_blocked data,int servertype)
{
	int result = 0;
	char temp[1024];
	struct soap soap;
//	client_init(&soap,0,servertype);
	if(client_init(&soap,0,servertype) == -2)
		return 0;

//cout<<"@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@senddata1"<<endl;
	struct ns__ResultList resultlist;
	struct ns__MsgList state;

	resultlist.__size = 0;


	state.__size = 1;
	state.__ptr = (struct ns__MsgItem *)soap_malloc(&soap,(state.__size)*sizeof(struct ns__MsgItem));

	memset((state.__ptr),0,sizeof(struct ns__MsgItem));
	memset(temp,0,sizeof(temp));
	sprintf( temp,"<device_id>%s</device_id><dip>%s</dip><sip>%s</sip><sport>%d</sport><start_time>%s</start_time><expired_time>%s</expired_time><block_desc>%s</block_desc>",data.device_id,data.dip,data.sip,data.sport,data.start_time,data.expired_time,data.block_desc);
	(state.__ptr)->type = (char*)soap_malloc(&soap,strlen("session_blocked")+1);
	(state.__ptr)->msg = (char*)soap_malloc(&soap,strlen(temp)+1);
	strcpy((state.__ptr)->type,(char*)"session_blocked");
	strcpy((state.__ptr)->msg,temp);

	//	cout<<temp<<endl;
	if(soap_call_ns__upload_message(&soap,Server,NULL,Device_ID,0,&state,t_auth_str,&resultlist) == SOAP_OK)
	{
		cout<<"send to server OK !(SendData1)"<<endl;
	//	cout<<"resultlist->__size = "<<resultlist.__size<<endl;
//		cout<<resultlist.__ptr[0].TimeStamp<<"  "<<resultlist.__ptr[0].Order<<"  "<<resultlist.__ptr[0].Param<<endl;
	}
	else
	{
		cout<<"send to server failed !(SendData1)"<<endl;
		result = -1;
	}

	client_end(&soap);
	return result;
}



int SendConfig(struct config_struct* config,char* cmdbuff,int servertype)
{
	int result = 0; 
	int command_len = 0;
	struct soap soap;
	//client_init(&soap,0,servertype);
	if(client_init(&soap,0,servertype) == -2)
		return 0;

	struct ns__ResultList resultlist;
	struct ns__MsgList state;
	
	resultlist.__size = 0;

	get_device_info(&soap,&state,config);
  fprintf (stdout, "Server ============= %s\n", Server);            
  if(soap_call_ns__upload_message(&soap,Server, NULL, Device_ID,0,&state,t_auth_str,&resultlist) == SOAP_OK)
	{
		cout<<"send to server OK !(SendConfig)"<<endl;
	//	cout<<"resultlist->__size = "<<resultlist.__size<<endl;
//		cout<<resultlist.__ptr[0].TimeStamp<<"  "<<resultlist.__ptr[0].Order<<"  "<<resultlist.__ptr[0].Param<<endl;
/*		if(resultlist.__size == 0)
			command_len = 0;
		else
		{
//			sprintf(cmdbuff,"%s %s",resultlist.__ptr[0].Order,resultlist.__ptr[0].Param);
			command_len = strlen(resultlist.__ptr[0]->Order);
		}
*/
	}
	else
	{
		cout<<"send to server failed !(SendConfig)"<<endl;
		result = -1;
	}

	client_end(&soap);
	return result;
}

int LoadRules(char * filename, void * * rule_obj, int type,int servertype)
{//给*rule_obj分配空间，赋值，返回个数	
	int result = 0;
	struct soap soap;
	//client_init(&soap,2,servertype);
	if(client_init(&soap,2,servertype) == -2)
		return 0;
	
	char **files;
	int count = 1;
	files = (char**)soap_malloc(&soap,count* sizeof(char*));
//	char* tempfile;
//	tempfile = (char*)malloc(32*sizeof(char));
//	strcpy(tempfile,"dnslist_2013.43.23.7.rules");
	files[0] = filename;

	rulescontent = NULL;
	ruleslen = 0;

	if(client_getData(&soap,count,files) == 0)
	{
		cout<<"Get file success !(LoadRules)"<<endl;
	}
	else
	{
		cout<<"Get file failed !(LoadRules)"<<endl;
		result = -1;
	}
	
//	cout<<1<<endl;
//	free(files);
//	free(tempfile);
	client_end(&soap);

	*rule_obj = NULL;
	get_rules_info(rule_obj,type);
	if(rulescontent != NULL)
		free(rulescontent);
	cout <<"rulesnum ===================" << rulesnum <<endl;
	return rulesnum;
}

int UpdateSys(char * filename,int servertype)
{//下载系统升级包到/etc/tipscheck
	int result = 0;
	struct soap soap;
//	client_init(&soap,1,servertype);
	if(client_init(&soap,1,servertype) == -2)
		return 0;

	char **files;
	int count = 1;
	files = (char**)soap_malloc(&soap,count* sizeof(char*));
	char tempfile[256] = "";
	memset(tempfile,0,sizeof(tempfile));
//	strcpy(tempfile,"/etc/tipscheck/");
//	strcpy(tempfile,"./");
	strcat(tempfile,filename);
	files[0] = tempfile;
	if(client_getData(&soap,count,files) == 0)
	{
		cout<<"Get file success !(UpdateSys)"<<endl;
	}
	else
	{
		cout<<"Get file failed !(UpdateSys)"<<endl;
		result = -1;
	}
//	cout<<2<<endl;
//	free(files);
//	free(tempfile);
	client_end(&soap);
	return result;
}

int UploadData(char *filename,int servertype)
{
	int result = 0;
	struct soap soap;
	//client_init(&soap,1,servertype);
	if(client_init(&soap,1,servertype) == -2)
		return 0;

	char **files;
	int count = 1;
	files = (char**)soap_malloc(&soap,count* sizeof(char*));
//	char* tempfile;
//	tempfile = (char*)malloc(32*sizeof(char));
//	strcpy(tempfile,"dnslist_2013.43.23.7.rules");
	files[0] = filename;
	
	if(client_putData(&soap,count,files) == 0)
	{
		cout<<"Put file success !(UploadData)"<<endl;
	}
	else
	{
		cout<<"Put file failed !(UploadData)"<<endl;
		result = -1;
	}
//	cout<<3<<endl;
//	free(files);
//	free(tempfile);

	client_end(&soap);
	return result;
}

int client_init(struct soap *soap,int type,int servertype)
{
#if REPORT
	memset(Server,0,sizeof(Server));
	sprintf(Server,"https://%s",manageip);
//	sprintf(Server,"http://%s",manageip);
	if(servertype == 0)
  {
		sprintf(Server,"https://%s",manageip);
//	sprintf(Server,"http://%s",manageip);
  }
	else
 {
		sprintf(Server,"https://%s",manageip1);
//	sprintf(Server,"http://%s",manageip1);
	} 
#else
	memset(Server,0,sizeof(Server));
	//sprintf(Server,"https://%s",manageip);
	sprintf(Server,"http://%s",manageip);
	if(servertype == 0)
  {
		//sprintf(Server,"https://%s",manageip);
	sprintf(Server,"http://%s",manageip);
  }
	else
 {
		//sprintf(Server,"https://%s",manageip1);
	sprintf(Server,"http://%s",manageip1);
	} 
#endif
	if(strlen(Server) < 15)
			return -2;
//	cout<<"###################################"<<Server<<"###############################"<<endl;
//	soap_init(soap);
	soap_init1(soap,SOAP_ENC_MTOM);
//	soap_set_mode(soap,SOAP_C_UTFSTRING);
//	soap->mode |= SOAP_C_UTFSTRING;
	soap_set_mime(soap,NULL,NULL);
#if 1
	char config[2048];
	char* line = NULL;
	char* proxyhost = NULL;
	char* proxyport = NULL;
	char* proxyuser = NULL;
	char* proxypasswd = NULL;

	memset(config,0,sizeof(config));

	//int fd = open("/etc/tipscheck/config",O_RDONLY);
	//2013.11.05 by yong 
	int fd = open(UploadXConfig,O_RDONLY);
	if(fd < 0)return 0;
	else
	{
		read(fd,config,sizeof(config));
		close(fd);	
		char* p = NULL;
		if((p=strstr(config,"SERVE_ADDR")) != NULL)
		{
				char* q = p;
				while((++q)[0] != '\n');
				int n = (q-p);
				line = (char*)soap_malloc(soap,n+1);
				line[n] = '\0';
				memcpy(line,p,n);
				char* x = NULL;
				if((x = strchr(p,':')) == NULL)
				{
						proxyhost = (char*)soap_malloc(soap,n+1);
						proxyhost[n] = '\0';
	   				memcpy(proxyhost,p+11,n);		
				}
				else
				{	
						q = x;
						n = (x-p-11);
						proxyhost = (char*)soap_malloc(soap,n+1);
						proxyhost[n] = '\0';
						memcpy(proxyhost,p+11,n);
						while((++q)[0] != '\n');
						n = (q-x-1);	
						proxyport = (char*)soap_malloc(soap,n+1);
						proxyport[n] = '\0';
						memcpy(proxyport,x+1,n);
				}
					
				p = NULL;
				q = NULL;
				if((p=strstr(config,"SERVE_USER")) != NULL)
				{
					q = p;
					while((++q)[0] != '\n');
					n = (q-p-11);
					proxyuser = (char*)soap_malloc(soap,n+1);
					proxyuser[n] = '\0';
					memcpy(proxyuser,p+11,n);
				}
	
				p = NULL;
				q = NULL;
				if((p=strstr(config,"SERVE_PASSWORD")) != NULL)
				{
					q = p;
					while((++q)[0] != '\n');
					n = (q-p-15);
					proxypasswd = (char*)soap_malloc(soap,n+1);
					proxypasswd[n] = '\0';
					strncpy(proxypasswd,p+15,n);
				}
	
		//	soap->proxy_http_version="1.0";
				soap->proxy_host=proxyhost;
				soap->proxy_port=atoi(proxyport);
				soap->proxy_userid=proxyuser;
				soap->proxy_passwd=proxypasswd;
printf("host:%s\nport:%d\nuserid:%s\npasswd:%s\n",soap->proxy_host,soap->proxy_port,soap->proxy_userid,soap->proxy_passwd);
		}
	}	
#endif
	if(strstr(Server,"https") != NULL)
	{
		soap_ssl_init();
		if(soap_ssl_client_context(soap,SOAP_SSL_NO_AUTHENTICATION,NULL,NULL,NULL,NULL,NULL))
		{
			soap_print_fault(soap,stderr);
			exit(1);
		}
	}
	soap->connect_timeout = 60;
	soap->send_timeout = soap->recv_timeout = 30;
	if(type == 1)
	{
		soap->fmimereadopen = mime_read_open;
		soap->fmimereadclose = mime_read_close;
		soap->fmimeread = mime_read;
		soap->fmimewriteopen = mime_client_write_open;
		soap->fmimewriteclose = mime_client_write_close;
		soap->fmimewrite = mime_client_write;
	}
	else if(type == 2)
	{
		soap->fmimereadopen = mime_read_open;
		soap->fmimereadclose = mime_read_close;
		soap->fmimeread = mime_read;
		soap->fmimewriteopen = mime_client_write_open_rules;
		soap->fmimewriteclose = mime_client_write_close;
		soap->fmimewrite = mime_client_write_rules;
	}
	else
		;
	return 0;
}

int client_end(struct soap* soap)
{
	soap_destroy(soap);
	soap_end(soap);
	soap_done(soap);

	return 0;
}


void *mime_read_open(struct soap *soap, void *handle, const char *id, const char *type, const char *description)
{ 
//	cout<<"Putdata read opening"<<endl;
	FILE *fd = (FILE*)handle;
//	fprintf(stderr, "Opening streaming outbound MIME channel for id=%s type=%s\n", id, type);
	return (void*)fd;
}

size_t mime_read(struct soap *soap, void *handle, char *buf, size_t len)
{ 
//cout<<"===Putdata reading==="<<endl;
	return fread(buf, 1, len, (FILE*)handle);
}

void mime_read_close(struct soap *soap, void *handle)
{ 
//	cout<<"===Putdata closing==="<<endl;
	//fprintf(stderr, "Closing streaming outbound MIME channel\n");
	fclose((FILE*)handle);
}

void *mime_client_write_open(struct soap *soap, void *unused_handle, const char *id, const char *type, const char *description, enum soap_mime_encoding encoding)
{ 
//cout<<"===in write open==="<<endl;
	/* Note: the 'unused_handle' is always NULL */
	FILE *fd;
	const char *file;
	int len = 0;
	//char *tempdir = (char*)"/etc/tipscheck";
	//2013.11.05
	char *tempdir = UploadXTemp;
	
	file = *(char**)soap->user;
	soap->user = (void*)(((char**)soap->user)+1);

	len = strlen(tempdir)+strlen(file)+2;
	char *tempfile = (char*)soap_malloc(soap,len*sizeof(char));
	if(tempfile == NULL)
		soap->error = soap_receiver_fault(soap,"Can't save to file !(tempfile = NULL)",file);
	else
	{
		memset(tempfile,0,len);
		strcpy(tempfile,tempdir);
		strcat(tempfile,"/");
		strcat(tempfile,file);
		fd = fopen(tempfile,"wb");
//		cout<<"_______________"<<fd<<endl;
		if(!fd)
			soap->error = soap_receiver_fault(soap,"Can't save data to tempfile !(fopen tempfile)",tempfile);
//		cout<<4<<endl;
		//free(tempfile);
	}
	
	return (void*)fd;
}

void *mime_client_write_open_rules(struct soap *soap, void *unused_handle, const char *id, const char *type, const char *description, enum soap_mime_encoding encoding)
{ 
//	cout<<"===in write open==="<<endl;
	FILE *fd;
	const char *file;
	char *tempfile = (char*)"tempfile";
	file = *(char**)soap->user;
	soap->user = (void*)(((char**)soap->user)+1);

	fd = fopen(tempfile,"wb");
	if(!fd)
		soap->error = soap_receiver_fault(soap,"Can't open rules to read !",tempfile);
	
	return (void*)fd;
}


void mime_client_write_close(struct soap *soap, void *handle)
{ 
//cout<<"==in mime_client_write_close=="<<endl;
	fclose((FILE*)handle);
}

int mime_client_write(struct soap *soap, void *handle, const char *buf, size_t len)
{ 
//cout<<"===writing==="<<endl;
	FILE *fd = (FILE*)handle;
	while (len)                                       //写文件，当一次没写完时循环，到写完为止
	{ 
		size_t nwritten = fwrite(buf, 1, len, fd);
		if (!nwritten)
		{
			soap->errnum = errno;
			return SOAP_EOF;
		}
		len -= nwritten;
		buf += nwritten;
	}
	return SOAP_OK;
}


int mime_client_write_rules(struct soap *soap, void *handle, const char *buf, size_t len)
{ 
	if(len > 0)
	{
		ruleslen += len;
		rulescontent = (char*)realloc(rulescontent,ruleslen);
		memcpy(rulescontent+ruleslen-len,buf,len);
	}
//cout<<">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"<<endl;
//cout<<buf<<endl;
//cout<<">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"<<endl;

	return SOAP_OK;
}

int open_data(struct soap *soap,const char *file,struct x__Data *data)
{
	struct stat sb;
	FILE *fd = NULL;
	int size;
	char *temp_type = (char*)"*/*";
	soap_default_x__Data(soap,data);
	fd = fopen(file,"rb");
//cout<<"===in putdata opening==="<<endl<<file<<endl;
	if (!fd)
	{
		fprintf(stderr,"Cannot open file %s\n",file);
		return soap->error = SOAP_EOF;
	}
	data->xop__Include.__ptr = (unsigned char *)fd;
	if (!fstat(fileno(fd),&sb) && sb.st_size>0)
	{
		size = sb.st_size;
	} 
	else
	{
		soap_set_omode(soap,SOAP_IO_CHUNK);
		size = 0;
	}
	data->xop__Include.__size = size;
	data->xmime5__contentType = temp_type;
	data->xop__Include.id = NULL;
	data->xop__Include.type = data->xmime5__contentType;
	data->xop__Include.options = NULL;
	return SOAP_OK;
}

int client_putData(struct soap * soap,int count,char **files)
{	
	int i = 0;
	struct x__DataSet data;
	struct x__Keys keys;
	char *ret = NULL;
	char *name = NULL;
	char *iflog = NULL;
	char *newlogname = NULL;
	int response;
		
	keys.__size = count;
	keys.key = (char**)soap_malloc(soap,count*sizeof(char*));
	data.__size = count;
	data.item = (struct x__Data*)soap_malloc(soap,sizeof(struct x__Data));

	for(i=0;i<count;i++)
	{
		ret = strrchr(files[i],'/');
		if(ret != NULL)
			name = ++ret;
		else
			name = files[i];
		keys.key[i] = name;
		open_data(soap,files[i],data.item);
	}

//cout<<keys.__size<<"||"<<keys.key[0]<<endl;
/*
cout<<data.__size<<endl;
cout<<data.item->xmime5__contentType<<endl;
cout<<"===xop_Include==="<<data.item->xop__Include.__size<<"||"<<data.item->xop__Include.type<<endl;
*/
	soap->user = (void *)files;
	if(soap_call_ns__PutData(soap,Server,NULL,&data,&keys,Device_ID,t_auth_str,&response))
	{
		soap_print_fault(soap,stderr);
		cout << "Put data error !" << endl;
	}
	else
	{
		cout << "Put data ok !" << endl;
	}
//cout<<"putdata response :"<<response<<endl;
	return soap_errno;
}

int client_getData(struct soap *soap,int count,char **files)
{
	int i;
	int ret = 0;
	struct x__Keys keys;
	struct ns__GetDataResponse response;

	keys.__size = count;
	keys.key = (char**)soap_malloc(soap,count*sizeof(char*));
	for(i = 0;i<count;i++)
		keys.key[i] = files[i];

	soap->user = (void *)keys.key;

	if(soap_call_ns__GetData(soap,Server,NULL,&keys,Device_ID,t_auth_str,&response))
	{
		soap_print_fault(soap,stderr);
	}
	else
	{
	//	cout<<"Get data ok !"<<endl;
	}
//	cout<<5<<endl;
	//free(keys.key);

	return soap->error;
}

int get_device_state(struct soap *soap,struct ns__MsgList *state,struct status_struct *status)
{
	char temp[1024];
	state->__size = 1;
	state->__ptr = (struct ns__MsgItem*)soap_malloc(soap,(state->__size)*sizeof(struct ns__MsgItem));
	memset(&(state->__ptr[0]),0,sizeof(struct ns__MsgItem));
	memset(temp,0,sizeof(temp));
	sprintf( temp,"<device_id>%s</device_id><stime>%s</stime><mem>%d</mem><cpu>%d</cpu><disk>%0.2lf</disk><mirror>%c</mirror><flow>%d</flow><active_peer>%d</active_peer><module_state>%s</module_state>",status->device_id,status->stime,status->mem,status->cpu,status->disk,'1',status->flow,status->active_peer,status->module_state);
  cout <<"temp  =============================== " << temp<<endl;
  //exit(0);
	(state->__ptr[0]).type = (char*)"device_state";
	(state->__ptr[0]).msg = (char*)soap_malloc(soap,strlen(temp)+1); 
	strcpy((state->__ptr[0]).msg,temp);
//	(state->__ptr[0]).msg = temp;
//	cout<<"=======device state======"<<endl<<(state->__ptr[0]).msg<<endl;
//	memcpy((state->__ptr[0]).type,"device_state",sizeof("device_state"));
//	memcpy((state->__ptr[0]).msg,temp,strlen(temp));
	return 1;
}

int get_device_info(struct soap *soap,struct ns__MsgList *state,struct config_struct* config)
{
	char temp[1024];
	state->__size = 1;
	state->__ptr = (struct ns__MsgItem*)soap_malloc(soap,(state->__size)*sizeof(struct ns__MsgItem));
	memset(&(state->__ptr[0]),0,sizeof(struct ns__MsgItem));
	memset(temp,0,sizeof(temp));
//sprintf( temp,"<device_id>1303030001</device_id><device_locate>520102</device_locate><device_made>03</device_made><device_type>1</device_type><commu_addr></commu_addr><commu_mask>0.0.0.0</commu_mask><commu_mac>00:02:B6:85:00:67</commu_mac><commu_gateway>172.16.0.4</commu_gateway><manage_addr>172.16.0.202</manage_addr><manage_mask>255.255.255.0</manage_mask><manage_mac>00:02:B6:85:00:66</manage_mac><manage_gateway>172.16.0.1</manage_gateway><center_ip1>222.85.131.110</center_ip1><center_ip2></center_ip2><soft_version>1</soft_version><dns_version>0</dns_version><ip_version>0</ip_version><swip_version>0</swip_version><geoip_version>0</geoip_version><sen_version>0</sen_version><mail_version>0</mail_version><spe_version>0</spe_version><own_version>0</own_version>");
 sprintf( temp,"<device_id>%s</device_id><device_locate>%s</device_locate><device_made>%s</device_made><device_type>%s</device_type><commu_addr>%s</commu_addr><commu_mask>%s</commu_mask><commu_mac>%s</commu_mac><commu_gateway>%s</commu_gateway><manage_addr>%s</manage_addr><manage_mask>%s</manage_mask><manage_mac>%s</manage_mac><manage_gateway>%s</manage_gateway><center_ip1>%s</center_ip1><center_ip2>%s</center_ip2><soft_version>%s</soft_version><dns_version>%s</dns_version><ip_version>%s</ip_version><swip_version>%s</swip_version><geoip_version>%s</geoip_version><sen_version>%s</sen_version><mail_version>%s</mail_version><spe_version>%s</spe_version><own_version>%s</own_version>",config->device_id,config->device_locate,config->device_made,config->device_type,config->ip,config->mask,config->mac,config->gateway,config->manageip,config->managemask,config->managemac,config->managegateway,config->center_ip,config->center_ip1,config->version,config->dns_version,config->ip_version,config->swip_version,config->geoip_version,config->sen_version,config->mail_version,config->spe_version,config->own);
//	sprintf( temp,"<device_id>%s</device_id><device_locate>%s</device_locate><device_made>%s</device_made><device_type>%s</device_type><commu_addr>%s</commu_addr><commu_mask>%s</commu_mask><commu_mac>%s</commu_mac><commu_gateway>%s</commu_gateway><manage_addr>%s</manage_addr><manage_mask>%s</manage_mask><manage_mac>%s</manage_mac><manage_gateway>%s</manage_gateway><center_ip>%s</center_ip><soft_version>%s</soft_version><dns_version>%s</dns_version><ip_version>%s</ip_version><swip_version>%s</swip_version><geoip_version>%s</geoip_version><sen_version>%s</sen_version><spe_version>%s</spe_version><own_version>%s</own_version>",config->device_id,config->device_locate,config->device_made,config->device_type,config->ip,config->mask,config->mac,config->gateway,config->manageip,config->managemask,config->managemac,config->managegateway,config->center_ip,config->version,config->dns_version,config->ip_version,config->swip_version,config->geoip_version,config->sen_version,config->spe_version,config->own);
	
	(state->__ptr[0]).type = (char*)"device_info";
	(state->__ptr[0]).msg = (char*)soap_malloc(soap,strlen(temp)+1); 
	strcpy((state->__ptr[0]).msg,temp);
//	(state->__ptr[0]).msg = temp;
	cout<<"=======device info======"<<endl<<(state->__ptr[0]).msg<<endl;
//	memcpy((state->__ptr[0]).type,"device_state",sizeof("device_state"));
//	memcpy((state->__ptr[0]).msg,temp,strlen(temp));
	return 1;
}

int get_device_data(struct soap *soap,struct ns__MsgList *state,void **thedata,int size,int type)
{
//cout<<"$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$"<<type<<endl;
	int i = 0;
	char temp[1024];
	state->__size = size;
	state->__ptr = (struct ns__MsgItem *)soap_malloc(soap,(state->__size)*sizeof(struct ns__MsgItem));
	switch(type)
	{
		case 0://alarm
			{
#if 0
struct alarm{
char alarm_id[12];
char device_id[12];
char trojan_id[6];
char sip[19];
char dip[19];
char dmac[20];
char alarm_time[25];
char type[3];
char risk;
char scc[65];
char scc_flag;
char descry[65];
}; 
#endif
	//if(1 != ((struct dns_rules *)rule)[i].type)
	
			printf("(1)==%s\n",((struct alarm *)thedata)[0].alarm_id);
			printf("(2)==%s\n",((struct alarm *)thedata)[1].alarm_id);

				struct alarm** data;
				data = (struct alarm**)thedata;
				for(i=0;i<size;i++)
				{
		//			memset((state->__ptr+i),0,sizeof(struct ns__MsgItem));

					memset(temp,0,sizeof(temp));
					if((*data + i)->alarm_id == NULL)
					{
						strcpy((*data + i)->alarm_id,"");
						//memcpy((*data + i)->alarm_id,"\0",1);
					}
					if((*data + i)->device_id == NULL)
					{
						strcpy((*data + i)->device_id,"");
						//memcpy((*data + i)->device_id,"\0",1);
					}
					if((*data + i)->trojan_id == NULL)
					{
						strcpy((*data + i)->trojan_id,"");
						//memcpy((*data + i)->trojan_id,"\0",1);
					}
					if((*data + i)->sip == NULL)
					{
						strcpy((*data + i)->sip,"");
						//memcpy((*data + i)->sip,"\0",1);
					}
					if((*data + i)->dip == NULL)
					{
						strcpy((*data + i)->dip,"");
						//memcpy((*data + i)->dip,"\0",1);
					}
					if((*data + i)->dmac == NULL)
					{
						strcpy((*data + i)->dmac,"");
						//memcpy((*data + i)->dmac,"\0",1);
					}
					if((*data + i)->alarm_time == NULL)
					{
						strcpy((*data + i)->alarm_time,"");
						//memcpy((*data + i)->alarm_time,"\0",1);
					}
					if((*data + i)->type == NULL)
					{
						strcpy((*data + i)->type,"");
						//memcpy((*data + i)->type,"\0",1);
					}
					if((*data + i)->scc == NULL)
					{
						strcpy((*data + i)->scc,"");
						//memcpy((*data + i)->scc,"\0",1);
					}
					if((*data + i)->descry == NULL)
					{
						strcpy((*data + i)->descry,"");
						//memcpy((*data + i)->descry,"\0",1);
					}
					
					sprintf( temp,"<alarm_id>%s</alarm_id><device_id>%s</device_id><trojan_id>%s</trojan_id><sip>%s</sip><dip>%s</dip><dmac>%s</dmac><alarm_time>%s</alarm_time><type>%s</type><risk>%c</risk><scc>%s</scc><scc_flag>%c</scc_flag><descr>%s</descr>",(*data + i)->alarm_id,(*data + i)->device_id,(*data + i)->trojan_id,(*data + i)->sip,(*data + i)->dip,(*data + i)->dmac,(*data + i)->alarm_time,(*data + i)->type,(*data + i)->risk,(*data + i)->scc,(*data + i)->scc_flag != '/'?(*data + i)->scc_flag: 'e',(*data + i)->descry);
				//	sprintf( temp,"<alarm_id>%s</alarm_id><device_id>%s</device_id><trojan_id>%s</trojan_id><sip>%s</sip><dip>%s</dip><dmac>%s</dmac><alarm_time>%s</alarm_time><type>%s</type><risk>%c</risk><scc>%s</scc><scc_flag>%c</scc_flag><descr>%s</descr>",(*data + i)->alarm_id,(*data + i)->device_id,(*data + i)->trojan_id,(*data + i)->sip,(*data + i)->dip,(*data + i)->dmac,(*data + i)->alarm_time,(*data + i)->type,(*data + i)->risk,(*data + i)->scc,'0',(*data + i)->descry);
//cout<<"ALARM<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"<<temp<<endl;
				//	sleep(1);
					(state->__ptr+i)->type = (char*)soap_malloc(soap,strlen("alarm")+1);
					(state->__ptr+i)->msg = (char*)soap_malloc(soap,strlen(temp)+1);
					strcpy((state->__ptr+i)->type,(char*)"alarm");
					strcpy((state->__ptr+i)->msg,temp);
				}
				break;
			}
		case 1://alert
		{
			struct alert** data;
			data = (struct alert**)thedata;
			for(i=0;i<size;i++)
			{
				memset((state->__ptr+i),0,sizeof(struct ns__MsgItem));
				memset(temp,0,sizeof(temp));
//cout<<"^^^^^^^^^^^^^^^^^^^^^^alert^^^^^^^^^^^^^^^^^^^^^^^^^^"<<endl;
				if((*data + i)->alert_id == NULL)
				{
					strcpy((*data + i)->alert_id,"");
				}
				if((*data + i)->device_id == NULL)
				{
					strcpy((*data + i)->device_id,"");
				}
				if((*data + i)->trojan_id == NULL)
				{
					strcpy((*data + i)->trojan_id,"");
				}
				if((*data + i)->alarm_time == NULL)
				{
					strcpy((*data + i)->alarm_time,"");
				}
				if((*data + i)->sip == NULL)
				{
					strcpy((*data + i)->sip,"");
				}
				if((*data + i)->dip == NULL)
				{
					strcpy((*data + i)->dip,"");
				}
				if((*data + i)->dmac == NULL)
				{
					strcpy((*data + i)->dmac,"");
				}
				if((*data + i)->descry == NULL)
				{
					strcpy((*data + i)->descry,"");
				}
				if((*data + i)->dnshash == NULL)
				{
					strcpy((*data + i)->dnshash,"");
				}
				if((*data + i)->scc == NULL)
				{
					strcpy((*data + i)->scc,"");
				}
				if((*data + i)->extra_text == NULL)
				{
					strcpy((*data + i)->extra_text,"");
				}
				
				sprintf( temp,"<alert_id>%s</alert_id><device_id>%s</device_id><plugin_id>%d</plugin_id><risk>%c</risk><trojan_id>%s</trojan_id><alarm_time>%s</alarm_time><sip>%s</sip><sport>%d</sport><dip>%s</dip><dport>%d</dport><dmac>%s</dmac><protocol>%d</protocol><descr>%s</descr><dnshash>%s</dnshash><scc>%s</scc><scc_flag>%c</scc_flag><extra_text>%s</extra_text>",(*data + i)->alert_id,(*data + i)->device_id,(*data + i)->plugin_id,(*data + i)->risk,(*data + i)->trojan_id,(*data + i)->alarm_time,(*data + i)->sip,(*data + i)->sport,(*data + i)->dip,(*data + i)->dport,(*data + i)->dmac,(*data + i)->protocol,(*data + i)->descry,(*data + i)->dnshash,(*data + i)->scc,(*data + i)->scc_flag,(*data + i)->extra_text);
//cout<<"alert temp ==================="<< temp <<endl;
				(state->__ptr+i)->type = (char*)soap_malloc(soap,strlen("alert")+1);
				(state->__ptr+i)->msg = (char*)soap_malloc(soap,strlen(temp)+1);
				strcpy((state->__ptr+i)->type,(char*)"alert");
				strcpy((state->__ptr+i)->msg,temp);
			}
			break;
		}
		case 2://senstive_alert
		{
			struct senstive_alert **data;
			data = (struct senstive_alert**)thedata;
			for(i=0;i<size;i++)
			{
				memset((state->__ptr+i),0,sizeof(struct ns__MsgItem));
				memset(temp,0,sizeof(temp));
				if((*data + i)->event_id == NULL)
				{
					strcpy((*data + i)->event_id,"");
				}
				if((*data + i)->device_id == NULL)
				{
					strcpy((*data + i)->device_id,"");
				}
				if((*data + i)->event_type == NULL)
				{
					strcpy((*data + i)->event_type,"");
				}
				if((*data + i)->event_time == NULL)
				{
					strcpy((*data + i)->event_time,"");
				}
				if((*data + i)->sip == NULL)
				{
					strcpy((*data + i)->sip,"");
				}
				if((*data + i)->dmac == NULL)
				{
					strcpy((*data + i)->dmac,"");
				}
				if((*data + i)->dip == NULL)
				{
					strcpy((*data + i)->dip,"");
				}
				if((*data + i)->sender == NULL)
				{
					strcpy((*data + i)->receiver,"");
				}
				if((*data + i)->title == NULL)
				{
					strcpy((*data + i)->title,"");
				}
				if((*data + i)->filename == NULL)
				{
					strcpy((*data + i)->filename,"");
				}
				if((*data + i)->descr == NULL)
				{
					strcpy((*data + i)->descr,"");
				}
				if((*data + i)->sen_msg == NULL)
				{
					strcpy((*data + i)->sen_msg,"");
				}	
				
				sprintf( temp,"<event_id>%s</event_id><device_id>%s</device_id><event_class>%d</event_class><event_type>%s</event_type><event_time>%s</event_time><sip>%s</sip><sport>%d</sport><dmac>%s</dmac><dip>%s</dip><dport>%d</dport><sender>%s</sender><receiver>%s</receiver><title>%s</title><file_name>%s</file_name><descr>%s</descr><sen_msg>%s</sen_msg>",(*data + i)->event_id,(*data + i)->device_id,(*data + i)->event_class,(*data + i)->event_type,(*data + i)->event_time,(*data + i)->sip,(*data + i)->sport,(*data + i)->dmac,(*data + i)->dip,(*data + i)->dport,(*data + i)->sender,(*data + i)->receiver,(*data + i)->title,(*data + i)->filename,(*data + i)->descr,(*data + i)->sen_msg);
				(state->__ptr+i)->type = (char*)soap_malloc(soap,strlen("senstive_alert")+1);
				(state->__ptr+i)->msg = (char*)soap_malloc(soap,strlen(temp)+1);
				strcpy((state->__ptr+i)->type,(char*)"senstive_alert");
				strcpy((state->__ptr+i)->msg,temp);
			}
			break;
		}
		case 3://session_blocked
		{		
			struct session_blocked **data;
			data = (struct session_blocked**)thedata;
			for(i=0;i<size;i++)
			{
				memset((state->__ptr+i),0,sizeof(struct ns__MsgItem));
				memset(temp,0,sizeof(temp));
				if((*data + i)->device_id == NULL)
				{
					strcpy((*data + i)->dip,"");
				}
				if((*data + i)->sip == NULL)
				{
					strcpy((*data + i)->sip,"");
				}
			 	if((*data + i)->start_time == NULL)
				{
					strcpy((*data + i)->start_time,"");
				}
				if((*data + i)->expired_time == NULL)
				{
					strcpy((*data + i)->expired_time,"");
				}
				if((*data + i)->block_desc == NULL)
				{
					strcpy((*data + i)->block_desc,"");
				}
				
				sprintf( temp,"<device_id>%s</device_id><dip>%s</dip><sip>%s</sip><sport>%d</sport><start_time>%s</start_time><expired_time>%s</expired_time><block_desc>%s</block_desc>",(*data + i)->device_id,(*data + i)->dip,(*data + i)->sip,(*data + i)->sport,(*data + i)->start_time,(*data + i)->expired_time,(*data + i)->block_desc);
//				memcpy((state->__ptr[0]).type,"session_blocked",sizeof("session_blocked"));
//				memcpy((state->__ptr[0]).msg,temp,strlen(temp));
				(state->__ptr+i)->type = (char*)soap_malloc(soap,strlen("session_blocked")+1);
				(state->__ptr+i)->msg = (char*)soap_malloc(soap,strlen(temp)+1);
				strcpy((state->__ptr+i)->type,(char*)"session_blocked");
				strcpy((state->__ptr+i)->msg,temp);
			}
			break;
		}
		default:
			break;
	}
	return 0;
}

int get_rules_info(void** rule_obj, int type)
{
	int num = 0;
	char *tempp = NULL;
	char *position = NULL;
	char tempbuff[ruleslen+1];
	memset(tempbuff,0,sizeof(ruleslen));
	memcpy(tempbuff,rulescontent,ruleslen);
	position = tempbuff;
	rulesnum = 0;
	switch (type)
	{
		case 1:
			{
				cout<<"dns file:"<<endl<<rulescontent<<endl;
				struct dns_rules** tempprt = (struct dns_rules**)rule_obj;
				while((tempp = strchr(position,'\n')) != NULL)
				{
					char *front = NULL;
					char *back = NULL;
					*rule_obj= realloc(*rule_obj,(num+1)*sizeof(struct dns_rules));
					front = strchr(position,'#');
					if(rules_NULL_error(front))
						break;
					(*tempprt+num)->type = position[0];
					back = strchr(front+1,'#');
					if(rules_NULL_error(back))
						break;
					memset((*tempprt+num)->dns_name,0,sizeof((*tempprt+num)->dns_name));
					strncpy((*tempprt+num)->dns_name,front+1,back-front-1);
					front = strchr(back+1,'#');
					if(rules_NULL_error(front))
						break;
					(*tempprt+num)->match_mode = (back+1)[0];
					front = strchr(front+1,'#');
					if(rules_NULL_error(front))
						break;
					(*tempprt+num)->dangerous = (front+1)[0];
					position = tempp + 1;

//cout<<"~~~~~~~~~~~~"<<(*tempprt+num)->dns_name<<endl;
					num ++;
				}
				break;
			}
		case 2:
			{
				cout<<"iplist file:"<<endl<<rulescontent<<endl;
				struct ip_rules** tempptr = (struct ip_rules**)rule_obj;
				while((tempp = strchr(position,'\n')) != NULL)
				{
					char *front = NULL;
					char *back = NULL;
					char temp[20];
					*rule_obj= realloc(*rule_obj,(num+1)*sizeof(struct ip_rules));
					front = strchr(position,'#');
					if(rules_NULL_error(front))
						break;
					(*tempptr+num)->type = position[0];
	//				cout<<"num ========"<<num<<"	"<<(*tempptr+num)->type<<endl;
					back = strchr(front+1,'#');
					if(rules_NULL_error(back))
						break;
					memset(temp,0,sizeof(temp));
					strncpy(temp,front+1,back-front-1);
					(*tempptr+num)->startip = (u_int)atoll(temp);
					front = strchr(back+1,'#');
					if(rules_NULL_error(front))
						break;
					memset(temp,0,sizeof(temp));
					strncpy(temp,back+1,front-back-1);
					(*tempptr+num)->endip = (u_int)atoll(temp);
					back = strchr(front+1,'#');
					if(rules_NULL_error(back))
						break;
					memset(temp,0,sizeof(temp));
					strncpy(temp,front+1,back-front-1);
					(*tempptr+num)->port= (u_short)atoll(temp);
					front = strchr(back+1,'#');
					if(rules_NULL_error(front))
						break;
					back = strchr(front+1,'#');
					if(rules_NULL_error(back))
						break;				
					(*tempptr+num)->dangerous = (front+1)[0];
					position = tempp + 1;
//cout<<"~~~~~~~~~~~~"<<(*tempptr+num)->startip<<endl;
					num ++;
				}
				break;
			}
		case 3:
			{
				cout<<"swiplist file:"<<endl<<rulescontent<<endl;
				struct ipaudit_rules** tempptr = (struct ipaudit_rules**)rule_obj;
				while((tempp = strchr(position,'\n')) != NULL)
				{
					char *front = NULL;
					char *back = NULL;
					char temp[20];
					*rule_obj= realloc(*rule_obj,(num+1)*sizeof(struct ipaudit_rules));
					front = strchr(position,'#');
					if(rules_NULL_error(front))
						break;
					memset(temp,0,sizeof(temp));
					strncpy(temp,position,front-position);
	//				cout<<"@@@@@"<<temp<<endl;
					(*tempptr+num)->startip = (u_int)atoll(temp);
					back = strchr(front+1,'#');
					if(rules_NULL_error(back))
						break;
					memset(temp,0,sizeof(temp));
					strncpy(temp,front+1,back-front-1);
					(*tempptr+num)->endip = (u_int)atoll(temp);
					front = strchr(back+1,'#');
					if(rules_NULL_error(front))
						break;
					memset(temp,0,sizeof(temp));
					strncpy(temp,back+1,front-back-1);
					(*tempptr+num)->port = (u_short)atoll(temp);
					position = tempp + 1;
//cout<<"~~~~~~~~~~~~"<<(*tempptr+num)->startip<<endl;
					num ++;
				}
				break;
			}
		case 6:
			{
//				cout<<"geoip file:"<<endl<<rulescontent<<endl;
			struct ippos_rules** tempptr = (struct ippos_rules**)rule_obj;
				while((tempp = strchr(position,'\n')) != NULL)
				{
	//	cout<<"ippostemp====="<<tempp<<endl;
					char *front = NULL;
					char *back = NULL;
					char temp[20];
					*rule_obj = realloc(*rule_obj,(num+1)*sizeof(struct ippos_rules));
					front = strchr(position,'#');
					if(rules_NULL_error(front))
						break;
					memset(temp,0,sizeof(temp));
					strncpy(temp,position,front-position);
	//				cout<<"temp:"<<temp<<endl;
					(*tempptr+num)->startip = (u_int)atoll(temp);
					back = strchr(front+1,'#');
					if(rules_NULL_error(back))
						break;
					memset(temp,0,sizeof(temp));
					strncpy(temp,front+1,back-front-1);
					(*tempptr+num)->endip = (u_int)atoll(temp);
					front	 = strchr(back+1,'#');
					if(rules_NULL_error(front))
						break;
					memset((*tempptr+num)->region,0,sizeof((*tempptr+num)->region));
					strncpy((*tempptr+num)->region,back+1,front-back-1);
					back = strchr(front+1,'#');
					if(rules_NULL_error(back))
						break;
					memset((*tempptr+num)->tirr,0,sizeof((*tempptr+num)->tirr));
					strncpy((*tempptr+num)->tirr,front+1,back-front-1);
					front = strchr(back+1,'\n');
					if(rules_NULL_error(front))
						break;
					(*tempptr+num)->flag = (back+1)[0];
					position = tempp + 1;
cout<<(*tempptr+num)->startip<<"@"<<(*tempptr+num)->endip<<"@"<<(*tempptr+num)->region<<"@"<<(*tempptr+num)->tirr<<"@"<<(*tempptr+num)->flag<<endl;
//cout<<"lllllllllllllllladaaaaaaaaaaaaaa"<<(*tempptr+num)->flag<<endl;
//cout<<"~~~~~~~~~~~~"<<(*tempptr+num)->startip<<endl;
					num ++;
				}
				break;
			}
		case 4:
			{
				cout<<"senlist file:"<<endl<<rulescontent<<endl;
//		cout<<"!!!!!!!!!!!!!!!!!!!!!!!!!!"<<endl;
				struct sec_rules** tempptr = (struct sec_rules**)rule_obj;				
				while((tempp = strchr(position,'\n')) != NULL)
				{
					char *front = NULL;
					char *back = NULL;
					*rule_obj = realloc(*rule_obj,(num+1)*sizeof(struct sec_rules));
					front = strchr(position,'#');
					if(rules_NULL_error(front))
						break;
					memset((*tempptr+num)->content,0,sizeof(tempptr[num]->content));
					strncpy((*tempptr+num)->content,position,front-position);
//cout<<"~~~~~~~~~~~~"<<(*tempptr+num)->content<<endl;
					position = tempp + 1;
					num ++;
				}
				break;
			}
/*		case 5:
			{
				cout<<"spectro file:"<<endl<<rulescontent<<endl;
				struct spectro_rules** tempptr = (struct spectro_rules**)rule_obj;				

				*rule_obj = realloc(*rule_obj,(num+1)*sizeof(struct spectro_rules));
				memset((*tempptr+num)->content,0,sizeof(tempptr[num]->content));
				strncpy((*tempptr+num)->content,rulescontent,ruleslen);

				num ++;
				break;
			}*/
		case 5:
      {
       cout<<"spectro file:"<<endl<<rulescontent<<endl;
        struct spectro_rules** tempptr = (struct spectro_rules**)rule_obj;

        while((tempp = strchr(position,'\n')) != NULL)
        {
          char *front = NULL;
          char *back = NULL;
          *rule_obj = realloc(*rule_obj,(num+1)*sizeof(struct spectro_rules));
          memset((*tempptr+num)->content,0,sizeof(tempptr[num]->content));
          strncpy((*tempptr+num)->content,position,tempp-position);
          position = tempp + 1;
          num ++;
        }
        break;
      }
		case 7:
			{
       cout<<"mail file:"<<endl<<rulescontent<<endl;
					struct mail_rules** tempprt = (struct mail_rules**)rule_obj;
        	while((tempp = strchr(position,'\n')) != NULL)
					{
						char* front = NULL;
						char* back = NULL;
         	  *rule_obj = realloc(*rule_obj,(num+1)*sizeof(struct mail_rules));

						front = strchr(position,'#');
						if(rules_NULL_error(front))
							break;
						(*tempprt+num)->sign = position[0];
						back = strchr(front+1,'#');
						if(rules_NULL_error(back))
							break;
						memset((*tempprt+num)->content,0,sizeof((*tempprt+num)->content));
						strncpy((*tempprt+num)->content,front+1,back-front-1);
						front = strchr(back+1,'#');
						if(rules_NULL_error(front))
							break;
						(*tempprt+num)->x = (back+1)[0];
						back = strchr(front+1,'#');
						if(rules_NULL_error(back))
							break;
						memset((*tempprt+num)->starttime,0,sizeof((*tempprt+num)->starttime));
						strncpy((*tempprt+num)->starttime,front+1,back-front-1);
						front = strchr(back+1,'\n');
						memset((*tempprt+num)->endtime,0,sizeof((*tempprt+num)->endtime));
						strncpy((*tempprt+num)->endtime,back+1,front-back-1);
						position = tempp + 1;
//cout<<"^^^^^^^^^^^^^^^^^^^^^mail0^^^^^^^^^^^^^^^^^^^^^"<<endl;
//cout<<(*tempprt+num)->sign<<"||"<<(*tempprt+num)->content<<"||"<<(*tempprt+num)->x<<"||"<<(*tempprt+num)->starttime<<"||"<<(*tempprt+num)->endtime<<endl;
//cout<<"^^^^^^^^^^^^^^^^^^^^^mail1^^^^^^^^^^^^^^^^^^^^^"<<endl;
						num ++;
					}
					break;
		 }
		default:
			break;
	}
	rulesnum = num;
	cout <<"num ===========================" <<endl;
	return 0;
}

int rules_NULL_error(char *p)
{
	if(p == NULL)
	{
		cout<<"There is wrong format in the geted rules infor !"<<endl;
		return 1;
	}
	else
		return 0;
}
