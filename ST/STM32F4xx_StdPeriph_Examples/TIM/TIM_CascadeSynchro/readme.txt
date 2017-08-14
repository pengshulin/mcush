/**
  @page TIM_CascadeSynchro TIM Cascade Synchro example
  
  @verbatim
  ******************** (C) COPYRIGHT 2016 STMicroelectronics *******************
  * @file    TIM/TIM_CascadeSynchro/readme.txt 
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Description of the TIM Cascade Synchro example.
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

This example shows how to synchronize TIM peripherals in cascade mode.
In this example three timers are used:

Timers synchronisation in cascade mode:

1)TIM2 is configured as Master Timer:
  - PWM Mode is used
  - The TIM2 Update event is used as Trigger Output

2)TIM3 is slave for TIM2 and Master for TIM4,
  - PWM Mode is used
  - The ITR1(TIM2) is used as input trigger 
  - Gated mode is used, so start and stop of slave counter
    are controlled by the Master trigger output signal(TIM2 update event).
  - The TIM3 Update event is used as Trigger Output. 

3)TIM4 is slave for TIM3,
  - PWM Mode is used
  - The ITR2(TIM3) is used as input trigger
  - Gated mode is used, so start and stop of slave counter  are controlled by the
    Master trigger output signal(TIM3 update event).

The TIM2 counter clock is 84 MHz.

The Master Timer TIM2 is running at TIM2 frequency :
TIM2 frequency = (TIM2 counter clock)/ (TIM2 period + 1) = 328.125 KHz 
and the duty cycle = TIM2_CCR1/(TIM2_ARR + 1) = 25%.

The TIM3 is running at:
(TIM2 frequency)/ (TIM3 period + 1) = 82.02 KHz and a duty cycle equal 
to TIM3_CCR1/(TIM3_ARR + 1) = 25%

The TIM4 is running at:
(TIM3 frequency)/ (TIM4 period + 1) = 20.5 Hz and a duty cycle equal 
to TIM4_CCR1/(TIM4_ARR + 1) = 25%  


@par Directory contents 
  
  - TIM/TIM_CascadeSynchro/system_stm32f4xx.c   STM32F4xx system clock configuration file
  - TIM/TIM_CascadeSynchro/stm32f4xx_conf.h     Library Configuration file
  - TIM/TIM_CascadeSynchro/stm32f4xx_it.c       Interrupt handlers
  - TIM/TIM_CascadeSynchro/stm32f4xx_it.h       Interrupt handlers header file
  - TIM/TIM_CascadeSynchro/main.c               Main program
  - TIM/TIM_CascadeSynchro/main.h               Main program header file
  
  
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
        - TIM2 CH1 (PA.00) 
        - TIM3 CH1 (PB.04)
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
