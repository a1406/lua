#include <stdio.h>
#include <assert.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

int main(int argc, char *argv[])
{
	struct lua_State *L;	
	
	(void)(argc);
	(void)(argv);
	L = luaL_newstate();

	luaL_openlibs(L);	  // link lua lib

	lua_getglobal(L, "ttt");

	lua_pushinteger(L, 100);
	lua_setglobal(L, "ttt");

	lua_getglobal(L, "ttt");	
	return (0);
}

