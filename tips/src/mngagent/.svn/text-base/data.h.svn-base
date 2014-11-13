#ifndef DATA_H
#define DATA_H
#include<iostream>
#include<sys/types.h>
#include <stdint.h>
#define TopLimit 4294967295
#define NUM_FILE (char *)"./ID.txt"
#define PIDFILE (char *)"/etc/tipscheck/pid"
#define DEVKEY (char *)"DEVICE_ID"
#define BUFFSIZE 256
#define DEVIDNUM 11
#define DARA_DIR "/var/tips_net_guard"

struct config_struct{
	char device_id[12];
	char device_locate[7];
	char device_made[3];
	char device_type[3];
	char ip[17];
	char mac[20];
	char mask[17];
	char gateway[17];
	char manageip[16];
	char managemac[20];
	char managemask[17];
	char managegateway[17];
	char center_ip[17];
	char center_ip1[17];
	char version[17];
	char dns_version[17];
	char ip_version[17];
	char swip_version[17];
	char geoip_version[17];
	char sen_version[17];
	char spe_version[17];
	char mail_version[17];
	char own[17];
};
struct status_struct{
char device_id[11];
char stime[21];
int mem;
int cpu;
double disk;
char mirror;//镜像口状态1是开启，0是关闭
int flow;//镜像口流量
int active_peer;
char module_state[9];
};//设备状态信息
struct check_status{
char mirror;
int flow;
int active_peer;
int module_state;//待定
};
struct dev_struct{
char device_id[12];
char device_locate[7];
char device_made[3];
};//设备配置信息

struct alarm{
char alarm_id[12];
char device_id[12];
char trojan_id[6];
char sip[19];
char dip[19];
char dmac[20];
char alarm_time[25];
char type[3];
char risk;
char scc[65];
char scc_flag;
char descry[65];
}; 
struct alert{
char alert_id[12];
char device_id[12];
uint32_t plugin_id;
char risk;
char trojan_id[6];
char alarm_time[25];
char sip[19];
uint32_t sport;
char dip[19];
uint32_t dport;
char dmac[20];
uint32_t protocol; 
char descry[65];
char dnshash[33];
char scc[65];
char scc_flag;
char extra_text[65];
};
struct senstive_alert{
char event_id[12];
char device_id[12];
uint32_t event_class;
char event_type[3];
char event_time[25];
char sip[19];
uint32_t sport;
char dip[19];
uint32_t dport;
char dmac[20];
char sender[65];
char receiver[257];
char title[129];
char filename[257];
char descr[129];
char sen_msg[129];
};
struct session_blocked{
char device_id[12];
char dip[19];
char sip[19];
uint32_t sport;
char start_time[25];
char expired_time[25];
char block_desc[33];
};
//add by lipeiting 2014-6-18
typedef struct config_manage
{
    char manage_IP[50];     /*管理口IP*/
    char manage_IP_MAC[50];     /*管理口IP的MAC地址*/
    char manage_mask[50];       /*管理口IP的子网掩码*/
    char manage_gateway[50];    /*管理口网关*/
    char manage_PORT[50];       /*管理中心PORT*/
    char manage_PORT1[50];      /*管理中心PORT*/
    char net_IP[50];            /*通信网口IP*/
    char gateway[50];           /*通信口网关*/
    char mac[50];               /*通信口MAC地址*/
    char mask[50];              /*通信口子网掩码*/
    char static_route[50];      /*静态路由*/
    char supervisor[50];        /*管理机IP*/
    char serve_addr[50];        /*代理服务器地址*/
    char serve_user[50];        /*代理服务器地址*/
    char serve_password[50];        /*代理服务器口令*/
    char adminstration_IP[50];      /*管理中心IP*/
    char adminstration_IP1[50];     /*管理中心IP*/
    char unit_name[256];        /*部属单位名称*/
    int modify_flag;
}config_manage_t;
#endif
