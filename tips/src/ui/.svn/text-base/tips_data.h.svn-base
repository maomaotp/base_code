#ifndef TIPS_DATA
#define TIPS_DATA
#include "tips_sqlite.h"
#include <iostream>

using namespace std;

typedef struct data_stut{
	unsigned long long int backid;
	char name[100];
	char time[20];
}data_stut_t;

class CTips_data{
public:
	char* Data_exec_sql();
	int Data_one_init(char **sqltemp, char *name, char *value, char* match);
	int Data_init_sql(data_stut_t *sdata, uint32_t pagenum);
	int Count_init_sql(data_stut_t* sdata, uint32_t pagenum);
	int Count_exec_sql();
private:
	uint32_t data_pagenum;
	char *sql;
};

#endif
