/**
  @page FMC_SDRAM SDRAM memory layered access example
  
  @verbatim
  ******************* (C) COPYRIGHT 2016 STMicroelectronics ********************
  * @file    FMC/FMC_SDRAM/readme.txt 
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Description of the FMC SDRAM memory layered access example.
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
memory mounted on STM324x9I-EVAL RevB evaluation board. It uses an associate 
driver developed to interface with the indicated SDRAM memory device. 

After initializing the device, user can perform read/write operations on it. 
A data buffer is written to the SDRAM memory, then read back and checked to verify 
its correctness.
  
The user can choose his own configuration by commenting/uncommenting the defines 
for undesired/desired configurations in the memory associate driver's header file,
for example, to change memory bus width configuration to 16 bit, uncomment the 
define for the configuration "FMC_SDMemory_Width_16b" in "stm324x9i_eval_fmc_sdram.h" 
file.

If the data is read correctly from SDRAM, the LED1 is ON, otherwise the LED2 is ON.


@par Directory contents
 
 - FMC/FMC_SDRAM/system_stm32f4xx.c   STM32F4xx system clock configuration file
 - FMC/FMC_SDRAM/stm32f4xx_conf.h     Library Configuration file
 - FMC/FMC_SDRAM/stm32f4xx_it.c       Interrupt handlers
 - FMC/FMC_SDRAM/stm32f4xx_it.h       Interrupt handlers header file
 - FMC/FMC_SDRAM/main.c               Main program
 - FMC/FMC_SDRAM/main.h               Main program header file
      
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
     - Utilities\STM32_EVAL\STM324x9I_EVAL\stm324x9i_eval_fmc_sdram.c
            
 - Rebuild all files and load your image into target memory
 - Run the example

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
