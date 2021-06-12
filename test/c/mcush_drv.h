#ifndef __MCUSH_DRV_H__
#define __MCUSH_DRV_H__

#include "mcush_api.h"

#pragma once

#ifndef __cplusplus
#error "This SDK is for C++ compiler"
#endif



class McushDrv {
private:
    mcush_dev_t device;

public:
    int err;

    McushDrv( const char *ttyname, int baudrate, int timeout );
    McushDrv( const char *ttyname, int baudrate );
    McushDrv( const char *ttyname );
    ~McushDrv();

    int connect();
    int disconnect();

    int scpiIdn(char *output);
    int scpiRst();

};



#endif
