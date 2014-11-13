#ifndef TIPS_BLOCK
#define TIPS_BLOCK
#include "tips_sqlite.h"
#include <iostream>

using namespace std;

typedef struct block_stut{
	char *dip;
	char *sip;
	char *Sport;
	char *Start_time;
	char *Expired_time;
	char *Block_desc;

}block_stut_t;

class CTips_block{
public:
	char* Block_exec_sql();
	int Block_one_init(char **sqltemp, char *name, char *value, char* match);
	int Block_init_sql(block_stut_t *sblock);
private:
	char *sql;
};

#endif
