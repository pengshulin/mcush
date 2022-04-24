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


#ifndef HAL_LED_NUM
    #define HAL_LED_NUM  0
#endif

static const uint8_t led_num = HAL_LED_NUM;
#if HAL_LED_NUM
    static const uint8_t led_ports[] = HAL_LED_PORTS;
    static const uint8_t led_pins[] = HAL_LED_PINS;
    #if HAL_LED_NUM <= 8
        typedef uint8_t led_bitmap_t;
    #elif HAL_LED_NUM <= 16
        typedef uint8_t led_bitmap_t;
    #elif HAL_LED_NUM <= 32
        typedef uint8_t led_bitmap_t;
    #elif HAL_LED_NUM <= 64
        typedef uint8_t led_bitmap_t;
    #else
        #error "HAL_LED_NUM too large"
    #endif
    static led_bitmap_t led_bitmap;
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

    ret = dbus_bus_request_name(connection,"mcush.posix.sim.led.service",DBUS_NAME_FLAG_REPLACE_EXISTING,&err);
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

static int sendSignal(char *cmd, int arg1)
{
    DBusMessage *msg;
    DBusMessageIter arg;

    msg = dbus_message_new_signal("/mcush/posix/sim/led","mcush.posix.sim.led.signal",cmd);
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


void hal_led_init(void)
{
#if HAL_LED_NUM
    int i;

    for( i=0; i<led_num; i++ )
    {
        hal_gpio_set_output( led_ports[i], 1<<led_pins[i], 0 );
#if defined(HAL_LED_REV)
        hal_gpio_set( led_ports[i], 1<<led_pins[i]);
#else
        hal_gpio_clr( led_ports[i], 1<<led_pins[i]);
#endif
    }
#endif
}

int hal_led_get_num(void)
{
    return led_num;
}

void hal_led_set(int index)
{
#if HAL_LED_NUM
    #if defined(HAL_LED_REV)
        hal_gpio_clr( led_ports[index], 1<<led_pins[index]);
    #else
        hal_gpio_set( led_ports[index], 1<<led_pins[index]);
    #endif
        led_bitmap |= 1<<index;
#else
    (void)index;
#endif
#if HAL_DBUS
    if( checkDB() )
    {
        sendSignal( "led_set", index );
    }
#endif
}

void hal_led_clr(int index)
{
#if HAL_LED_NUM
    #if defined(HAL_LED_REV)
        hal_gpio_set( led_ports[index], 1<<led_pins[index]);
    #else
        hal_gpio_clr( led_ports[index], 1<<led_pins[index]);
    #endif
        led_bitmap &= ~(1<<index);
#else
    (void)index;
#endif
#if HAL_DBUS
    if( checkDB() )
    {
        sendSignal( "led_clr", index );
    }
#endif
}

void hal_led_toggle(int index)
{
#if HAL_LED_NUM
    hal_gpio_toggle( led_ports[index], 1<<led_pins[index]);
    led_bitmap ^= 1<<index;
#else
    (void)index;
#endif
#if HAL_DBUS
    if( checkDB() )
    {
        sendSignal( "led_toggle", index );
    }
#endif
}

int hal_led_get(int index)
{
#if HAL_LED_NUM
    return (led_bitmap & (1<<index)) ? 1 : 0;
#else
    (void)index;
    return 0;
#endif
}


void hal_led_update(void)
{
#if HAL_LED_NUM
    int i;
    for( i=0; i<HAL_LED_NUM; i++ )
    {
        if( led_bitmap & (1<<i) )
            hal_gpio_set( led_ports[i], 1<<led_pins[i]);
        else
            hal_gpio_clr( led_ports[i], 1<<led_pins[i]);
    }
#endif
}


