/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "hal.h"
#include <stdio.h>
#include <unistd.h>
#if HAL_DBUS
#include <stdlib.h>
#include <string.h>
#include <dbus/dbus-glib.h>
#include <dbus/dbus.h>
#include <unistd.h>
#endif

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

    ret = dbus_bus_request_name(connection,"mcush.posix.sim.ws2812.service",DBUS_NAME_FLAG_REPLACE_EXISTING,&err);
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

static int sendSignalColor(int offset, int color)
{
    DBusMessage *msg;
    DBusMessageIter arg;

    msg = dbus_message_new_signal ("/mcush/posix/sim/ws2812","mcush.posix.sim.ws2812.signal","color");
    if( msg == NULL )
    {
        printf("Error: message NULL\n");
        return 0;
    }

    dbus_message_iter_init_append( msg, &arg );
    if( !dbus_message_iter_append_basic( &arg, DBUS_TYPE_INT32, &offset ) )
    {
        printf("Error: append arg1\n");
        dbus_message_unref( msg );
        return 0;
    }
    if( !dbus_message_iter_append_basic( &arg, DBUS_TYPE_INT32, &color ) )
    {
        printf("Error: append arg2\n");
        dbus_message_unref( msg );
        return 0;
    }

    if( !dbus_connection_send( connection, msg, NULL ) )
    {
        printf("Error: send\n");
        dbus_message_unref( msg );
        return 0;
    }
    dbus_connection_flush( connection );
    dbus_message_unref( msg );
    return 1; 
}

static int sendSignalUpdate(int total_length)
{
    DBusMessage *msg;
    DBusMessageIter arg;

    msg = dbus_message_new_signal ("/mcush/posix/sim/ws2812","mcush.posix.sim.ws2812.signal","update");
    if( msg == NULL )
    {
        printf("Error: message NULL\n");
        return 0;
    }

    dbus_message_iter_init_append( msg, &arg );
    if( !dbus_message_iter_append_basic( &arg, DBUS_TYPE_INT32, &total_length ) )
    {
        printf("Error: append arg1\n");
        dbus_message_unref( msg );
        return 0;
    }

    if( !dbus_connection_send( connection, msg, NULL ) )
    {
        printf("Error: send\n");
        dbus_message_unref( msg );
        return 0;
    }
    dbus_connection_flush( connection );
    dbus_message_unref( msg );
    return 1; 
}
#endif

static int flush_offset;

int hal_ws2812_init(int port, int pin)
{
    printf("hal_ws2812_init, %d.%d\n", port, pin);
    return 1;
}


void hal_ws2812_deinit(void)
{
    printf("hal_ws2812_deinit\n");
}


void hal_ws2812_clr(void)
{
#if HAL_DBUS
    if( flush_offset == 0 )
        return;
    if( checkDB() )
    {
        sendSignalUpdate( flush_offset );
    }
    flush_offset = 0;
#endif
}

void ws2812_flush_pixel( int dat )
{
#if HAL_DBUS
    if( checkDB() )
    {
        //printf("offset %d, color 0x%06X\n", flush_offset, dat);
        sendSignalColor( flush_offset++, dat );
    }
#endif
}


//void hal_ws2812_write0(void)
//{
//    usleep( 1 );
//}
//
//
//void hal_ws2812_write1(void)
//{
//    usleep( 1 );
//}

