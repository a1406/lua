#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <set>
extern "C"
{
#include "stack_test/stack_dump.h"
#include "myapi.h"	
};


using namespace std;
static int raid_uuid;
static int monster_uuid;
set<int> all_raid;

int c_func (lua_State *L)
{
	static int val;
	++val;
	double a = lua_tonumber(L, -1);
	long long b = lua_tointeger(L, -2);
	printf("%s %d: %d, %lf, %lld\n", __FUNCTION__, __LINE__, val, a, b);
	return (0);
}

void init_lua(struct lua_State *L)
{
	lua_newtable(L);
	for (int i = 0; i < 100; ++i)
	{
		lua_pushinteger(L, 100 + i);
		lua_rawseti(L, -2, i - 20);
	}
	printf("after newtable\n");
	print_tablesize(L, -1);

	for (int i = 0; i < 100; ++i)
	{
		int ret = lua_rawgeti(L, -1, i - 20);
		lua_pop(L, 1);
		assert(ret == LUA_TNUMBER);
	}
	
	int type = lua_getglobal(L, "ailib");
	printf("get test1 return %d\n", type);
	
	lua_pushboolean(L, false);
	lua_pushinteger(L, 99999);
	lua_pushnumber(L, 5.555);
	lua_pushstring(L, "abc");
	lua_pushstring(L, "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890");	
	print_stack(L);
	lua_pop(L, lua_gettop(L));
	
	luaL_openlibs(L);	  // link lua lib
	luaL_loadfile(L, "test.lua");
	lua_pcall(L, 0, 0, 0);

	lua_getglobal(L, "ailib");	
	lua_pushstring(L, "test1");
	lua_gettable(L, -2);
	lua_pushcfunction(L, c_func);
	lua_pcall(L, 1, 0, 0);
	lua_pop(L, 1);
	assert(lua_gettop(L) == 0);						
}

#define MAX_GM_ARGV 10
void parse_cmd( char *line, int *argc, char *argv[] )
{
	int n     = 0;
	argv[ 0 ] = line;
	while ( *line && n < MAX_GM_ARGV )
	{
		switch ( *line )
		{
			case ' ':
			case '	':
			case '\n':
				*line = '\0';
				if ( argv[ n ][ 0 ] != '\0' ) ++n;
				++line;
				argv[ n ] = line;
				break;
			default:
				++line;
		}
	}
	*argc = n;
}

int main(int argc, char *argv[])
{
    fd_set         rfds;
    struct timeval tv;
	char buf[201];
	int max_fd = 0;
	int   cmd_argc = 0;
	char *cmd_argv[ MAX_GM_ARGV ];

	struct lua_State *L = luaL_newstate();
	init_lua(L);

	for (;;)
	{
		tv.tv_sec  = 3;
		tv.tv_usec = 600;
		FD_ZERO(&rfds);
		FD_SET(0, &rfds);
		int retval = select(max_fd + 1, &rfds, NULL, NULL, &tv);
		if (retval == -1)
		{
			perror("select error");
			exit(1);
		}
		else if (retval)
		{  //retval>0说明监听的集合中有改变
			if (FD_ISSET(max_fd, &rfds))
			{  //所关心的文件应在集合中
				int nread = read(max_fd, buf, 200);
				if (nread <= 0)
				{
					perror("read failed");
					exit(1);
				}
				buf[nread] = '\0';
//				printf("read[%d] %s\n", nread, buf);
				parse_cmd(buf, &cmd_argc, &cmd_argv[0]);
				if (strcmp(cmd_argv[0], "add_raid") == 0)
				{
					++raid_uuid;
					all_raid.insert(raid_uuid);
					printf("add_raid %d\n", raid_uuid);
					lua_getglobal(L, "ailib");
					lua_pushstring(L, "create_raid");
					lua_gettable(L, -2);
					lua_pushinteger(L, raid_uuid);
					lua_pcall(L, 1, 0, 0);
					lua_pop(L, 1);
//					stack_dump(L, "luapcall");
					assert(lua_gettop(L) == 0);					
				}
				else if (strcmp(cmd_argv[0], "del_raid") == 0)
				{
					if (cmd_argc != 2)
					{
						printf("argc[%d] != 2\n", cmd_argc);
						continue;
					}
					int uuid = atoi(cmd_argv[1]);
					if (all_raid.find(uuid) == all_raid.end())
					{
						printf("can not find raid %d\n", uuid);
						continue;
					}
					all_raid.erase(uuid);
					printf("del_raid %d\n", uuid);						
				}
				else if (strcmp(cmd_argv[0], "add_monster") == 0)
				{
					if (cmd_argc != 3)
					{
						printf("argc[%d] != 3\n", cmd_argc);
						continue;
					}
					int uuid = atoi(cmd_argv[1]);					
					int monster_id = atoi(cmd_argv[2]);
					if (all_raid.find(uuid) == all_raid.end())
					{
						printf("can not find raid %d\n", uuid);
						continue;
					}
					++monster_uuid;
					printf("add_monster[%d][%d] at raid %d\n", monster_id, monster_uuid, uuid);						
				}
				else if (strcmp(cmd_argv[0], "kill_monster") == 0)
				{
					if (cmd_argc != 3)
					{
						printf("argc[%d] != 3\n", cmd_argc);
						continue;
					}
					int uuid = atoi(cmd_argv[1]);					
					int monster_uuid = atoi(cmd_argv[2]);
					if (all_raid.find(uuid) == all_raid.end())
					{
						printf("can not find raid %d\n", uuid);
						continue;
					}
					printf("kill_monster[%d] at raid %d\n", monster_uuid, uuid);
					lua_getglobal(L, "ailib");
					lua_pushstring(L, "del_monster");
					lua_gettable(L, -2);
					lua_pushinteger(L, uuid);
					lua_pushinteger(L, 0);
					lua_pushinteger(L, monster_uuid);					
					int ret = lua_pcall(L, 3, 0, 0);
					if (ret != 0)
					{
						printf("luapcall ret %d\n", ret);
						lua_pop(L, 2);
					}
					else
					{
						lua_pop(L, 1);
					}
//					stack_dump(L, "luapcall");
					assert(lua_gettop(L) == 0);
				}
				else
				{
					printf("unknow command\n");
				}
			}
		}
		else
		{
//			printf("timeout\n");
		}
	}
    return 0;
}
