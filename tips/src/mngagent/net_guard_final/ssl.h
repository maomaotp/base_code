/*
	ssl.h
*/
#import "soap12.h"
#import "xop.h"
#import "xmime5.h"

//gsoap ns service name:	ssl
//gsoap ns service style:	rpc
//gsoap ns service encoding:	encoded
//gsoap ns service namespace:	urn:ssl
//gsoap ns service location:	https://127.0.0.1:8000
//gsoap x schema namespace:	http://127.0.0.1/m.xsd
//gsoap x schema elementForm:	qualified
//gsoap x schema type-documentation: Data holds a MIME attachment

typedef double xsd__double;
#define SOAP_RESEND 10000;

//客户端发送的数据结构定义
struct ns__MsgItem
{
	char *type;	//消息类型
	char *msg;	//消息体
};
struct ns__MsgList
{
	struct ns__MsgItem *__ptr; 	//消息链
	int __size;	//消息个数
};
//服务端返回的数据结构定义
struct ns__ResultItem
{
	unsigned long TimeStamp;	//命令时间戳
	char *Order;	//命令
	char *Param;	//命令参数
};
struct ns__ResultList
{
	struct ns__ResultItem **__ptr; 		//命令链
	int __size;  //命令个数
};
//客户端和服务端消息通信接口函数
/****
Device_ID 检测器设备号
returntype 是否需要检测器管理中心返回控制指令,true为返回控制指令，false为不返回
state 检测器上报的消息
authstr 认证码，不同厂家有不同的认证码，需要从国家保密局获得
result 检测器管理中心返回的控制指令
****/
int ns__upload_message(char *Device_ID,int returntype, struct ns__MsgList *state, char *authstr, struct ns__ResultList *result);

//gsoap x schema type-documentation: Data holds a MIME attachment
struct x__Data
{ _xop__Include xop__Include;
  @char *xmime5__contentType;
};

//gsoap x schema type-documentation: DataSet holds a set of MIME attachments
struct x__DataSet
{ int __size;//文件个数
  struct x__Data *item;//文件内容
};

//gsoap x schema type-documentation: Keys holds a set of strings to access the data collection at the server side
struct x__Keys
{ int __size;//文件名个数
  char **key;//文件名链
};

//gsoap ns service method-mime-type: PutData */*
//gsoap ns service method-documentation: PutData stores a data set at the server side and returns a set of unique keys to access that data
int ns__PutData(
  struct x__DataSet *x__data,//上传文件
  struct x__Keys *x__keys,//上传文件的文件名
  char *device_id,//检测器设备编号
  char *authstr,//检测器认证码
  int *response//服务端应答
);

//gsoap ns service method-mime-type: GetData */*
//gsoap ns service method-documentation: GetData gets a set of data items from the server given a set of keys
int ns__GetData(
  struct x__Keys *x__keys,//下载文件名
  char *device_id,//检测器设备编号
  char *authstr,//检测器认证码
  struct ns__GetDataResponse
  { struct x__DataSet x__data;//下载的文件内容
  }*
);
