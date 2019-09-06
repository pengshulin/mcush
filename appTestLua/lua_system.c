#include "mcush.h"
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"


static int lua_system_delay( lua_State *L )
{
    uint32_t delay_ms;

    if( lua_gettop(L) != 1 )
    {
        delay_ms = 1000;
    }
    else
    {
        delay_ms = luaL_checkinteger(L, 1);
    }

    if( delay_ms )
        vTaskDelay( delay_ms * configTICK_RATE_HZ / 1000 );

    return 0;
}


static const struct luaL_Reg systemlib[] =
{
    { "delay", lua_system_delay },
    { NULL, NULL}
};


LUAMOD_API int luaopen_systemlib(lua_State *L)
{
    luaL_newlib(L, systemlib);
    return 1;
}

