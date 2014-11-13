#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <stdio.h>
#include <sys/stat.h>
#include "../data.h"

#include "soapH.h"


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

int client_init(struct soap * soap,int type);
int client_end(struct soap * soap);

int GetTime(unsigned long* timebuff);
int SendStatus(struct status_struct* status,char*** cmdbuff);//���û�йܿ������0�����򷵻������
int SendData(void **data,int size,int type);
int SendData1(struct session_blocked data);
int LoadRules(char* filename,void** rule_obj,int type);//��*rule_obj����ռ䣬��ֵ�����ظ���
int UpdateSys(char* filename);//����ϵͳ��������/etc/tipscheck
int UploadData(char * filename);
int SendConfig(struct config_struct* config,char *cmdbuff);

int get_device_data(struct soap *soap,struct ns__MsgList *state,void **data,int size,int type);
int get_device_info(struct soap *soap,struct ns__MsgList *state,struct config_struct* config);
int get_device_state(struct soap *soap,struct ns__MsgList *state,struct status_struct *status);
int get_rules_info(void**rule_obj,int type);
int rules_NULL_error(char * p);

#endif



