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
};


using namespace std;
static int raid_uuid;
static int monster_uuid;
set<int> all_raid;

void init_lua(struct lua_State *L)
{
	luaL_openlibs(L);	  // link lua lib
	luaL_loadfile(L, "test.lua");
	lua_pcall(L, 0, 0, 0);

	int type = lua_getglobal(L, "ailib");
	printf("get test1 return %d\n", type);
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
