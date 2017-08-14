/**
  @page ADC_DMA ADC conversion using DMA for Data transfer example
                                                            
  @verbatim
  ******************** (C) COPYRIGHT 2016 STMicroelectronics *******************
  * @file    ADC/ADC_DMA/readme.txt 
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Description of the ADC conversion using DMA for Data transfer 
  *          example.
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

This example shows how to use the ADC3 and DMA to transfer continuously 
converted data from ADC3 to memory.
The ADC3 is configured to convert continuously channel7(for STM324xG-EVAL
and STM32437I-EVAL)/channel8(for STM324x9I-EVAL RevB).
Each time an end of conversion occurs the DMA transfers, in circular mode, the
converted data from ADC3 DR register to the uhADCxConvertedValue variable.

In this example, the system clock is 144MHz, APB2 = 72MHz and ADC clock = APB2/2. 
Since ADC3 clock is 36 MHz and sampling time is set to 3 cycles, the conversion 
time to 12bit data is 12 cycles so the total conversion time is (12+3)/36= 0.41us(2.4Msps).

User can vary the ADC3 channel7(for STM324xG-EVAL and STM32437I-EVAL)/channel8
(for STM324x9I-EVAL RevB) voltage using the Eval Board potentiometer.
The converted voltage is displayed on the Eval Board LCD (when the define USE_LCD
is enabled in main.h file)


@par Directory contents 

  - ADC/ADC_DMA/system_stm32f4xx.c   STM32F4xx system clock configuration file
  - ADC/ADC_DMA/stm32f4xx_conf.h     Library Configuration file
  - ADC/ADC_DMA/stm32f4xx_it.c       Interrupt handlers
  - ADC/ADC_DMA/stm32f4xx_it.h       Interrupt handlers header file
  - ADC/ADC_DMA/main.c               Main program
  - ADC/ADC_DMA/main.h               Main program header file
  

@par Hardware and Software environment 

  - This example runs on STM32F405xx/407xx, STM32F415xx/417xx, STM32F427xx/437xx and 
    STM32F429xx/439xx devices.
    
  - This example has been tested with STMicroelectronics STM324xG-EVAL (STM32F40xx/
    STM32F41xx Devices), STM32437I-EVAL (STM32F427xx/STM32F437xx Devices) and 
    STM324x9I-EVAL RevB (STM32F429xx/STM32F439xx Devices) evaluation boards and 
    can be easily tailored to any other supported device and development board.

  - STM324xG-EVAL and STM32437I-EVAL Set-up
    - Use the Potentiometer (RV1) of the Eval board (connected to PF.09).
  
  - STM324x9I-EVAL RevB Set-up
    - Use the Potentiometer (RV1) of the Eval board (connected to PF.10).


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
