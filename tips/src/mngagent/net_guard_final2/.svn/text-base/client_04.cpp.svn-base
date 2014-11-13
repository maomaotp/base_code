#include "client.h"
#include "../rulesdata.h"
#include "ssl.nsmap"

using namespace std;

char Server[50] = "";      //用来存储服务器的IP地址后面是端口号
char Device_ID[] = "1303030001";
char t_auth_str[] = "2DB84CFC3B1B3907DCB24FB3746064D244A40218625BCC60B908853A140EA89784338A7734FAFD3E";


int rulesnum = 0;//策略结构体个数；
int ruleslen = 0;//缓存策略文件的长度；
char *rulescontent = NULL; //用于缓存策略文件的buff；
	
int GetTime(unsigned long* timebuff)
{
	int result = 0;
	struct soap soap;
	client_init(&soap,0);

	struct ns__ResultList resultlist;
	struct ns__MsgList state;
	
	resultlist.__size = 0;

 	struct ns__MsgItem a;
    state.__size = 1;
    state.__ptr = (struct ns__MsgItem*)soap_malloc(&soap,(state.__size)*sizeof(a));
    //  memset((state->__ptr),0,sizeof(a));
	 (state.__ptr[0]).type = (char*)"get_time";
     (state.__ptr[0]).msg = (char*)"";
     printf("=======%s=======\n",(state.__ptr[0]).type);

	if(soap_call_ns__upload_message(&soap,Server, NULL, Device_ID,1,&state,t_auth_str,&resultlist) == SOAP_OK)
	{
		cout<<"send to server OK !"<<endl;
		cout<<"resultlist->__size = "<<resultlist.__size<<endl;
		cout<<"the geted time is :"<<resultlist.__ptr[0]->TimeStamp<<endl;
		*timebuff = resultlist.__ptr[0]->TimeStamp;
	}
	else
	{
		cout<<"send to server failed !\n"<<endl;
		result = -1;
	}

	client_end(&soap);
	return result; 
}

int SendStatus(struct status_struct * status, char*** cmdbuff)
{//如果没有管控命令返回0，否则返回命令长度
	int command_len = 0;
	struct soap soap;
	client_init(&soap,0);

	struct ns__ResultList resultlist;
	struct ns__MsgList state;
	
	resultlist.__size = 0;

	get_device_state(&soap,&state,status);
	if(soap_call_ns__upload_message(&soap,Server, NULL, Device_ID,1,&state,t_auth_str,&resultlist) == SOAP_OK)
	{
	     cout<<"send to server OK !"<<endl;
	     cout<<"resultlist->__size = "<<resultlist.__size<<endl;
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
                }
	     }
	}
	else
	{
		cout<<"send to server failed !\n"<<endl;
		client_end(&soap);
		return -1;
	}

	client_end(&soap);
	return resultlist.__size; 
}

int SendData(void **data,int size,int type)
{
	int result = 0;
	struct soap soap;
	client_init(&soap,0);

	struct ns__ResultList resultlist;
	struct ns__MsgList state;

	resultlist.__size = 0;

	get_device_data(&soap,&state,data,size,type);
	if(soap_call_ns__upload_message(&soap,Server,NULL,Device_ID,0,&state,t_auth_str,&resultlist) == SOAP_OK)
	{
		cout<<"send to server OK !"<<endl;
		cout<<"resultlist->__size = "<<resultlist.__size<<endl;
//		cout<<resultlist.__ptr[0].TimeStamp<<"  "<<resultlist.__ptr[0].Order<<"  "<<resultlist.__ptr[0].Param<<endl;
	}
	else
	{
		cout<<"send to server failed !\n"<<endl;
		result = -1;
	}

	client_end(&soap);
	return result;
}

int SendData1(struct session_blocked data)
{
	int result = 0;
	char temp[1024];
	struct soap soap;
	client_init(&soap,0);

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

		cout<<temp<<endl;
	if(soap_call_ns__upload_message(&soap,Server,NULL,Device_ID,0,&state,t_auth_str,&resultlist) == SOAP_OK)
	{
		cout<<"send to server OK !"<<endl;
		cout<<"resultlist->__size = "<<resultlist.__size<<endl;
//		cout<<resultlist.__ptr[0].TimeStamp<<"  "<<resultlist.__ptr[0].Order<<"  "<<resultlist.__ptr[0].Param<<endl;
	}
	else
	{
		cout<<"send to server failed !\n"<<endl;
		result = -1;
	}

	client_end(&soap);
	return result;
}



int SendConfig(struct config_struct* config,char* cmdbuff)
{
	int result = 0; 
	int command_len = 0;
	struct soap soap;
	client_init(&soap,0);

	struct ns__ResultList resultlist;
	struct ns__MsgList state;
	
	resultlist.__size = 0;

	get_device_info(&soap,&state,config);
	if(soap_call_ns__upload_message(&soap,Server, NULL, Device_ID,0,&state,t_auth_str,&resultlist) == SOAP_OK)
	{
		cout<<"send to server OK !"<<endl;
		cout<<"resultlist->__size = "<<resultlist.__size<<endl;
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
		cout<<"send to server failed !\n"<<endl;
		result = -1;
	}

	client_end(&soap);
	return result;
}

int LoadRules(char * filename, void * * rule_obj, int type)
{//给*rule_obj分配空间，赋值，返回个数	
	int result = 0;
	struct soap soap;
	client_init(&soap,2);
	
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
		cout<<"Get file success !"<<endl;
	}
	else
	{
		cout<<"Get file failed !"<<endl;
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

	return rulesnum;
}

int UpdateSys(char * filename)
{//下载系统升级包到/etc/tipscheck
	int result = 0;
	struct soap soap;
	client_init(&soap,1);

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
		cout<<"Get file success !"<<endl;
	}
	else
	{
		cout<<"Get file failed !"<<endl;
		result = -1;
	}
//	cout<<2<<endl;
//	free(files);
//	free(tempfile);
	client_end(&soap);
	return result;
}

int UploadData(char *filename)
{
	int result = 0;
	struct soap soap;
	client_init(&soap,1);

	char **files;
	int count = 1;
	files = (char**)soap_malloc(&soap,count* sizeof(char*));
//	char* tempfile;
//	tempfile = (char*)malloc(32*sizeof(char));
//	strcpy(tempfile,"dnslist_2013.43.23.7.rules");
	files[0] = filename;
	
	if(client_putData(&soap,count,files) == 0)
	{
		cout<<"Put file success !"<<endl;
	}
	else
	{
		cout<<"Put file failed !"<<endl;
		result = -1;
	}
//	cout<<3<<endl;
//	free(files);
//	free(tempfile);

	client_end(&soap);
	return result;
}

int client_init(struct soap *soap,int type)
{
	memset(Server,0,sizeof(Server));
	sprintf(Server,"http://172.16.0.139:8225");

//	soap_init(soap);
	soap_init1(soap,SOAP_ENC_MTOM);
//	soap_set_mode(soap,SOAP_C_UTFSTRING);
//	soap->mode |= SOAP_C_UTFSTRING;
	soap_set_mime(soap,NULL,NULL);
/*
	soap_ssl_init();
	if(soap_ssl_client_context(soap,SOAP_SSL_NO_AUTHENTICATION,NULL,NULL,NULL,NULL,NULL))
	{
		soap_print_fault(soap,stderr);
		exit(1);
	}
*/
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
cout<<"===Putdata reading==="<<endl;
	return fread(buf, 1, len, (FILE*)handle);
}

void mime_read_close(struct soap *soap, void *handle)
{ 
	cout<<"===Putdata closing==="<<endl;
	//fprintf(stderr, "Closing streaming outbound MIME channel\n");
	fclose((FILE*)handle);
}

void *mime_client_write_open(struct soap *soap, void *unused_handle, const char *id, const char *type, const char *description, enum soap_mime_encoding encoding)
{ 
cout<<"===in write open==="<<endl;
	/* Note: the 'unused_handle' is always NULL */
	FILE *fd;
	const char *file;
	int len = 0;
	char *tempdir = (char*)"/etc/tipscheck";
	
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
	cout<<"===in write open==="<<endl;
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
cout<<"==in mime_client_write_close=="<<endl;
	fclose((FILE*)handle);
}

int mime_client_write(struct soap *soap, void *handle, const char *buf, size_t len)
{ 
cout<<"===writing==="<<endl;
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
cout<<"===in putdata opening==="<<endl<<file<<endl;
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
/*
cout<<keys.__size<<"||"<<keys.key[0]<<endl;
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
cout<<"putdata response :"<<response<<endl;
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
		cout<<"Get data ok !"<<endl;
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
	sprintf( temp,"<device_id>%s</device_id><stime>%s</stime><mem>%d</mem><cpu>%d</cpu><disk>%0.2lf</disk><mirror>%c</mirror><flow>%d</flow><active_peer>%d</active_peer><module_state>%s</module_state>",status->device_id,status->stime,status->mem,status->cpu,status->disk,status->mirror,1024,status->active_peer,"11110000");
	(state->__ptr[0]).type = (char*)"device_state";
	(state->__ptr[0]).msg = (char*)soap_malloc(soap,strlen(temp)+1); 
	strcpy((state->__ptr[0]).msg,temp);
//	(state->__ptr[0]).msg = temp;
	cout<<"=======device state======"<<endl<<(state->__ptr[0]).msg<<endl;
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
	sprintf( temp,"<device_id>%s</device_id><device_locate>%s</device_locate><device_made>%s</device_made><device_type>%s</device_type><commu_addr>%s</commu_addr><commu_mask>%s</commu_mask><commu_mac>%s</commu_mac><commu_gateway>%s</commu_gateway><center_ip>%s</center_ip><soft_version>%s</soft_version><dns_version>%s</dns_version><ip_version>%s</ip_version><swip_version>%s</swip_version><geoip_version>%s</geoip_version><sen_version>%s</sen_version><spe_version>%s</spe_version><own_version>%s</own_version>",config->device_id,config->device_locate,config->device_made,config->device_type,config->ip,config->mask,config->mac,"172.16.0.1",config->center_ip,config->version,config->dns_version,config->ip_version,config->swip_version,config->geoip_version,config->sen_version,config->spe_version,config->own);
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
	int i = 0;
	char temp[1024];
	state->__size = size;
	state->__ptr = (struct ns__MsgItem *)soap_malloc(soap,(state->__size)*sizeof(struct ns__MsgItem));
	switch(type)
	{
		case 0://alarm
			{
				struct alarm** data;
				data = (struct alarm**)thedata;
				for(i=0;i<size;i++)
				{
					memset((state->__ptr+i),0,sizeof(struct ns__MsgItem));
					memset(temp,0,sizeof(temp));
					sprintf( temp,"<alarm_id>%s</alarm_id><device_id>%s</device_id><trojan_id>%s</trojan_id><sip>%s</sip><dip>%s</dip><dmac>%s</dmac><alarm_time>%s</alarm_time><type>%s</type><risk>%c</risk><scc>%s</scc><scc_flag>%c</scc_flag><descr>%s</descr>",(*data + i)->alarm_id,(*data + i)->device_id,(*data + i)->trojan_id,(*data + i)->sip,(*data + i)->dip,(*data + i)->dmac,(*data + i)->alarm_time,(*data + i)->type,(*data + i)->risk,(*data + i)->scc,(*data + i)->scc_flag,(*data + i)->descry);
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
				sprintf( temp,"<alert_id>%s</alert_id><device_id>%s</device_id><plugin_id>%d</plugin_id><risk>%c</risk><trojan_id>%s</trojan_id><alarm_time>%s</alarm_time><sip>%s</sip><sport>%d</sport><dip>%s</dip><dport>%d</dport><dmac>%s</dmac><protocol>%d</protocol><descr>%s</descr><dnshash>%s</dnshash><scc>%s</scc><scc_flag>%c</scc_flag><extra_text>%s</extra_text>",(*data + i)->alert_id,(*data + i)->device_id,(*data + i)->plugin_id,(*data + i)->risk,(*data + i)->trojan_id,(*data + i)->alarm_time,(*data + i)->sip,(*data + i)->sport,(*data + i)->dip,(*data + i)->dport,(*data + i)->dmac,(*data + i)->protocol,(*data + i)->descry,(*data + i)->dnshash,(*data + i)->scc,(*data + i)->scc_flag,(*data + i)->extra_text);
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
			cout <<"Alert_id==========="<<endl;
			struct session_blocked **data;
			data = (struct session_blocked**)thedata;
			cout <<"size====================="<<size<<endl;
			for(i=0;i<size;i++)
			{
				memset((state->__ptr+i),0,sizeof(struct ns__MsgItem));
				memset(temp,0,sizeof(temp));
				sprintf( temp,"<device_id>%s</device_id><dip>%s</dip><sip>%s</sip><sport>%d</sport><start_time>%s</start_time><expired_time>%s</expired_time><block_desc>%s</block_desc>",(*data + i)->device_id,(*data + i)->dip,(*data + i)->sip,(*data + i)->sport,(*data + i)->start_time,(*data + i)->expired_time,(*data + i)->block_desc);
				cout <<"temp==============="<<temp<<endl;
//				memcpy((state->__ptr[0]).type,"session_blocked",sizeof("session_blocked"));
//				memcpy((state->__ptr[0]).msg,temp,strlen(temp));
				(state->__ptr+i)->type = (char*)soap_malloc(soap,strlen("session_blocked")+1);
				(state->__ptr+i)->msg = (char*)soap_malloc(soap,strlen(temp)+1);
				strcpy((state->__ptr+i)->type,(char*)"session_blocked");
				strcpy((state->__ptr+i)->msg,temp);
			}
			cout <<"Alert_id===========" <<endl;
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
//					cout<<"0"<<endl;
					char *front = NULL;
					char *back = NULL;
//					cout<<"1=="<<num*sizeof(struct dns_rules)<<endl;
					*rule_obj= realloc(*rule_obj,(num+1)*sizeof(struct dns_rules));
//					cout<<"10"<<endl;
					front = strchr(position,'#');
					if(rules_NULL_error(front))
						break;
//					cout<<position[0]<<endl;
					(*tempprt+num)->type = position[0];
//					cout<<"6"<<endl;
					back = strchr(front+1,'#');
//					cout<<"7"<<endl;
					if(rules_NULL_error(back))
						break;
//					cout<<"2"<<endl;
					memset((*tempprt+num)->dns_name,0,sizeof((*tempprt+num)->dns_name));
					strncpy((*tempprt+num)->dns_name,front+1,back-front-1);
					front = strchr(back+1,'#');
					if(rules_NULL_error(front))
						break;
//					cout<<"3"<<endl;
					(*tempprt+num)->match_mode = (back+1)[0];
					front = strchr(front+1,'#');
					if(rules_NULL_error(front))
						break;
//					cout<<"4"<<endl;
					(*tempprt+num)->dangerous = (front+1)[0];
					position = tempp + 1;
//					cout<<"5"<<endl;
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
					cout<<"num ========"<<num<<"	"<<(*tempptr+num)->type<<endl;
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
					cout<<"@@@@@"<<temp<<endl;
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
					num ++;
				}
				break;
			}
		case 6:
			{
				cout<<"geoip file:"<<endl<<rulescontent<<endl;
				struct ippos_rules** tempptr = (struct ippos_rules**)rule_obj;
				while((tempp = strchr(position,'\n')) != NULL)
				{
					char *front = NULL;
					char *back = NULL;
					char temp[20];
					*rule_obj = realloc(*rule_obj,(num+1)*sizeof(struct ippos_rules));
					front = strchr(position,'#');
					cout<<1<<endl;
					if(rules_NULL_error(front))
						break;
					memset(temp,0,sizeof(temp));
					strncpy(temp,position,front-position);
					cout<<"temp:"<<temp<<endl;
					(*tempptr+num)->startip = (u_int)atoll(temp);
					back = strchr(front+1,'#');
					cout<<2<<endl;
					if(rules_NULL_error(back))
						break;
					memset(temp,0,sizeof(temp));
					strncpy(temp,front+1,back-front-1);
					(*tempptr+num)->endip = (u_int)atoll(temp);
					front	 = strchr(back+1,'#');
					cout<<3<<endl;
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
//cout<<(*tempptr+num)->startip<<"@"<<(*tempptr+num)->endip<<"@"<<(*tempptr+num)->region<<"@"<<(*tempptr+num)->tirr<<"@"<<(*tempptr+num)->flag<<endl;
					num ++;
				}
				break;
			}
		case 4:
			{
				cout<<"senlist file:"<<endl<<rulescontent<<endl;
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
					position = tempp + 1;
					num ++;
				}
				break;
			}
			case 5:
			{
				cout<<"spectro file:"<<endl<<rulescontent<<endl;
				struct spectro_rules** tempptr = (struct spectro_rules**)rule_obj;				

				*rule_obj = realloc(*rule_obj,(num+1)*sizeof(struct spectro_rules));
				memset((*tempptr+num)->content,0,sizeof(tempptr[num]->content));
				strncpy((*tempptr+num)->content,rulescontent,ruleslen);

				num ++;
				break;
			}
		default:
			break;
	}
	rulesnum = num;
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
