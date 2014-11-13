#include <stdio.h>

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"


lua_State *L;
int luaAdd(int x, int y)
{
	int sum;
	lua_getglobal(L, "add");
	lua_pushnumber(L, x);
	lua_pushnumber(L, y);
	lua_call(L, 2, 1);
	sum = (int)lua_tonumber(L, -1);
	lua_pop(L, 1);
	return sum;
}

int main(int argc, char *argv[])
{
	int sum = 0;
	L = luaL_newstate();
	luaopen_base(L);
	luaL_openlibs(L);
	luaL_dofile(L, "add.lua");
	sum = luaAdd(10, 15);
	printf("The sum is %d\n", sum);
	lua_close(L);

	return 0;
}
