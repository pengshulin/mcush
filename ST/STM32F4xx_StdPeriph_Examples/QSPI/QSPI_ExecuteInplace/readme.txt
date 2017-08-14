/**
  @page QSPI_ExecuteInPlace example
  
  @verbatim
  ******************** (C) COPYRIGHT 2016 STMicroelectronics *******************
  * @file    QSPI/QSPI_ExecuteInPlace/readme.txt 
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Description of the QSPI Execute In Place example.
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

This example describes how to use the QSPI firmware library to execution of a part of the code from 
QSPI FLASH device.

In this example,the QSPI is interfacing with SPANSION S25FL512S FLASH or MICRON MT25QL512AB FLASH memory
and up to user to select the right flash connected on the STM32446E-EVAL/STM32469I-EVAL/STM32412G-DISCO/STM32F413H-DISCO board 
by uncommenting the required line in main.h:
- QSPI_FLASH_SPANSION: QSPI FLASH Spansion
- QSPI_FLASH_MICRON: QSPI FLASH Micron 

At the startup, the QSPI memory is erased, then the data are copied from the initialization
section of the flash to the QSPI memory using DMA.Then the QSPI is configured in memory-mapped 
mode to read and execute the code in a forever loop.

The code performs a GPIO toggle(PC13 or PG13),it can be displayed using an oscilloscope.

@note
- The QSPI Clock is configured to reach maximum frequency at 90 MHZ.
- For EWARM toolchain, ensure to use standard with IAR extensions compiler option.

@par Directory contents
  - QSPI/QSPI_ExecuteInPlace/system_stm32f4xx.c   STM32F4xx system clock configuration file
  - QSPI/QSPI_ExecuteInPlace/stm32f4xx_conf.h     Library configuration file
  - QSPI/QSPI_ExecuteInPlace/stm32f4xx_it.c       Interrupt handlers
  - QSPI/QSPI_ExecuteInPlace/stm32f4xx_it.h       Interrupt handlers header file
  - QSPI/QSPI_ExecuteInPlace/main.c               Main program
  - QSPI/QSPI_ExecuteInPlace/main.h               Header for main.c module

@note
You have to replace linker file by the one provided with in the QSPI/QSPI_ExecuteInPlace example folder
  - QSPI/QSPI_ExecuteInPlace/EWARM/STM32F412xG/stm32f4xx_flash.icf          EWARM linker file for STM32F412xG devices
  - QSPI/QSPI_ExecuteInPlace/EWARM/STM32413_23xx/stm32f4xx_flash.icf        EWARM linker file for STM32F413_423xx devices
  - QSPI/QSPI_ExecuteInPlace/EWARM/STM32446xx/stm32f4xx_flash.icf           EWARM linker file for STM32F446xx devices
  - QSPI/QSPI_ExecuteInPlace/EWARM/STM32F469_479_Disco/stm32f4xx_flash.icf  EWARM linker file for STM32F469_479xx devices
  - QSPI/QSPI_ExecuteInPlace/EWARM/STM32F469_479_Eval/stm32f4xx_flash.icf   EWARM linker file for STM32F469_479xx devices
  - QSPI/QSPI_ExecuteInPlace/MDK-ARM/STM32F446xx/STM32F446xx.sct            MDK-ARM Scatter file for STM32F446xx devices
  - QSPI/QSPI_ExecuteInPlace/TrueSTUDIO/STM32F446ZETx_FLASH.ld              TrueSTUDIO linker file for STM32F446xx devices
  - QSPI/QSPI_ExecuteInPlace/SW4STM32/STM32F446ZETx_FLASH.ld                SW4STM32 linker file for STM32F446xx devices
  - QSPI/QSPI_ExecuteInPlace/MDK-ARM/STM32F469_479xx/STM32F469_479xx.sct    MDK-ARM Scatter file for STM32F469_479xx devices
  - QSPI/QSPI_ExecuteInPlace/TrueSTUDIO/STM32F469NIHx_FLASH.ld              TrueSTUDIO linker file for STM32F469_479xx devices
  - QSPI/QSPI_ExecuteInPlace/SW4STM32/STM32F469NIHx_FLASH.ld                SW4STM32 linker file for STM32F469_479xx devices
  - QSPI/QSPI_ExecuteInPlace/MDK-ARM/ STM32F413_423xx.sct                   MDK-ARM Scatter file for STM32F413_423xx devices
  - QSPI/QSPI_ExecuteInPlace/TrueSTUDIO/ STM32F413ZH_FLASH.ld               TrueSTUDIO linker file for STM32F413_423xx devices
  - QSPI/QSPI_ExecuteInPlace/SW4STM32/ STM32F413ZHTx_FLASH.ld               SW4STM32 linker file for STM32F413_423xx devices
  - QSPI/QSPI_ExecuteInPlace/MDK-ARM/ STM32F412xG.sct                       MDK-ARM Scatter file for STM32F412xG devices
  - QSPI/QSPI_ExecuteInPlace/TrueSTUDIO/ STM32F412ZG_FLASH.ld               TrueSTUDIO linker file for STM32F412xG devices
  - QSPI/QSPI_ExecuteInPlace/SW4STM32/ STM32F412ZGTx_FLASH.ld               SW4STM32 linker file for STM32F412xG devices

@par Hardware and Software environment
  - This example runs on STM32F446xx , STM32F469/479xx , STM32F412xG and STM32F413/423xx devices.

  - This example has been tested with STMicroelectronics STM32446E-EVAL 
    (STM32F446xx Devices), STM32412G-DISCO (STM32F412xG devices), STM32F413H-DISCO
    (STM32F413_423xx devices) and STM32469I-EVAL(STM32F469xx Devices) evaluation boards
    and can be easily tailored to any other supported device and development board.

  - STM32F446-EVAL, STM32F469I-EVAL and STM32F412G-DISCO Set-up: Connect the PC13 pin to an oscilloscope
    to monitor its waveform.
    
    STM32F413H-DISCO Set-up : Connect the PG13 (D2 on CN8) to an oscilloscope to monitor its waveform.

@par How to use it ?
In order to make the program work, you must do the following:
 - Copy all source files from this example folder to the template folder under
   Project\STM32F4xx_StdPeriph_Templates
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
