#include <stdio.h>
#include <assert.h>
#include "lua.h"
#include <lualib.h>
#include <lauxlib.h>

static void stack_dump(lua_State* L, char *head)
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
};

