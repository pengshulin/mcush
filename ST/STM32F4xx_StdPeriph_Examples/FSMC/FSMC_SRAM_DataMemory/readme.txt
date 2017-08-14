/**
  @page FSMC_SRAM_DataMemory FSMC SRAM Data Memory example
  
  @verbatim
  ******************* (C) COPYRIGHT 2016 STMicroelectronics ********************
  * @file    FSMC/FSMC_SRAM_DataMemory/readme.txt 
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Description of the FSMC SRAM Data Memory example.
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

This example shows how to use the SRAM mounted on on STM3240_41_G_EVAL or STM32437I_EVAL 
board as program data memory (including heap and stack).

The example scenario does not reflect a real application case; its purpose is to
provide only the procedure to follow to use the external SRAM as data memory.

This example does not use the default library startup file. It uses a modified 
startup file provided with the example. The user has to add the new startup 
file in the project source list. While startup, the SRAM memory is configured 
and initialized to be ready to contain data.

The user have to configure his preferred toolchain using the provided linker file.
The RAM zone is modified in order to use the external memory as a RAM. 

At this stage, all the used data can be located in the external SRAM memory.

The user can use the debugger's watch to evaluate "uwTabAddr" and "MSPValue" variables
values which should be equal to "0x640xxxxx".

@par Directory contents

 - FSMC/FSMC_SRAM_DataMemory/system_stm32f4xx.c   STM32F4xx system clock configuration file
 - FSMC/FSMC_SRAM_DataMemory/stm32f4xx_conf.h     Library Configuration file
 - FSMC/FSMC_SRAM_DataMemory/stm32f4xx_it.c       Interrupt handlers
 - FSMC/FSMC_SRAM_DataMemory/stm32f4xx_it.h       Interrupt handlers header file
 - FSMC/FSMC_SRAM_DataMemory/main.c               Main program
 - FSMC/FSMC_SRAM_DataMemory/main.h               Main program header file
 - FSMC/FSMC_SRAM_DataMemory/startup              Directory containing startup file for each toolchain

@par Hardware and Software environment 

  - This example runs on STM32F405xx/407xx and STM32F415xx/417xx devices.
    
  - This example has been tested with STMicroelectronics STM324xG-EVAL (STM32F40xx/
    STM32F41xx Devices) and STM32437I-EVAL (STM32F427x/STM32F437x Devices) evaluation 
    boards and can be easily tailored to any other supported device and development 
    board.
    
  - This example runs on STM32F405xx/407xx and STM32F415xx/417xx devices.
    
  - This example has been tested with STMicroelectronics STM324xG-EVAL (STM32F40xx/
    STM32F41xx Devices) evaluation boards and can be easily tailored to any 
    other supported device and development board.


@par How to use it ? 

 In order to make the program work, you must do the following:
 - Copy all source files from this example folder to the template folder under
   Project\STM32F4xx_StdPeriph_Templates
 - Open your preferred toolchain   
 - Update your project settings as follows:
<ul>
 <li> MDK-ARM 
    - in Project->Options for Linker window, un-check the option "Use Memory Layout
      from Target Dialog". You can then import the scatter file dedicated for this 
      example.

 <li> EWARM 
    - use "stm32f4xx_flash_extsram.icf" as linker file (under Project\STM32F4xx_StdPeriph_Templates\EWARM)

 <li> TrueSTUDIO 
    - In the project properties window, select 'C/C++ Build'->settings node->'Tool Settings' Tab then 
      the 'C Linker'->General node and use "stm32f4xx_flash_extsram.ld" as Script File
      (under Project\STM32F4xx_StdPeriph_Templates\TrueSTUDIO).

 <li> SW4STM32
    - In the project properties window, select C/C++ Build->settings then the C Linker->General
      and refer to " STM32F417IGHx_FLASH_extsram.ld " as a linker file.

</ul>
 - Rebuild all files and load your image into target memory
 - Run the example

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
