/**
  @page LTDC_Display_2Layers LTDC Display 2 layers example
  
  @verbatim
  ******************** (C) COPYRIGHT 2016 STMicroelectronics *******************
  * @file    LTDC/LTDC_Display_2Layers/readme.txt 
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Description of the LTDC Display 2 layers example.
  ******************************************************************************
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  @endverbatim

@par Example Description

This example shows how to configure the LTDC peripheral to display two Layers 
at the same time.
The goal of this example is to explain how to blend two layers and how to display 
an image with L8 (8bits per pixels indexed) as pixel format. 
 
After LCD initialization, the LCD layer 1 and Layer 2 are configured as following :
 - Layer 1 is configured to display an image loaded from flash memory with direct
   color (RGB565) as pixel format and 320x240 size.
 - Layer 2 is configured to display an image loaded from flash memory with indirect
   color (L8) as pixel format and 320x240 size.
   To display an image with an indirect color as pixel format a color lookup table (CLUT) 
   is loaded, then every byte from image data (L8_320x240) is considered as 
   a position in color lookup table (CLUT).(see example below)  

The blending is always active and the two layers can be blended following 
the configured blending factors and the constant alpha.
In this example the constant alpha for layer 2 is decreased to see the layer 1
in the intersection zone.

  Example:
  ------------------------------------------------------------------------------
  |Image data   |  position   CLUT(RGB Value)  |   Alpha  | Output (ARGB value)|
  |-------------|------------------------------|----------|------------------- |
  |00 10 FF 25  |  0   -----> 0xFD10EA         | 0xFF     | 0xFFFD10EA         |
  |             |  .                           |          | 0xFF1548AD         |
  |             |  .                           |          | 0xFFAE6547         |
  |             |  .                           |          | 0xFFDA14EA         |
  |             |  .                           |          | .                  |
  |             |  16  -----> 0x1548AD         |          | .                  |
  |             |  .                           |          | .                  |
  |             |  .                           |          | .                  |
  |             |  37  -----> 0xDA14EA         |          | .                  |
  |             |  .                           |          |                    |
  |             |  .                           |          |                    |
  |             |  255 -----> 0xAE6547         |          |                    |
  ------------------------------------------------------------------------------       


@par Directory contents
    
  - LTDC/LTDC_Display_2Layers/system_stm32f4xx.c   STM32F4xx system clock configuration file
  - LTDC/LTDC_Display_2Layers/stm32f4xx_conf.h     Library Configuration file
  - LTDC/LTDC_Display_2Layers/stm32f4xx_it.c       Interrupt handlers
  - LTDC/LTDC_Display_2Layers/stm32f4xx_it.h       Interrupt handlers header file
  - LTDC/LTDC_Display_2Layers/main.c               Main program
  - LTDC/LTDC_Display_2Layers/main.h               Main program header file
  - LTDC/LTDC_Display_2Layers/RGB565_320x240.h     Image used for LTDC Validation
  - LTDC/LTDC_Display_2Layers/L8_320x240.h         Image used for LTDC Validation


@par Hardware and Software environment
  
  - This example runs on and STM32F429xx/439xx devices.
    
  - This example has been tested with STMicroelectronics STM32429I-EVAL 
    (STM32F429xx/STM32F439xx Devices) evaluation boards and can be easily 
    tailored to any other supported device and development board.
  
  - This example has been tested with STM324x9I-EVAL RevB board which includes
    the MB1046 LCD board. 
    

@par How to use it ?

In order to make the program work, you must do the following:
 - Copy all source files from this example folder to the template folder under
   Project\STM32F4xx_StdPeriph_Templates
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example
  
 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
                                   