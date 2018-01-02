#include "stack_test/stack_dump.h"

static const char * load_config = "\
	local config_name = ...\
	local f = assert(io.open(config_name))\
	local code = assert(f:read \'*a\')\
	local function getenv(name) return assert(os.getenv(name), \'os.getenv() failed: \' .. name) end\
	code = string.gsub(code, \'%$([%w_%d]+)\', getenv)\
	f:close()\
	local result = {}\
	assert(load(code,\'=(load)\',\'t\',result))()\
	return result\
";

int testfunc1(lua_State *L, char *lua_file_name)
{
	if (1 == luaL_dofile(L, lua_file_name))
	{
		printf("do file failed, file_name:%s\n", lua_file_name);
		return -1;
	}

	stack_dump(L, "load config lua");
	
	lua_pushnil(L);
	    // 现在的栈：-1 => nil; index => table
//	int index = -2;
	while (lua_next(L, -2))
	{
			// 现在的栈：-1 => value; -2 => key; index => table
			// 拷贝一份 key 到栈顶，然后对它做 lua_tostring 就不会改变原始的 key 值了
		lua_pushvalue(L, -2);
			// 现在的栈：-1 => key; -2 => value; -3 => key; index => table

	}
	return (0);
}

int main(int argc, char *argv[])
{
	char *config_file = "config.lua";
	struct lua_State *L = luaL_newstate();

	luaL_openlibs(L);	  // link lua lib

//	testfunc1(L, "EscortTask.lua");

	luaL_loadfile(L, config_file);
	stack_dump(L, "load config file");
	lua_rawsetp(L, LUA_REGISTRYINDEX, config_file);
	stack_dump(L, "rawsetp");	
	
	int err = luaL_loadstring(L, load_config);
	assert(err == LUA_OK);
	stack_dump(L, "load config string");
	
	lua_pushstring(L, config_file);

	/* stack_dump(L, "push 'config.lua'"); */

	/* lua_setglobal(L, "g1"); */
	/* stack_dump(L, "set 'g1'"); */

	/* lua_getglobal(L, "g1"); */
	/* stack_dump(L, "get 'g1'"); */

	/* lua_getglobal(L, "g2"); */
	/* stack_dump(L, "get 'g2'");	 */

	err = lua_pcall(L, 1, 1, 0);
	if (err) {
		fprintf(stderr,"%s\n",lua_tostring(L,-1));
		lua_close(L);
		return 1;
	}

	lua_close(L);
	return 0;
}


