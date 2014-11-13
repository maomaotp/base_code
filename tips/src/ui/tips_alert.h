#ifndef TIPS_ALERT
#define TIPS_ALERT
#include "tips_sqlite.h"
#include <iostream>

using namespace std;

typedef struct alert_stut{
	char *plugin_id;
	char *Risk;
	char *Alarm_time;
	char *Sip;
	char *Dip;
	char *Dmac;
	char *Scc_flag;

}alert_stut_t;

class CTips_alert{
public:
	char* Alert_exec_sql();
	int Alert_one_init(char **sqltemp, char *name, char *value, char* match);
	int Alert_init_sql(alert_stut_t *salert);
private:
	char *sql;
};

#endif
