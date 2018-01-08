#include "myapi.h"
#include "lobject.h"
#include "ltable.h"
#include "lapi.h"
#include "lstate.h"
#include <stdio.h>

void print_tvalue(TValue *k1)
{
	int tt = ttype(k1);
	switch (tt) {
		case LUA_TNIL:
			printf("nil: nil");
			break;
		case LUA_TNUMINT:
			printf("int: %lld", ivalue(k1));
			break;
		case LUA_TNUMFLT:
			printf("num: %lf", fltvalue(k1));
			break;
//			return luai_numeq(fltvalue(k1), fltvalueraw(keyval(n2)));
		case LUA_TBOOLEAN:
			printf("bool: %d", bvalue(k1));
			break;
//			return bvalue(k1) == bvalueraw(keyval(n2));
		case LUA_TLIGHTUSERDATA:
			printf("lightuserdata: %p", pvalue(k1));
			break;			
//			return pvalue(k1) == pvalueraw(keyval(n2));
		case LUA_TLCF:
			printf("cfunction: %p", fvalue(k1));
			break;			
//			return fvalue(k1) == fvalueraw(keyval(n2));
		case LUA_TLNGSTR:
			printf("long str: %s", getstr(tsvalue(k1)));
			break;
		case LUA_TSHRSTR:
			printf("short str: %s", getstr(tsvalue(k1)));
			break;			
//			return luaS_eqlngstr(tsvalue(k1), keystrval(n2));
		default:
			printf("type = %d", tt);
			break;
//			return gcvalue(k1) == gcvalueraw(keyval(n2));
	}
}

void print_stack(struct lua_State *L)
{
	StkId io = L->top - 1;
//	StkId io = L->ci->func;
	while (io > L->ci->func)
	{
		print_tvalue(&io->val);
		printf("\n");
		io--;
	}
	printf("finished\n");	
}

void print_global(struct lua_State *L)
{
	Table *reg = hvalue(&G(L)->l_registry);	
}

void print_tablesize(struct lua_State *L, int idx)
{
	TValue *k1 = index2value(L, idx);
	Table *t;
	int valid_array = 0;
	int valid_node = 0;
	int nodesize;
	if (!ttistable(k1))
	{
		printf("not table\n");
		return;
	}
	t = hvalue(k1);
	for (unsigned int i = 0; i < t->sizearray; ++i)
	{
		TValue *tv = &t->array[i];
		if (ttisnil(tv))
			continue;
		++valid_array;
	}

	nodesize = twoto(t->lsizenode);
	for (int i = 0; i < nodesize; ++i)
	{
		Node *no = &t->node[i];
		for (;;)
		{
			int nx;
			TValue *tv = gval(no);
			if (!ttisnil(tv))
				++valid_node;
			nx = gnext(no);
			if (nx == 0) break;			
			no += nx;
		}
	}
	
	printf("array[%d][%d] node[%d][%d]\n", t->sizearray, valid_array, nodesize, valid_node);
}
