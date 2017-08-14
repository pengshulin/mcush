/**
  @page DMA2D_MemToMemWithPFC DMA2D memory to memory with PFC example
  
  @verbatim
  ******************** (C) COPYRIGHT 2016 STMicroelectronics *******************
  * @file    DMA2D/DMA2D_MemToMemWithPFC/readme.txt 
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Description of the DMA2D memory to memory with PFC example.
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

This example shows how to configure DMA2D peripheral in Memory to Memory with 
pixel format conversion transfer mode.
 
In this basic example the goal is to explain the different fields of the DMA2D 
structure in the case of Memory_to_Memory with pixel format conversion transfer mode
and the difference between pixel coded on 32bits and coded on 16bits.
 
An image is transferred from flash memory to internal RAM and during the transfer,
a pixel format conversion is applied from ARGB8888 to ARGB4444. 
The original image and the transferred image are displayed on the LCD to see 
the difference between an image coded on 16 bits and an image coded on 32 bits.
 
In this example two LTDC layers are used to display the original and the converted
images as follows:
- Layer 1 is configured to display the original image with ARGB8888 as 
  pixel format and 320x120 size.
- Layer 2 is configured to display the converted image with ARGB4444 as 
  pixel format and 320x120 size.

 
 @note How to calculate the size of the transferred data ? 
       => selected color mode gives the number of bits per pixel and we have 
       the number of pixel per line and the number of line, therefore :
    
       data_size = (bits per pixel) X (pixel per line) X (number of line)
    
 @note How to convert pixel format from ARGB8888 to ARGB4444 ?
       => only the four MSB are taken into account 
      eg : 0x AB  CD  12  34 --> 0x A   C   1   3    
           |_| |_| |_| |_|       |_| |_| |_| |_|
            A   R   G   B         A   R   G   B
         
In general, 
 => if the number of bits per pixel in source data is more then the number of 
    bits per pixel in destination data, only the MSB are taken into account
    
 => else, if the number of bits per pixel in source data is less then the number 
    of bits per pixel in destination data, a bit replication of MSB in LSB is applied
    
    eg : from ARGB1555 to ARGB8888       PFC
    0xAB35 --> 0b1 01010 11001 10101   -------> 0b11111111 01010010 11001110 10101101     
                 A   R     G     B          --> 0xFF       52       CE       AD


@par Directory contents

  - DMA2D/DMA2D_MemToMemWithPFC/system_stm32f4xx.c   STM32F4xx system clock configuration file
  - DMA2D/DMA2D_MemToMemWithPFC/stm32f4xx_conf.h     Library Configuration file
  - DMA2D/DMA2D_MemToMemWithPFC/stm32f4xx_it.c       Interrupt handlers
  - DMA2D/DMA2D_MemToMemWithPFC/stm32f4xx_it.h       Interrupt handlers header file
  - DMA2D/DMA2D_MemToMemWithPFC/main.c               Main program
  - DMA2D/DMA2D_MemToMemWithPFC/main.h               Main program header file
  - DMA2D/DMA2D_MemToMemWithPFC/ARGB8888_300x120.h   Image to be converted and transferred
  
  
@par Hardware and Software environment
  
  - This example runs on and STM32F429xx/439xx devices.
    
  - This example has been tested with STMicroelectronics STM324x9I-EVAL RevB
    (STM32F429xx/STM32F439xx Devices) evaluation boards and can be easily 
    tailored to any other supported device and development board. 


@par How to use it ?

In order to make the program work, you must do the following:
 - Copy all source files from this example folder to the template folder under
   Project\STM32F4xx_StdPeriph_Templates
 - Open your preferred toolchain
  - Select "STM32F429_439xx" workspace related to the used device and Add 
    the following files in the project source list:
     - Utilities\STM32_EVAL\STM324x9I_EVAL\stm324x9i_eval.c
     - Utilities\STM32_EVAL\STM324x9I_EVAL\stm324x9i_eval_lcd.c 
     - Utilities\STM32_EVAL\STM324x9I_EVAL\stm324x9i_eval_fmc_sdram.c
     - Utilities\STM32_EVAL\STM324x9I_EVAL\stm324x9i_eval_ioe8.c
      
 - Rebuild all files and load your image into target memory
 - Run the example
  
 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
 