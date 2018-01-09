#ifndef MYAPI_H
#define MYAPI_H
#include "lua.h"
#include "lobject.h"

LUA_API void print_tvalue(TValue *k1, const char *end);
LUA_API void print_stack(struct lua_State *L);
LUA_API void print_registry(struct lua_State *L);
LUA_API void print_global(struct lua_State *L);
LUA_API void print_tabledata(struct lua_State *L, int idx);
LUA_API void print_tablesize(struct lua_State *L, int idx);
#endif /* MYAPI_H */
