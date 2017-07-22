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


#include "defs.h"
#include "owhal.h"


#define OW_SLOT_LENGTH 78

#define OW_LOW_PULSE_TIME 3
#define OW_READ_SAMPLE_TIME 12
#define OW_READ_SAMPLE_WAIT ( OW_READ_SAMPLE_TIME - OW_LOW_PULSE_TIME )
#define OW_END_READ_SLOT_WAIT ( OW_SLOT_LENGTH - OW_READ_SAMPLE_TIME )

#define OW_RECOVERY_TIME 6
#define OW_WRITE_SLOT_WAIT ( OW_SLOT_LENGTH - OW_LOW_PULSE_TIME )

#define OW_RESET_PULSE_TIME 520

char owReadBit()
{
    char bit_in;

    /*pull low to initiate read*/
    OW_OUTPUT();
    OW_LOW();

    __delay_us( OW_LOW_PULSE_TIME );

    /*float high*/
    OW_INPUT();
    __delay_us( OW_READ_SAMPLE_WAIT );

    /*sample bus*/
    bit_in = OW_READ();

    /*wait until end of time slot*/
    __delay_us( OW_END_READ_SLOT_WAIT );

    return bit_in;
}

void owWriteBit( char bit_out )
{
    /*pull low to initiate write*/
    OW_OUTPUT();
    OW_LOW();
    __delay_us( OW_LOW_PULSE_TIME );

    /*write next bit*/
    if (bit_out & 0x01)
        OW_HIGH();
    else
        OW_LOW();
    
    /*wait until end of slot*/
    __delay_us( OW_WRITE_SLOT_WAIT );

    /*float high and let device recover*/
    OW_INPUT();
    __delay_us( OW_RECOVERY_TIME );

}

char owTouchReset()
{
    char presence = FALSE;
    char sample_count = ( OW_RESET_PULSE_TIME / 8 );
    char i = 0;

    /*low reset pulse*/
    OW_OUTPUT();
    OW_LOW();

    /* delay during reset pulse time, split it in 10 chunks in case __delay_us
     * can't handle big numbers */
    for ( i = 0; i < 10; i++ )
        __delay_us( OW_RESET_PULSE_TIME / 10 );

    /*float high*/
    OW_INPUT();

    while ( sample_count-- != 0 )
    {
        __delay_us(8);

        /*sample bus to check for connected devices*/
        if ( OW_READ() == 0 )
            presence = TRUE;
    }

    return presence;
}

void owWriteByte(unsigned char data)
{
    unsigned char bit_count;

    for (bit_count = 0; bit_count < 8; bit_count++)
    {
        /*pull low to initiate write*/
        OW_OUTPUT();
        OW_LOW();
        __delay_us( OW_LOW_PULSE_TIME );

        /*write next bit*/
        if (data & 0x01)
            OW_HIGH();
        else
            OW_LOW();

        data = data >> 1;

        /*wait until end of slot*/
        __delay_us( OW_WRITE_SLOT_WAIT );

        /*float high and let device recover*/
        OW_INPUT();
        __delay_us( OW_RECOVERY_TIME );
    }
}

unsigned char owReadByte()
{
    unsigned char bit_count;
    unsigned char data;

    data = 0;

    for ( bit_count = 0; bit_count < 8; bit_count++ )
    {
        /*pull low to initiate read*/
        OW_OUTPUT();
        OW_LOW();
        __delay_us(OW_LOW_PULSE_TIME);

        /*float high*/
        OW_INPUT();
        __delay_us(OW_READ_SAMPLE_WAIT);

        /*sample bus and shift into msb*/
        data = data >> 1;
        if (OW_READ() != 0)
            data |= 0x80;

        /*wait until end of time slot*/
        __delay_us(OW_END_READ_SLOT_WAIT);
    }
    return data;
}
