#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <stdio.h>
#include <sys/stat.h>

#include "soapH.h"





#define UploadXConfig (char *)"/usr/local/tipschecker/etc/tipscheck/config"
#define UploadXTemp (char *)"/usr/local/tipschecker/etc/tipscheck"





//////////////////////////////////////////////////////////////////////////
//���뷢���ļ�ʱ��ȡ�ļ�                                                //
//////////////////////////////////////////////////////////////////////////
void *mime_read_open(struct soap *soap, void *handle, const char *id, const char *type, const char *description);
void mime_read_close(struct soap *soap, void *handle);
size_t mime_read(struct soap *soap, void *handle, char *buf, size_t len);

//////////////////////////////////////////////////////////////////////////
//��������ļ�ʱ�洢�ļ�                                                //
//////////////////////////////////////////////////////////////////////////
void *mime_client_write_open(struct soap *soap, void *unused_handle, const char *id, const char *type, const char *description, enum soap_mime_encoding encoding);
void *mime_client_write_open_rules(struct soap * soap, void * unused_handle, const char * id, const char * type, const char * description, enum soap_mime_encoding encoding);
void mime_client_write_close(struct soap *soap, void *handle);
int mime_client_write(struct soap *soap, void *handle, const char *buf, size_t len);
int mime_client_write_rules(struct soap * soap, void * handle, const char * buf, size_t len);

//////////////////////////////////////////////////////////////////////////
//���ļ�                                                              //
//////////////////////////////////////////////////////////////////////////
int open_data(struct soap *soap,const char *file,struct x__Data *data);

//////////////////////////////////////////////////////////////////////////
//�ϴ��ļ�                                                              //
//////////////////////////////////////////////////////////////////////////
int client_putData(struct soap * soap,int count,char ** files);

//////////////////////////////////////////////////////////////////////////
//�����ļ�                                                              //
//////////////////////////////////////////////////////////////////////////
int client_getData(struct soap * soap,int count,char ** files);

int client_init(struct soap * soap, int type, int servertype);
int client_end(struct soap * soap);

int GetTime(unsigned long * timebuff, int servertype, time_t * old_time, time_t * new_time);


#endif



