/**
  @page FMC_SDRAM_LowPower SDRAM memory Low Power mode example 
  
  @verbatim
  ******************* (C) COPYRIGHT 2016 STMicroelectronics ********************
  * @file    FMC/FMC_SDRAM_LowPower/readme.txt 
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Description of the FMC SDRAM Low Power mode example.
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
memory, mounted on STM324x9I-EVAL RevB board, in low power mode (self refresh mode).
            
This example gives a simple application of the FMC SDRAM low power mode (self 
refresh mode) while the MCU is in a low power mode (STOP mode). 
  
The purpose is to allow the SDRAM to retain data written after entering STOP mode. 
STOP mode is a CPU low power mode which stops all peripherals clocks in the 1.2V 
domain, only internal SRAM and registers content are preserved.
  
After SDRAM initialization, the data is written to the memory and a "self refresh" 
command is sent to the SDRAM. The program waits for TAMPER/KEY button to be pushed 
to enter CPU in STOP mode, the LED3 is then turned ON. 
The wakeup from STOP mode is done when pushing WAKEUP Button and the CPU returns 
to RUN mode. At this time, the system clock is reconfigured. 
Finally, a "normal mode" command is sent to SDRAM memory to exit self refresh mode. 
The data written to SDRAM is read back and checked.  
   
LEDs are used to indicate the system state as follows:
  - LED3 ON: system in STOP mode.
  - LED3 OFF: system is in RUN mode.
  - LED1 ON: correct data transfer (PASS). 
  - LED2 ON: incorrect data transfer (FAIL).


@par Directory contents
 
 - FMC/FMC_SDRAM_LowPower/system_stm32f4xx.c   STM32F4xx system clock configuration file
 - FMC/FMC_SDRAM_LowPower/stm32f4xx_conf.h     Library Configuration file
 - FMC/FMC_SDRAM_LowPower/stm32f4xx_it.c       Interrupt handlers
 - FMC/FMC_SDRAM_LowPower/stm32f4xx_it.h       Interrupt handlers header file
 - FMC/FMC_SDRAM_LowPower/main.c               Main program
 - FMC/FMC_SDRAM_LowPower/main.h               Main program header file
      
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
