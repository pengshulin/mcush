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
#ifndef FSL_SSD1306_H_
#define FSL_SSD1306_H_

#include "fsl_common.h"
#include "fsl_Font5x7.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define OLED_SETCONTRAST 							(0x81)
#define OLED_DISPLAYALLON_RESUME 					(0xA4)
#define OLED_DISPLAYALLON 							(0xA5)
#define OLED_NORMALDISPLAY 							(0xA6)
#define OLED_INVERTDISPLAY 							(0xA7)
#define OLED_DISPLAYOFF 							(0xAE)
#define OLED_DISPLAYON 								(0xAF)
#define OLED_SETDISPLAYOFFSET 						(0xD3)
#define OLED_SETCOMPINS 							(0xDA)
#define OLED_SETVCOMDETECT 							(0xDB)
#define OLED_SETDISPLAYCLOCKDIV 					(0xD5)
#define OLED_SETPRECHARGE 							(0xD9)
#define OLED_SETMULTIPLEX 							(0xA8)
#define OLED_SETLOWCOLUMN 							(0x00)
#define OLED_SETHIGHCOLUMN 							(0x10)
#define OLED_SETSTARTLINE 							(0x40)
#define OLED_MEMORYMODE 							(0x20)
#define OLED_COLUMNADDR 							(0x21)
#define OLED_PAGEADDR   							(0x22)
#define OLED_COMSCANINC 							(0xC0)
#define OLED_COMSCANDEC 							(0xC8)
#define OLED_SEGREMAP 								(0xA0)
#define OLED_CHARGEPUMP 							(0x8D)
#define OLED_EXTERNALVCC 							(0x01)
#define OLED_SWITCHCAPVCC 							(0x02)
#define OLED_ACTIVATE_SCROLL 						(0x2F)
#define OLED_DEACTIVATE_SCROLL 						(0x2E)
#define OLED_SET_VERTICAL_SCROLL_AREA 				(0xA3)
#define OLED_RIGHT_HORIZONTAL_SCROLL 				(0x26)
#define OLED_LEFT_HORIZONTAL_SCROLL 				(0x27)
#define OLED_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 	(0x29)
#define OLED_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL 	(0x2A)

/* OLED Data/Command PIN. */
#ifndef BOARD_OLED_DC_GPIO
#define BOARD_OLED_DC_GPIO GPIO
#endif
#define BOARD_OLED_DC_GPIO_PORT 0U
#ifndef BOARD_OLED_DC_GPIO_PIN
#define BOARD_OLED_DC_GPIO_PIN 	1U
#endif

/* OLED Reset PIN. */
#ifndef BOARD_OLED_RST_GPIO
#define BOARD_OLED_RST_GPIO GPIO
#endif
#define BOARD_OLED_RST_GPIO_PORT 0U
#ifndef BOARD_OLED_RST_GPIO_PIN
#define BOARD_OLED_RST_GPIO_PIN  16U
#endif

/* OLED CS PIN. */
#ifndef BOARD_OLED_CS_GPIO
#define BOARD_OLED_CS_GPIO GPIO
#endif
#define BOARD_OLED_CS_GPIO_PORT 0U
#ifndef BOARD_OLED_CS_GPIO_PIN
#define BOARD_OLED_CS_GPIO_PIN 	20U
#endif

/* OLED Width*/
#ifndef OLED_WIDTH
#define OLED_WIDTH 128
#endif

/* OLED Height*/
#ifndef OLED_HEIGHT
#define OLED_HEIGHT 64
#endif

/*! @brief OLED display mode. */
enum _OLED_Display_Mode
{
	kOLED_Display_Normal,
	kOLED_Dislpay_Invert
};

/*! @brief OLED Set/Clear select. */
enum _OLED_Pixel_CLR_SET
{
	kOLED_Pixel_Clear,
	kOLED_Pixel_Set
};


/*******************************************************************************
 * API
 ******************************************************************************/

/*!
 * @brief Init the Pins for the OLED.
 *
 * This function init the SPI and GPIO Pins for the OLED.
 *
 * @param void.
 */
void OLED_Init(void);

/*!
 * @brief Refresh the OLED
 *
 * This function refresh the OLED.
 *
 * @param void.
 */
void OLED_Refresh(void);

/*!
 * @brief Clear the buffer.
 *
 * This function clear (with 0) the buffer for the display.
 *
 * @param void.
 */
void OLED_Clear(void);

/*!
 * @brief Fill the Buffer
 *
 * This function fill the buffer with the Pattern that received.
 *
 * @param pattern witch the buffer is going to fill.
 */
void OLED_Fill(uint8_t Pattern);

/*!
 * @brief Select the mode for the Display
 *
 * This function select between normal or invert display mode.
 *
 * @param choose the mode.
 */
void OLED_Display_Mode (uint8_t Mode);

/*!
 * @brief Set o Clear a pixel in the buffer.
 *
 * This function Set or Clear a pixel in the buffer for the Display.
 *
 * @param X_axis Coordinate in the x axis.
 * @param Y_axis Coordinate in the y axis.
 * @param SC Select between Set or Clear.
 */
void OLED_Set_Pixel (uint8_t X_axis, uint8_t Y_axis, uint8_t SC);

/*!
 * @brief Set text in the buffer.
 *
 * This function Set a string text in the buffer for the Display.
 *
 * @param X_axis Coordinate in the x axis.
 * @param Y_axis Coordinate in the y axis.
 * @param SC Select between Set or Clear.
 * @param String The text string to be sending.
 * @param Scale Select the size of the Text.
 */
void OLED_Set_Text (uint8_t X_axis, uint8_t Y_axis, uint8_t SC, char* String, uint8_t Scale);

/*!
 * @brief Set o Clear a group pixels in the buffer.
 *
 * This function Set or Clear a group of pixels in the buffer for the Display.
 *
 * @param X_axis Coordinate in the x axis.
 * @param Y_axis Coordinate in the y axis.
 * @param Width size of width to set or clear.
 * @param Height size of height to set or clear.
 * @param SC Select between Set or Clear.
 */
void OLED_Fill_Rect (uint8_t X_axis, uint8_t Y_axis, uint8_t Width, uint8_t Height, uint8_t SC);

void OLED_Copy_Image(const uint8_t *Img, uint16_t size);

#endif /* FSL_SSD1306_H_ */
