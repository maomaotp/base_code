#ifndef TIPS_TIME_H
#define TIPS_TIME_H
#include <time.h>
#include <sys/types.h>
#include <regex.h>
#include <iostream>
#include <cstdio>
#include <stdlib.h>

using namespace std;

class CTips_time{
public:
	time_t CatchData(char *seetime);
	int CatchSeeTime(time_t clock, char *clock_str);
private:
	regmatch_t pmatch[20];
};

#endif
