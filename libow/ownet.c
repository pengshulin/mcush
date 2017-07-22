//
// ownet.c based on Maxim 1-Wire Public Domain Kit, copyright notice below
//
//
//---------------------------------------------------------------------------
// Copyright (C) 2000 Dallas Semiconductor Corporation, All Rights Reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY,  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL DALLAS SEMICONDUCTOR BE LIABLE FOR ANY CLAIM, DAMAGES
// OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
//
// Except as contained in this notice, the name of Dallas Semiconductor
// shall not be used except as stated in the Dallas Semiconductor
// Branding Policy.
//---------------------------------------------------------------------------



#include "ownet.h"
#include "owlink.h"
#include "crcutil.h"
#include "defs.h"

#define MAX_PORTNUM 1
// global variables for this module to hold search state information
static SMALLINT LastDiscrepancy[MAX_PORTNUM];
static SMALLINT LastFamilyDiscrepancy[MAX_PORTNUM];
static SMALLINT LastDevice[MAX_PORTNUM];
uchar SerialNum[MAX_PORTNUM][8];

//--------------------------------------------------------------------------
// The 'owNext' function does a general search.  This function
// continues from the previos search state. The search state
// can be reset by using the 'owFirst' function.
// This function contains one parameter 'alarm_only'.
// When 'alarm_only' is TRUE (1) the find alarm command
// 0xEC is sent instead of the normal search command 0xF0.
// Using the find alarm command 0xEC will limit the search to only
// 1-Wire devices that are in an 'alarm' state.
//
// 'portnum'    - number 0 to MAX_PORTNUM-1.  This number is provided to
//                indicate the symbolic port number.
// 'do_reset'   - TRUE (1) perform reset before search, FALSE (0) do not
//                perform reset before search.
// 'alarm_only' - TRUE (1) the find alarm command 0xEC is
//                sent instead of the normal search command 0xF0
//
// Returns:   TRUE (1) : when a 1-Wire device was found and it's
//                       Serial Number placed in the global SerialNum[portnum]
//            FALSE (0): when no new device was found.  Either the
//                       last search was the last device or there
//                       are no devices on the 1-Wire Net.
//
SMALLINT owNext(int portnum, SMALLINT do_reset, SMALLINT alarm_only)
{
   uchar bit_test, search_direction, bit_number;
   uchar last_zero, serial_byte_number, next_result;
   uchar serial_byte_mask;
   uchar lastcrc8 = 0;

   // initialize for search
   bit_number = 1;
   last_zero = 0;
   serial_byte_number = 0;
   serial_byte_mask = 1;
   next_result = 0;

   // if the last call was not the last one
   if (!LastDevice[portnum])
   {
      // check if reset first is requested
      if (do_reset)
      {
         // reset the 1-wire
         // if there are no parts on 1-wire, return FALSE
         if ( !owTouchReset(/*portnum*/) )
         {
            // printf("owTouchReset failed\r\n");
            // reset the search
            LastDiscrepancy[portnum] = 0;
            LastFamilyDiscrepancy[portnum] = 0;
            //OWERROR(OWERROR_NO_DEVICES_ON_NET);
            return FALSE;
         }
      }

      // If finding alarming devices issue a different command
      if (alarm_only)
         owWriteByte(0xEC);  // issue the alarming search command
      else
         owWriteByte(0xF0);  // issue the search command

      //pause before beginning the search
      //usDelay(100);

      // loop to do the search
      do
      {
         // read a bit and its compliment
         bit_test = owReadBit(/*portnum,1*/) << 1;
         bit_test |= owReadBit(/*portnum,1*/);

         // check for no devices on 1-wire
         if (bit_test == 3)
            break;
         else
         {
            // all devices coupled have 0 or 1
            if (bit_test > 0)
              search_direction = !(bit_test & 0x01);  // bit write value for search
            else
            {
               // if this discrepancy if before the Last Discrepancy
               // on a previous next then pick the same as last time
               if (bit_number < LastDiscrepancy[portnum])
                  search_direction = ((SerialNum[portnum][serial_byte_number] & serial_byte_mask) > 0);
               else
                  // if equal to last pick 1, if not then pick 0
                  search_direction = (bit_number == LastDiscrepancy[portnum]);

               // if 0 was picked then record its position in LastZero
               if (search_direction == 0)
               {
                  last_zero = bit_number;

                  // check for Last discrepancy in family
                  if (last_zero < 9)
                     LastFamilyDiscrepancy[portnum] = last_zero;
               }
            }

            // set or clear the bit in the SerialNum[portnum] byte serial_byte_number
            // with mask serial_byte_mask
            if (search_direction == 1)
              SerialNum[portnum][serial_byte_number] |= serial_byte_mask;
            else
              SerialNum[portnum][serial_byte_number] &= ~serial_byte_mask;

            // serial number search direction write bit
            owWriteBit(/*portnum,*/search_direction);

            // increment the byte counter bit_number
            // and shift the mask serial_byte_mask
            bit_number++;
            serial_byte_mask <<= 1;

            // if the mask is 0 then go to new SerialNum[portnum] byte serial_byte_number
            // and reset mask
            if (serial_byte_mask == 0)
            {
               // The below has been added to accomidate the valid CRC with the
               // possible changing serial number values of the DS28E04.
               if (((SerialNum[portnum][0] & 0x7F) == 0x1C) && (serial_byte_number == 1))
                  crc_check(0x7F, &lastcrc8);
               else
                  crc_check(SerialNum[portnum][serial_byte_number], &lastcrc8);  // accumulate the CRC

               serial_byte_number++;
               serial_byte_mask = 1;
            }
         }
      }
      while(serial_byte_number < 8);  // loop until through all SerialNum[portnum] bytes 0-7

      // if the search was successful then
      if (!((bit_number < 65) || lastcrc8))
      {
         // search successful so set LastDiscrepancy[portnum],LastDevice[portnum],next_result
         LastDiscrepancy[portnum] = last_zero;
         LastDevice[portnum] = (LastDiscrepancy[portnum] == 0);
         next_result = TRUE;
      }
   }

   // if no device found then reset counters so next 'next' will be
   // like a first
   if (!next_result || !SerialNum[portnum][0])
   {
      LastDiscrepancy[portnum] = 0;
      LastDevice[portnum] = FALSE;
      LastFamilyDiscrepancy[portnum] = 0;
      next_result = FALSE;
   }

   return next_result;
}

//--------------------------------------------------------------------------
// The 'owFirst' finds the first device on the 1-Wire Net  This function
// contains one parameter 'alarm_only'.  When
// 'alarm_only' is TRUE (1) the find alarm command 0xEC is
// sent instead of the normal search command 0xF0.
// Using the find alarm command 0xEC will limit the search to only
// 1-Wire devices that are in an 'alarm' state.
//
// 'portnum'    - number 0 to MAX_PORTNUM-1.  This number is provided to
//                indicate the symbolic port number.
// 'do_reset'   - TRUE (1) perform reset before search, FALSE (0) do not
//                perform reset before search.
// 'alarm_only' - TRUE (1) the find alarm command 0xEC is
//                sent instead of the normal search command 0xF0
//
// Returns:   TRUE (1) : when a 1-Wire device was found and it's
//                        Serial Number placed in the global SerialNum[portnum]
//            FALSE (0): There are no devices on the 1-Wire Net.
//
SMALLINT owFirst(int portnum, SMALLINT do_reset, SMALLINT alarm_only)
{
   // reset the search state
   LastDiscrepancy[portnum] = 0;
   LastDevice[portnum] = FALSE;
   LastFamilyDiscrepancy[portnum] = 0;

   return owNext(portnum,do_reset,alarm_only);
}


//--------------------------------------------------------------------------
// The 'owSerialNum' function either reads or sets the SerialNum buffer
// that is used in the search functions 'owFirst' and 'owNext'.
// This function contains two parameters, 'serialnum_buf' is a pointer
// to a buffer provided by the caller.  'serialnum_buf' should point to
// an array of 8 unsigned chars.  The second parameter is a flag called
// 'do_read' that is TRUE (1) if the operation is to read and FALSE
// (0) if the operation is to set the internal SerialNum buffer from
// the data in the provided buffer.
//
// 'portnum'       - number 0 to MAX_PORTNUM-1.  This number is provided to
//                   indicate the symbolic port number.
// 'serialnum_buf' - buffer to that contains the serial number to set
//                   when do_read = FALSE (0) and buffer to get the serial
//                   number when do_read = TRUE (1).
// 'do_read'       - flag to indicate reading (1) or setting (0) the current
//                   serial number.
//
void owSerialNum(int portnum, uchar *serialnum_buf, SMALLINT do_read)
{
   uchar i;

   // read the internal buffer and place in 'serialnum_buf'
   if (do_read)
   {
      for (i = 0; i < 8; i++)
         serialnum_buf[i] = SerialNum[portnum][i];
   }
   // set the internal buffer from the data in 'serialnum_buf'
   else
   {
      for (i = 0; i < 8; i++)
         SerialNum[portnum][i] = serialnum_buf[i];
   }
}
