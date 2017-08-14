/**
  @page TIM_Synchro TIM Synchro example
  
  @verbatim
  ******************** (C) COPYRIGHT 2016 STMicroelectronics *******************
  * @file    TIM/TIM_Synchro/readme.txt 
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Description of the TIM Synchro example.
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

This example shows how to synchronize TIM1 and Timers (TIM3 and TIM4) in parallel mode.

Timers synchronisation in parallel mode:

1) TIM1 is configured as Master Timer:
   - PWM Mode is used
   - The TIM1 Update event is used as Trigger Output 
 
2) TIM3 and TIM4 are slaves for TIM1,
   - PWM Mode is used
   - The ITR0(TIM1) is used as input trigger for both slaves
   - Gated mode is used, so starts and stops of slaves counters
     are controlled by the Master trigger output signal(update event).

The TIM1 counter clock is 168 MHz.

The Master Timer TIM1 is running at:
TIM1 frequency = TIM1 counter clock / (TIM1_Period + 1) = 656 KHz
and the duty cycle is equal to: TIM1_CCR1/(TIM1_ARR + 1) = 50%

The TIM3 is running at: 
(TIM1 frequency)/ ((TIM3 period +1)* (Repetition_Counter+1)) = 43.730 KHz and
a duty cycle equal to TIM3_CCR1/(TIM3_ARR + 1) = 33.3%

The TIM4 is running at:
(TIM1 frequency)/ ((TIM4 period +1)* (Repetition_Counter+1)) = 65.600 KHz and
a duty cycle equal to TIM4_CCR1/(TIM4_ARR + 1) = 50%
  

@par Directory contents 
 
  - TIM/TIM_Synchro/system_stm32f4xx.c   STM32F4xx system clock configuration file
  - TIM/TIM_Synchro/stm32f4xx_conf.h     Library Configuration file
  - TIM/TIM_Synchro/stm32f4xx_it.c       Interrupt handlers
  - TIM/TIM_Synchro/stm32f4xx_it.h       Interrupt handlers header file
  - TIM/TIM_Synchro/main.c               Main program

 
@par Hardware and Software environment 

  - This example runs on STM32F405xx/407xx, STM32F415xx/417xx, STM32F427xx/437xx and 
    STM32F429xx/439xx devices.
    
  - This example has been tested with STMicroelectronics STM324xG-EVAL (STM32F40xx/
    STM32F41xx Devices), STM32437I-EVAL (STM32F427xx/STM32F437xx Devices) and 
    STM324x9I-EVAL RevB (STM32F429xx/STM32F439xx Devices) evaluation boards and 
    can be easily tailored to any other supported device and development board.

  - STM324xG-EVAL/STM32437I-EVAL and STM324x9I-EVAL RevB Set-up 
   - Connect the following pins to an oscilloscope to monitor the different waveforms:
      - TIM1 CH1 (PA.08)
      - TIM3 CH1 (PC.06)
      - TIM4 CH1 (PB.06)  
  
  
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
