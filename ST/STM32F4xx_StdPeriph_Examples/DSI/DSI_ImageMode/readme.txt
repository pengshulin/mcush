/**
  @page DSI_ImageMode example
  
  @verbatim
  ******************** (C) COPYRIGHT 2016 STMicroelectronics *******************
  * @file    DSI/DSI_ImageMode/readme.txt 
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Description of the DSI Image Mode example.
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

This example shows how to configure DSI peripheral in Command mode and display the result
on LCD.

In this basic example the goal is to explain how to use different DSI commands to add an effect
on the image.

After DSI configuration, the data transfer is performed from FLASH memory to
LTDC. The LTDC is configured to display transferred picture on LCD (Only Layer 1 is used) 
(more details about LTDC configuration in LTDC examples)

Then, different delayed commands are used to add an effect on the image, each time we add an effect
we restore the image to its first view.
  
In this example, the image used has the following parameters:
- Size : 480 x 800 
- Color mode : RGB565 format 


How to customize the used picture:

Generate a C file using the "BmpCvtST" software Tool available under: Utilities\ST\STemWin\Software:
- Select the format High Color(565)  to use RGB565 format. 
- Select the format true color 24bpp to use RGB888 format.

Replace the table in "displayed_picture_rgbXXX.h" by the new table in the generated file .

Note :
Please ensure that your picture has same size used in the example.
JPEG format is not supported by the software Tool.

@par Directory contents
    
  - DSI/DSI_ImageMode/system_stm32f4xx.c   		STM32F4xx system clock configuration file
  - DSI/DSI_ImageMode/stm32f4xx_conf.h     		Library Configuration file
  - DSI/DSI_ImageMode/stm32f4xx_it.c       		Interrupt handlers
  - DSI/DSI_ImageMode/stm32f4xx_it.h       		Interrupt handlers header file
  - DSI/DSI_ImageMode/main.c               		Main program
  - DSI/DSI_ImageMode/main.h               		Main program header file
  - DSI/DSI_ImageMode/displayed_picture_rgb565.h	Image to be displayed


@par Hardware and Software environment 
 
  - This example runs on and STM32F469xx/479xx devices.
    
  - This example has been tested with STMicroelectronics STM32469I-EVAL 
    (STM32F469xx/STM32F479xx Devices) evaluation boards and can be easily 
    tailored to any other supported device and development board.
  
  - This example has been tested with STM32469I-EVAL RevB board which includes
    the MB1166 REV A LCD board. 

  - To use LCD hardware back-light, a hardware and software modification should be performed as below:
     * Software update: comment the OTM8008A_BACKLIGHT_SOFTWARE define line in main.c
     * Hardware update: remove R1 resistor connected on MB1166 REVA board
@par How to use it ?

In order to make the program work, you must do the following:
 - Copy all source files from this example folder to the template folder under
   Project\STM32F4xx_StdPeriph_Templates
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example
  
 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
 