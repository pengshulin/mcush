#include "mcush.h"
#if HAL_DBUS
#include <stdlib.h>
#include <string.h>
#include <dbus/dbus-glib.h>
#include <dbus/dbus.h>
#include <unistd.h>
#endif

#ifndef HAL_PWM_NUM
    #define HAL_PWM_NUM                 8
#endif
    
#ifndef HAL_PWM_FREQ_DEF
    #define HAL_PWM_FREQ_DEF            1000
#endif

#ifndef HAL_PWM_RANGE_DEF
    #define HAL_PWM_RANGE_DEF           100
#endif
 
#ifndef HAL_PWM_VALUE_DEF
    #define HAL_PWM_VALUE_DEF           50
#endif

#if HAL_PWM_VALUE_DEF > HAL_PWM_RANGE_DEF
    #error "HAL_PWM_VALUE_DEF larger than HAL_PWM_RANGE_DEF"
#endif


extern uint32_t SystemCoreClock;

static short _pwm_range = HAL_PWM_RANGE_DEF;
static short _pwm_value[HAL_PWM_NUM];

#if HAL_DBUS
static DBusError err;
static DBusConnection *connection;

static int checkDB(void)
{
    int ret;

    if( connection != NULL )
        return 1;
    
    dbus_error_init(&err);
    connection = dbus_bus_get(DBUS_BUS_SESSION, &err);
    if( dbus_error_is_set(&err) )
    {
        printf("Error: connection (%s)\n",err.message);
        dbus_error_free(&err);
    }
    if(connection == NULL)
    {
        printf("Error: connection NULL\n");
        return 0;
    }

    ret = dbus_bus_request_name(connection,"mcush.posix.sim.pwm.service",DBUS_NAME_FLAG_REPLACE_EXISTING,&err);
    if(dbus_error_is_set(&err))
    {
        printf("Error: name (%s)\n",err.message);
        dbus_error_free(&err);
    }
    if(ret != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER)
        return 0; 
    printf("DBus Connection: %p\n", connection);
    return 1;
}

static int sendSignal(char *cmd, int arg1, int arg2)
{
    DBusMessage *msg;
    DBusMessageIter arg;

    msg = dbus_message_new_signal ("/mcush/posix/sim/pwm","mcush.posix.sim.pwm.signal",cmd);
    if( msg == NULL )
    {
        printf("Error: message NULL\n");
        return 0;
    }

    dbus_message_iter_init_append( msg, &arg );
    if( !dbus_message_iter_append_basic( &arg, DBUS_TYPE_UINT32, &arg1 ) )
    {
        printf("Error: append arg1\n");
        return 0;
    }
    if( !dbus_message_iter_append_basic( &arg, DBUS_TYPE_UINT32, &arg2 ) )
    {
        printf("Error: append arg2\n");
        return 0;
    }

    if( !dbus_connection_send( connection, msg, NULL ) )
    {
        printf("Error: send\n");
        return 0;
    }
    dbus_connection_flush( connection );
    dbus_message_unref( msg );
    printf("pwm: %s (%d, %d)\n", cmd, arg1, arg2);
    return 1; 
}
#endif


void hal_pwm_init( int freq, int range, int value )
{
    if( freq == 0 )
        freq = HAL_PWM_FREQ_DEF;
    if( range <= 0 )
        range = HAL_PWM_RANGE_DEF;
    if( value < 0 )
        value = HAL_PWM_VALUE_DEF;
    _pwm_range = range;
#if HAL_DBUS
    if( checkDB() )
    {
        sendSignal( "init", range, value );
    }
#endif
}

 
void hal_pwm_deinit( void )
{
}


void hal_pwm_set( int index, int value )
{
    _pwm_value[index] = value;
#if HAL_DBUS
    if( checkDB() )
    {
        sendSignal( "set", index, value );
    }
#endif
}


int hal_pwm_get( int index )
{
    return _pwm_value[index];
}


int hal_pwm_get_num( void )
{
    return HAL_PWM_NUM;
}

