#include "client.h"
#include "ssl.nsmap"
#include <sys/time.h>
#include "../check.h"

using namespace std;

extern char manageip[];
extern char manageip1[];

char Server[50] = "";      //用来存储服务器的IP地址后面是端口号
char Device_ID[] = "1303030001";
char t_auth_str[] = "2DB84CFC3B1B3907DCB24FB3746064D244A40218625BCC60B908853A140EA89784338A7734FAFD3E";

int GetTime(unsigned long* timebuff,int servertype, time_t *old_time, time_t *new_time)
{
//	return 0;
	unsigned long ret_time;
	int result = 0;
	struct soap soap;
	//		 printf("line=%d\n", __LINE__);
	//		 printf("%s\n", manageip);
	if(client_init(&soap,0,servertype) == -2)
		return -1;
	//		 printf("line=%d\n", __LINE__);

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
		client_end(&soap);
		return result;
	}

	client_end(&soap);
#if 0
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
	it.it_interval.tv_usec=0;a
#endif
	
	timebuff = timebuff; old_time = old_time; new_time = new_time;		//hyj 靠靠
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
	else{}
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
	
	soap = soap; if(id){}; if(type){}; if(description){};		//hyj 靠靠
	return (void*)fd;
}

size_t mime_read(struct soap *soap, void *handle, char *buf, size_t len)
{ 
//cout<<"===Putdata reading==="<<endl;
	soap = soap;		//hyj 靠靠
	return fread(buf, 1, len, (FILE*)handle);
}

void mime_read_close(struct soap *soap, void *handle)
{ 
//	cout<<"===Putdata closing==="<<endl;
	//fprintf(stderr, "Closing streaming outbound MIME channel\n");
	fclose((FILE*)handle);

	soap = soap;	//hyj 靠靠
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
	
	unused_handle = unused_handle; soap = soap; if(id){}; if(type){}; if(description){}; encoding = encoding;		//hyj 靠靠
	
	return (void*)fd;
}


void mime_client_write_close(struct soap *soap, void *handle)
{ 
//cout<<"==in mime_client_write_close=="<<endl;
	fclose((FILE*)handle);
	soap = soap;		//hyj 靠靠
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
//hyj	char *iflog = NULL;
//hyj	char *newlogname = NULL;
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
//hyj	int ret = 0;
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


