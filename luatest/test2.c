#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include "stack_test/stack_dump.h"
#include "myapi.h"	

void init_lua(struct lua_State *L)
{
	print_global(L);

	lua_pushinteger(L, 100);
	lua_setglobal(L, "int1");
	lua_pushinteger(L, 200);
	lua_setglobal(L, "int2");
	lua_pushstring(L, "str100");
	lua_setglobal(L, "str1");
	printf("after setglobal-----------------\n\n");	
	print_global(L);
	
	luaL_openlibs(L);	  // link lua lib
	printf("after openlibs-----------------\n\n");
	print_global(L);	
	assert(lua_gettop(L) == 0);						
}


int main(int argc, char *argv[])
{
	struct lua_State *L = luaL_newstate();
	init_lua(L);
    return 0;
}
