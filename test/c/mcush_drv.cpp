/* MCUSH device CPP-Driver demo
 * control one/multi devices in CPP
 * MCUSH designed by Peng Shulin, all rights reserved.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mcush_drv.h"


McushDrv::McushDrv( const char *ttyname, int baudrate, int timeout )
{
    this->err = mcush_open( &this->device, ttyname, baudrate, timeout );
}

McushDrv::McushDrv( const char *ttyname, int baudrate )
{
    this->err = mcush_open( &this->device, ttyname, baudrate, 5 );
}

McushDrv::McushDrv( const char *ttyname )
{
    this->err = mcush_open( &this->device, ttyname, 9600, 5 );
}

McushDrv::~McushDrv()
{
    this->err = mcush_close( &this->device );
}


int McushDrv::connect()
{
    return mcush_connect( &this->device );
}



int McushDrv::scpiIdn(char *output)
{
    return mcush_scpi_idn( &this->device, output );
}


int McushDrv::scpiRst()
{
    return mcush_scpi_rst( &this->device );
}
