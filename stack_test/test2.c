#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

extern void ldb_loop();
extern void luaLdbLineHook(lua_State *lua, lua_Debug *ar);

void stack_dump(lua_State* L, char *head)
{
    int i;
    int top = lua_gettop(L);
    printf("stackDump(num=%d) %s:\n", top, head);
    
    for (i = 1; i <= top; i++) {  /* repeat for each level */
        int t = lua_type(L, i);
        switch (t) {
                
            case LUA_TSTRING:  /* strings */
                printf("'%s'", lua_tostring(L, i));
                break;
                
            case LUA_TBOOLEAN:  /* booleans */
                printf(lua_toboolean(L, i) ? "true" : "false");
                break;
                
            case LUA_TNUMBER:  /* numbers */
                printf("%g", lua_tonumber(L, i));
                break;
            default:  /* other values */
                printf("%s", lua_typename(L, t));
                break;
                
        }
        printf("  ");  /* put a separator */
    }
    printf("\n============\n");     /* end the listing */
    
}

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
