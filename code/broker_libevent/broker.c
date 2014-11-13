#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <error.h>
#include <stdlib.h>

#include <event.h>
#include <hiredis/hiredis.h>
#include <libxml/parser.h>
#include <libxml/xmlmemory.h>

#define DEBUG

#define REDIS_SERVER_IP "127.0.0.1"
#define REDIS_SERVER_PORT 6379

#define LOCAL_IP "192.168.88.129"
#define SOCKET_SERVER_PORT 8888

typedef struct _mrcpNode
{
	char mrcpId[20];
	char IPaddr[24];
	char current[10];
	char max[10];
}mrcpNode;

struct event_base* base;
redisContext* c;
redisReply* reply;
int i = 0;

void parse_xml(const char* buf, const int bufLen, mrcpNode* mrcpInfo)
{
	int i=0;
	xmlDocPtr doc;
	xmlNodePtr curNode;
	if(mrcpInfo == NULL) {
		printf("mrcpInfo is NULL\n");
		return;
	}

	xmlKeepBlanksDefault(0);
	doc = xmlParseMemory(buf, bufLen);
	if(doc == NULL) {
		printf("doc is NULL\n");
		return;
	}

	curNode = xmlDocGetRootElement(doc);
	if(curNode == NULL) {
		printf("parse xml error!!!\n");
		xmlFreeDoc(doc);
		return;
	}
	
	strcpy( mrcpInfo->IPaddr, xmlGetProp(curNode, BAD_CAST "IP") );
	strcpy( mrcpInfo->mrcpId, xmlGetProp(curNode, BAD_CAST "ID") );
	
	curNode = curNode->xmlChildrenNode;
	if(curNode == NULL) {
		printf("parse xml error!!!\n");
		xmlFreeDoc(doc);
		return;
	}
	
	strcpy( mrcpInfo->current, xmlGetProp(curNode, BAD_CAST "current") );
	strcpy( mrcpInfo->max, xmlGetProp(curNode, BAD_CAST "max") );

	xmlFreeDoc(doc);
}

void cat_xml(const mrcpNode* mrcpInfo, char* buf)
{
	xmlChar* xmlbuf;
	int bufsize;

	xmlDocPtr doc = xmlNewDoc(BAD_CAST"1.0");

	xmlNodePtr node = xmlNewNode(NULL, BAD_CAST"load_balance");
	xmlDocSetRootElement(doc, node);
	xmlNewProp(node, BAD_CAST"IP", BAD_CAST mrcpInfo->IPaddr);
	xmlNewProp(node, BAD_CAST"ID", BAD_CAST mrcpInfo->mrcpId);

	xmlNodePtr grandnode = xmlNewNode(NULL, BAD_CAST "channels");
	xmlAddChild(node, grandnode);
	xmlNewProp(grandnode, BAD_CAST"current", BAD_CAST mrcpInfo->current);
	xmlNewProp(grandnode, BAD_CAST"max", BAD_CAST mrcpInfo->max);

	xmlDocDumpFormatMemory(doc, &xmlbuf, &bufsize, 1);
	strncpy(buf, xmlbuf, bufsize);
	buf[bufsize] = '\0';

	xmlFree(xmlbuf);
	xmlFreeDoc(doc);

	return;
}

void onRead(int fd, short iEvent, void* arg)
{
	int iLen;
	int cmdLen;
	char buf[1500];
	mrcpNode mrcpInfo;

	iLen = recv(fd, buf, 1500, 0);
	if(iLen <= 0) {
		printf("client close!\n");
		struct event* pEvRead = (struct event*)arg;
		event_del(pEvRead);
		free(pEvRead);
		return;
	}
#ifdef DEBUG
	printf("recv from client buf:%s\n", buf);
#endif
		
	memset( &mrcpInfo, 0, sizeof(mrcpInfo) );
	if( !strncmp(buf, "get", 3) ) {
		char xmlbuf[1024];

		reply = redisCommand(c, "zrange mrcp 0 0 withscores");
		if( (reply->element[0]->str != NULL) && (reply->element[0]->str != NULL) ) {
			strcpy(mrcpInfo.current, reply->element[1]->str);
			sscanf(reply->element[0]->str, "%[^:]:%[^:]:%s", mrcpInfo.mrcpId, mrcpInfo.IPaddr, mrcpInfo.max);
			printf("all=%s\nmrcpId=%s IPaddr=%s max=%s\n", reply->element[0]->str, mrcpInfo.mrcpId, mrcpInfo.IPaddr, mrcpInfo.max);

			cat_xml(&mrcpInfo, xmlbuf);
#ifdef DEBUG
			printf("response_xml:\n%s\n", xmlbuf);
#endif
			send(fd, xmlbuf, sizeof(xmlbuf), 0);
		}
		else {
			printf("redis get content is NULL\n");
		}

		freeReplyObject(reply);
		return ;
	}

	parse_xml(buf, sizeof(buf), &mrcpInfo);
#ifdef DEBUG
	printf("parse xml:\n>>>ip=%s, id=%s, max=%s, current=%s\n\n", mrcpInfo.IPaddr, mrcpInfo.mrcpId, mrcpInfo.max, mrcpInfo.current);
#endif
	reply = redisCommand(c, "zadd mrcp %s %s:%s:%s", mrcpInfo.current, mrcpInfo.mrcpId, mrcpInfo.IPaddr, mrcpInfo.max);
	freeReplyObject(reply);	
}

void onAccept(int fd, short iEvent, void* arg)
{
	int clientfd;
	struct sockaddr_in caddr;

	socklen_t clen = sizeof(caddr);
	clientfd = accept(fd, (struct sockaddr*)&caddr, &clen);

	struct event* pEvRead = NULL;
	pEvRead = (struct event*)malloc(sizeof(struct event));
	if(pEvRead == NULL) {
		printf("malloc error\n");
		return;
	}

	event_set(pEvRead, clientfd, EV_READ|EV_PERSIST, onRead, pEvRead);
	event_base_set(base, pEvRead);
	event_add(pEvRead, NULL);
}

int main()
{
	int fd;
	struct sockaddr_in servaddr;

	c = redisConnect(REDIS_SERVER_IP, REDIS_SERVER_PORT);
	if( c == NULL || c->err ) {
		printf("redis connection error:%s\n", c->errstr);
		redisFree(c);
		exit(1);
	}
	printf("*******redis connect successed***********\n");

	if( (fd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
		printf("socket error\n");
		exit(1);
	}

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(LOCAL_IP);
	servaddr.sin_port = htons(SOCKET_SERVER_PORT);

	bind(fd, (struct sockaddr*)&servaddr, sizeof(servaddr));
	listen(fd, 5);

	base = event_base_new();
	struct event evListen;

	event_set(&evListen, fd, EV_READ|EV_PERSIST, onAccept, NULL);
	event_base_set(base, &evListen);
	event_add(&evListen, NULL);
	event_base_dispatch(base);

	redisFree(c);
	return 0;
}
