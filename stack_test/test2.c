#include "stack_test/stack_dump.h"
#include <stdlib.h>

extern void ldb_loop();
extern void luaLdbLineHook(lua_State *lua, lua_Debug *ar);

struct lua_State *L;
int ldb_exit = 0;

int main(int argc, char *argv[])
{
	int ret;
	L = luaL_newstate();

	luaL_openlibs(L);	  // link lua lib

	if (luaL_dofile(L, "ttt.lua") != LUA_OK)
	{
		printf("err1\n");
		exit(0);
	}
	if (luaL_dofile(L, "t1.lua") != LUA_OK)
	{
		printf("err2\n");
		exit(0);
	}

	ret = lua_getglobal(L, "recv_func");
	if (ret != LUA_TFUNCTION)
	{
		printf("err4 ret = % d\n", ret);
		exit(0);
	}

	lua_rawsetp(L, LUA_REGISTRYINDEX, &L);

	lua_rawgetp(L, LUA_REGISTRYINDEX, &L);
	lua_pushinteger(L, 2);
	lua_pcall(L, 1, 0, 0);

	lua_rawgetp(L, LUA_REGISTRYINDEX, &L);
	lua_pushinteger(L, 3);
	lua_pcall(L, 1, 0, 0);

	lua_rawgetp(L, LUA_REGISTRYINDEX, &L);
	lua_pushinteger(L, 4);
	lua_pcall(L, 1, 0, 0);


	if (luaL_dofile(L, "t2.lua") != LUA_OK)
	{
		printf("err2 t2.lua\n");
		exit(0);
	}
	
	lua_rawgetp(L, LUA_REGISTRYINDEX, &L);
	lua_pushinteger(L, 2);
	if (lua_pcall(L, 1, 0, 0) != LUA_OK)
	{
		luaL_checktype(L, -1, LUA_TSTRING);
		printf("pcall fail, err = %s\n", lua_tostring(L, -1));
		lua_pop(L, 1);
	}

	lua_rawgetp(L, LUA_REGISTRYINDEX, &L);
	lua_pushinteger(L, 3);
	if (lua_pcall(L, 1, 0, 0) != LUA_OK)
	{
		luaL_checktype(L, -1, LUA_TSTRING);
		printf("pcall fail, err = %s\n", lua_tostring(L, -1));
		lua_pop(L, 1);
	}

	lua_rawgetp(L, LUA_REGISTRYINDEX, &L);
	lua_pushinteger(L, 4);
	if (lua_pcall(L, 1, 0, 0) != LUA_OK)
	{
		luaL_checktype(L, -1, LUA_TSTRING);
		printf("pcall fail, err = %s\n", lua_tostring(L, -1));
		lua_pop(L, 1);
	}

	lua_sethook(L, luaLdbLineHook, LUA_MASKLINE, 100000);
	lua_rawgetp(L, LUA_REGISTRYINDEX, &L);	
	lua_pushinteger(L, 3);
	lua_pcall(L, 1, 0, 0);

	for (;!ldb_exit;)
	{
		ldb_loop();
		if (lua_gettop(L) != 0)
			stack_dump(L, "finish loop");
	}

	if (lua_gettop(L) != 0)
	{
		printf("stack no empty\n");
		stack_dump(L, "all end");
	}
	lua_close(L);	
    return 0;
}
