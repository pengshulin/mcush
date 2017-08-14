/**
  @page DMA2D_MemToMemWithLCD DMA2D memory to memory with LCD example
  
  @verbatim
  ******************** (C) COPYRIGHT 2016 STMicroelectronics *******************
  * @file    DMA2D/DMA2D_MemToMemWithLCD/readme.txt 
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Description of the DMA2D memory to memory with LCD example.
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

This example shows how to configure DMA2D peripheral in Memory to Memory 
transfer mode and display the result on LCD.

In this basic example the goal is to explain the different fields of the DMA2D 
structure in the case of Memory_to_Memory transfer mode.

After DMA2D configuration, the data transfer is performed from FLASH memory to
SRAM and then the LCD is configured to display transferred picture on LCD. 
(more details about LCD configuration in LCD examples)

=> Output offset is the number of pixel to be skipped after each transferred line

how to calculate the size of the transferred data ? 
=> selected color mode gives the number of bits per pixel and we have 
(ARGB4444 => 16bits/pixel) the number of pixel per line and the number of line, 
therefore :

data_size = (bits per pixel) X (pixel per line + output offset) X (number of line) 
       

@par Directory contents
    
  - DMA2D/DMA2D_MemToMemWithLCD/system_stm32f4xx.c   STM32F4xx system clock configuration file
  - DMA2D/DMA2D_MemToMemWithLCD/stm32f4xx_conf.h     Library Configuration file
  - DMA2D/DMA2D_MemToMemWithLCD/stm32f4xx_it.c       Interrupt handlers
  - DMA2D/DMA2D_MemToMemWithLCD/stm32f4xx_it.h       Interrupt handlers header file
  - DMA2D/DMA2D_MemToMemWithLCD/main.c               Main program
  - DMA2D/DMA2D_MemToMemWithLCD/main.h               Main program header file
  - DMA2D/DMA2D_MemToMemWithLCD/ARGB4444_64x59.h     Image to be transferred 


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
                                   