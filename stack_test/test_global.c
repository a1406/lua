#include "stack_test/stack_dump.h"

int main(int argc, char *argv[])
{
	struct lua_State *L;	
	L = luaL_newstate();

	luaL_openlibs(L);	  // link lua lib

	lua_getglobal(L, "ttt");

	lua_pushinteger(L, 100);
	lua_setglobal(L, "ttt");

	lua_getglobal(L, "ttt");	
	return (0);
}

