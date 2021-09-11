/*
 * Copyright 2019 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "fsl_GPSModule.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define GPS_USART USART1
#define GPS_USART_CLK_SRC kCLOCK_MainClk
#define GPS_USART_CLK_FREQ CLOCK_GetFreq(GPS_USART_CLK_SRC)

#define ECHO_BUFFER_LENGTH 200

/*******************************************************************************
 * Variables
 ******************************************************************************/
usart_handle_t g_usartHandle;
uint8_t g_rxBuffer[ECHO_BUFFER_LENGTH] = {0};

/*******************************************************************************
 * Code
 ******************************************************************************/

void GPS_Init(void)
{
	 usart_config_t config;
	/* Default config by using USART_GetDefaultConfig():
	 * config.baudRate_Bps = 9600U;
	 * config.parityMode = kUSART_ParityDisabled;
	 * config.stopBitCount = kUSART_OneStopBit;
	 * config.bitCountPerChar = kUSART_8BitsPerChar;
	 * config.loopback = false;
	 * config.enableRx = false;
	 * config.enableTx = false;
	 * config.syncMode = kUSART_SyncModeDisabled;
	 */
	USART_GetDefaultConfig(&config);
	config.enableRx = true;
	config.enableTx = true;

	/* Initialize the USART with configuration. */
	USART_Init(GPS_USART, &config, GPS_USART_CLK_FREQ);

	/* Create USART handle, this API will initialize the g_usartHandle and install the callback function. */
	USART_TransferCreateHandle(GPS_USART, &g_usartHandle, NULL, NULL);
}

void GPS_SendCommand(uint8_t *Cmd)
{

	/* Set xfer parameters for sending data. */
	//sendXfer.data = Cmd;
	//sendXfer.dataSize = strlen((char *)Cmd) - 1;

	/* Send Command out. */
	USART_WriteBlocking(GPS_USART, Cmd, strlen((char *)Cmd-1));
}

void GPS_ReceivedData(void)
{

	usart_transfer_t receiveXfer;

	/* Set xfers parameters for receiving data. */
	receiveXfer.data = g_rxBuffer;
	receiveXfer.dataSize = sizeof(g_rxBuffer);

	USART_TransferReceiveNonBlocking(GPS_USART, &g_usartHandle, &receiveXfer, NULL);

}

void GPS_Parse(GPS_location_data_t *Data)
{
	char *pString;
	uint32_t Time;
	uint32_t Date;
	bool NewValidData;

	NewValidData = false;
	pString = (char *)&g_rxBuffer;
	pString = strchr(pString,'$');
	if(strstr(pString,"$GPGGA"))
	{

		if(*(pString + 71) == '*')
		{
			NewValidData = true;
		}

		if(NewValidData)
		{
			pString = strchr(pString,',')+1;
			Time = atoi(pString);
			Data->Hour = Time / 10000;
			Data->Minute = (Time % 10000) / 100;
			Data->Seg = (Time % 100);

			pString = strchr(pString,',')+1;
			memset(Data->BufferDeg,0,sizeof(Data->BufferDeg));
			if(',' != *pString)
			{
				strncpy(Data->BufferDeg,pString,2);
				Data->LatitudDegree = atoi(Data->BufferDeg);
				strncpy(Data->BufferDeg,pString+2,2);
				Data->LatitudMinute = atoi(Data->BufferDeg);
				pString = strchr(pString,'.')+1;
				strncpy(Data->BufferDeg,pString,4);
				Data->LatitudSeg = atoi(Data->BufferDeg);
				pString = strchr(pString,',')+1;
				Data->LatitudCompass = *pString;
			}

			memset(Data->BufferDeg,0,sizeof(Data->BufferDeg));
			pString = strchr(pString,',')+1;
			if(',' != *pString)
			{

				strncpy(Data->BufferDeg,pString,3);
				Data->LongitudDegree = atoi(Data->BufferDeg);
				strncpy(Data->BufferDeg+1,pString+3,2);
				Data->LongitudMinute = atoi(Data->BufferDeg);
				pString = strchr(pString,'.')+1;
				strncpy(Data->BufferDeg,pString,4);
				Data->LongitudSeg = atoi(Data->BufferDeg);
				pString = strchr(pString,',')+1;
				Data->LongitudCompass = *pString;
			}

			pString = strchr(pString,',')+1;
			if(',' != *pString)
			{
				Data->Quality = atoi(pString);
			}


			pString = strchr(pString,',')+1;
			if(',' != *pString)
			{
				Data->Satellites = atoi(pString);
			}

			pString = strchr(pString,',')+1;
			pString = strchr(pString,',')+1;
			if(',' != *pString)
			{
				Data->Altitude = atoi(pString);
			}
		}
	}
	else if(strstr(pString,"$GPRMC"))
	{

		if(*(pString + 67) == '*')
		{
			NewValidData = true;
		}


		if(NewValidData)
		{
			pString = strchr(pString,',')+1;
			Time = atoi(pString);
			Data->Hour = Time / 10000;
			Data->Minute = (Time % 10000) / 100;
			Data->Seg = (Time % 100);

			pString = strchr(pString,',')+1;
			if(',' != *pString)
			{
				if(pString[0] == 'A')
				{
					Data->Fix = true;
				}
				else if(pString[0] == 'V')
				{
					Data->Fix = false;
				}
				else
				{
					//Do nothing
				}
			}


			pString = strchr(pString,',')+1;
			memset(Data->BufferDeg,0,sizeof(Data->BufferDeg));
			if(',' != *pString)
			{
				strncpy(Data->BufferDeg,pString,2);
				Data->LatitudDegree = atoi(Data->BufferDeg);
				strncpy(Data->BufferDeg,pString+2,2);
				Data->LatitudMinute = atoi(Data->BufferDeg);
				pString = strchr(pString,'.')+1;
				strncpy(Data->BufferDeg,pString,4);
				Data->LatitudSeg = atoi(Data->BufferDeg);
				pString = strchr(pString,',')+1;
				Data->LatitudCompass = *pString;
			}

			memset(Data->BufferDeg,0,sizeof(Data->BufferDeg));
			pString = strchr(pString,',')+1;
			if(',' != *pString)
			{

				strncpy(Data->BufferDeg,pString,3);
				Data->LongitudDegree = atoi(Data->BufferDeg);
				strncpy(Data->BufferDeg+1,pString+3,2);
				Data->LongitudMinute = atoi(Data->BufferDeg);
				pString = strchr(pString,'.')+1;
				strncpy(Data->BufferDeg,pString,4);
				Data->LongitudSeg = atoi(Data->BufferDeg);
				pString = strchr(pString,',')+1;
				Data->LongitudCompass = *pString;
			}

			pString = strchr(pString,',')+1;
			if(',' != *pString)
			{
				Data->SpeedI = atoi(pString);
				pString = strchr(pString,'.')+1;
				Data->SpeedF = atoi(pString);
			}

			pString = strchr(pString,',')+1;
			if(',' != *pString)
			{
				Data->AngleI = atoi(pString);
				pString = strchr(pString,'.')+1;
				Data->AngleF = atoi(pString);
			}

			pString = strchr(pString,',')+1;
			if(',' != *pString)
			{
				Date = atoi(pString);
				Data->Day = Date / 10000;
				Data->Month = (Date % 10000) / 100;
				Data->Year = 2000 + (Date % 100);
			}
		}
	}
}
