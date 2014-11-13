#ifndef AUDITINFO_H
#define AUDITINFO_H
#define IPOQUE_PROTOCOL_SHORT_STRING "ukn","ftp","pop","smtp","imap","dns","ipp","http","mdns","ntp","netbios","nfs","ssdp",\
	"bgp","snmp","xdmcp","smb","syslog","dhcp","postgres","mysql","tds","ddl","i23v5","apple","directconnect","socrates","winmx",\
	"manolito","pando","filetopia","iMESH","kontiki","openft","fasttrack","gnutella","edonkey","bittorrent","off","avi",\
	"flash","ogg","mpeg","quicktime","realmedia","windowsmedia","mms","xbox","qq","move","rtsp","feidian","icecast","pplive",\
	"ppstream","zattoo","shoutcast","sopcast","tvants","tvuplayer","veohtv","qqlive","thunder","soulseek","gadugadu","irc",\
	"popo","jabber","msn","oscar","yahoo","battlefield","quake","secondlife","steam","hl2","worldofwarcraft","telnet","stun",\
	"ipsec","gre","icmp","igmp","egp","sctp","ospf","ipip","rtp","rdp","vnc","pcanywhere","ssl","ssh","usenet","mgcp","iax",\
	"tftp","afp","stealthnet","aimini","sip","truphone","icmpv6","dhcpv6","armagetron","crossfire","dofus","fiesta","florensia",\
	"guildwars","httpactivesync","kerberos","ldap","maplestory","mssql","pptp","warcraft3","wokf","meebo"

char * prot_short_str[] = {IPOQUE_PROTOCOL_SHORT_STRING};
typedef struct audit_info_t 
{
	uint32_t sip;
	uint32_t dip;
	uint16_t sport;
	uint16_t dport;
	uint32_t protocol;
	uint64_t up_bytes;
	uint64_t down_bytes;
	time_t 	 start_time;
	time_t   end_time;
}audit_info_t;
#endif
