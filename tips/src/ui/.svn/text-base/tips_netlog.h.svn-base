#ifndef TIPS_NETLOG
#define TIPS_NETLOG
#include "tips_sqlite.h"
#include <iostream>

#define IPOQUE_PROTOCOL_SHORT_STRING "ukn","ftp","pop","smtp","imap","dns","ipp","http","mdns","ntp","netbios","nfs","ssdp",\
	"bgp","snmp","xdmcp","smb","syslog","dhcp","postgres","mysql","tds","ddl","i23v5","apple","directconnect","socrates","winmx",\
	"manolito","pando","filetopia","iMESH","kontiki","openft","fasttrack","gnutella","edonkey","bittorrent","off","avi",\
	"flash","ogg","mpeg","quicktime","realmedia","windowsmedia","mms","xbox","qq","move","rtsp","feidian","icecast","pplive",\
	"ppstream","zattoo","shoutcast","sopcast","tvants","tvuplayer","veohtv","qqlive","thunder","soulseek","gadugadu","irc",\
	"popo","jabber","msn","oscar","yahoo","battlefield","quake","secondlife","steam","hl2","worldofwarcraft","telnet","stun",\
	"ipsec","gre","icmp","igmp","egp","sctp","ospf","ipip","rtp","rdp","vnc","pcanywhere","ssl","ssh","usenet","mgcp","iax",\
	"tftp","afp","stealthnet","aimini","sip","truphone","icmpv6","dhcpv6","armagetron","crossfire","dofus","fiesta","florensia",\
	"guildwars","httpactivesync","kerberos","ldap","maplestory","mssql","pptp","warcraft3","wokf","meebo"

using namespace std;

typedef struct netlog_stut{
	char *Dip;			/*内部主机IP*/
	char *Dport;			/*内部主机端口*/
	char *Sip;			/*外部主机ip*/
	char *Sport;			/*外部主机端口*/
	char *Protocol;			/*协议类型*/
	char *Start_time;		/*开始时间*/
	char *End_time;			/*结束时间*/
	char *Load;			/*下载数据流量流入*/
	char *Upload;			/*上传数据流量露出*/
	char *Every_pagenum;
}netlog_stut_t;

class CTips_netlog{
public:
	int Netlog_init_sql_interface(netlog_stut_t *snetlog, char *sqlpart,int how = 1);
	char* Netlog_exec_sql(int all = 0);
	int Netlog_one_init(char **sqltemp, char *name, char *value, char* match);
	int Netlog_init_sql(netlog_stut_t *snetlog, uint32_t pagenum,int sort = 1);
	int Copy_init_sql(netlog_stut_t *snetlog, uint32_t pagenum);
	int Copy_exec_sql(int all = 0);
	int Delete_init_sql(netlog_stut_t *snetlog, uint32_t pagenum);
	int Delete_exec_sql(int all = 0);
	int Count_init_sql(netlog_stut_t* snetlog, uint32_t pagenum);
	int Count_exec_sql(uint32_t *total_number = NULL);
	int Count_picture_sql(int *count, const char *search_type);
private:
	uint32_t netlog_pagenum;
	char *sql;
};

#endif
