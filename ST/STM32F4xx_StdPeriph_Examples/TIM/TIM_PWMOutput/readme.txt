/**
  @page TIM_PWMOutput TIM PWM Output example
  
  @verbatim
  ******************** (C) COPYRIGHT 2016 STMicroelectronics *******************
  * @file    TIM/TIM_PWMOutput/readme.txt 
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Description of the TIM PWM Output example.
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

This example shows how to configure the TIM peripheral in PWM (Pulse Width Modulation) 
mode.

The TIM3CLK frequency is set to SystemCoreClock / 2 (Hz), to get TIM3 counter
clock at 21 MHz the Prescaler is computed as following:
   - Prescaler = (TIM3CLK / TIM3 counter clock) - 1

SystemCoreClock is set to 168 MHz for STM32F4xx Devices.

The TIM3 is running at 30 KHz: TIM3 Frequency = TIM3 counter clock/(ARR + 1)
                                              = 21 MHz / 666 = 31.530 KHz
The TIM3 CCR1 register value is equal to 333, so the TIM3 Channel 1 generates a 
PWM signal with a frequency equal to 30 KHz and a duty cycle equal to 50%:
TIM3 Channel1 duty cycle = (TIM3_CCR1/ TIM3_ARR + 1)* 100 = 50%

The TIM3 CCR2 register value is equal to 249, so the TIM3 Channel 2 generates a 
PWM signal with a frequency equal to 30 KHz and a duty cycle equal to 37.5%:
TIM3 Channel2 duty cycle = (TIM3_CCR2/ TIM3_ARR + 1)* 100 = 37.5%

The TIM3 CCR3 register value is equal to 166, so the TIM3 Channel 3 generates a 
PWM signal with a frequency equal to 30 KHz and a duty cycle equal to 25%:
TIM3 Channel3 duty cycle = (TIM3_CCR3/ TIM3_ARR + 1)* 100 = 25%

The TIM3 CCR4 register value is equal to 83, so the TIM3 Channel 4 generates a 
PWM signal with a frequency equal to 30 KHz and a duty cycle equal to 12.5%:
TIM3 Channel4 duty cycle = (TIM3_CCR4/ TIM3_ARR + 1)* 100 = 12.5%

The PWM waveform can be displayed using an oscilloscope.


@par Directory contents 
    
  - TIM/TIM_PWMOutput/system_stm32f4xx.c   STM32F4xx system clock configuration file
  - TIM/TIM_PWMOutput/stm32f4xx_conf.h     Library Configuration file
  - TIM/TIM_PWMOutput/stm32f4xx_it.c       Interrupt handlers
  - TIM/TIM_PWMOutput/stm32f4xx_it.h       Interrupt handlers header file
  - TIM/TIM_PWMOutput/main.c               Main program
  

@par Hardware and Software environment 

  - This example runs on STM32F405xx/407xx, STM32F415xx/417xx, STM32F427xx/437xx and 
    STM32F429xx/439xx devices.
    
  - This example has been tested with STMicroelectronics STM324xG-EVAL (STM32F40xx/
    STM32F41xx Devices), STM32437I-EVAL (STM32F427xx/STM32F437xx Devices) and 
    STM324x9I-EVAL RevB (STM32F429xx/STM32F439xx Devices) evaluation boards and 
    can be easily tailored to any other supported device and development board.

  - STM324xG-EVAL/STM32437I-EVAL and STM324x9I-EVAL RevB Set-up
    - Connect the following pins to an oscilloscope to monitor the different 
      waveforms:
        - TIM3_CH1 (PC.06)
        - TIM3_CH2 (PC.07)
        - TIM3_CH3 (PC.08)
        - TIM3_CH4 (PC.09)  


@par How to use it ? 

In order to make the program work, you must do the following:
 - Copy all source files from this example folder to the template folder under
   Project\STM32F4xx_StdPeriph_Templates
 - Open your preferred toolchain 
 - Select the project workspace related to the used device 
   - If "STM32F40_41xxx" is selected as default project Add the following files in the project source list:
     - Utilities\STM32_EVAL\STM3240_41_G_EVAL\stm324xg_eval.c
           
   - If "STM32F427_437xx" is selected as default project Add the following files in the project source list:
     - Utilities\STM32_EVAL\STM324x7I_EVAL\stm324x7i_eval.c
          
   - If "STM32F429_439xx" is selected as default project Add the following files in the project source list:
     - Utilities\STM32_EVAL\STM324x9I_EVAL\stm324x9i_eval.c
                 
 - Rebuild all files and load your image into target memory
 - Run the example
   
 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
