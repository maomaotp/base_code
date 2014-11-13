#include <stdio.h>
#include <sqlite3.h>

int callback(void *p,int argc, char **argv, char **argvv)
{
	printf("%s\n",argv[0]);
	return 0;

}

int main(void)
{
	sqlite3 *db;
	char *err;
	int ret = sqlite3_open("./tips.db",&db);
	if(ret < 0)
		printf("ss:\n");
	//ret = sqlite3_exec(db, "insert into tips_Event(Event_class,Event_type,Event_time,Sip,Sport,Dip,Dport,Dmac,Sender,Receiver,Title,File_name,Descr,Sen_msg,Event_iid,Device_id) values('1011','1011','33','44','55','66','77','88','99','111','222','333','444','555','666','777')",NULL,NULL,&err);
	//ret = sqlite3_exec(db, "insert into tips_netlog(Dip,Dport,Sip,Sport,Protocol,Start_time,End_time,Load,Upload) values('1011','1011','33','44','55','66','77','88','99')",NULL,NULL,&err);
	
	ret = sqlite3_exec(db, "select tips_netlog.Dip,tips_netlog.Dport,tips_netlog.Sip,tips_netlog.Sport,tips_netlog.Protocol,tips_netlog.Start_time,tips_netlog.End_time,tips_netlog.Load,tips_netlog.Upload from tips_netlog,tips_back where tips_back.BackID=tips_netlog.BackID and tips_back.BackID='33'",callback,NULL,&err);
	if(ret < 0){
		printf("ss:%s\n",err);
	}
	return 0;
}
