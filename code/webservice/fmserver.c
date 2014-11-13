#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <error.h>
#include <stdlib.h>

#include <event.h>
#include <evhttp.h>

#include <hiredis/hiredis.h>
#include <libxml/parser.h>
#include <libxml/xmlmemory.h>
#include "sqlhandler.h"

#define DEBUG

#define REDIS_SERVER_IP "192.168.1.120"
#define REDIS_SERVER_PORT 6379

#define LOCAL_IP		"192.168.1.120"
#define HTTP_PORT		8888


struct event_base* base;
redisContext* c;
redisReply* reply;
int i = 0;


void genericHandler(struct evhttp_request *req, void *arg)
{
	char xmlBuf[102400];
	struct evbuffer *evBuf = evbuffer_new();

	if (NULL == evBuf){
		puts("failed to create response buffer \n");
		return;
	}
	//解析url中的请求参数
	struct evkeyvalq get_param;
	const char *uri = evhttp_request_get_uri(req);
	evhttp_parse_query(uri, &get_param);
	const char *opName = evhttp_find_header(&get_param, "opName");


	//将Post内容转换为get格式，使用get解析的函数获取参数
	struct evkeyvalq post_param;
	
	char *post_data = (char *)EVBUFFER_DATA(req->input_buffer);
	int bufsize = EVBUFFER_LENGTH(req->input_buffer);
	
	char * tmp = (char *)malloc(bufsize + 5);
	tmp[0] = '/'; tmp[1] = '?';
	strncpy((char *) &tmp[2], post_data, bufsize);
	tmp[bufsize + 2] = 0;
	tmp[bufsize + 3] = 0;
	char *decoded_uri = evhttp_decode_uri(tmp);
	printf(">>>decoded_uri=%s\n", decoded_uri);
	
	evhttp_parse_query(decoded_uri, &post_param);

	//请求操作为查询电台信息
	if( strncmp(opName, "queryFm", strlen(opName)) == 0 ) {
		const char *cityName = evhttp_find_header(&post_param, "cityName");
		queryFm(cityName, xmlBuf);
	}

	//请求操作为查询市区信息
	else if( strncmp(opName, "queryCity", strlen(opName)) == 0 ) {
		const char *province = evhttp_find_header(&post_param, "provinceSpell");
		printf(">>>province=%s\n", province);
		queryCity(province, xmlBuf);		
	}

	//请求操作为查询电台节目信息
	else if( strncmp(opName, "queryShow", strlen(opName)) == 0 ) {
		const char *radioId = evhttp_find_header(&post_param, "radioId");
		queryShow(radioId, xmlBuf);		
	}


	evbuffer_add_printf(evBuf, "%s\n", xmlBuf);
	evhttp_send_reply(req, HTTP_OK, "OK", evBuf);
	memset(req, 0, sizeof(req));

	evbuffer_free(evBuf);
}

int eventHttp(void)
{
	struct evhttp * httpServer = evhttp_new(base);
	if (NULL == httpServer){
		printf("evhttp_new error\n");
		return -1;
	}

	if (evhttp_bind_socket(httpServer, LOCAL_IP, HTTP_PORT) != 0){
		printf("evhttp_bind_socket error\n");
		return -1;
	}
	//printf("http server IP:%s\nhttp port:%d\n", LOCAL_IP, HTTP_PORT);

	//设置超时时间
	evhttp_set_timeout(httpServer , 10);	

	//服务只接收POST请求
	evhttp_set_allowed_methods(httpServer , EVHTTP_REQ_POST);

	evhttp_set_gencb(httpServer, genericHandler, NULL);
	return 0;
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
	
	char xmlBuf[10240];

	base = event_base_new();
	if (NULL == base) {
		printf("event_base_new error\n");
		return -1;
	}
	eventHttp();

	event_base_dispatch(base);

	redisFree(c);

	return 0;
}
