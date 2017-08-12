#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

extern struct lua_State *L;
extern void stack_dump(lua_State* L, char *head);

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
	const char *current_file;
	int current_line;
};

struct ldb ldb;

int ldb_is_break(int line, const char *filename)
{
	for (int i = 0; i < ldb.bpcount; ++i)
	{
		if (line == ldb.bp[i].linenum && strcmp(filename+1, ldb.bp[i].filename) == 0)
			return (1);
	}
	return (0);
}

int ldb_add_break(int line, const char *filename)
{
	if (ldb.bpcount >= LDB_BREAKPOINTS_MAX - 1)
		return -1;
	ldb.bp[ldb.bpcount].filename = strdup(filename);
	ldb.bp[ldb.bpcount].linenum = line;
	++ldb.bpcount;
	return (0);
}

static void break_cmd(int n, char *param1, char *param2)
{
	switch (n)
	{
		case 1:
			ldb_add_break(ldb.current_line, ldb.current_file);
			break;
		case 2:
			ldb_add_break(atoi(param2), ldb.current_file);										
			break;
		case 3:
			ldb_add_break(atoi(param2), param1);					
			break;
		default:
			printf("break filename line\n");
			break;
	}	
}

static int test_cmd(int n, char *param1)
{
	if (n != 2)
	{
		printf("no param\n");
		return (0);
	}
	lua_rawgetp(L, LUA_REGISTRYINDEX, &L);
	lua_pushinteger(L, atoi(param1));
	if (lua_pcall(L, 1, 0, 0) != LUA_OK)
	{
		luaL_checktype(L, -1, LUA_TSTRING);
		printf("pcall fail, err = %s\n", lua_tostring(L, -1));
		lua_pop(L, 1);
	}
	return (0);	
}

static int run_cmd(int n, char *param1)
{
	if (n != 2)
	{
		printf("no lua file\n");
		return (0);
	}
	if (luaL_dofile(L, param1) != LUA_OK)
	{
		printf("wrong lua file\n");
	}
	return (0);		
}

static void next_cmd()
{
/* TODO: 跳过函数调用 */
	ldb.step = 1;
}
static void step_cmd()
{
	ldb.step = 1;
}

int ldb_step()
{
	static char ldb_buf[1024];
	char command[64], param1[128], param2[64];

	printf("$>>>> ");
	
	fgets(ldb_buf, 1024, stdin);

	int n = sscanf(ldb_buf, "%s %s %s", command, param1, param2);
	if (n <= 0)
		return (0);

	if (strcmp(command, "b") == 0 || strcmp(command, "break") == 0)
	{
		break_cmd(n, param1, param2);
	}
	else if (strcmp(command, "n") == 0 || strcmp(command, "next") == 0)
	{
		next_cmd();
	}
	else if (strcmp(command, "s") == 0 || strcmp(command, "step") == 0)
	{
		step_cmd();
	}
	else if (strcmp(command, "test") == 0)
	{
		test_cmd(n, param1);
	}
	else if (strcmp(command, "r") == 0 || strcmp(command, "run") == 0)
	{
		run_cmd(n, param1);
	}
	else if (strcmp(command, "q") == 0 || strcmp(command, "quit") == 0)
	{
		return (-1);
	}
	else
	{
		printf("unknow command %s\n", command);
	}

	if (lua_gettop(L) != 0)
	{
		printf("stack no empty\n");
		stack_dump(L, "ldb step");
	}
	return (0);
}

void ldb_loop()
{
	while (ldb_step() == 0)
	{
	}
}

void luaLdbLineHook(lua_State *lua, lua_Debug *ar) {
    lua_getstack(lua,0,ar);
    lua_getinfo(lua,"Sl",ar);
    ldb.current_line = ar->currentline;
	ldb.current_file = ar->source;

//	printf("%s: %s %d\n", __FUNCTION__, ar->source, ar->currentline);
    int bp = ldb_is_break(ar->currentline, ar->source) || ldb.luabp;
//    int timeout = 0;

    /* Events outside our script are not interesting. */
//    if(strstr(ar->short_src,"user_script") == NULL) return;

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
		ldb_step();
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

