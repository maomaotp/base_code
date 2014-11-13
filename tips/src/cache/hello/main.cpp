#include "sqlite3.h"
#include <string.h>
#include <iostream>
using namespace std;
int main()
{
	sqlite3* db;
	int rc=sqlite3_open("/root/lirongxun/manage_20130324/tips.db",&db);
	if(rc)
	{
		cout<<"openfaild"<<endl;
	}
	rc=sqlite3_key(db,"admin",5);
	if(rc)cout<<"key error!"<<endl;
	char* err_msg;
	char sql[2048];
	strcpy(sql,"insert into tips_netlog(Dip, Dport, Sip, Sport,Protocol, Start_time,End_time,Load, Upload,flag) values('124.228.254.111','6001','192.168.1.12','4718','7','2013-10-25 03:21:30','2013-10-25 03:21:30','0','1072330752','0');");
	rc=sqlite3_exec(db,sql,0,0,&err_msg);
	if(SQLITE_OK!=rc)
	{
		cout<<"失败!"<<endl;
	}
	else 
	{
		cout<<"成功!"<<endl;
	}
}
