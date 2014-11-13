#ifndef _UNIXDOMAIN_H__
#define _UNIXDOMAIN_H__
#include "data.h"
#include "checkobj.h"
#include "cachestruct.h"
#include "client++.h"
//#include "./IPBlack/ipblackchecker.h"
#include "dnschecker.h"

#define UNIX_DOMAIN "/tmp/domain.alarm"

#define LoadTabCnt 20 		/*定义loadtab时用到的数组个数*/
#define TrueLoadTabCnt 3	/*实际使用loadtab时用到的数组个数*/
/*与manager协定的策略标记*/
#define   MPro 		  	'L'   
#define  DNS_NO 	   	 1 	/*DNS策略*/
#define  IPBlack_NO    2 	/*IP黑\白名单*/
#define  Comptro_NO    3 	/*审计白名单*/
#define  Arcanum_NO    4 	/*涉密标识*/
#define  Trojan_NO 	   5 	/*木马策略*/
#define  Location_NO   6	/*地理信息*/
#define  Mail_NO 	   	 7	/*邮件策略*/

/*与manager协定的管控命令标记*/
#define  MCmd 		  	'C'   
#define CmdParCom_NO   11   /*审计模块中的netshark命令*/
#define CmdNoCom_NO    12   /*审计模块中的stopnetshark命令*/
#define CmdParBlock_NO 13	/*阻断模块中的netblock命令*/
#define CmdNoBlock_NO  14	/*阻断模块中的stopnetblock命令*/

struct rules_fsm{
		unsigned char pro;/*'L'*/
		int module_no;/*模块标号*/
		/*FIXME c++类检查项,用于调用其Load_Rules加载各自的策略 如：dns\ipblack ...*/
		ITipsChecker * obj;
		/*FIXME c函数检查项,用于调用其Load_Rules加载各自的策略 如：木马\审计 ...*/
		int (*rules_action)(char *rules,int len);
		/*用于插入opt表的操作描述*/
		char *  opt_dsc;
};

extern CCacheclient cacheclient;
extern ITipsChecker* checker[CLASSCOUNT];

class UnixDomain
{
		public:
				int manager_domain,data_domain; //用于建立与manager通信的socket 和 接收管控命令、策略的socket
				bool Init_UnixDomain();
				void InitFsm();
				bool Start(pthread_t *id);
};
#endif
