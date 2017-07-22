/*
 * Copyright (c) 2012, Jens Nielsen
 * All rights reserved.

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the <organization> nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL JENS NIELSEN BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include "ownet.h"
#include "owlink.h"
#include <string.h> /*for memset*/
#include "ds18b20.h"

device_t devices[ NUM_DEVICES ];

typedef enum state_e
{
    STATE_SCAN,
    STATE_CONVERT,
    STATE_WAIT_CONVERT,
    STATE_FETCH_TEMPS,
}state_t;

static state_t state;


static void ds18b20_scan(void);
static char ds18b20_isParasite();
static void ds18b20_startConvert(void);
static void ds18b20_fetchTemp( unsigned char device );

void ds18b20_init()
{
    memset( devices, 0, sizeof(devices) );

    state = STATE_SCAN;
}

void ds18b20_work()
{
    static unsigned char fetchDevice = 0;
    switch ( state )
    {
        case STATE_SCAN:
            ds18b20_scan();
            state = STATE_CONVERT;
            break;

        case STATE_CONVERT:
            ds18b20_startConvert();
            state = STATE_WAIT_CONVERT;
            break;

        case STATE_WAIT_CONVERT:
            if ( owReadBit() != 0 )
            {
                state = STATE_FETCH_TEMPS;
                fetchDevice = 0;
            }
            break;

        case STATE_FETCH_TEMPS:
            ds18b20_fetchTemp( fetchDevice );
            fetchDevice++;
            if ( fetchDevice >= NUM_DEVICES )
                state = STATE_CONVERT;
            break;
    }
}

static void ds18b20_scan()
{
    unsigned char found;
    unsigned char i;

    found = owFirst( 0, 1, 0 );
    if ( found )
    {
        owSerialNum( 0, devices[ 0 ].serial, 1 );
    }

    for ( i = 1; found && i < NUM_DEVICES; i++ )
    {
        found = owNext( 0, 1, 0 );
        if ( found )
        {
            owSerialNum( 0, devices[ i ].serial, 1 );
        }
    }
}

static char ds18b20_isParasite()
{
    owTouchReset();
    owWriteByte(0xCC);
    owWriteByte(0xB4);
    return ( owReadBit() == 0 );
}

static void ds18b20_startConvert()
{
    /*issue convert temp to all devices*/
    owTouchReset();
    owWriteByte(0xCC);
    owWriteByte(0x44);
}

static void ds18b20_fetchTemp( unsigned char device )
{
    if ( device < NUM_DEVICES && devices[ device ].serial[ 0 ] != 0 )
    {
        unsigned char i;
        unsigned char b1, b2;

        /*address specified device*/
        owTouchReset();
        owWriteByte(0x55);
        for (i = 0; i < 8; i++)
        {
            owWriteByte(devices[ device ].serial[ i ]);
        }

        /*read the first two bytes of scratchpad*/
        owWriteByte(0xBE);
        b1 = owReadByte();
        b2 = owReadByte();

        devices[ device ].lastTemp = ( (signed short) b2 << 8 ) | ( b1 & 0xFF );
        devices[ device ].ftemp = devices[ device ].lastTemp;
        devices[ device ].ftemp *= 0.0625;
    }
}

