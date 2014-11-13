#ifndef TIPS_OPT
#define TIPS_OPT
#include "tips_sqlite.h"
#include <iostream>

using namespace std;

typedef struct opt_stut{
	char *username;
	char *Otime;
	char *Etime;
	char *Optdsc;
	char *Every_pagenum;
}opt_stut_t;

class CTips_opt{
public:
	char* Opt_exec_sql(int how = 0);
	int Opt_one_init(char **sqltemp, char *name, char *value, char* match);
	int Opt_init_sql(opt_stut_t *sopt, uint32_t pagenum,int sort = 1);
	int Opt_init_sql_interface(opt_stut_t *sopt, char *sqlpart,int how = 1);
	int Count_init_sql(opt_stut_t* sopt, uint32_t pagenum);
	int Count_exec_sql(uint32_t *total_number=NULL);
	int Copy_init_sql(opt_stut_t *salarm, uint32_t pagenum);
	int Copy_exec_sql(int all = 0);
	int Count_picture_sql(int *count, const char *search_type);

private:
	uint32_t opt_pagenum;
	char *sql;
};

#endif
