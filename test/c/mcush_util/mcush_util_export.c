#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#ifdef WIN32
#include <windows.h>
#endif
#include "mcush_api.h"
#ifdef WIN32
#define EXPORT __declspec (dllexport)
#else
#define EXPORT __attribute__((visibility("default")))
#endif


EXPORT int mcush_command( const char *port, const char *command )
{
    mcush_dev_t dev;
    int ret;

    if( (port == NULL) || (command == NULL) )
        return MCUSH_ERR_API;

    ret = mcush_open( &dev, port, 9600, 5 );
    if( ret <= 0 )
        return ret;

    ret = mcush_connect( &dev );
    if( ret <= 0 )
        goto stop;

    ret = mcush_write_command( &dev, command );

stop:
    mcush_close( &dev );
    return ret;
}

