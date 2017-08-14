/**
  @page LTDC_Display_1Layer LTDC Display Layer 1 example
  
  @verbatim
  ******************** (C) COPYRIGHT 2016 STMicroelectronics *******************
  * @file    LTDC/LTDC_Display_1Layer/readme.txt 
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Description of the LTDC Display Layer 1 example.
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

This example shows how to configure LTDC peripheral to display BMP image on 
LCD using only one layer.

In this basic example the goal is to explain the different fields of the LTDC 
structure. 

After LCD initialization, the LCD layer 1 is configured to display image 
(modeled by an array) loaded from flash memory. 

  LCD_TFT synchronous timings configuration :
  -------------------------------------------
 
                                             Total Width
                          <--------------------------------------------------->
                    Hsync width HBP             Active Width                HFP
                          <---><--><--------------------------------------><-->
                      ____    ____|_______________________________________|____ 
                          |___|   |                                       |    |
                                  |                                       |    |
                      __|         |                                       |    |
         /|\    /|\  |            |                                       |    |
          | VSYNC|   |            |                                       |    |
          |Width\|/  |__          |                                       |    |
          |     /|\     |         |                                       |    |
          |  VBP |      |         |                                       |    |
          |     \|/_____|_________|_______________________________________|    |
          |     /|\     |         | / / / / / / / / / / / / / / / / / / / |    |
          |      |      |         |/ / / / / / / / / / / / / / / / / / / /|    |
 Total    |      |      |         |/ / / / / / / / / / / / / / / / / / / /|    |
 Height    |      |      |         |/ / / / / / / / / / / / / / / / / / / /|    |
          |Active|      |         |/ / / / / / / / / / / / / / / / / / / /|    |
          |Height |      |         |/ / / / / / Active Display Area / / / /|    |
          |      |      |         |/ / / / / / / / / / / / / / / / / / / /|    |
          |      |      |         |/ / / / / / / / / / / / / / / / / / / /|    |
          |      |      |         |/ / / / / / / / / / / / / / / / / / / /|    |
          |      |      |         |/ / / / / / / / / / / / / / / / / / / /|    |
          |      |      |         |/ / / / / / / / / / / / / / / / / / / /|    |
          |     \|/_____|_________|_______________________________________|    |
          |     /|\     |                                                      |
          |  VFP |      |                                                      |
         \|/    \|/_____|______________________________________________________|
         
         
  Each LCD device has its specific synchronous timings values.
  This example uses AM480272H3TMQW-T01H LCD (MB1046 Rev.A) and configures 
  the synchronous timings as follows:

  Horizontal Synchronization (Hsync) = 41
  Horizontal Back Porch (HBP)        = 2
  Active Width                       = 480
  Horizontal Front Porch (HFP)       = 2

  Vertical Synchronization (Vsync)   = 10
  Vertical Back Porch (VBP)          = 2
  Active Height                       = 272
  Vertical Front Porch (VFP)         = 2
  
  LCD_TFT windowing configuration :
  ---------------------------------

  To configure the active display window, this example configures the 
  horizontal/vertical start and stop. 

  HorizontalStart = (Offset_X + Hsync + HBP);
  HorizontalStop  = (Offset_X + Hsync + HBP + Window_Width - 1); 
  Verticalstart   = (Offset_Y + Vsync + VBP);
  VerticalStop    = (Offset_Y + Vsync + VBP + Window_Heigh - 1);
  
  Window_width and Window_heigh should be in line with the image size to be 
  displayed.


@par Directory contents
    
  - LTDC/LTDC_Display_1Layer/system_stm32f4xx.c   STM32F4xx system clock configuration file
  - LTDC/LTDC_Display_1Layer/stm32f4xx_conf.h     Library Configuration file
  - LTDC/LTDC_Display_1Layer/stm32f4xx_it.c       Interrupt handlers
  - LTDC/LTDC_Display_1Layer/stm32f4xx_it.h       Interrupt handlers header file
  - LTDC/LTDC_Display_1Layer/main.c               Main program
  - LTDC/LTDC_Display_1Layer/main.h               Main program header file
  - LTDC/LTDC_Display_1Layer/RGB565_480x272.h     Image to be displayed


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
 