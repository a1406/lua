#ifndef MYAPI_H
#define MYAPI_H
#include "lua.h"
#include "lobject.h"

LUA_API void print_tvalue(TValue *k1);
LUA_API void print_stack(struct lua_State *L);
LUA_API void print_global(struct lua_State *L);
#endif /* MYAPI_H */
