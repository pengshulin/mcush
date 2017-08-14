/**
  @page TIM_7PWMOutput TIM 7 PWM Output example
  
  @verbatim
  ******************** (C) COPYRIGHT 2016 STMicroelectronics *******************
  * @file    TIM/TIM_7PWMOutput/readme.txt 
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Description of the TIM 7 PWM Output example.
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

This example shows how to configure the TIM1 peripheral to generate 7 PWM signals 
with 4 different duty cycles (50%, 37.5%, 25% and 12.5%).

TIM1CLK = SystemCoreClock, Prescaler = 0, TIM1 counter clock = SystemCoreClock
SystemCoreClock is set to 168 MHz for STM32F4xx devices.

The objective is to generate 7 PWM signal at 17.57 KHz:
  - TIM1_Period = (SystemCoreClock / 17570) - 1
The channel 1 and channel 1N duty cycle is set to 50%
The channel 2 and channel 2N duty cycle is set to 37.5%
The channel 3 and channel 3N duty cycle is set to 25%
The channel 4 duty cycle is set to 12.5%
The Timer pulse is calculated as follows:
  - ChannelxPulse = DutyCycle * (TIM1_Period - 1) / 100

The TIM1 waveform can be displayed using an oscilloscope.


@par Directory contents 
 
  - TIM/TIM_7PWMOutput/system_stm32f4xx.c   STM32F4xx system clock configuration file
  - TIM/TIM_7PWMOutput/stm32f4xx_conf.h     Library Configuration file
  - TIM/TIM_7PWMOutput/stm32f4xx_it.c       Interrupt handlers
  - TIM/TIM_7PWMOutput/stm32f4xx_it.h       Interrupt handlers header file
  - TIM/TIM_7PWMOutput/main.c               Main program


@par Hardware and Software environment 

  - This example runs on STM32F405xx/407xx, STM32F415xx/417xx, STM32F427xx/437xx and 
    STM32F429xx/439xx devices.
    
  - This example has been tested with STMicroelectronics STM324xG-EVAL (STM32F40xx/
    STM32F41xx Devices), STM32437I-EVAL (STM32F427xx/STM32F437xx Devices) and 
    STM324x9I-EVAL RevB (STM32F429xx/STM32F439xx Devices) evaluation boards and 
    can be easily tailored to any other supported device and development board.

  - STM324xG-EVAL/STM32437I-EVAL and STM324x9I-EVAL RevB Set-up 
    - Connect the TIM1 pins to an oscilloscope to monitor the different waveforms:
      - TIM1_CH1  (PA.08)
      - TIM1_CH1N (PB.13)
      - TIM1_CH2  (PE.11)
      - TIM1_CH2N (PB.14)
      - TIM1_CH3  (PA.10)
      - TIM1_CH3N (PB.15)
      - TIM1_CH4  (PA.11)


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
