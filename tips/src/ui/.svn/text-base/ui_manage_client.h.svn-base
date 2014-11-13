#ifndef UI_MANAGE_CLIENT_H
#define UI_MANAGE_CLIENT_H
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stddef.h>
#include <string.h>

using namespace std;

class CUI_Manage_cli{
public:
	int cli_connect(const char *name, const char *sname);
	int cli_write(int fd, int cmd);
	int cli_read_string(int fd, void* devicesta, int len);
};

#endif
