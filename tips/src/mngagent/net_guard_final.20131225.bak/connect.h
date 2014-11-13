#import "soap12.h"
#import "xop.h"
#import "xmime5.h"

//gsoap ns service name:		connect
//gsoap ns service namespace:	http://www.genivia.com/wsdl/connect.wsdl

//gsoap x schema namespace:	http://www.genivia.com/schemas/connect.xsd
//gsoap x schema elementForm:	unqualified

//gsoap x schema type-documentation: Data holds a MIME attachment
struct x__Data
{ _xop__Include xop__Include;
  @char *xmime5__contentType;
};

//gsoap x schema type-documentation: DataSet holds a set of MIME attachments
struct x__DataSet
{ int __size;
  struct x__Data *item;
};

//gsoap x schema type-documentation: Keys holds a set of strings to access the data collection at the server side
struct x__Keys
{ int __size;
  char **key;
};

struct ns__MsgItem
{
	char *type;//消息类型
	char *msg;//消息体
};
struct ns__MsgList
{
	struct ns__MsgItem *__ptr;//消息链
	int __size;//消息个数
};
struct ns__ResultItem
{
	char *TimeStamp;//命令时间戳
	char *Order;//命令
	char *Param;//命令参数
};
struct ns__ResultList
{
	struct ns__ResultItem *__ptr;//管控命令链
	int __size;
};

int ns2__upload_message( char *Device_ID,bool buer,struct ns__MsgList *state,char *t_auth_str,struct ns__ResultList *resultlist );

//gsoap ns service method-mime-type: PutData */*
//gsoap ns service method-documentation: PutData stores a data set at the server side and returns a set of unique keys to access that data
int ns__PutData(
  struct x__DataSet *x__data,
  struct x__Keys *x__Keys,
  char *device_id,
  char *authstr,
  int* response
);

//gsoap ns service method-mime-type: GetData */*
//gsoap ns service method-documentation: GetData gets a set of data items from the server given a set of keys
int ns__GetData(
  struct x__Keys *x__keys,
  char *device_id,
  char *authstr,
  struct ns__GetDataResponse
  { struct x__DataSet x__data;
  }*
);
