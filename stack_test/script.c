#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#define LDB_BREAKPOINTS_MAX 10
struct ldb_break
{
	int linenum;
	char *filename;
};

struct ldb
{
	struct ldb_break bp[LDB_BREAKPOINTS_MAX]; /* An array of breakpoints line numbers. */
	int bpcount; /* Number of valid entries inside bp. */
	int step;   /* Stop at next line ragardless of breakpoints. */	
	int luabp;  /* Stop at next line because redis.breakpoint() was called. */
	char *current_file;
	int current_line;
};

struct ldb ldb;

int ldb_is_break(int line, const char *filename)
{
	for (int i = 0; i < ldb.bpcount; ++i)
	{
		if (line == ldb.bp[i].linenum && strcmp(filename, ldb.bp[i].filename) == 0)
			return (1);
	}
	return (0);
}

void ldb_loop(struct lua_State *L)
{
	static char ldb_buf[1024];
	char command, param1[64];
	while (fgets(ldb_buf, 1024, stdin))
	{	
		int n = sscanf(ldb_buf, "%c %s", command, param1);
		if (n <= 0)
			continue;
		switch (command)
		{
			case 'b':
			{
			}
			break;
			case 't':
			{
				if (n != 2)
				{
					printf("no lua file\n");
					break;
				}
				lua_rawgetp(L, LUA_REGISTRYINDEX, &L);
				lua_pushinteger(L, atoi(param1));
				if (lua_pcall(L, 1, 0, 0) != LUA_OK)
				{
					luaL_checktype(L, -1, LUA_TSTRING);
					printf("pcall fail, err = %s\n", lua_tostring(L, -1));
					lua_pop(L, 1);
				}
			}
			break;
			case 'r':
			{
				if (n != 2)
				{
					printf("no lua file\n");
					break;
				}
				if (luaL_dofile(L, "t2.lua") != LUA_OK)
				{
					printf("wrong lua file\n");
					break;
				}
			}
			break;
			case 'n':   //
			{
			}
			break;
			default:
			{
				printf("unknow command %c\n", command);
			}
			break;
		}
		if (lua_gettop(L) != 0)
		{
			printf("stack no empty\n");
//			stack_dump(L, "all end");
		}
		
// 		switch (n)
// 		{
// 			case 1:
// 				printf("%s\n", command);
// 				break;
// 			case 2:
// 				printf("[%s] %s\n", command, param1);
// 				break;
// 			default:
// 				printf("n = %d\n", n);
// 		}
	}
}

void luaLdbLineHook(lua_State *lua, lua_Debug *ar) {
    lua_getstack(lua,0,ar);
    lua_getinfo(lua,"Sl",ar);
    ldb.current_line = ar->currentline;

    int bp = ldb_is_break(ar->currentline, ar->source) || ldb.luabp;
//    int timeout = 0;

    /* Events outside our script are not interesting. */
    if(strstr(ar->short_src,"user_script") == NULL) return;

    /* Check if a timeout occurred. */
//    if (ar->event == LUA_HOOKCOUNT && ldb.step == 0 && bp == 0) {
//        mstime_t elapsed = mstime() - server.lua_time_start;
//        mstime_t timelimit = server.lua_time_limit ?
//                             server.lua_time_limit : 5000;
//        if (elapsed >= timelimit) {
//            timeout = 1;
//            ldb.step = 1;
//        } else {
//            return; /* No timeout, ignore the COUNT event. */
//        }
//    }

    if (ldb.step || bp) {
        char *reason = "step over";
        if (bp) reason = ldb.luabp ? "redis.breakpoint() called" :
                                     "break point";
//        else if (timeout) reason = "timeout reached, infinite loop?";
        ldb.step = 0;
        ldb.luabp = 0;
//        ldbLog(sdscatprintf(sdsempty(),
//            "* Stopped at %d, stop reason = %s",
//            ldb.currentline, reason));
//        ldbLogSourceLine(ldb.currentline);
//        ldbSendLogs();
//        if (ldbRepl(lua) == C_ERR && timeout) {
//           /* If the client closed the connection and we have a timeout
//             * connection, let's kill the script otherwise the process
//             * will remain blocked indefinitely. */
//            lua_pushstring(lua, "timeout during Lua debugging with client closing connection");
//            lua_error(lua);
//        }
//        server.lua_time_start = mstime();
    }
}

