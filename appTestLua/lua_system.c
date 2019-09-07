/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "mcush.h"
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "task_blink.h"


static int lua_system_delay( lua_State *L )
{
    uint32_t delay_ms;

    if( lua_gettop(L) < 1 )
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


static int lua_system_errno( lua_State *L )
{
    int errno;

    if( lua_gettop(L) < 1 )
    {
        errno = get_errno();
        if( errno < 0 )
            lua_pushstring( L, "stop" );
        else
            lua_pushinteger( L, errno );
        return 1;
    }
    else
    {
        errno = luaL_checkinteger(L, 1);
        set_errno( errno );
        return 0;
    }
}


static int lua_system_beep( lua_State *L )
{
    int freq=-1, ms=-1;
    int top=lua_gettop(L);

    if( top >= 2 )
        ms = luaL_checkinteger(L, 2); 
    if( top >= 1 )
        freq = luaL_checkinteger(L, 1);

    if( freq < 0 )
        freq = 4000;
    if( ms < 0 )
        ms = 50;
    hal_beep_on( freq );
    vTaskDelay(ms*configTICK_RATE_HZ/1000);
    hal_beep_off();

    return 0;
}


static int lua_system_cmd( lua_State *L )
{
    size_t l;
    const char *p;

    if( lua_gettop(L) < 1 )
    {
        shell_write_err( "cmd" );
        return 0;
    }
 
    p = luaL_checklstring(L, 1, &l);
    if( p == 0 || *p == 0 )
        return 0;
 
    shell_call_line( (char*)p );

    return 0;
}



static const struct luaL_Reg systemlib[] =
{
    { "beep", lua_system_beep },
    { "delay", lua_system_delay },
    { "errno", lua_system_errno },
    { "cmd", lua_system_cmd },
    { NULL, NULL}
};


LUAMOD_API int luaopen_systemlib(lua_State *L)
{
    luaL_newlib(L, systemlib);
    return 1;
}

