#ifndef _HEAD_SDSD_H
#define _HEAD_SDS_H

struct sdshdr {
	int len;
	int free;
	char buf[];
}

#endif
