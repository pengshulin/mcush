/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "mcush.h"
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

typedef void (*led_f)(int);

static int _led_process( lua_State *L, led_f f )
{
    int top=lua_gettop(L), i;

    if( top > 0 )
    {
        for( i=1; i<=top; i++ )
            (*f)(luaL_checkinteger(L, i));
    }
    else
        (*f)(0);
    return 0;
}

static int lua_led_on(lua_State *L)
{
    return _led_process( L, hal_led_set );
}

static int lua_led_off(lua_State *L)
{
    return _led_process( L, hal_led_clr );
}

static int lua_led_toggle(lua_State *L)
{
    return _led_process( L, hal_led_toggle );
}


static int lua_led_get(lua_State *L)
{
    int top=lua_gettop(L), i;

    if( top < 1 )
        return 0;

    i = luaL_checkinteger(L, 1);
    lua_pushboolean( L, hal_led_get(i) );
    
    return 1;
}


static const struct luaL_Reg ledlib[] =
{
    { "on", lua_led_on },
    { "off", lua_led_off },
    { "toggle", lua_led_toggle },
    { "get", lua_led_get },
    { NULL, NULL}
};



LUAMOD_API int luaopen_ledlib(lua_State *L)
{
    luaL_newlib(L, ledlib);
    return 1;
}



