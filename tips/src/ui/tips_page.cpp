#include "tips_page.h"
#include "gsad_base.h"

CTips_Page::CTips_Page()
:pagenum(NULL),url(NULL),urlpart(NULL)
{

}

char* CTips_Page::init_pagenum(uint32_t nowpage, uint32_t lastpage,uint32_t total_number)
{
	//int numfirst;
	if(0 == nowpage)
		nowpage = 1;
	//numfirst = nowpage - ((nowpage-1) % 10)-1;
	char page_order[100];
	memset(page_order,0,sizeof(page_order));
	sprintf(page_order,"第%d%c%d页 ",nowpage,'/',lastpage);
	//pagenum = g_strdup_printf("<nowpage>%d</nowpage><lastpage>%d</lastpage><page_order>%s</page_order>",nowpage,lastpage,page_order);
	pagenum = g_strdup_printf("<nowpage>%d</nowpage><lastpage>%d</lastpage><total_number>%d</total_number><page_order>%s</page_order>",nowpage,lastpage,total_number,page_order);
	return pagenum;
}

char* CTips_Page::init_url(uint32_t nowpage, uint32_t lastpage, int sort,char *every_pagenum)
{
	//int numfirst;
	//numfirst = nowpage - ((nowpage-1) % 10)-1;//tips_urlf显示的是首页的内容,sort为:0
	if(every_pagenum == NULL)
		url = g_strdup_printf("<tips_urlf>%s&amp;pagenum=%u&amp;tips_sort=%d&amp;every_pagenum=%s</tips_urlf><tips_urll>%s&amp;pagenum=%u&amp;tips_sort=%d&amp;every_pagenum=%s</tips_urll><tips_urlp>%s&amp;pagenum=%u&amp;tips_sort=%d&amp;every_pagenum=%s</tips_urlp><tips_urln>%s&amp;pagenum=%u&amp;tips_sort=%d&amp;every_pagenum=%s</tips_urln>", urlpart, 1, sort,"50", urlpart, lastpage, sort,"50", urlpart, nowpage-1, sort,"50", urlpart, nowpage+1, sort,"50");
	else
		url = g_strdup_printf("<tips_urlf>%s&amp;pagenum=%u&amp;tips_sort=%d&amp;every_pagenum=%s</tips_urlf><tips_urll>%s&amp;pagenum=%u&amp;tips_sort=%d&amp;every_pagenum=%s</tips_urll><tips_urlp>%s&amp;pagenum=%u&amp;tips_sort=%d&amp;every_pagenum=%s</tips_urlp><tips_urln>%s&amp;pagenum=%u&amp;tips_sort=%d&amp;every_pagenum=%s</tips_urln>", urlpart, 1, sort,every_pagenum, urlpart, lastpage, sort,every_pagenum, urlpart, nowpage-1, sort,every_pagenum, urlpart, nowpage+1, sort,every_pagenum);
	//url = g_strdup_printf("<tips_urlf>%s&amp;pagenum=%u&amp;tips_sort=%d</tips_urlf><tips_urll>%s&amp;pagenum=%u&amp;tips_sort=%d</tips_urll><tips_urlp>%s&amp;pagenum=%u&amp;tips_sort=%d</tips_urlp><tips_urln>%s&amp;pagenum=%u&amp;tips_sort=%d</tips_urln>", urlpart, 1, sort, urlpart, lastpage, sort, urlpart, nowpage-1, sort, urlpart, nowpage+1, sort);
	//url = g_strdup_printf("<tips_urlf>pagenum=%u&amp;tips_sort=%d&amp;%s&amp;</tips_urlf><tips_urll>pagenum=%u&amp;tips_sort=%d&amp;%s&amp;</tips_urll><tips_urlp>pagenum=%u&amp;tips_sort=%d&amp;%s&amp;</tips_urlp><tips_urln>pagenum=%u&amp;tips_sort=%d&amp;%s&amp;</tips_urln>", 1, sort, urlpart, lastpage, sort, urlpart, nowpage-1, sort, urlpart, nowpage+1, sort,urlpart);
	return url;//tips_urll显示的为尾页内容
}

int CTips_Page::Page_free()
{
	if(NULL != pagenum)
		g_free(pagenum);
	if(NULL != url)
		g_free(url);
	if(NULL != pagenum)
		g_free(urlpart);
	return 0;
}

int CTips_Page::init_urlpart_alarm(alarm_stut_t *value, char *token, char *interval)
{
	urlpart = g_strdup_printf("/omp?token=%s&amp;cmd=tips_alarm_display&amp;tips_Trogan_id=%s&amp;tips_Sip=%s&amp;tips_Dip=%s&amp;tips_Dmac=%s&amp;tips_Alarm_time=%s&amp;tips_endtime=%s&amp;tips_Type=%s&amp;tips_Risk=%s&amp;tips_Scc_flag=%s&amp;refresh_interval=%s", token, value->Trojan_id, value->Sip, value->Dip, value->Dmac, value->Alarm_time,value->Alarm_end_time,value->Type, value->Risk, value->Scc_flag, interval);
	return 0;
}

int CTips_Page::init_urlpart_event(event_stut_t *value, char *token, char *interval)
{
	urlpart = g_strdup_printf("/omp?token=%s&amp;cmd=tips_event_display&amp;tips_Event_class=%s&amp;tips_Event_time=%s&amp;tips_Sip=%s&amp;tips_Sport=%s&amp;tips_Dip=%s&amp;tips_Sender=%s&amp;tips_Receiver=%s&amp;tips_Sen_msg=%s&amp;refresh_interval=%s", token, value->Event_class, value->Event_time, value->Sip, value->Sport, value->Dip, value->Sender, value->Receiver, value->Sen_msg, interval);
	return 0;
}

int CTips_Page::init_urlpart_netlog(netlog_stut_t *value, char *token, char *interval)
{
	//urlpart = g_strdup_printf("/omp?token=%s&amp;cmd=tips_netlog_display&amp;tips_Dip=%s&amp;tips_Sip=%s&amp;tips_Sport=%s&amp;tips_Protocol=%s&amp;tips_Start_time=%s&amp;tips_End_time=%s&amp;tips_Load=%s&amp;tips_Upload=%s&amp;refresh_interval=%s&amp;every_pagenum=%s", token, value->Dip, value->Sip, value->Sport, value->Protocol, value->Start_time, value->End_time, value->Load,value->Upload,interval,value->Every_pagenum);
	urlpart = g_strdup_printf("/omp?token=%s&amp;cmd=tips_netlog_display&amp;tips_Dip=%s&amp;tips_Sip=%s&amp;tips_Sport=%s&amp;tips_Dport=%s&amp;tips_Protocol=%s&amp;tips_Start_time=%s&amp;tips_End_time=%s&amp;tips_Load=%s&amp;tips_Upload=%s&amp;refresh_interval=%s", token, value->Dip, value->Sip, value->Sport, value->Dport, value->Protocol, value->Start_time, value->End_time, value->Load,value->Upload,interval);
	return 0;
}

int CTips_Page::init_urlpart_opt(opt_stut_t *value, char *token)
{
	urlpart = g_strdup_printf("/omp?token=%s&amp;cmd=tips_opt_display&amp;tips_username=%s&amp;tips_Optdsc=%s&amp;tips_Otime=%s&amp;tips_Oetime=%s", token, value->username, value->Optdsc, value->Otime, value->Etime);
	return 0;
}

int CTips_Page::init_urlpart_data(data_stut_t *value, char *token)
{
	urlpart = g_strdup_printf("/omp?token=%s&amp;cmd=data_display&amp;bakname=%s&amp;baktime=%s", token,value->name,value->time);
	return 0;
}
