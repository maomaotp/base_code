#ifndef CACHE_STRUCT_H
#define CACHE_STRUCT_H
#define ALARM		0
#define AUDIT		1
#define OPT		2
#define SECRET		3
#define BLOCK		4
#define ALERT		5

#define INIT_ALARM_HEAD(alarm_ptr)				\
{								\
	(alarm_ptr)->head.type = ALARM;				\
	(alarm_ptr)->head.size = sizeof(cache_head_t);		\
}					

#define INIT_AUDIT_HEAD(audit_ptr)				\
{								\
	(audit_ptr)->head.type = AUDIT;				\
	(audit_ptr)->head.size = sizeof(cache_audit_t);		\
}

#define INIT_OPT_HEAD(opt_ptr) 					\
{								\
	(opt_ptr)->head.type = OPT;				\
	(opt_ptr)->head.size = sizeof(cache_opt_t);		\
}					

#define INIT_SECRET_HEAD(secret_ptr)				\
{								\
	(secret_ptr)->head.type = SECRET;			\
	(secret_ptr)->head.size = sizeof(cache_secret_t);	\
}					
					
#define INIT_BLOCK_HEAD(block_ptr) 				\
{								\
	(block_ptr)->head.type = BLOCK;				\
	(block_ptr)->head.size = sizeof(cache_block_t);		\
}		

#define INIT_ALERT_HEAD(alert_ptr)				\
{								\
	(alert_ptr)->head.type = ALERT;				\
	(alert_ptr)->head.size = sizeof(cache_alert_t);		\
}

typedef struct cache_head_t
{
	int type;
	int size;
}cache_head_t;

typedef struct cache_alarm_t
{
	cache_head_t head;
	char alarm_id[12];
	char device_id[12];
	char trojan_id[6];
	char sip[19];
	char dip[19];
	char dmac[20];
	time_t alarm_time;
	char type[3];
	char risk;
	char scc[65];
	char scc_flag;
	char descry[65];
}cache_alarm_t;

typedef struct cache_audit_t
{
	cache_head_t head;
	uint32_t sip;
	uint32_t dip;
	uint16_t sport;
	uint16_t dport;
	uint32_t protocol;
	uint32_t below_protocol;
	float	 up_KB;
	float	 down_KB;
	time_t 	 start_time;
	time_t   end_time;
}cache_audit_t;

typedef struct cache_opt_t
{
	cache_head_t head;
	char username[64];
	char opttime[24];
	char describe[128];
}cache_opt_t;

typedef struct cache_secret_t
{
	cache_head_t head;
}cache_secret_t;

typedef struct cache_block_t
{
	cache_head_t head;
}cache_block_t;

typedef struct cache_alert_t
{
	cache_head_t head;
}cache_alert_t;

#endif
