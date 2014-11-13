#include "client.h"
#include "../rulesdata.h"
#include "connect.nsmap"
using namespace std;

char Server[50] = "";      //用来存储服务器的IP地址后面是端口号
char Device_ID[] = "1203010088";
char t_auth_str[] = "Efdfiutrefqpoiutytrehvnm$dghjwdf%123ddgf";

int rulesnum = 0;//策略结构体个数；
int ruleslen = 0;//缓存策略文件的长度；
char *rulescontent = NULL; //用于缓存策略文件的buff；
	
int SendStatus(struct status_struct * status, char * cmdbuff)
{//如果没有管控命令返回0，否则返回命令长度
	int command_len = 0;
	struct soap soap;
	client_init(&soap,0);

	struct ns__ResultList resultlist;
	struct ns__MsgList state;
	
	get_device_state(&soap,&state,status);
	if(soap_call_ns2__upload_message(&soap,Server, NULL, Device_ID,0,&state,t_auth_str,&resultlist) == SOAP_OK)
	{
		cout<<"send to server OK !"<<endl;
		cout<<"resultlist->__size = "<<resultlist.__size<<endl;
		cout<<resultlist.__ptr[0].TimeStamp<<"  "<<resultlist.__ptr[0].Order<<"  "<<resultlist.__ptr[0].Param<<endl;
		if(resultlist.__ptr[0].Order == NULL)
			command_len = 0;
		else
		{
			sprintf(cmdbuff,"%s %s",resultlist.__ptr[0].Order,resultlist.__ptr[0].Param);
			command_len = strlen(resultlist.__ptr[0].Order);
		}
	}
	else
	{
		cout<<"send to server failed !\n"<<endl;
	}

	client_end(&soap);
	return command_len;
}

int LoadRules(char * filename, void * * rule_obj, int type)
{//给*rule_obj分配空间，赋值，返回个数	
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
	}
	
//	cout<<1<<endl;
//	free(files);
//	free(tempfile);
	client_end(&soap);

	*rule_obj = NULL;
	get_rules_info(rule_obj,type);

	for(int i=0;i<rulesnum;i++)
	{
		cout<<"-----------client.h-------------"<<endl;
		cout<<((struct dns_rules*)*rule_obj+i)->type<<endl;
		cout<<((struct dns_rules*)*rule_obj+i)->dns_name<<endl;
		cout<<((struct dns_rules*)*rule_obj+i)->match_mode<<endl;
		cout<<((struct dns_rules*)*rule_obj+i)->dangerous<<endl;
		cout<<"-----------client.h-------------"<<endl;
	}
	

	if(rulescontent != NULL)
		free(rulescontent);

	return rulesnum;
}

int UpdateSys(char * filename)
{//下载系统升级包到/etc/tipscheck
	struct soap soap;
	client_init(&soap,1);

	char **files;
	int count = 1;
	files = (char**)soap_malloc(&soap,count* sizeof(char*));
	char tempfile[256] = "";
	memset(tempfile,0,sizeof(tempfile));
//	strcpy(tempfile,"/etc/tipscheck/");
	strcpy(tempfile,"./");
	strcat(tempfile,filename);
	files[0] = tempfile;
	if(client_getData(&soap,count,files) == 0)
	{
		cout<<"Get file success !"<<endl;
	}
	else
	{
		cout<<"Get file failed !"<<endl;
	}
//	cout<<2<<endl;
//	free(files);
//	free(tempfile);
	client_end(&soap);
	return 0;
}

int UploadData(char *filename)
{
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
	}
//	cout<<3<<endl;
//	free(files);
//	free(tempfile);

	client_end(&soap);
	return 0;
}

int client_init(struct soap *soap,int type)
{
	memset(Server,0,sizeof(Server));
	sprintf(Server,"172.16.0.139:1234");

//	soap_init(&soap);
	soap_init1(soap,SOAP_ENC_MTOM);
	soap_set_mode(soap,SOAP_C_UTFSTRING);
	soap->mode |= SOAP_C_UTFSTRING;
	soap_set_namespaces(soap, namespaces); 
	soap_set_mime(soap,NULL,NULL);

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
cout<<"Putdata read opening"<<endl;
	FILE *fd = (FILE*)handle;
	fprintf(stderr, "Opening streaming outbound MIME channel for id=%s type=%s\n", id, type);
	return (void*)fd;
}

size_t mime_read(struct soap *soap, void *handle, char *buf, size_t len)
{ 
cout<<"===Putdata reading==="<<endl;
	return fread(buf, 1, len, (FILE*)handle);
}

void mime_read_close(struct soap *soap, void *handle)
{ 
	fprintf(stderr, "Closing streaming outbound MIME channel\n");
	fclose((FILE*)handle);
}

void *mime_client_write_open(struct soap *soap, void *unused_handle, const char *id, const char *type, const char *description, enum soap_mime_encoding encoding)
{ 
cout<<"===in write open==="<<endl;
	/* Note: the 'unused_handle' is always NULL */
	FILE *fd;
	const char *file;
	int len = 0;
	char *tempdir = (char*)".";
	
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
	/* Note: the 'unused_handle' is always NULL */
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
cout<<"writing  ====================="<<endl;
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
cout<<"*****in putdata opening*****"<<file<<endl;
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
	struct ns__PutDataResponse response;
		
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
cout<<keys.__size<<"||"<<keys.key[0]<<endl;
cout<<data.__size<<endl;
cout<<data.item->xmime5__contentType<<endl;
cout<<"===xop_Include==="<<data.item->xop__Include.__size<<"||"<<data.item->xop__Include.type<<endl;
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
		cout<<"Get data ok !"<<endl;
	}
//	cout<<5<<endl;
	//free(keys.key);

	return soap->error;
}

///////////////////////////////////////////////////////////////////////////////////////////////

int get_session_blocked(struct soap *soap,struct ns__MsgList *state)
{
	char temp[1024];

	state->__size = 4;
	state->__ptr = (struct ns__MsgItem *)soap_malloc(soap,(state->__size)*sizeof(struct ns__MsgItem));

	memset(&(state->__ptr[0]),0,sizeof(struct ns__MsgItem));
	memset(temp,0,sizeof(temp));
	sprintf( temp,"<Device_Id>1203010088</Device_Id>\
				<Dip>17.43.21.15</Dip>\
				<Sip></Sip>\
				<Sport>80</Sport>\
				<Start_Time>2011-12-6 18:08:24</Start_Time>\
				<Expired_Time>2011-12-6 20:18:24</Expired_Time>\
				<Block_Desc></Block_Desc>");
	memcpy((state->__ptr[0]).type,"session_blocked",sizeof("session_blocked"));
	memcpy((state->__ptr[0]).msg,temp,strlen(temp));

	memset(&(state->__ptr[1]),0,sizeof(struct ns__MsgItem));
	memset(temp,0,sizeof(temp));
	sprintf( temp,"<Device_Id>1203010088</Device_Id>\
				<Dip>17.43.31.25</Dip>\
				<Sip></Sip>\
				<Sport>1080</Sport>\
				<Start_Time>2011-12-6 18:18:24</Start_Time>\
				<Expired_Time>2011-12-6 20:28:24</Expired_Time>\
				<Block_Desc></Block_Desc>");
	memcpy((state->__ptr[1]).type,"session_blocked",sizeof("session_blocked"));
	memcpy((state->__ptr[1]).msg,temp,strlen(temp));

	memset(&(state->__ptr[2]),0,sizeof(struct ns__MsgItem));
	memset(temp,0,sizeof(temp));
	sprintf( temp,"<Device_Id>1203010088</Device_Id>\
				<Dip>17.43.41.35</Dip>\
				<Sip></Sip>\
				<Sport>21</Sport>\
				<Start_Time>2011-12-6 18:28:24</Start_Time>\
				<Expired_Time>2011-12-6 20:38:24</Expired_Time>\
				<Block_Desc></Block_Desc>");
	memcpy((state->__ptr[2]).type,"session_blocked",sizeof("session_blocked"));
	memcpy((state->__ptr[2]).msg,temp,strlen(temp));

	memset(&(state->__ptr[3]),0,sizeof(struct ns__MsgItem));
	memset(temp,0,sizeof(temp));
	sprintf( temp,"<Device_Id>1203010088</Device_Id>\
				<Dip>17.43.51.45</Dip>\
				<Sip></Sip>\
				<Sport>110</Sport>\
				<Start_Time>2011-12-6 18:38:24</Start_Time>\
				<Expired_Time>2011-12-6 20:48:24</Expired_Time>\
				<Block_Desc></Block_Desc>");
	memcpy((state->__ptr[3]).type,"session_blocked",sizeof("session_blocked"));
	memcpy((state->__ptr[3]).msg,temp,strlen(temp));

	return 1;
}

int get_device_state(struct soap *soap,struct ns__MsgList *state,struct status_struct *status)
{
	char temp[1024];
	state->__size = 1;
	state->__ptr = (struct ns__MsgItem*)soap_malloc(soap,(state->__size)*sizeof(struct ns__MsgItem));
	memset(&(state->__ptr[0]),0,sizeof(struct ns__MsgItem));
	memset(temp,0,sizeof(temp));
	sprintf( temp,"<device_id>%s</device_id><stime>%s</stime><mem>%d</mem><cpu>%d</cpu><disk>%lf</disk><mirror>%c</mirror><flow>%d</flow><active_peer>%d</active_peer><module_state>%s</module_state>",status->device_id,status->stime,status->mem,status->cpu,status->disk,status->mirror,status->flow,status->active_peer,status->module_state);
	(state->__ptr[0]).type = (char*)"device_state";
	(state->__ptr[0]).msg = temp;
//	memcpy((state->__ptr[0]).type,"device_state",sizeof("device_state"));
//	memcpy((state->__ptr[0]).msg,temp,strlen(temp));
	return 1;
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
				cout<<"!!!!!!"<<rulescontent<<endl;
				struct dns_rules** tempprt = (struct dns_rules**)rule_obj;
				while((tempp = strchr(position,'\n')) != NULL)
				{
					cout<<"int reconfig dns rules"<<endl;
					char *front = NULL;
					char *back = NULL;
//					cout<<"1=="<<(num+1)*sizeof(struct dns_rules)<<endl;
					*rule_obj= realloc(*rule_obj,(num+1)*sizeof(struct dns_rules));
//					cout<<"10"<<endl;
					front = strchr(position,'#');
					if(rules_NULL_error(front))
						return 1;
//					cout<<position[0]<<endl;
					(*tempprt+num)->type = position[0];
//					cout<<"6"<<endl;
					back = strchr(front+1,'#');
//					cout<<"7"<<endl;
					if(rules_NULL_error(back))
						return 1;
//					cout<<"2"<<endl;
					memset((*tempprt+num)->dns_name,0,65);
					strncpy((*tempprt+num)->dns_name,front+1,back-front-1);
					front = strchr(back+1,'#');
					if(rules_NULL_error(front))
						return 1;
//					cout<<"3"<<endl;
					(*tempprt+num)->match_mode = (back+1)[0];
					front = strchr(front+1,'#');
					if(rules_NULL_error(front))
						return 1;
//					cout<<"4"<<endl;
					(*tempprt+num)->dangerous = (front+1)[0];
					position = tempp + 1;
//					cout<<"5"<<endl;
					num ++;
				}
				break;
			}
		case 2:
			while((tempp = strchr(position,'\n')) != NULL)
			{
				char *front = NULL;
				char *back = NULL;
				char temp[20];
				struct ip_rules** tempptr = (struct ip_rules**)rule_obj;
				tempptr = (struct ip_rules**)realloc(tempptr,sizeof(struct ip_rules));
				front = strchr(tempbuff,'#');
				if(rules_NULL_error(front))
					return 1;
				tempptr[num]->type = tempbuff[0];
				back = strchr(front+1,'#');
				if(rules_NULL_error(back))
					return 1;
				memset(temp,0,sizeof(temp));
				strncpy(temp,front+1,back-front-1);
				tempptr[num]->startip = (u_int)atoi(temp);
				front = strchr(back+1,'#');
				if(rules_NULL_error(front))
					return 1;
				memset(temp,0,sizeof(temp));
				strncpy(temp,back+1,front-back-1);
				tempptr[num]->endip = (u_int)atoi(temp);
				back = strchr(front+1,'#');
				if(rules_NULL_error(back))
					return 1;
				memset(temp,0,sizeof(temp));
				strncpy(temp,front+1,back-front-1);
				tempptr[num]->port= (u_short)atoi(temp);
				front = strchr(back+1,'#');
				if(rules_NULL_error(front))
					return 1;
				back = strchr(front+1,'#');
				if(rules_NULL_error(back))
					return 1;				
				tempptr[num]->dangerous = (front+1)[0];
				position = tempp + 1;
				num ++;
			}
			break;
		case 3:
			while((tempp = strchr(position,'\n')) != NULL)
			{
				char *front = NULL;
				char *back = NULL;
				char temp[20];
				struct ipaudit_rules** tempptr = (struct ipaudit_rules**)rule_obj;
				tempptr = (struct ipaudit_rules**)realloc(tempptr,sizeof(struct ipaudit_rules));
				front = strchr(tempbuff,'#');
				if(rules_NULL_error(front))
					return 1;
				memset(temp,0,sizeof(temp));
				strncpy(temp,tempbuff,front-tempbuff-1);
				tempptr[num]->startip = (u_int)atoi(temp);
				back = strchr(front+1,'#');
				if(rules_NULL_error(back))
					return 1;
				memset(temp,0,sizeof(temp));
				strncpy(temp,front+1,back-front-1);
				tempptr[num]->endip = (u_int)atoi(temp);
				front = strchr(back+1,'#');
				if(rules_NULL_error(front))
					return 1;
				memset(temp,0,sizeof(temp));
				strncpy(temp,back+1,front-back-1);
				tempptr[num]->port = (u_short)atoi(temp);
				position = tempp + 1;
				num ++;
			}
			break;
		case 6:
			while((tempp = strchr(position,'\n')) != NULL)
			{
				char *front = NULL;
				char *back = NULL;
				char temp[20];
				struct ippos_rules** tempptr = (struct ippos_rules**)rule_obj;
				tempptr = (struct ippos_rules**)realloc(tempptr,sizeof(struct ippos_rules));
				front = strchr(tempbuff,'#');
				if(rules_NULL_error(front))
					return 1;
				memset(temp,0,sizeof(temp));
				strncpy(temp,tempbuff,front-tempbuff-1);
				tempptr[num]->startip = (u_int)atoi(temp);
				back = strchr(front+1,'#');
				if(rules_NULL_error(back))
					return 1;
				memset(temp,0,sizeof(temp));
				strncpy(temp,front+1,back-front-1);
				tempptr[num]->endip = (u_int)atoi(temp);
				front = strchr(back+1,'#');
				if(rules_NULL_error(front))
					return 1;
				strncpy(tempptr[num]->region,back+1,front-back-1);
				back = strchr(front+1,'#');
				if(rules_NULL_error(back))
					return 1;
				strncpy(tempptr[num]->tirr,front+1,back-front-1);
				front = strchr(back+1,'\n');
				if(rules_NULL_error(front))
					return 1;
				tempptr[num]->flag = (back+1)[0];
				position = tempp + 1;
				num ++;
			}
			break;
		case 4:
			while((tempp = strchr(position,'\n')) != NULL)
			{
				char *front = NULL;
				char *back = NULL;
				char temp[20];
				struct sec_rules** tempptr = (struct sec_rules**)rule_obj;
				tempptr = (struct sec_rules**)realloc(tempptr,sizeof(struct sec_rules));
				front = strchr(tempbuff,'#');
				if(rules_NULL_error(front))
					return 1;
				memset(temp,0,sizeof(temp));
				strncpy(tempptr[num]->content,tempbuff,front-tempbuff-1);
				position = tempp + 1;
				num ++;
			}
			break;
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
		cout<<"The format of the geted rules infor is not right !"<<endl;
		return 1;
	}
	else
		return 0;
}
