/**
  @page LTDC_PicturesFromSDCard LTDC Pictures From SD Card example
  
  @verbatim
  ******************************************************************************
  * @file    LTDC/LTDC_PicturesFromSDCard/readme.txt 
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Description of the LTDC Pictures From SD Card example.
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
  * limitations under the License
  *   
  ******************************************************************************
  @endverbatim

@par Example Description

This example shows how to display on LCD pictures saved under SD card.

The user has to copy the directory all pictures from "PICTURES" directory 
available with this project under a dedicated directory named as "PICT" in
the micro SD card root. 
 
 @note The user can add pictures of his choice but the file type, resolution
       and pixel format described below must be respected.
           
Once the LCD, SD card and file systems are initialized and configured, 
a content check of the "PICT" directory is done and the number of ".BMP" files 
is retained.
  
 @note An associate LCD eval driver is used in this example 
  
 @note The maximum number of BMP file is fixed at 25. It can be raised 
       until reaching the maximum of SD card memory space.
           
The following steps are performed to scroll all the images stored in the SD card:

  Step1:
  ------
  The foreground layer is set, the image copied from SD card to intermediate
  SDRAM memory and then copied to LCD frame buffer. The image is fully visible when 
  the level of transparency is increased until it becomes totally opaque (reaches 255).
 
  Press tamper button to display next image.
 
  Step2:
  ------
  When the TAMPER button is pressed the transparency of the foreground layer
  begins to decrease until become totally transparent and the background layer 
  is set, the image copied from SD card to intermediate SDRAM memory and then 
  copied to LCD frame buffer and finally the level of transparency of the 
  background layer increase until it reaches 255 (totally opaque). 
            
  Press TAMPER button to display next image.
 
  Step3:
  ------
  When the TAMPER button is pressed the transparency of the background layer
  begins to decrease until become totally transparent and return to Step1.
  

 @note  => If the "PICT" directory is empty, a warning message is displayed on 
           the LCD: "  No Bitmap files...  "
               
        => If the file type stored in the "PICT" directory is not supported,
           a warning message is displayed on the LCD : " file type not supported. "
    
        => If the SD card is not inserted, a warning message is displayed on the 
           LCD: " Please insert SD card. "     

 @note This example can be tested with AMPIRE640x480 or with AMPIRE480x272
  
   
@par Directory contents   
    
  - LTDC/LTDC_PicturesFromSDCard/system_stm32f4xx.c   STM32F4xx system clock configuration file
  - LTDC/LTDC_PicturesFromSDCard/stm32f4xx_conf.h     Library Configuration file
  - LTDC/LTDC_PicturesFromSDCard/stm32f4xx_it.c       Interrupt handlers
  - LTDC/LTDC_PicturesFromSDCard/stm32f4xx_it.h       Interrupt handlers header file
  - LTDC/LTDC_PicturesFromSDCard/main.c               Main program
  - LTDC/LTDC_PicturesFromSDCard/main.h               Main program header file
  - LTDC/LTDC_PicturesFromSDCard/ffconf.h             Configuration file for FatFs module
  - LTDC/LTDC_PicturesFromSDCard/fatfs_storage.c      Storage (FatFs) driver
  - LTDC/LTDC_PicturesFromSDCard/fatfs_storage.h      Storage (FatFs) driver header file
  - LTDC/LTDC_PicturesFromSDCard/fatfs_drv.c          diskio interface


@par Hardware and Software environment  

  - This example runs on and STM32F429xx/439xx devices.
    
  - This example has been tested with STMicroelectronics STM324x9I-EVAL RevB 
    (STM32F429xx/STM32F439xx Devices) evaluation boards and can be easily 
    tailored to any other supported device and development board.

  - This example has been tested with STM324x9I-EVAL RevB board which includes
    the MB1046 or MB1063 LCD daughter board.
    

@par How to use it ?

In order to make the program work, you must do the following:
  - The bitmap images should be copied inside a dedicated directory named as "PICT" 
    at the micro SD card root.
    The images should have the following properties:
     file type    : *.bmp
     resolution   : up to 480x272 with AM480272H3TMQW LCD and up to 640x480 with 
                    AM-640480G5TNQW-T00H LCD
     pixel format : RGB565, RGB888 or ARGB8888     
 - Copy all source files from this example folder to the template folder under
   Project\STM32F4xx_StdPeriph_Templates
 - Open your preferred toolchain
 - Select "STM32F429_439xx" workspace related to the used device and Add 
   the following files in the project source list:
     - Utilities\STM32_EVAL\STM324x9I_EVAL\stm324x9i_eval.c
     - Utilities\STM32_EVAL\STM324x9I_EVAL\stm324x9i_eval_lcd.c 
     - Utilities\STM32_EVAL\STM324x9I_EVAL\stm324x9i_eval_fmc_sdram.c
     - Utilities\STM32_EVAL\STM324x9I_EVAL\stm324x9i_eval_ioe8.c
     - Utilities\STM32_EVAL\STM324x9I_EVAL\stm324x9i_eval_ioe16.c
     - Utilities\STM32_EVAL\STM324x9I_EVAL\stm324x9i_eval_sdio_sd.c
     - Utilities\Third_Party\fat_fs\src\ff.c  
     - fatfs_storage.c
     - fatfs_drv.c
 - Add the include path of the directory : Utilities\Third_Party\fat_fs\inc
 - Rebuild all files and load your image into target memory
 - Run the example

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
                                   