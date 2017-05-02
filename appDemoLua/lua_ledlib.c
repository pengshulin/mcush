#include "mcush.h"
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "string.h"

static int lua_led_on(lua_State *L)
{
    lua_Integer n = luaL_checkinteger(L, 1);
    hal_led_set(n);
    lua_pushnil(L);
    return 1;
}

static int lua_led_off(lua_State *L)
{
    lua_Integer n;
    //if( lua_isnil(L) )
    //    n = 0;
    //else
        n = luaL_checkinteger(L, 1);
    hal_led_clr(n);
    lua_pushnil(L);
    return 1;
}

static int lua_led_toggle(lua_State *L)
{
    lua_Integer n = luaL_checkinteger(L, 1);
    hal_led_toggle(n);
    lua_pushnil(L);
    return 1;
}


static const struct luaL_Reg ledlib[] =
{
    {"on",lua_led_on},
    {"off",lua_led_off},
    {"toggle",lua_led_toggle},
    {NULL,NULL}
};


//
LUAMOD_API int luaopen_ledlib(lua_State *L)
{
    luaL_newlib(L, ledlib);
    return 1;
}



