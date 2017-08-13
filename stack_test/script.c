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

void ldbTrace()
{
    lua_Debug ar;
    int level = 0;

    while(lua_getstack(L,level,&ar)) {
        lua_getinfo(L,"Snl",&ar);
		
		printf("#%d in %s()   at %s:%d\n", level,
			ar.name ? ar.name : "top level",
			ar.source, ar.currentline);
//		ldbLogSourceLine(ar.currentline);
        level++;
    }
    if (level == 0) {
        printf("<error> Can't retrieve Lua stack.\n");
    }
}

/* Append an human readable representation of the Lua value at position 'idx'
 * on the stack of the 'lua' state, to the SDS string passed as argument.
 * The new SDS string with the represented value attached is returned.
 * Used in order to implement ldbLogStackValue().
 *
 * The element is not automatically removed from the stack, nor it is
 * converted to a different type. */
#define LDB_MAX_VALUES_DEPTH (LUA_MINSTACK/2)
char *ldbCatStackValueRec(char *s, lua_State *lua, int idx, int level)
{
    int t = lua_type(lua,idx);

    if (level++ == LDB_MAX_VALUES_DEPTH)
        return "<max recursion level reached! Nested table?>";

    switch(t) {
		case LUA_TSTRING:
        {
			size_t strl;
			char *strp = (char*)lua_tolstring(lua,idx,&strl);
			s = strncat(s,strp,strl);
        }
        break;
		case LUA_TBOOLEAN:
			s = strcat(s,lua_toboolean(lua,idx) ? "true" : "false");
			break;
		case LUA_TNUMBER:
		{
			char tmp[128];
			sprintf(tmp, "%g", (double)lua_tonumber(lua,idx));
			s = strcat(s, tmp);
		}
		break;
		case LUA_TNIL:
			s = strcat(s,"nil");
			break;
		case LUA_TTABLE:
        {
			int expected_index = 1; /* First index we expect in an array. */
			int is_array = 1; /* Will be set to null if check fails. */
				/* Note: we create two representations at the same time, one
				 * assuming the table is an array, one assuming it is not. At the
				 * end we know what is true and select the right one. */
//			sds repr1 = sdsempty();
//			sds repr2 = sdsempty();
			lua_pushnil(lua); /* The first key to start the iteration is nil. */
			while (lua_next(lua,idx-1)) {
					/* Test if so far the table looks like an array. */
				if (is_array &&
					(lua_type(lua,-2) != LUA_TNUMBER ||
						lua_tonumber(lua,-2) != expected_index)) is_array = 0;
					/* Stack now: table, key, value */
					/* Array repr. */
				s = ldbCatStackValueRec(s,lua,-1,level);
				s = strncat(s,"; ",2);
					/* Full repr. */
				s = strncat(s,"[",1);
				s = ldbCatStackValueRec(s,lua,-2,level);
				s = strncat(s,"]=",2);
				s = ldbCatStackValueRec(s,lua,-1,level);
				s = strncat(s,"; ",2);
				lua_pop(lua,1); /* Stack: table, key. Ready for next iteration. */
				expected_index++;
			}
				/* Strip the last " ;" from both the representations. */
//			if (sdslen(repr1)) sdsrange(repr1,0,-3);
//			if (sdslen(repr2)) sdsrange(repr2,0,-3);
				/* Select the right one and discard the other. */
			s = strncat(s,"{",1);
//			s = sdscatsds(s,is_array ? repr1 : repr2);
			s = strncat(s,"}",1);
//			sdsfree(repr1);
//			sdsfree(repr2);
        }
        break;
		case LUA_TFUNCTION:
		case LUA_TUSERDATA:
		case LUA_TTHREAD:
		case LUA_TLIGHTUSERDATA:
        {
			const void *p = lua_topointer(lua,idx);
			char *typename = "unknown";
			if (t == LUA_TFUNCTION) typename = "function";
			else if (t == LUA_TUSERDATA) typename = "userdata";
			else if (t == LUA_TTHREAD) typename = "thread";
			else if (t == LUA_TLIGHTUSERDATA) typename = "light-userdata";
			char tmp[128];
			sprintf(tmp, "\"%s@%p\"",typename,p);
			s = strcat(s, tmp);
        }
        break;
		default:
			s = strcat(s,"\"<unknown-lua-type>\"");
			break;
    }
    return s;
}

/* Higher level wrapper for ldbCatStackValueRec() that just uses an initial
 * recursion level of '0'. */
char *ldbCatStackValue(char *s, lua_State *lua, int idx) {
    return ldbCatStackValueRec(s,lua,idx,0);
}

/* Produce a debugger log entry representing the value of the Lua object
 * currently on the top of the stack. The element is ot popped nor modified.
 * Check ldbCatStackValue() for the actual implementation. */
void ldbLogStackValue(lua_State *lua, char *prefix) {
//    sds s = sdsnew(prefix);
	static char s[4096];
	s[0] = '\0';
    ldbCatStackValue(s,lua,-1);
//    ldbLogWithMaxLen(s);
	printf("%s %s\n", prefix, s);
}

void ldbPrintAll()
{
    lua_Debug ar;
    int vars = 0;

    if (lua_getstack(L,0,&ar) != 0) {
        const char *name;
        int i = 1; /* Variable index. */
        while((name = lua_getlocal(L,&ar,i)) != NULL) {
            i++;
            if (!strstr(name,"(*temporary)")) {
//                sds prefix = sdscatprintf(sdsempty(),"<value> %s = ",name);
				char prefix[128];
				sprintf(prefix, "<value> %s = ",name);
                ldbLogStackValue(L,prefix);
//                sdsfree(prefix);
                vars++;
            }
            lua_pop(L,1);
        }
    }

    if (vars == 0) {
        printf("No local variables in the current context.\n");
    }
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

static void info_cmd(int n, char *param1)
{
	if (n != 2)
	{
		printf("info b/s");
		return;
	}
	if (strcmp(param1, "b") == 0 || strcmp(param1, "break") == 0)
	{
		for (int i = 0; i < ldb.bpcount; ++i)
		{
			printf("break file[%s] line[%d]\n", ldb.bp[i].filename, ldb.bp[i].linenum);
		}
	}
	if (strcmp(param1, "s") == 0 || strcmp(param1, "stack") == 0)
	{
		ldbTrace();
	}
}

static void	print_cmd(int n, char *param1)
{
	if (n != 2)
	{
		ldbPrintAll();
		return;
	}
	
}

static void continue_cmd()
{
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
		return 1;
	}
	else if (strcmp(command, "s") == 0 || strcmp(command, "step") == 0)
	{
		step_cmd();
		return 1;
	}
	else if (strcmp(command, "info") == 0)
	{
		info_cmd(n, param1);
	}
	else if (strcmp(command, "test") == 0)
	{
		test_cmd(n, param1);
		return 1;
	}
	else if (strcmp(command, "c") == 0 || strcmp(command, "continue") == 0)
	{
		continue_cmd(n, param1);
		return 1;
	}
	else if (strcmp(command, "p") == 0 || strcmp(command, "print") == 0)
	{
		print_cmd(n, param1);
		return 0;
	}	
	else if (strcmp(command, "r") == 0 || strcmp(command, "run") == 0)
	{
		run_cmd(n, param1);
		return 1;
	}
	else if (strcmp(command, "q") == 0 || strcmp(command, "quit") == 0)
	{
		return (-1);
	}
	else
	{
		printf("unknow command %s\n", command);
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
		ldb_loop();
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

