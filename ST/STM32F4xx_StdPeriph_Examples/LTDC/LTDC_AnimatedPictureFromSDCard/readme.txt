/**
  @page LTDC_AnimatedPictureFromSDCard LTDC Animated Picture From SD Card example
  
  @verbatim
  ******************************************************************************
  * @file    LTDC/LTDC_AnimatedPictureFromSDCard/readme.txt 
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Description of the LTDC Animated Picture From SD Card example.
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

This example shows how to display on LCD an animated picture saved under
microSD card.

The animated picture is the display of a sequence of images with a determined 
frequency to be seen like one animated image.

The user has to copy the two directories "/BACK" and "/TOP" available with this 
project under the micro SD card root. 
 
  * Background picture display
  ----------------------------           
  Once the LCD, SD card and file system are initialized and configured, a check
  of the existence and the content of the "/BACK" directory is done.

  * Foreground animated pictures display
  -------------------------------------  
  A content check of the "/TOP" directory is done and the number of ".BMP" files 
  is retained.

 @note The maximum number of BMP file is fixed at 25. It can be raised 
       until reaching the maximum of SD card memory space.
 
The following steps are performed to scroll all the images stored in the SD card:

  - The foreground layer is set, the image copied from SD card (from "/TOP" directory)
    to intermediate SDRAM memory space and then copied to LCD frame buffer. 
 
  - The color keying feature is applied to remove the bottom of foreground layer (transparent)
    and then replaced by the background layer.
 
  - Jump to next image  

 @note => If the "/Foreground" directory is empty, a warning message is  
          displayed on the LCD: "  No Bitmap files...  "
    
       => If the file type stored in the "/BACK" or "/TOP" directories is not 
          supported, a warning message is displayed on the LCD: " file type not 
          supported. "
          
       => If the SD card is not inserted, a warning message is displayed on the 
          LCD: " Please insert SD Card. "    

 @note This example can be tested with AMPIRE640x480 (MB1046) or with 
       AMPIRE480x272 (MB1063)
      
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
  - The two directories "BACK" and "TOP" must be copied at the micro SD card root.    
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
                                   