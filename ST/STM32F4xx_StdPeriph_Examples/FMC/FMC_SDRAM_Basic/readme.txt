/**
  @page FMC_SDRAM_Basic SDRAM memory basic functionalities example
  
  @verbatim
  ******************* (C) COPYRIGHT 2016 STMicroelectronics ********************
  * @file    FMC/FMC_SDRAM_Basic/readme.txt 
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Description of the SDRAM memory basic functionalities example.
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

This example shows how to configure the FMC to drive the MT48LC2M3B2B5-7E SDRAM
memory mounted on STM324x9I-EVAL RevB evaluation board. 
  
The goal of this example is to explain the different fields of the FMC structure,
and how to initialize the FMC SDRAM device without using an associate driver for 
the external memory.          

In this example, the FMC SDRAM bank is configured and initialized explicitly 
following all initialization sequence steps. After initializing the device, user 
can perform read/write operations on it. A data buffer is written to the SDRAM 
memory, then read back and checked to verify its correctness.
  
The user can choose his own configuration by commenting/uncommenting the defines for  
undesired/desired configurations in "main.h", for example, to change memory bus width 
configuration to 16 bit, uncomment the define for the configuration 
"FMC_SDMemory_Width_16b".
  
If the data is read correctly from SDRAM, the LED1 is ON, otherwise the LED2 is ON. 

 @note  The FMC mode register definition is configured using defines for the external 
        memory device mode register, defined in "main.h" file.
        The function "Delay()" is used to have a delay measured in microseconds.  
 
 
@par Directory contents
                       
 - FMC/FMC_SDRAM_Basic/system_stm32f4xx.c   STM32F4xx system clock configuration file
 - FMC/FMC_SDRAM_Basic/stm32f4xx_conf.h     Library Configuration file
 - FMC/FMC_SDRAM_Basic/stm32f4xx_it.c       Interrupt handlers
 - FMC/FMC_SDRAM_Basic/stm32f4xx_it.h       Interrupt handlers header file
 - FMC/FMC_SDRAM_Basic/main.c               Main program
 - FMC/FMC_SDRAM_Basic/main.h               Main program header file

      
@par Hardware and Software environment 

  - This example runs on STM32F429xx/439xx devices.
    
  - This example has been tested with STMicroelectronics STM324x9I-EVAL RevB 
    (STM32F429xx/STM32F439xx Devices) evaluation boards and can be easily 
    tailored to any other supported device and development board.


@par How to use it ? 

In order to make the program work, you must do the following:
 - Copy all source files from this example folder to the template folder under
   Project\STM32F4xx_StdPeriph_Templates
 - Open your preferred toolchain 
 - Select the project workspace related to the STM32F429_439xx device and add 
   the following files in the project source list:
     - Utilities\STM32_EVAL\STM324x9I_EVAL\stm324x9i_eval.c
                 
 - Rebuild all files and load your image into target memory
 - Run the example

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
