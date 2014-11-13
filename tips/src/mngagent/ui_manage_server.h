#ifndef UI_MANAGE_SERVE_H
#define UI_MANAGE_SERVE_H
#include <iostream>
using namespace std;

class CUI_Manage_ser{
public:
	int serv_listen(const char *name);
	int serv_accept(int listenfd);
	int serv_read(int accfd);
	char* serv_read_string(int fd);
	int serv_write_string(int fd, void *des, int len);
};


#endif
