/**
  @page ADC_DualModeInterleaved  Dual interleaved mode and DMA mode3 example

  @verbatim
  ******************** (C) COPYRIGHT 2016 STMicroelectronics *******************
  * @file    ADC/ADC_DualModeInterleaved/readme.txt 
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Description of the Dual interleaved mode and DMA mode3 example.
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

This example shows how to use the ADC peripheral to convert a regular channel 
in Dual interleaved mode using DMA in mode 3 with 5Msps.

DMA mode 3 is used in interleaved mode in 6-bit and 8-bit resolutions.

The Dual interleaved delay is configured 6 ADC clock cycles.

On each DMA request (two data items are available) two bytes representing two 
ADC-converted data items are transferred as a half word.
The data transfer order is similar to that of the DMA mode 2.

A DMA request is generated each time 2 data items are available
1st request: ADC_CDR[15:0] = (ADC2_DR[7:0] << 8) | ADC1_DR[7:0] 
2nd request: ADC_CDR[15:0] = (ADC2_DR[7:0] << 8) | ADC1_DR[7:0]

The ADC1 and ADC2 are configured to convert ADC Channel 12, with conversion 
triggered by software.
By this way, ADC channel 12 is converted each 6 cycles.

In this example, the system clock is 144MHz, APB2 =72MHz and ADC clock = APB2 /2.
Since ADCCLK= 36MHz and Conversion rate = 6 cycle
==> Conversion Time = 36M/6cyc = 6Msps.

 @note Refer to "simulation.xls" file to have the diagram simulation of the example.


@par Directory contents 

  - ADC/ADC_DualModeInterleaved/system_stm32f4xx.c   STM32F4xx system clock configuration file
  - ADC/ADC_DualModeInterleaved/stm32f4xx_conf.h     Library Configuration file
  - ADC/ADC_DualModeInterleaved/stm32f4xx_it.h       Interrupt handlers header file
  - ADC/ADC_DualModeInterleaved/stm32f4xx_it.c       Interrupt handlers
  - ADC/ADC_DualModeInterleaved/main.c               Main program
  - ADC/ADC_DualModeInterleaved/main.h               Main program header file
  

@par Hardware and Software environment 

  - This example runs on STM32F405xx/407xx, STM32F415xx/417xx, STM32F427xx/437xx and 
    STM32F429xx/439xx devices.
    
  - This example has been tested with STMicroelectronics STM324xG-EVAL (STM32F40xx/
    STM32F41xx Devices), STM32437I-EVAL (STM32F427xx/STM32F437xx Devices) and 
    STM324x9I-EVAL RevB (STM32F429xx/STM32F439xx Devices) evaluation boards and 
    can be easily tailored to any other supported device and development board.

  - STM324xG-EVAL, STM32437I-EVAL and STM324x9I-EVAL RevB Set-up
    - Connect PC.2 to a power supply (do not forget to connect the power supply 
      GND to the EVAL board GND).


@par How to use it ? 

In order to make the program work, you must do the following:
 - Copy all source files from this example folder to the template folder under
   Project\STM32F4xx_StdPeriph_Templates
 - Open your preferred toolchain 
 - Select the project workspace related to the used device 
   - If "STM32F40_41xxx" is selected as default project Add the following files in the project source list:
     - Utilities\STM32_EVAL\STM3240_41_G_EVAL\stm324xg_eval.c
     - Utilities\STM32_EVAL\STM3240_41_G_EVAL\stm324xg_eval_lcd.c 
     - Utilities\STM32_EVAL\STM3240_41_G_EVAL\stm324xg_eval_fsmc_sram.c
     - Utilities\STM32_EVAL\STM3240_41_G_EVAL\stm324xg_eval_ioe.c 
      
   - If "STM32F427_437xx" is selected as default project Add the following files in the project source list:
     - Utilities\STM32_EVAL\STM324x7I_EVAL\stm324x7i_eval.c
     - Utilities\STM32_EVAL\STM324x7I_EVAL\stm324x7i_eval_lcd.c 
     - Utilities\STM32_EVAL\STM324x7I_EVAL\stm324x7i_eval_fmc_sram.c
     - Utilities\STM32_EVAL\STM324x7I_EVAL\stm324x7i_eval_ioe.c
     
   - If "STM32F429_439xx" is selected as default project Add the following files in the project source list:
     - Utilities\STM32_EVAL\STM324x9I_EVAL\stm324x9i_eval.c
     - Utilities\STM32_EVAL\STM324x9I_EVAL\stm324x9i_eval_lcd.c 
     - Utilities\STM32_EVAL\STM324x9I_EVAL\stm324x9i_eval_fmc_sdram.c
     - Utilities\STM32_EVAL\STM324x9I_EVAL\stm324x9i_eval_ioe8.c   
     - Utilities\STM32_EVAL\STM324x9I_EVAL\stm324x9i_eval_ioe16.c 
            
 - Rebuild all files and load your image into target memory
 - Run the example

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
