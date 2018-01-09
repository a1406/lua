#include <fcntl.h>
#include "stdbool.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include "stack_test/stack_dump.h"
#include "myapi.h"	

int f1(lua_State *L)
{
	printf("%s\n", __FUNCTION__);
	return 0;
}
int f2(lua_State *L)
{
	printf("%s\n", __FUNCTION__);	
	return 0;
}

typedef int (*lua_CFunction) (lua_State *L);

void init_lua(struct lua_State *L)
{
	luaL_openlibs(L);	  // link lua lib
	lua_pushcfunction(L, f1);
	lua_setglobal(L, "cfunc_f1");
	lua_pushcfunction(L, f2);
	lua_setglobal(L, "cfunc_f2");
	
	luaL_loadfile(L, "test_upvalue.lua");
	lua_pcall(L, 0, 0, 0);
	assert(lua_gettop(L) == 0);						
}

int main(int argc, char *argv[])
{
	struct lua_State *L = luaL_newstate();
	init_lua(L);

    return 0;
}
