/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "mcush.h"
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "task_logger.h"


static int _log_write_message( lua_State *L, int type )
{
    size_t l;
    const char *p;
    int i, top=lua_gettop(L);

    if( top < 1 )
    {
        shell_write_err( "message" );
        return 0;
    }
    else
    {
        for( i=1; i<=top; i++ )
        {
            p = luaL_checklstring(L, i, &l);
            if( p == 0 || *p == 0 )
                break;
 
            logger_module_str( type, "lua", p );
        }
    }

    return 0;
}

static int lua_log_debug(lua_State *L)
{
    return _log_write_message( L, LOG_DEBUG );
}

static int lua_log_info(lua_State *L)
{
    return _log_write_message( L, LOG_INFO );
}

static int lua_log_warn(lua_State *L)
{
    return _log_write_message( L, LOG_WARN );
}

static int lua_log_error(lua_State *L)
{
    return _log_write_message( L, LOG_ERROR );
}

static int lua_log_enable(lua_State *L)
{
    (void)L;
    logger_enable();
    return 0;
}

static int lua_log_disable(lua_State *L)
{
    (void)L;
    logger_disable();
    return 0;
}






static const struct luaL_Reg loglib[] =
{
    { "debug", lua_log_debug },
    { "info", lua_log_info },
    { "warn", lua_log_warn},
    { "error", lua_log_error },
    { "enable", lua_log_enable },
    { "disable", lua_log_disable },
    { NULL, NULL}
};


LUAMOD_API int luaopen_loglib(lua_State *L)
{
    luaL_newlib(L, loglib);
    return 1;
}

