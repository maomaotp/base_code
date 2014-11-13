#include "sqlhandler.h"
#include <stdio.h>
#include <string.h>

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>


void Init_Sql(MYSQL* mysql)
{
	#ifdef _THREAD
	pthread_mutex_init(&mutex,NULL);
	printf("thread init\n");
	#endif
	mysql_init(mysql);
	
	if(NULL == mysql_real_connect(mysql, SERVER_IP, USER_NAME, PASSWD, DB_NAME, 0, NULL, 0))
	{
		printf("connect:%s\n",mysql_error(mysql));
		return;
	}
	printf("connect success!\n");

	return;
}

int queryproXml(MYSQL* mysql, char* sql, char *xmlBuf)
{
	mysql_set_character_set(mysql, "utf8");
	if(mysql_query(mysql,sql)) {
		printf("error making query:%s\n",mysql_error(mysql));
		return -1;
	}
	MYSQL_RES* res=mysql_use_result(mysql);//获取查询结果
	if(!res) {
		printf("err use_result:%s\n",mysql_error(mysql));
		return -2;
	}

	MYSQL_ROW row;

	int bufsize;
	xmlChar *resBuf;
	
	xmlDocPtr doc = xmlNewDoc(BAD_CAST"1.0");
	//设置第一层节点
	xmlNodePtr firstNode = xmlNewNode(NULL, BAD_CAST"FM_LIST");
	xmlDocSetRootElement(doc, firstNode);

	while(1) {
		row=mysql_fetch_row(res);
		if(!row)break;

		//设置第二层节点
		xmlNodePtr secondNode = xmlNewNode(NULL, BAD_CAST "cityName");
		xmlAddChild(firstNode, secondNode);
		xmlAddChild(secondNode, xmlNewText(BAD_CAST row[0]));	
	}

	xmlDocDumpFormatMemory(doc, &resBuf, &bufsize, 1);
	strcpy(xmlBuf, (char *)resBuf);

	xmlFree(resBuf);
	xmlFreeDoc(doc);
	mysql_free_result(res);
	return 1;
}




int queryShowXml(MYSQL* mysql, char* sql, char *xmlBuf)
{
	mysql_set_character_set(mysql, "utf8");
	if( mysql_query(mysql,sql) ) {
		printf("error making query:%s\n",mysql_error(mysql));
		return -1;
	}
	MYSQL_RES* res=mysql_use_result(mysql);//获取查询结果
	if(!res) {
		printf("err use_result:%s\n",mysql_error(mysql));
		return -2;
	}

	MYSQL_ROW row;

	int bufsize;
	int fieldNum;
	xmlChar *resBuf;
	char *field[8] = {"programId", "radioId", "programName", "introduction", "webSite", "createTime", "updateTime"};
	
	xmlDocPtr doc = xmlNewDoc(BAD_CAST"1.0");
	//设置第一层节点
	xmlNodePtr firstNode = xmlNewNode(NULL, BAD_CAST"SHOW_LIST");
	xmlDocSetRootElement(doc, firstNode);

	while(1) {
		row=mysql_fetch_row(res);
		if(!row)break;

		//设置第二层节点
		xmlNodePtr secondNode = xmlNewNode(NULL, BAD_CAST "SHOW_INFO");
		xmlAddChild(firstNode, secondNode);
		//xmlNewProp(secondNode, BAD_CAST"number", BAD_CAST rowNum);

		//设置第三层节点
		for(fieldNum=0; fieldNum<8; fieldNum++) {
			xmlNodePtr thirdNode = xmlNewNode(NULL, BAD_CAST field[fieldNum]);
			xmlAddChild(secondNode, thirdNode);
			//设置字段内容
			xmlAddChild(thirdNode, xmlNewText(BAD_CAST row[fieldNum]));	
		}
	}

	xmlDocDumpFormatMemory(doc, &resBuf, &bufsize, 1);
	strcpy(xmlBuf, (char *)resBuf);

	xmlFree(resBuf);
	xmlFreeDoc(doc);
	mysql_free_result(res);
	return 1;
}

int Select_Sql_Exec(MYSQL* mysql, char* sql, char *xmlBuf)
{
	mysql_set_character_set(mysql, "utf8");
	if(mysql_query(mysql,sql)) {
		printf("error making query:%s\n",mysql_error(mysql));
		return -1;
	}
	MYSQL_RES* res=mysql_use_result(mysql);//获取查询结果
	if(!res) {
		printf("err use_result:%s\n",mysql_error(mysql));
		return -2;
	}

	MYSQL_ROW row;

	int bufsize;
	int fieldNum;
	xmlChar *resBuf;
	char *field[14] = {"radioId", "nameCn", "nameEn", "url", "webSite", "introduction", "address", "zip", "scheduleURL", "radioLevel", "provinceSpell","cityName","createTime","updateTime"};
	
	xmlDocPtr doc = xmlNewDoc(BAD_CAST"1.0");
	//设置第一层节点
	xmlNodePtr firstNode = xmlNewNode(NULL, BAD_CAST"FM_LIST");
	xmlDocSetRootElement(doc, firstNode);

	while(1) {
		row=mysql_fetch_row(res);
		if(!row)break;

		//设置第二层节点
		xmlNodePtr secondNode = xmlNewNode(NULL, BAD_CAST "FM_INFO");
		xmlAddChild(firstNode, secondNode);
		//xmlNewProp(secondNode, BAD_CAST"number", BAD_CAST rowNum);

		//设置第三层节点
		for(fieldNum=0; fieldNum<14; fieldNum++) {
			xmlNodePtr thirdNode = xmlNewNode(NULL, BAD_CAST field[fieldNum]);
			xmlAddChild(secondNode, thirdNode);
			//设置字段内容
			xmlAddChild(thirdNode, xmlNewText(BAD_CAST row[fieldNum]));	
		}
	}

	xmlDocDumpFormatMemory(doc, &resBuf, &bufsize, 1);
	strcpy(xmlBuf, (char *)resBuf);

	xmlFree(resBuf);
	xmlFreeDoc(doc);
	mysql_free_result(res);
	return 1;
}

int Insert_Sql_Exec(MYSQL* mysql, char* sql)
{

	#ifdef _THREAD
	pthread_mutex_lock(&mutex);
	#endif
	if(mysql_query(mysql, sql))
	{
		printf("error making query:%s\n",mysql_error(mysql));
		#ifdef _THREAD
		pthread_mutex_unlock(&mutex);
		#endif
		return -1;
	}
	int ret=mysql_affected_rows(mysql);
	#ifdef _THREAD
	pthread_mutex_unlock(&mutex);
	#endif
	
	return ret;
}

int Exec_Begin()
{
	printf("begin\n");
}

int Exec_Rollback()
{
	printf("rollback\n");
}

int stores_Sql_Exec(MYSQL* mysql, char* sql, void (*call_back)(char** result, int row, char *resBuf), char *resBuf)
{
	if(mysql_query(mysql,sql)) {
		printf("error making query:%s\n",mysql_error(mysql));
		return -1;
	}
	MYSQL_RES* res=mysql_use_result(mysql);//获取查询结果
	if(!res) {
		printf("err use_result:%s\n",mysql_error(mysql));
		return -2;
	}
	MYSQL_ROW row;
	int column_count=mysql_field_count(mysql);
	while(1)
	{
		row=mysql_fetch_row(res);
		if(!row)break;
		call_back((char**)row,column_count,resBuf);
	}
	mysql_free_result(res);
	return 1;
}
