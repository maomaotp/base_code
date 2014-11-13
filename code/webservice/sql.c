#include "sqlhandler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>


void call_back_table_name(char** result,int column,void* param)
{
	*((int *)param) = atoll(result[0]);
	printf("%s\n",result[0]);
	printf("\n");
}
void call_back(char** result,int row, char *resBuf)
{
	int i;
	char *content[11] = {"fmId", "wbRid", "fmName", "url", "intro", "logoAddr", "weiboAddr", "province", "hdvAddr", "microAddr", "updateTime"};
	xmlChar* xmlbuf;
	int bufsize;
	char rowNum[5];

	sprintf(rowNum, "%d", row);

	xmlDocPtr doc = xmlNewDoc(BAD_CAST"1.0");
	
	//设置第一层节点
	xmlNodePtr firstNode = xmlNewNode(NULL, BAD_CAST"FM_LIST");
	xmlDocSetRootElement(doc, firstNode);

	//设置第二层节点
	xmlNodePtr secondNode = xmlNewNode(NULL, BAD_CAST "FM_INFO");
	xmlAddChild(firstNode, secondNode);
	xmlNewProp(secondNode, BAD_CAST"number", BAD_CAST rowNum);

	//设置第三层节点
	for(i=0; i<11; i++) {
		xmlNodePtr thirdNode = xmlNewNode(NULL, BAD_CAST content[i]);
		xmlAddChild(secondNode, thirdNode);
		//设置字段内容
		xmlAddChild(thirdNode, xmlNewText(BAD_CAST result[i]));	
	}

	xmlDocDumpFormatMemory(doc, &xmlbuf, &bufsize, 1);
	resBuf = (char *)xmlbuf;

	//保存文件
	//int nRel = xmlSaveFile("CreatedXml.xml",doc);

	xmlFree(xmlbuf);
	xmlFreeDoc(doc);
}

/*new code*/
void queryCity(const char *province, char *xmlBuf)
{
	MYSQL mysql;
	char sql_buf[256];

	Init_Sql(&mysql);

	snprintf(sql_buf, sizeof(sql_buf), "select cityName from Radio_Info where provinceSpell='%s' group by cityName", province);

	printf("%s\n", sql_buf);

	int ret1 = queryproXml(&mysql, sql_buf, xmlBuf);

	if(ret1 < 0) {
		printf("select sql fail\n");
	}
	mysql_close(&mysql);
}

void queryFm(const char *cityName, char *xmlBuf)
{
	MYSQL mysql;
	char sql_buf[256];
	
	Init_Sql(&mysql);

	snprintf(sql_buf, sizeof(sql_buf), "select radioId,nameCn,nameEn,url,webSite,introduction,address,zip,scheduleURL,radioLevel,provinceSpell,cityName,createTime,updateTime from Radio_Info where cityName='%s' limit 20", cityName);

	printf("%s\n", sql_buf);

	int ret1 = Select_Sql_Exec(&mysql, sql_buf, xmlBuf);

	if(ret1 < 0) {
		printf("select sql fail\n");
	}
	mysql_close(&mysql);
}


void queryShow(const char *radioId, char *xmlBuf)
{
	MYSQL mysql;
	char sql_buf[256];

	Init_Sql(&mysql);

	snprintf(sql_buf, sizeof(sql_buf), "select programId,radioId,programName,introduction,webSite,createTime,updateTime from Program_Info where radioId='%s' and programState=0", radioId);
	printf("%s\n", sql_buf);

	int ret1 = queryShowXml(&mysql, sql_buf, xmlBuf);

	if(ret1 < 0) {
		printf("select sql fail\n");
	}
	mysql_close(&mysql);
}

