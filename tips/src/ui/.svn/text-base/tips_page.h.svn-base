#ifndef TIPS_PAGE_H
#define TIPS_PAGE_H

#include <sys/types.h>
#include "tips_alarm.h"
#include "tips_event.h"
#include "tips_netlog.h"
#include "tips_opt.h"
#include "tips_data.h"

class CTips_Page{
public:
	CTips_Page();
	char* init_pagenum(uint32_t nowpage, uint32_t lastpage,uint32_t total_number=0);
	char* init_url(uint32_t nowpage, uint32_t lastpage, int sort = 0,char *every_pagenum=NULL);
	int init_urlpart_alarm(alarm_stut_t *value, char *token, char* interval);
	int init_urlpart_event(event_stut_t *value, char *token, char* interval);
	int init_urlpart_netlog(netlog_stut_t *value, char *token, char* interval);
	int init_urlpart_opt(opt_stut_t *value, char *token);
	int init_urlpart_data(data_stut_t *value, char *token);
	int Page_free();
private:
	char *pagenum;
	char *url;
	char *urlpart;
};

#endif
