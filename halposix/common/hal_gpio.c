/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "hal.h"
#include <stdio.h>
#if HAL_DBUS
#include <stdlib.h>
#include <string.h>
#include <dbus/dbus-glib.h>
#include <dbus/dbus.h>
#include <unistd.h>
#endif


#ifndef PORT_NUM
    #define PORT_NUM  5
#endif

int port_mode[PORT_NUM];
int port_set[PORT_NUM];
int port_otype[PORT_NUM];
int port_pullup[PORT_NUM];
int port_pulldown[PORT_NUM];

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

    ret = dbus_bus_request_name(connection,"mcush.posix.sim.gpio.service",DBUS_NAME_FLAG_REPLACE_EXISTING,&err);
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

    msg = dbus_message_new_signal ("/mcush/posix/sim/gpio","mcush.posix.sim.gpio.signal",cmd);
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
    return 1; 
}
#endif

static void _set_mode( int port, int bits, int mode, int otype, int pull )
{
    int i;

    for( i=0; i<32; i++ )
    {
        if( bits & (1<<i) )
        {
            printf("port %d, bits 0x%04X, mode %d, otype %d, pull %d\n", port, bits, mode, otype, pull);
            if( mode )
                port_mode[port] |= 1>>i;
            else
                port_mode[port] &= ~(1>>i);
            if( otype )
                port_otype[port] |= 1>>i;
            else
                port_otype[port] &= ~(1>>i);
            if( pull > 0 )
            {
                port_pullup[port] |= 1>>i;
                port_pulldown[port] &= ~(1>>i);
            }
            else if( pull < 0 )
            {
                port_pullup[port] &= ~(1>>i);
                port_pulldown[port] |= 1>>i;
            }
            else
            {
                port_pullup[port] &= ~(1>>i);
                port_pulldown[port] &= ~(1>>i);
            }
        }
    }
}


int hal_gpio_get_port_num(void)
{
    return PORT_NUM;
}


void hal_gpio_init(void)
{
}


void hal_gpio_set_input(int port, int bits, int pull)
{
    _set_mode( port, bits, 0, 0, pull );
}


void hal_gpio_set_output(int port, int bits, int open_drain)
{
    if( open_drain )
        _set_mode( port, bits, 1, 1, 1 );
    else
        _set_mode( port, bits, 1, 0, 0 );
}


void hal_gpio_set(int port, int bits)
{
    port_set[port] |= bits;
    printf("port %d, set 0x%04X\n", port, bits);
#if HAL_DBUS
    if( checkDB() )
    {
        sendSignal( "gpio_set", port, bits );
    }
#endif
}


void hal_gpio_clr(int port, int bits)
{
    port_set[port] &= ~bits;
    printf("port %d, clr 0x%04X\n", port, bits);
#if HAL_DBUS
    if( checkDB() )
    {
        sendSignal( "gpio_clr", port, bits );
    }
#endif
}


void hal_gpio_toggle(int port, int bits)
{
    port_set[port] ^= bits;
    printf("port %d, toggle 0x%04X\n", port, bits);
#if HAL_DBUS
    if( checkDB() )
    {
        sendSignal( "gpio_toggle", port, bits );
    }
#endif
}


int hal_gpio_get(int port, int bits)
{
    return port_set[port] & bits;
}


