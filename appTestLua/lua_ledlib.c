#include "mcush.h"
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

typedef void (*led_f)(int);

static int _led_process( lua_State *L, led_f f )
{
    int i=lua_gettop(L), j;
    //shell_printf( "gettop: %d\n", i );
    if( i )
    {
        do
        {
            j=luaL_checkinteger(L, i);
            //shell_printf( "checkinteger(%d)->%d\n", i, j );
            (*f)(j);
        }while(--i);
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


static const struct luaL_Reg ledlib[] =
{
    { "on", lua_led_on },
    { "off", lua_led_off },
    { "toggle", lua_led_toggle },
    { NULL, NULL}
};



LUAMOD_API int luaopen_ledlib(lua_State *L)
{
    luaL_newlib(L, ledlib);
    return 1;
}



