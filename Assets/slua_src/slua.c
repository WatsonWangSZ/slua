/* 
* This file is based off of MonoLuaInterface's wrapping: https://github.com/stevedonovan/MonoLuaInterface
*/

#define LUA_LIB
#include "lua.h"
#include "lauxlib.h"
#include <stdio.h>

/*  LUA INTERFACE SUPPORT  */
#ifndef _WIN32
#define __stdcall
#endif
typedef int (__stdcall *lua_stdcallCFunction) (lua_State *L);

static int stdcall_closure(lua_State *L) 
{
    lua_stdcallCFunction fn = (lua_stdcallCFunction)lua_touserdata(L, lua_upvalueindex(1));
    int r = fn(L);
    return r;
}


LUA_API void lua_pushstdcallcfunction(lua_State *L, lua_stdcallCFunction fn)
{
    lua_pushlightuserdata(L, fn);
    lua_pushcclosure(L, stdcall_closure, 1);
}


LUA_API void luaS_newudata(lua_State *L, int val)
{
  int* pointer=(int*)lua_newuserdata(L,sizeof(int));
  *pointer=val;
}

LUA_API int luaS_rawnetobj(lua_State *L,int index) 
{
  int *udata=lua_touserdata(L,index);
  if(udata!=NULL) return *udata;
  return -1;
}

#define SLuaCallback "SLuaCallback"
LUA_API int luaS_checkcallback(lua_State *L, int index) 
{
	int r;
	lua_Debug ar;
	index = index > 0 ? index : lua_gettop(L) + index + 1;

	lua_pushvalue(L, index);
	lua_getinfo(L, ">S", &ar);

	char funcinfo[260];
	sprintf(funcinfo, "%s:%d", ar.source, ar.linedefined);
	lua_getfield(L, LUA_REGISTRYINDEX, SLuaCallback);
	if (lua_isnil(L, -1)) {
		lua_pop(L, 1);
		lua_createtable(L, 0, 0);
		lua_pushvalue(L, -1);
		lua_setfield(L, LUA_REGISTRYINDEX, SLuaCallback);
	}

	lua_getfield(L, -1, funcinfo);
	if (lua_isnil(L, -1)) {
		lua_pop(L, 1);
		lua_pushvalue(L, index);
		r =luaL_ref(L, LUA_REGISTRYINDEX);
		lua_pushinteger(L, r);
		lua_setfield(L, -2, funcinfo);
	}
	else {
		r = lua_tointeger(L, -1);
		lua_pop(L, 1);
	}
	lua_pop(L, 1); // pop SLuaCallback table

	return r;
}
