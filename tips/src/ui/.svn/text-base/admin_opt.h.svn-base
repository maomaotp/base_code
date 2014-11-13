#ifndef ADMIN_OPT
#define ADMIN_OPT
#include "gsad_base.h"
#include "tips_block.h"
#include "tips_event.h"
#include "tips_netlog.h"
#include "tips_opt.h"
#include "tips_alarm.h"
#include "tips_alert.h"
#include "tips_data.h"
#include "tips_xml_part.h"
#include "tips_page.h"
#include "tipsmanager.h"
#include "GetNicInfo.h"

typedef char *admin_opt_fun(credentials_t* credentials, char *value);

#define CONFIG_FILE "/usr/local/tipschecker/etc/tipscheck/config"
#define PRODUCT_FILE "/usr/local/tipschecker/etc/tipscheck/product_state"
#define SYSTEM_CONFIG_FILE "/usr/local/tipschecker/etc/tipscheck/system_config"
#define CONFIG_MAXLINE 4098
extern CUser_sqlite_handle tips_sqlite_handle;
extern GMutex *filemutex;

struct slave_status
{
   char managerPortIp[32];
   char managerPort;
   char ConnectPortIp[32];
   char connectPortStatus;
   char mirrorPortStatus;
   unsigned int datasize;
   unsigned int connectNumer;
};

typedef struct product_upgrade_info{
	char name[30];
	char version[30];
	char describe[30];
	char upgrade_time[30];
}product_INFO;

typedef struct product__infostate{
	char name[50];
	char mode[30];
	char code[30];
	char version[15];
	char trojan_newtime[30];
	char policy_newtime[30];
	char apply_time[30];
	char end_time[30];
	char authorization_type[15];
}product_INFOSTATE;

class CAdmin_opt{
public:
	CAdmin_opt();
	CAdmin_opt(credentials_t *scredentials, char *scmd, params_t* sparams, void* sopt_fun);
	int init_opt(credentials_t *scredentials, char *scmd, params_t* sparams, void* sopt_fun);
	char* cmd_opt_get();
	char* cmd_opt_post();
	//char* cmd_to_display(char *cmd);
	char* display_user_html(const char *errmsg, char *role);
	int get_config_manage_xml(void *config);
	int get_system_config_xml(void *config);
	int init_one_config(const char *source, const char *descript, FILE *fp);
	char* config_manage_modify(config_manage_t* mconfig, credentials_t* credentials,int type);
	char* system_config_modify(system_config_t* mconfig);
	void TurnTime(const time_t clock, char *tips_clock, int clocksize);
	int Getpid(pid_t *pid);
	int Change_ip(char *value, char *ethpath);
	int change_route(char *route_ip, char *gateway_ip);
	int Change_gateway(char *value, char *ethpath);
	int is_netline(char *eth);
	int str_isdight(char *str);
	int str_isalpha(char *str);
	int config_get_device(status_struct_t *devicesta);
	void MyTurnTime(const time_t clock, char *tips_clock, int clocksize);
	time_t MyObtainTime(char* date);
	int caculate_time(char date_time[][11]);
	int judge_time(const char *start_time,const char *end_time);
	int Upgrade(product_INFO *upgrade_info=NULL,int type=0);
	int get_product_state(product_INFOSTATE *upgrade_infostate);
private:
	credentials_t* credentials;
	char *cmd;
	params_t *params;
	void* opt_fun;
};

#endif
