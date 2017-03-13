// OpenBBG
#include <OpenBBG/Modules/Module_Lua.h>
#include <OpenBBG/Game.h>

// Standard Library
#include <iostream>

// LuaJIT
#include <lua.hpp>
// Required for custom library loading
#define lib_init_c
#define LUA_LIB
#include "lj_arch.h"

#define DO_SANDBOX 1

namespace openbbg {

//---------- Custom Lua Init ----------------

static const luaL_Reg lj_lib_load[] = {
	{ "",				luaopen_base },
	{ LUA_LOADLIBNAME,	luaopen_package },
	{ LUA_TABLIBNAME,	luaopen_table },
	{ LUA_IOLIBNAME,	luaopen_io },
	{ LUA_OSLIBNAME,	luaopen_os },
	{ LUA_STRLIBNAME,	luaopen_string },
	{ LUA_MATHLIBNAME,	luaopen_math },
	{ LUA_DBLIBNAME,	luaopen_debug },
	{ LUA_BITLIBNAME,	luaopen_bit },
	{ LUA_JITLIBNAME,	luaopen_jit },
	{ NULL,		NULL }
};

static const luaL_Reg lj_lib_preload[] = {
#if LJ_HASFFI
	{ LUA_FFILIBNAME,	luaopen_ffi },
#endif
	{ NULL,		NULL }
};

LUALIB_API void luaL_openlibs_custom(lua_State *L)
{
  const luaL_Reg *lib;
  for (lib = lj_lib_load; lib->func; lib++) {
    lua_pushcfunction(L, lib->func);
    lua_pushstring(L, lib->name);
    lua_call(L, 1, 0);
  }
  luaL_findtable(L, LUA_REGISTRYINDEX, "_PRELOAD",
		 sizeof(lj_lib_preload)/sizeof(lj_lib_preload[0])-1);
  for (lib = lj_lib_preload; lib->func; lib++) {
    lua_pushcfunction(L, lib->func);
    lua_setfield(L, -2, lib->name);
  }
  lua_pop(L, 1);
}

static int loader_Custom(lua_State *L)
{
/*  size_t size;
  const char *name = luaL_checkstring(L, 1);
  const char *content = GetContents(name, &size);
  if(content) {
    lua_pushfstring("@%s", name);
    if(luaL_loadbuffer(L, content, size, lua_tostring(L, -1)) != 0)
      luaL_error(L, "error loading module " LUA_QS ":\n\t%s", name,
lua_tostring(L, -1));
  } else {
    lua_pushfstring("\n\tno file " LUA_QS, name);
  }
*/  return 1;
}



//---------------------------------------------------------------------------------------

IMPLEMENT_MODULE(Module_Lua, Phase_Startup, true)
MODULE_DEPENDENCY(Module_Lua, Module_Game, true)

// TODO: Encapsulate luaState into scripting contexts

lua_State *g_luaState;

bool Module_Lua::ModuleInit(Game *game)
{
	g_luaState = luaL_newstate();
	luaL_openlibs_custom(g_luaState);

	luaL_loadstring(g_luaState, "table.insert(package.loaders, 3, ...)");
	lua_pushcfunction(g_luaState, loader_Custom);
	lua_call(g_luaState, 1, 0);

	// Execute scripts
#if DO_SANDBOX
	ExecuteScript("assets/scripts/sandbox.lua");
#endif
	ExecuteScript("assets/scripts/test.lua");

	return true;
}

void Module_Lua::ModuleCleanup(Game *game)
{
	lua_close(g_luaState);
}


void Module_Lua::ExecuteScript(const char *file)
{
	if (luaL_loadfile(g_luaState, file) || lua_pcall(g_luaState, 0, LUA_MULTRET, 0)) {
		std::cerr << "Couldn't load file " << file << " : " << lua_tostring(g_luaState, -1) << std::endl;
	}
}

}