#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <stdio.h>
#include <sys/stat.h>
#include "../data.h"

#include "soapH.h"





#define UploadXConfig (char *)"/usr/local/tipschecker/etc/tipscheck/config"
#define UploadXTemp (char *)"/usr/local/tipschecker/etc/tipscheck"





//////////////////////////////////////////////////////////////////////////
//用与发送文件时读取文件                                                //
//////////////////////////////////////////////////////////////////////////
void *mime_read_open(struct soap *soap, void *handle, const char *id, const char *type, const char *description);
void mime_read_close(struct soap *soap, void *handle);
size_t mime_read(struct soap *soap, void *handle, char *buf, size_t len);

//////////////////////////////////////////////////////////////////////////
//用与接收文件时存储文件                                                //
//////////////////////////////////////////////////////////////////////////
void *mime_client_write_open(struct soap *soap, void *unused_handle, const char *id, const char *type, const char *description, enum soap_mime_encoding encoding);
void *mime_client_write_open_rules(struct soap * soap, void * unused_handle, const char * id, const char * type, const char * description, enum soap_mime_encoding encoding);
void mime_client_write_close(struct soap *soap, void *handle);
int mime_client_write(struct soap *soap, void *handle, const char *buf, size_t len);
int mime_client_write_rules(struct soap * soap, void * handle, const char * buf, size_t len);

//////////////////////////////////////////////////////////////////////////
//打开文件                                                              //
//////////////////////////////////////////////////////////////////////////
int open_data(struct soap *soap,const char *file,struct x__Data *data);

//////////////////////////////////////////////////////////////////////////
//上传文件                                                              //
//////////////////////////////////////////////////////////////////////////
int client_putData(struct soap * soap,int count,char ** files);

//////////////////////////////////////////////////////////////////////////
//下载文件                                                              //
//////////////////////////////////////////////////////////////////////////
int client_getData(struct soap * soap,int count,char ** files);

int client_init(struct soap * soap, int type, int servertype);
int client_end(struct soap * soap);

int GetTime(unsigned long * timebuff, int servertype);
int SendStatus(struct status_struct * status, char * * * cmdbuff, int servertype);
int SendData(void **data,int size,int type, int servertype);
int SendData1(struct session_blocked data, int servertype);
int LoadRules(char* filename,void** rule_obj,int type, int servertype);//给*rule_obj分配空间，赋值，返回个数
int UpdateSys(char* filename, int servertype);//下载系统升级包到/etc/tipscheck
int UploadData(char * filename, int servertype);
int SendConfig(struct config_struct* config,char *cmdbuff, int servertype);

int get_device_data(struct soap *soap,struct ns__MsgList *state,void **data,int size,int type);
int get_device_info(struct soap *soap,struct ns__MsgList *state,struct config_struct* config);
int get_device_state(struct soap *soap,struct ns__MsgList *state,struct status_struct *status);
int get_rules_info(void**rule_obj,int type);
int rules_NULL_error(char * p);

#endif



